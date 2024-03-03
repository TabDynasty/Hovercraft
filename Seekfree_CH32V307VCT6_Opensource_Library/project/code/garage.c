/*================================ 全局变量 ==================================*/
#include "control.h"
#include "image.h"
#include "utils.h"
#include "garage.h"
#include "cross.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum garage_type_e garage_type = GARAGE_NONE;
void run_garage(){
    if(garage_type==GARAGE_FOUND_LEFT)
    {
        track_type=TRACK_LEFT;
        Integral_vel_flag=1;
    }

    if(garage_type==GARAGE_OUT_RIGHT)//出车库固定打角
    {
        Integral_vel_flag=1;
        if(total_distance>2500)
        {
            garage_type=GARAGE_NONE;
            Integral_vel_flag=0;
        }
    }
    if(garage_type==GARAGE_FOUND_RIGHT)//入右车库，先寻右近线
    {
        track_type=TRACK_RIGHT;
        check_Right_Cross();
        if(Lpt1_found==0)
        {
            farline_type=1;
            check_Left_Cross();
            Integral_vel_flag=1;


        }
    }

}
