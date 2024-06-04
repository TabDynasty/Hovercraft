#include "straight_road.h"
#include "image.h"
#include "control.h"
#include "zf_common_headfile.h"
#include "Motor.h"
enum straigh_troad_type_e straight_road_type = STRAIGHT_NONE;

static uint8 check_straight = 0;//用于计直道标志的帧数
static uint8 check_bend = 0; //用于计弯道标志的帧数
static uint8 check_angle = 0; //用于计角度标志的帧数
int check_straight_thred;//8
int check_angle_thred;//2
int check_bend_thred;//3
int break_dis;
void check_straight_road(void)
{

    if(bend_flag == false)
    {
        check_straight++;
    }
    if(pure_angle < angle_thred1 )
        {
            check_angle++;
        }
    if(check_straight>check_straight_thred&& check_angle > check_angle_thred)
    {
        check_straight = 0;
        check_angle = 0;
        straight_road_type = STRAIGHT_IN;
    }
}

void Run_straight(void)
{
    switch (straight_road_type) {
        case STRAIGHT_IN:
            if(bend_flag == true)
            {
                check_bend++;
            }
            //当直道检测到弯道标志的时候，进入一个小的弯道预判断状态机，即在此状态机中一旦有直道标志出现，就将弯道标志清零，防止误判
            if(check_bend > 0)
            {
                if(bend_flag == false)
                {
                    check_straight++;
                }
                if(check_straight>0)
                {
                    check_bend = 0;
                    check_straight = 0;
                }
            }
            //出直道
            if(check_bend > check_bend_thred )
            {
                straight_road_type = STRAIGHT_OUT;
            }
            break;
            //反向推进弯道减速
        case STRAIGHT_OUT:
            Integral_vel_flag = 1;
            if(total_distance > break_dis)
            {
                straight_road_type = STRAIGHT_NONE;
                Integral_vel_flag = 0;
            }
            break;
        default:
            break;
    }
}
