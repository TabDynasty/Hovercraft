/*================================ 全局变量 ==================================*/
#include "control.h"
#include "image.h"
#include "utils.h"
#include "garage.h"
#include "cross.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum garage_type_e garage_type = GARAGE_OUT;
extern image_t img_raw ;
bool zebra_L_flag,zebra_R_flag;
int zebraL_x,zebraR_x;
int out_distance,stop_distance;//3000,1200

void check_garage()
{
    //利用左右边线上比起始点更远一点的点，检查赛道中间是否有斑马线
    zebra_L_flag=0;
    zebra_R_flag=0;
    zebraL_x=0;
    zebraR_x=0;
    for (int i = 1; i < pixel_per_meter * ROAD_WIDTH/2; i++)
    {
       if(zebra_L_flag==0 && AT_IMAGE(&img_raw, (int)(ipts0[1][0]+i+20), (int)(ipts0[1][1])) < Ostu_Thres//左边线上的第二个点，向右寻找白黑像素点
           && AT_IMAGE(&img_raw, (int)(ipts0[1][0]+i+20-1), (int)(ipts0[1][1])) >= Ostu_Thres)
       {
//           if(AT_IMAGE(&img_raw, (int)(ipts0[1][0]+i), (int)(ipts0[1][1])) < Ostu_Thres)
           zebra_L_flag = 1;
           zebraL_x=ipts0[1][0]+i+20;
       }
       if(zebra_R_flag==0 && AT_IMAGE(&img_raw, (int)(ipts1[1][0]-i-20), (int)(ipts1[1][1])) < Ostu_Thres//右边线上的第二个点，向左寻找黑白像素点
               && AT_IMAGE(&img_raw, (int)(ipts1[1][0]-i-20+1), (int)(ipts1[1][1])) >= Ostu_Thres)
       {
           zebra_R_flag = 1;
           zebraR_x=ipts1[1][0]-i-20;
       }
       if(zebra_L_flag && zebra_R_flag && zebraR_x-zebraL_x>40)//检测到赛道中间有黑色像素点，且两像素点间隔一定距离
       {
           garage_type=GARAGE_FOUND;
           break;
       }
       if((ipts1[1][0]-i)-(ipts0[1][0]+i)<5)//当检测点在x轴上距离足够近时结束循环
           break;
    }
}

void run_garage()
{
    if(garage_type==GARAGE_OUT)//出车库
    {
        Integral_vel_flag=1;
        if(total_distance>out_distance)
        {
            garage_type=GARAGE_NONE;
            Integral_vel_flag=0;
        }
    }
    if(garage_type==GARAGE_FOUND)//停车
    {
        Integral_vel_flag=1;
        if(total_distance>stop_distance)
        {
            garage_type=GARAGE_STOP;
            Integral_vel_flag=0;
        }
    }

}
