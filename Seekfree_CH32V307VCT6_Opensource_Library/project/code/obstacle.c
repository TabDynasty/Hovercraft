#include"obstacle.h"
#include "image.h"
#include "control.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum obstacle_type_e  obstacle_type=OBSTACLE_NONE;

int obs_Lpt_id;//40
int obs_dir_num;//5
int obs_distance;//1500
void check_obstacle()
{
    if(dir_rightnum0>=obs_dir_num && is_longstraight1&&Lpt0_s_found && Lpt0_s_rpts0s_id<obs_Lpt_id)
//    if(Lpt0_s_found && Lpt0_s_rpts0s_id<obs_Lpt_id && is_longstraight1
//            && rpts0s[clip(Lpt0_s_rpts0s_id,0,rpts0s_num-1)][0]>rpts0s[clip(Lpt0_s_rpts0s_id+5,0,rpts0s_num-1)][0])
        obstacle_type=OBSTACLE_LEFT_IN;
    if(dir_leftnum1>=obs_dir_num && is_longstraight0&&Lpt1_s_found && Lpt1_s_rpts1s_id<obs_Lpt_id)
//    if(Lpt1_s_found && Lpt1_s_rpts1s_id<obs_Lpt_id && is_longstraight0
//            && rpts1s[clip(Lpt1_s_rpts1s_id,0,rpts1s_num-1)][0]<rpts1s[clip(Lpt1_s_rpts1s_id+5,0,rpts1s_num-1)][0])
        obstacle_type=OBSTACLE_RIGHT_IN;
}

void run_Lobstacle()
{
    switch (obstacle_type) {
        case OBSTACLE_LEFT_BEGIN:
                obstacle_type=OBSTACLE_LEFT_IN;
            break;
        case OBSTACLE_LEFT_IN:
            track_type = TRACK_RIGHT;
            Integral_vel_flag=1;
            if(total_distance>obs_distance){
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
    switch (obstacle_type) {
        case OBSTACLE_RIGHT_BEGIN:
                obstacle_type=OBSTACLE_RIGHT_IN;
            break;
        case OBSTACLE_RIGHT_IN:
            track_type = TRACK_LEFT;
            Integral_vel_flag=1;
            if(total_distance>obs_distance){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
            }
            break;
        default:
            break;
    }
}
