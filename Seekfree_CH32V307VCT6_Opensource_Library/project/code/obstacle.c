#include"obstacle.h"
#include "image.h"
#include "control.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum obstacle_type_e  obstacle_type=OBSTACLE_NONE;
void run_obstacle()
{
    switch (obstacle_type) {
        case OBSTACLE_BEGIN:
            Count_ang_Flag=1;
            if(total_angel_z>35)//积分值改动调整过路障的路径
            {
                obstacle_type=OBSTACLE_OUT;
            }
            break;

        case OBSTACLE_IN:
//            if(ipts0_num>20||ipts1_num>20)
//                obstacle_type = OBSTACLE_NONE;
            if(total_distance>3000)//积分值改动调整过路障的路径
                {
                obstacle_type=OBSTACLE_OUT;
                }
            break;

        case OBSTACLE_OUT:
            if(total_angel_z<-25&&(ipts0_num>20||ipts1_num>20))//积分值改动调整过路障的路径
                {
                obstacle_type=OBSTACLE_NONE;
                tof_flag=1;
                Integral_vel_flag=0;
                Count_ang_Flag=1;
                }
            break;
        default:
            break;
    }
}
