/*================================ 全局变量 ==================================*/
#include "control.h"
#include "image.h"
#include "utils.h"
#include "garage.h"
#include "cross.h"
#include "Motor.h"
#include "zf_common_headfile.h"
enum garage_type_e garage_type = GARAGE_NONE;

void run_garage()
{
    if(garage_type==GARAGE_OUT)//出车库
    {
        Integral_vel_flag=1;
        if(total_distance>2500)
        {
            garage_type=GARAGE_NONE;
            Integral_vel_flag=0;
        }
    }
    if(garage_type==GARAGE_FOUND)//停车
    {

        Integral_vel_flag=1;

    }

}
