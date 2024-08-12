#include"obstacle.h"
#include "image.h"
#include "control.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum obstacle_type_e  obstacle_type=OBSTACLE_NONE;

int obs_Lpt_id;//40
int obs_dir_num;//10
int obs_distance;//2000
void check_obstacle()
{
    if(dir_rightnum0>obs_dir_num && Lpt0_s_found && is_straight1 && Lpt0_s_rpts0s_id<obs_Lpt_id)
        obstacle_type=OBSTACLE_LEFT_BEGIN;
    if(dir_leftnum1>obs_dir_num && Lpt1_s_found && is_straight0 && Lpt1_s_rpts1s_id<obs_Lpt_id)
        obstacle_type=OBSTACLE_RIGHT_BEGIN;
}

void run_Lobstacle()
{
    switch (obstacle_type) {
    track_type = TRACK_RIGHT;
        case OBSTACLE_LEFT_BEGIN:
            Integral_vel_flag=1;
            if(total_distance>obs_distance){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
                element_num++;
            }
            break;
        default:
            break;
    }
}

void run_Robstacle()
{
    switch (obstacle_type) {
    track_type = TRACK_LEFT;
        case OBSTACLE_RIGHT_BEGIN:
            Integral_vel_flag=1;
            if(total_distance>obs_distance){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
                element_num++;
            }
            break;
        default:
            break;
    }
}
