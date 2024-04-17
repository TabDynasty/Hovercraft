#include"obstacle.h"
#include "image.h"
#include "control.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum obstacle_type_e  obstacle_type=OBSTACLE_NONE;
void run_Lobstacle()
{
    track_type = TRACK_RIGHT;
    switch (obstacle_type) {
        case OBSTACLE_LEFT_BEGIN:
            Integral_vel_flag=1;
            if(total_distance>1800){
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
            if(total_distance>1800){
                obstacle_type=OBSTACLE_NONE;
                Integral_vel_flag=0;
            }
            break;
        default:
            break;
    }
}
