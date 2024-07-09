#include"obstacle.h"
#include "image.h"
#include "control.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum obstacle_type_e  obstacle_type=OBSTACLE_NONE;


void check_obstacle()
{
    if(dir_rightnum0>10 && Lpt0_s_found && is_straight1 && Lpt0_s_rpts0s_id<40)
        obstacle_type=OBSTACLE_LEFT_BEGIN;
    if(dir_leftnum1>10 && Lpt1_s_found && is_straight0 && Lpt1_s_rpts1s_id<40)
        obstacle_type=OBSTACLE_RIGHT_BEGIN;
}

void run_Lobstacle()
{
    track_type = TRACK_RIGHT;
    switch (obstacle_type) {
        case OBSTACLE_LEFT_BEGIN:
            Integral_vel_flag=1;
            if(total_distance>2000){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
            }
            break;
        default:
            break;
    }
}

void run_Robstacle()
{
    track_type = TRACK_LEFT;
    switch (obstacle_type) {
        case OBSTACLE_RIGHT_BEGIN:
            Integral_vel_flag=1;
            if(total_distance>2000){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
            }
            break;
        default:
            break;
    }
}
