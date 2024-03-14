#include "control.h"
#include "cross.h"
#include "image.h"
#include "utils.h"
#include "garage.h"
#include "PID.h"
#include "Motor.h"
#include "obstacle.h"
#include "circle.h"
#include "zf_common_headfile.h"

uint32 aimSpeed=0;/**< 目标速度*/
uint32 aim_distance=440;//除了1000
uint32 AIM_DISTANCE=440;/**< 直接用于计算*/
float angle;
enum track_type_e track_type;
extern float rpts0s[LINE_LENTH][2];/**< 左边线等距采样*/
extern float rpts1s[LINE_LENTH][2];/**< 右边线等距采样*/
extern int rpts0s_num, rpts1s_num;

extern float rptsc0[LINE_LENTH][2];/**< 透视变换后左中线 0x1y*/
extern float rptsc1[LINE_LENTH][2];/**< 透视变换后右中线 0x1y*/
extern int rptsc0_num, rptsc1_num;

extern image_t img_raw ;

float (*rpts)[2];/**< 中线*/
float cx,cy;/**< 纯跟踪起始点*/
int aim_idx;
float error;
float pure_angle;
int keepangleflag=0;//圆环定角出flag


#define ABS(x) (((x) > 0) ? (x) : (-(x)))
int rpts_num;

float rptsn[MT9V03X_W][2];/**< 归一化中线*/
int rptsn_num;
void control_Init()
{
    //根据边线长度切换循线方式
    if (rpts0s_num < rpts1s_num / 2 && rpts0s_num < 25) {
        track_type = TRACK_RIGHT;
    } else if (rpts1s_num < rpts0s_num / 2 && rpts1s_num < 25) {
        track_type = TRACK_LEFT;
    } else if (rpts0s_num < 10 && rpts1s_num > rpts0s_num) {
        track_type = TRACK_RIGHT;
    } else if (rpts1s_num < 10 && rpts0s_num > rpts1s_num) {
        track_type = TRACK_LEFT;
    }
    //十字根据远线控制
    if(cross_type!=CROSS_NONE)
        run_Cross();
    if(circle_type!=CIRCLE_NONE)
    {
        run_Lcircle();
        run_Rcircle();
    }
    if(obstacle_type!=OBSTACLE_NONE)
    {
        run_Lobstacle();
        run_Robstacle();
    }
    if(cross_type==CROSS_IN)//近十字，切寻远线
    {
        if (track_type == TRACK_LEFT){
            rpts = far_rptsc0;
            rpts_num = far_rptsc0_num;
        }
        else {
            rpts = far_rptsc1;
            rpts_num = far_rptsc1_num;
        }
    }
    else if ((circle_type==CIRCLE_RIGHT_IN)&&farline_type==1)//入圆环，切特殊远线
    {
        rpts = far_rptsc0;
        rpts_num = far_rptsc0_num;
    }
    else{//正常寻左右线,左障碍寻左线，右障碍寻右线
       if (track_type == TRACK_LEFT){
           rpts = rptsc0;
           rpts_num = rptsc0_num;
       }
       else {
           rpts = rptsc1;
           rpts_num = rptsc1_num;
       }
    }

       float H_zoom = 0.95f;
       float Half_width = MT9V03X_W/2;
       cx = (rot[1][0]*MT9V03X_H*H_zoom+rot[1][1]*Half_width+rot[1][2])/(rot[2][0]*MT9V03X_H*H_zoom+rot[2][1]*Half_width+rot[2][2]);
       cy = (rot[0][0]*MT9V03X_H*H_zoom+rot[0][1]*Half_width+rot[0][2])/(rot[2][0]*MT9V03X_H*H_zoom+rot[2][1]*Half_width+rot[2][2]);

       float min_dist = 100000;
       int begin_id = 0;
       //在中线上找与车轮起始点最近的点
           for (int i = 0; i < rpts_num; i++) {

               float dx = rpts[i][0] - cx;//中线点与车轮点距离
               float dy = rpts[i][1] - cy;
               float dist = sqrt(dx * dx + dy * dy);
               if (dist < min_dist) {
                   min_dist = dist;
                   begin_id = i;
               }
           }

           //利用中线上最近的点，向两侧检查该点附近是否有斑马线
           bool zebra_L_flag=0;
           bool zebra_R_flag=0;
           for (int i = 0; i < pixel_per_meter * (ROAD_WIDTH/2 - 0.15); i++)//检查宽度需要考虑避障
           {
               if(obstacle_type!=OBSTACLE_NONE)break;
               if(AT_IMAGE(&img_raw, (int)(rpts[begin_id][0]-i), (int)(rpts[begin_id][1])) < Ostu_Thres)
               {
                   zebra_L_flag = 1;
               }
               if(AT_IMAGE(&img_raw, (int)(rpts[begin_id][0]+i), (int)(rpts[begin_id][1])) < Ostu_Thres)
               {
                   zebra_R_flag = 1;
               }
               if(zebra_L_flag && zebra_R_flag)
               {
                   garage_type=GARAGE_FOUND;
                   break;
               }
           }
           if(garage_type!=GARAGE_NONE)
               run_garage();

           // 中线有点，同时最近点不是最后几个点
           if (begin_id >= 0 && rpts_num - begin_id >= 3)//切摄像头
           {
               rpts[begin_id][0] = cx;
               rpts[begin_id][1] = cy;
               rptsn_num = sizeof(rptsn) / sizeof(rptsn[0]);
               resample_points(rpts + begin_id, rpts_num - begin_id, rptsn, &rptsn_num, sample_dist * pixel_per_meter);
               AIM_DISTANCE=aim_distance;


               aim_idx = (int)clip(round(AIM_DISTANCE/1000.0/sample_dist), 0, rptsn_num - 1);
               // 计算远锚点偏差值
               float dx    = rptsn[aim_idx][0] - cx;
               float dy    = cy - rptsn[aim_idx][1];
               float dn    = sqrt(dx * dx + dy * dy);

               error=atan2f(dx,-dy)*180.0/PI;
               // 纯跟踪算法
              // pure_angle = atanf(pixel_per_meter * 2 * 0.2 * dx / dn / dn*1.1) / PI * 180.0;//pure_angle测试
               pure_angle = atanf(pixel_per_meter * 2 * 0.15 * dx / dn / dn) / PI * 180.0;
               //外环角度环
               angle    = PID_Realize(&Angle_PID, Angle,pure_angle,0);
               debug_show_float("angl", angle, 3);
           }
           else  // 中线点过少(出现问题)，此时不转角
           {
               rptsn_num = 0;
           }
}
void check_all()
{
    //斑马线检查位于control_Init()中
    if(garage_type==GARAGE_NONE&&circle_type==CIRCLE_NONE&&cross_type==CROSS_NONE&&obstacle_type==OBSTACLE_NONE)
    check_circle();
    if(garage_type==GARAGE_NONE&&circle_type==CIRCLE_NONE&&obstacle_type==OBSTACLE_NONE)
    check_Cross();

}
