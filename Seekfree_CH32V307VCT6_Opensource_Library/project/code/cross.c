/*=============================== 头文件包含 =================================*/
#include "cross.h"
#include "control.h"
#include "image.h"
#include "utils.h"
#include "circle.h"
#include "garage.h"
#include "Motor.h"
#include "zf_common_headfile.h"
/*============================= 宏定义/重定义 ================================*/
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
/*================================ 全局变量 ==================================*/
enum cross_type_e cross_type = CROSS_NONE;
extern image_t img_raw ;
uint32 back_Position;/**< 左右L角点往后点的位置*/
float inv_Lpt0[2],inv_Lpt1[2];/**< 左右L角点对应原图的点*/
float inv_Back_Lpt0[2],inv_Back_Lpt1[2];/**< 左右L角点后的一个点*/
float inv_far_Lpt0[2],inv_far_Lpt1[2];/**< 远处左右L角点*/

int far_x0,far_y0,far_x1,far_y1;/**< 远处搜线起始点*/
int far_Show_x0,far_Show_y0,far_Show_x1,far_Show_y1;/**< 图显远处搜线起始点*/

// 以下定义为十字寻远线设定，均为远处的线
int far_ipts0[MT9V03X_H][2];/**< 远处左边线*/
int far_ipts1[MT9V03X_H][2];/**< 远处右边线*/
int far_ipts0_num, far_ipts1_num;

int far_dipts0[MT9V03X_H][2];/**< 去畸变左边线*/
int far_dipts1[MT9V03X_H][2];/**< 去畸变右边线*/
int far_dipts0_num, far_dipts1_num;

float far_rpts0[MT9V03X_H][2];/**< 透视变换后左边线*/
float far_rpts1[MT9V03X_H][2];/**< 透视变换后右边线*/
int far_rpts0_num, far_rpts1_num;

float far_rpts0b[MT9V03X_H][2];/**< 左边线滤波*/
float far_rpts1b[MT9V03X_H][2];/**< 右边线滤波*/
int far_rpts0b_num, far_rpts1b_num;

float far_rpts0s[MT9V03X_H][2];/**< 左边线等距采样*/
float far_rpts1s[MT9V03X_H][2];/**< 右边线等距采样*/
int far_rpts0s_num, far_rpts1s_num;

float far_rpts0a[MT9V03X_H];/**< 左边线局部角度变化率*/
float far_rpts1a[MT9V03X_H];/**< 右边线局部角度变化率*/
int far_rpts0a_num, far_rpts1a_num;

float far_rpts0an[MT9V03X_H];/**< 非极大抑制后的左边线局部角度变化率*/
float far_rpts1an[MT9V03X_H];/**< 非极大抑制后的边线局部角度变化率*/
int far_rpts0an_num, far_rpts1an_num;

float far_rptsc0[MT9V03X_H][2];/**< 透视变换后远左中线*/
float far_rptsc1[MT9V03X_H][2];/**< 透视变换后远右中线*/
int far_rptsc0_num, far_rptsc1_num;

bool far_Lpt0_found, far_Lpt1_found;/**< 左右角点标志位*/
int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;/**< 左右角点坐标*/

uint32 far_begin_x=50;/**< 远线搜寻的起始点*/
uint32 far_begin_y=65;/**< 远线搜寻的起始点*/

float far_conf0_max,far_conf0;/**< 用于图显*/
float far_conf1_max,far_conf1;/**< 用于图显*/

int not_have_line=0;

/*================================ 接口函数 ==================================*/
void check_Cross();
void check_Left_Cross();
void check_Right_Cross();
void run_Cross();
/******************************************************************************
* FunctionName   : check_Cross1()
* Description    : 正入十字
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Cross1()
{
    if (cross_type == CROSS_NONE && Lpt0_found && Lpt1_found)
        cross_type = CROSS_BEGIN;
}
/******************************************************************************
* FunctionName   : check_Cross()
* Description    : 判断十字的各类标志位
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Cross()
{
    //找到上下两个角点即进入CROSS_BEGIN
    if(Lpt0_found&&cross_type==CROSS_NONE&&!is_straight1&&rpts1s_num>5)
        {
            check_Left_Cross ();
            if(far_Lpt0_found&&!is_straight1)
            {
                cross_type=CROSS_BEGIN;
            }
        }
    if(Lpt1_found&&cross_type==CROSS_NONE&&!is_straight0&&rpts0s_num>5)
        {
            check_Right_Cross();
            if(far_Lpt1_found&&!is_straight0)
            {
                cross_type=CROSS_BEGIN;
            }
        }
}

/******************************************************************************
* FunctionName   : run_Cross()
* Description    : 执行十字
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void run_Cross()
{
    switch (cross_type)
    {
        //对边线进行截断处理
        case CROSS_BEGIN:
            check_Left_Cross ();
            check_Right_Cross();
            if (Lpt0_found)
            {
                rptsc0_num = rpts0s_num= Lpt0_rpts0s_id-10;//用于后面的近线控制
            }
            if (Lpt1_found)
            {
                rptsc1_num = rpts0s_num =Lpt1_rpts1s_id-10;//用于后面的近线控制
            }

            //两近处角点靠下
            if((Lpt0_rpts0s_id<25&&(rpts0s_num<50&&ipts0_num>2)&&far_ipts0_num>15)||(Lpt1_rpts1s_id<25&&(rpts1s_num<50&&ipts1_num>2)&&far_ipts1_num>15))
            {
                cross_type=CROSS_IN;
            }
             break;


        case CROSS_IN:
            check_Left_Cross();
            check_Right_Cross();

            if (rpts1s_num < 5 && rpts0s_num < 5) { not_have_line++; }
            if(not_have_line>2 && (rpts1s_num > 5 || rpts0s_num > 5))
            {
                cross_type = CROSS_NONE;
                not_have_line=0;

            }
            if (far_Lpt1_found) { track_type = TRACK_RIGHT; }
            else if (far_Lpt0_found) { track_type = TRACK_LEFT; }
            else if(far_rpts0s_num > far_rpts1s_num)track_type = TRACK_LEFT;
            else if(far_rpts1s_num >= far_rpts0s_num)track_type = TRACK_RIGHT;
            break;

    }

}
/******************************************************************************
* FunctionName   : check_Left_Cross()
* Description    : 十字,圆环左远线的处理
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Left_Cross()
{
    //以下是寻找起始点

    //十字情况
    if(Lpt0_found && circle_type==0)//左近L角点找到,则向左上上偏移作为寻找原线的起始点
    {
         inv_Lpt0[0]=Cal_inv_rot_x(rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][0],rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][1]);
         inv_Lpt0[1]=Cal_inv_rot_y(rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][0],rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][1]);

         inv_Back_Lpt0[0]=Cal_inv_rot_x((rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][0]),rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][1]);
         inv_Back_Lpt0[1]=Cal_inv_rot_y((rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][0]),rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][1]);
         far_x0=round((0.5*inv_Back_Lpt0[0]+0.5*inv_Lpt0[0]))-4;
         far_y0=round((0.5*inv_Back_Lpt0[1]+0.5*inv_Lpt0[1]))-5;//偏移是为了防止还在黑线处

    }

    else if(circle_type==CIRCLE_LEFT_IN)//L角点没找到，但是处于圆环IN阶段，右线快要丢线，则采用右线倒数第三个点，偏移后作为起始点
    {
        if(ipts0_num<8){//右线太少，则固定点
            far_x0=28;
            far_y0=90;
        }else{
        far_x0=round(ipts0[ipts0_num-5][0]);
        far_y0=round(ipts0[ipts0_num-5][1])-3;//减3向上做一定的偏移
        }
    }


    if(far_y0-far_ipts0[0][1]<5)
    {
        far_y0=far_ipts0[0][1]+5;
    }
    //用于图显
    far_Show_x0=far_x0;
    far_Show_y0=far_y0;

    far_ipts0_num = sizeof(far_ipts0) / sizeof(far_ipts0[0]);
    for(;far_y0>(adaptive_Block-1)/2;far_y0--){
        if (AT_IMAGE(&img_raw, far_x0, far_y0-1) < Ostu_Thres)
            break;
    }
    if (AT_IMAGE(&img_raw, far_x0, far_y0) >= Ostu_Thres)
           findline_lefthand_adaptive(&img_raw, adaptive_Block, clip_value, far_x0, far_y0, far_ipts0, &far_ipts0_num);
       else far_ipts0_num = 0;

    // 去畸变+透视变换
    distort_img_process(far_ipts0, far_ipts0_num, far_dipts0);
    for(int i=0;i<far_ipts0_num;i++)
    {
        far_rpts0[i][0]=Cal_rot_x(far_dipts0[i][0],far_dipts0[i][1]);
        far_rpts0[i][1]=Cal_rot_y(far_dipts0[i][0],far_dipts0[i][1]);
    }
    far_dipts0_num = far_rpts0_num = far_ipts0_num;

    // 边线滤波
    blur_points(far_rpts0, far_rpts0_num, far_rpts0b, (int) round(line_blur_kernel));
    far_rpts0b_num = far_rpts0_num;

    // 边线等距采样
    far_rpts0s_num = sizeof(far_rpts0s) / sizeof(far_rpts0s[0]);
    resample_points(far_rpts0b, far_rpts0b_num, far_rpts0s, &far_rpts0s_num, sample_dist * pixel_per_meter);

    // 边线局部角度变化率
    local_angle_points(far_rpts0s, far_rpts0s_num, far_rpts0a, (int) round(0.2 / sample_dist));
    far_rpts0a_num = far_rpts0s_num;

    // 角度变化率非极大抑制
    nms_angle(far_rpts0a, far_rpts0a_num, far_rpts0an, (int) round(0.2 / sample_dist) * 2 + 1);
    far_rpts0an_num = far_rpts0a_num;
    find_far_L0();

    if(circle_type==CIRCLE_LEFT_IN)
    {
        if(far_Lpt0_found){
        //将寻到的左远线的L角点当成右远线的起始点，此时可寻右远线入环
            for(int i=0;i<far_Lpt0_rpts0s_id;i++){
                far_rpts1s[i][0]=far_rpts0s[far_Lpt0_rpts0s_id-i][0];
                far_rpts1s[i][1]=far_rpts0s[far_Lpt0_rpts0s_id-i][1];}
            far_rpts1s_num=far_Lpt0_rpts0s_id;}
    }
    if(circle_obstacle_flag==0)//防止寻到远线，中线会出问题
     {
        if(cross_type!=CROSS_NONE)
        {
        track_leftline(far_rpts0s + far_Lpt0_rpts0s_id, far_rpts0s_num - far_Lpt0_rpts0s_id, far_rptsc0, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);
        far_rptsc0_num = far_rpts0s_num - far_Lpt0_rpts0s_id;
        }
        else{
            track_rightline(far_rpts1s, far_rpts1s_num, far_rptsc1, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);
            far_rptsc1_num = far_rpts1s_num;
        }
     }
}

/******************************************************************************
* FunctionName   : check_Right_Cross()
* Description    : 十字，圆环右远线的处理
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Right_Cross()
{
    //以下是寻找远线起始点
    if(Lpt1_found && circle_type==0)//右近L角点找到,则向右上偏移作为寻找原线的起始点
    {
        inv_Lpt1[0]=Cal_inv_rot_x((rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][1]);
        inv_Lpt1[1]=Cal_inv_rot_y((rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][1]);

        inv_Back_Lpt1[0]=Cal_inv_rot_x((rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][1]);
        inv_Back_Lpt1[1]=Cal_inv_rot_y((rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][1]);

        far_x1=round((0.5*inv_Back_Lpt1[0]+0.5*inv_Lpt1[0]))+4;
        far_y1=round((0.5*inv_Back_Lpt1[1]+0.5*inv_Lpt1[1]))-5;//偏移是为了防止还在黑线处
    }

    else if(circle_type==CIRCLE_RIGHT_IN)//L角点没找到，但是处于圆环IN阶段，右线快要丢线，则采用右线倒数第三个点，偏移后作为起始点
    {
        if(ipts1_num<8){//右线太少，则固定点
            far_x1=160;
            far_y1=90;
        }else{
            far_x1=round(ipts1[ipts1_num-5][0]);
            far_y1=round(ipts1[ipts1_num-5][1])-3;//减3向上做一定的偏移
            }
    }


    if(far_y1-far_ipts1[0][1]<5)
    {
        far_y1=far_ipts1[0][1]+5;
    }

    //用于图显
    far_Show_x1=far_x1;
    far_Show_y1=far_y1;

    far_ipts1_num = sizeof(far_ipts1) / sizeof(far_ipts1[0]);
    for(;far_y1>(adaptive_Block-1)/2;far_y1--){
        if (AT_IMAGE(&img_raw, far_x1, far_y1-1) < Ostu_Thres)
            break;
    }

    if (AT_IMAGE(&img_raw, far_x1, far_y1) >= Ostu_Thres)
           findline_righthand_adaptive(&img_raw, adaptive_Block, clip_value, far_x1, far_y1, far_ipts1, &far_ipts1_num);
       else far_ipts1_num = 0;

    // 去畸变+透视变换
    distort_img_process(far_ipts1, far_ipts1_num, far_dipts1);
    for(int i=0;i<far_ipts1_num;i++)
    {
        far_rpts1[i][0]=Cal_rot_x(far_dipts1[i][0],far_dipts1[i][1]);
        far_rpts1[i][1]=Cal_rot_y(far_dipts1[i][0],far_dipts1[i][1]);
    }
    far_dipts1_num = far_rpts1_num = far_ipts1_num;

    // 边线滤波
    blur_points(far_rpts1, far_rpts1_num, far_rpts1b, (int) round(line_blur_kernel));
    far_rpts1b_num = far_rpts1_num;

    // 边线等距采样
    far_rpts1s_num = sizeof(far_rpts1s) / sizeof(far_rpts1s[0]);
    resample_points(far_rpts1b, far_rpts1b_num, far_rpts1s, &far_rpts1s_num, sample_dist * pixel_per_meter);

    // 边线局部角度变化率
    local_angle_points(far_rpts1s, far_rpts1s_num, far_rpts1a, (int) round(angle_dist / sample_dist));
    far_rpts1a_num = far_rpts1s_num;

    // 角度变化率非极大抑制
    nms_angle(far_rpts1a, far_rpts1a_num, far_rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);
    far_rpts1an_num = far_rpts1a_num;

    find_far_L1();

    if(circle_type==CIRCLE_RIGHT_IN)
    {
        if(far_Lpt1_found){
        //将寻到的右远线的L角点当成左远线的起始点，此时可寻左远线入环
            for(int i=0;i<far_Lpt1_rpts1s_id;i++){
                far_rpts0s[i][0]=far_rpts1s[far_Lpt1_rpts1s_id-i][0];
                far_rpts0s[i][1]=far_rpts1s[far_Lpt1_rpts1s_id-i][1];}
            far_rpts0s_num=far_Lpt1_rpts1s_id;}
    }

    if(circle_obstacle_flag==0)//防止寻到远线，中线会出问题
     {
        if(cross_type!=CROSS_NONE)
        {
        track_rightline(far_rpts1s + far_Lpt1_rpts1s_id, far_rpts1s_num - far_Lpt1_rpts1s_id, far_rptsc1, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);
        far_rptsc1_num = far_rpts1s_num - far_Lpt1_rpts1s_id;
        }
        else{
            track_leftline(far_rpts0s, far_rpts0s_num, far_rptsc0, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);
            far_rptsc0_num = far_rpts0s_num;
        }
     }
}

/******************************************************************************
* FunctionName   : find_far_L0()
* Description    : 找远处左上角点
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/

void find_far_L0()
{
    far_Lpt0_found = false;
    //far_Lpt0_rpts0s_id=0;
    far_conf0_max=0;
    for (int i = 0; i < MIN(far_rpts0s_num, 90); i++)//限制搜线个数为90
    {
        if (far_rpts0an[i] == 0) continue;
        int im0 = clip(i - (int) round(0.3 / sample_dist), 0, far_rpts0s_num - 1);//向前取一个点
        int ip0 = clip(i + (int) round(0.3 / sample_dist), 0, far_rpts0s_num - 1);//向后取一个点
        far_conf0 = fabs(far_rpts0a[i]) - (fabs(far_rpts0a[im0]) + fabs(far_rpts0a[ip0])) / 2;
        far_conf0=far_conf0*180/PI;
        if(far_conf0>far_conf0_max)far_conf0_max=far_conf0;
        if (Lconf_Min< far_conf0 && far_conf0 <Lconf_Max)
        {
            far_Lpt0_rpts0s_id = i;
            far_Lpt0_found = true;
            break;
        }

    }
}
/******************************************************************************
* FunctionName   : find_far_L1()
* Description    : 找远处右上角点
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void find_far_L1()
{
    far_Lpt1_found = false;
    //far_Lpt1_rpts1s_id=0;
    far_conf1_max=0;
    for (int i = 0; i <MIN(far_rpts1s_num, 90); i++)//限制搜线个数为90
    {
        if (far_rpts1an[i] == 0) continue;
        int im1 = clip(i - (int) round(0.3 / sample_dist), 0, far_rpts1s_num - 1);//向前取一个点
        int ip1 = clip(i + (int) round(0.3 / sample_dist), 0, far_rpts1s_num - 1);//向后取一个点
        far_conf1 = fabs(far_rpts1a[i]) - (fabs(far_rpts1a[im1]) + fabs(far_rpts1a[ip1])) / 2;
        far_conf1=far_conf1*180/PI;
        if(far_conf1>far_conf1_max)far_conf1_max=far_conf1;
        if (Lconf_Min< far_conf1 && far_conf1 <Lconf_Max)
        {
            far_Lpt1_rpts1s_id = i;
            far_Lpt1_found = true;
            break;
        }

    }

}
