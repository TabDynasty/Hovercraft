#include "cross.h"
#include "circle.h"
#include "control.h"
#include "image.h"
#include "utils.h"
#include "garage.h"
#include "Motor.h"
#include "zf_common_headfile.h"
/*================================ 全局变量 ==================================*/
enum circle_type_e circle_type = CIRCLE_NONE;
extern image_t img_raw;
int broadcast_flag=1;

int none_left_line = 0, none_right_line = 0;
int have_left_line = 0, have_right_line = 0;

/*================================ 接口函数 ==================================*/
void check_Left_Circle();
void check_Right_Circle();
/******************************************************************************
* FunctionName   : check_Lcircle_Lgarage()
* Description    : 判断左圆环,左车库
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Lcircle()
{
    if(circle_type == CIRCLE_NONE &&garage_type==GARAGE_NONE && Lpt0_found && !Lpt1_found && is_straight1&&Lpt0_rpts0s_id<25)//左边长直道，右边近角点
    {
        circle_type=1;
    }
    if(circle_type==1)
    {
            check_Left_Cross();
            if(far_conf0_max<20&&far_conf0_max>3)//圆环标志，远线是弧线，角度最大不超过20，但判断太过严格，容易判不到
          {
            circle_type = CIRCLE_LEFT_BEGIN;
            aim_distance=380;
            none_right_line = 0;
            have_right_line = 0;
          }
            circle_type=0;
    }
}

/******************************************************************************
* FunctionName   : check_Rcircle_Rgarage()
* Description    : 判断右圆环，右车库
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Rcircle()
{
    if (circle_type == CIRCLE_NONE &&garage_type==GARAGE_NONE&& Lpt1_found && is_straight0&&Lpt1_rpts1s_id<40) //左边长直道，右边近角点
    {
        circle_type=1;

    }
    if(circle_type==1)
       {
            check_Right_Cross();//搜远线
            if(far_conf1_max<40&&far_conf1_max>0)//圆环标志，远线是弧线，角度最大不超过20，但判断太过严格，容易判不到
            {
            circle_type = CIRCLE_RIGHT_BEGIN;
            none_right_line = 0;
            have_right_line = 0;

            }
            circle_garage_type=0;
//                        if(c_g_ciecleflag==0)
//                        {
//                        circle_type = CIRCLE_RIGHT_BEGIN;
//                        c_g_ciecleflag=1;
//                        }
//                        if(c_g_ciecleflag==1)
//                        {
//                            c_g_ciecleflag=2;
//                            circle_type = CIRCLE_RIGHT_BEGIN;
//                        }
       }
}
/******************************************************************************
* FunctionName   : check_circle()
* Description    : 判断圆环,车库的各类标志位
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_circle()
{
    check_Lcircle();
    check_Rcircle();
}

/******************************************************************************
* FunctionName   : run_Lcircle()
* Description    : 执行左圆环
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void run_Lcircle()
{
    switch (circle_type)
        {
            case CIRCLE_LEFT_BEGIN:
                track_type = TRACK_RIGHT;
                //先丢左线后识别到左线转换到下一个阶段
                if (rpts0s_num < 0.2 / sample_dist&&!Lpt0_found) { none_left_line++; have_left_line = 0;}
                if (rpts0s_num > 0.2 / sample_dist && none_left_line > 2)have_left_line++;
                if (have_left_line > 1)
                {
                    circle_type = CIRCLE_LEFT_IN;
                    none_left_line = 0;
                    have_left_line = 0;
                }
                break;
            case CIRCLE_LEFT_IN:
                track_type = TRACK_LEFT;
                if(rpts1s_num < 0.2 / sample_dist)none_right_line++;
                if(none_right_line>3)//调整none_line可以改变响应时间
                {
                    check_Right_Cross();
                    farline_type=1;//切寻远线
                }
                if(rpts1s_num >15&&none_right_line>2)//右边近处线寻到
                {
                    circle_type = CIRCLE_LEFT_RUNNING;
                    none_right_line = 0;
                }
                break;
            case CIRCLE_LEFT_RUNNING:
                track_type = TRACK_RIGHT;
                if (Lpt1_found) rpts1s_num = rptsc1_num = Lpt1_rpts1s_id;                   //截断
                //外环拐点(右L点)
                begin_y=110;//近线起始点拉低，防止丢线
                if (Lpt1_found && Lpt1_rpts1s_id < 30)//右角点足够靠下
                {
                    circle_type = CIRCLE_LEFT_END;
                    begin_y=96;
                }

                break;
//            case CIRCLE_LEFT_OUT:
//                //check_Right_Circle();
//                track_type = TRACK_LEFT;
//                //右线为长直道
//                if (rpts1s_num<0.2/ sample_dist)
//                {
//                    circle_type = CIRCLE_LEFT_END;
//                }
//                break;

            case CIRCLE_LEFT_END:
                //track_type = TRACK_RIGHT;
                Integral_vel_flag=1;
                if (total_distance>=3800)//编码器running阶段角度出环，需要修改
                {
                    Integral_vel_flag=0;
                    circle_type = CIRCLE_NONE;
                }
                break;

        }
}

/******************************************************************************
* FunctionName   : run_Rcircle()
* Description    : 执行右圆环
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void run_Rcircle()
{
    switch (circle_type)
        {
            case CIRCLE_RIGHT_BEGIN:
                track_type = TRACK_LEFT;

                //先丢右线后识别到右线转换到下一个阶段
                if (rpts1s_num < 0.2 / sample_dist&&!Lpt1_found) { none_right_line++; have_right_line = 0;}
                if (rpts1s_num > 0.2 / sample_dist && none_right_line > 2) have_right_line++;
                if (have_right_line > 1)
               {
                   circle_type = CIRCLE_RIGHT_IN;
                   none_right_line = 0;
                   have_right_line = 0;
               }
                break;
                //寻右线，左线丢线后有线
            case CIRCLE_RIGHT_IN:
                track_type = TRACK_RIGHT;
                check_Right_Cross();

                //if(rpts0s_num < 0.2 / sample_dist) none_left_line++;

                if(far_Lpt1_found&&ipts1_num<20)//调整none_line可以改变响应时间
                {
                    check_Left_Cross();
                    Integral_vel_flag=1;
                    farline_type=1;//切寻远线
                }
                if(total_distance>2500)//左边近处线寻到
                {
                    circle_type = CIRCLE_RIGHT_RUNNING;
                    farline_type=0;
                    none_left_line = 0;
                    Integral_vel_flag=0;
                }
                break;
            case CIRCLE_RIGHT_RUNNING:
                track_type = TRACK_LEFT;
                if (Lpt1_found) rpts1s_num = rptsc1_num = Lpt1_rpts1s_id; //截断
                //外环拐点(右L点)
                begin_y=110;//近线起始点拉低，防止丢线
                if (Lpt0_found && Lpt0_rpts0s_id < 30)//左角点足够靠下
                {
                    circle_type = CIRCLE_RIGHT_OUT;
                    begin_y=96;
                }
                break;

            case CIRCLE_RIGHT_OUT:
                //track_type = TRACK_LEFT;
                 Integral_vel_flag=1;
                if (total_distance>=1000&&is_straight0)//编码器running阶段角度出环，需要修改
                {
                    Integral_vel_flag=0;
                    circle_type = CIRCLE_RIGHT_END;
                }
                break;
            case CIRCLE_RIGHT_END:
                track_type = TRACK_LEFT;
                Integral_vel_flag=1;
                begin_y=110;//近线起始点拉低，防止丢线
                if (total_distance>4000)//左角点足够靠下
                {
                    aim_distance=440;
                    circle_type = CIRCLE_NONE;
                    circle_garage_type=1;
                    begin_y=96;
                    Integral_vel_flag=0;
                }
                break;

                break;
        }

}
