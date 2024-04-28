#include "cross.h"
#include "circle.h"
#include "control.h"
#include "image.h"
#include "utils.h"
#include "obstacle.h"
#include "Motor.h"
#include "zf_common_headfile.h"
/*================================ 全局变量 ==================================*/
enum circle_type_e circle_type = CIRCLE_NONE;
extern image_t img_raw;
int broadcast_flag=1;
bool circle_obstacle_flag=0;//圆环障碍共用的一套标志位
int none_left_line = 0, none_right_line = 0;
int have_left_line = 0, have_right_line = 0;

/*================================ 接口函数 ==================================*/
void check_Left_Circle();
void check_Right_Circle();
/******************************************************************************
* FunctionName   : check_Lcircle_Lobstacle()
* Description    : 判断左圆环,左障碍
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Lcircle_Lobstacle()
{
    if(circle_obstacle_flag == 0 && Lpt0_found && !Lpt1_found && is_straight1 && Lpt0_rpts0s_id<40)//左边长直道，右边近角点
    {
        circle_obstacle_flag=1;
    }
    if(circle_obstacle_flag==1)
    {
          if(dir_rightnum0>10)obstacle_type=OBSTACLE_LEFT_BEGIN;
          else
          {
              check_Left_Cross();
              if(far_conf0_max<30&&far_conf0_max>3)//圆环标志，远线是弧线，角度最大不超过30，但判断太过严格，容易判不到
              {
                  circle_type = CIRCLE_LEFT_BEGIN;
                  none_left_line = 0;
                  have_left_line = 0;
              }
          circle_obstacle_flag=0;
          }
    }
}

/******************************************************************************
* FunctionName   : check_Rcircle_Robstacle()
* Description    : 判断右圆环，右障碍
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void check_Rcircle_Robstacle()
{
    if (circle_obstacle_flag == 0 && Lpt1_found && is_straight0 && Lpt1_rpts1s_id<40) //左边长直道，右边近角点
    {
        circle_obstacle_flag=1;

    }
    if(circle_obstacle_flag==1)
       {
            if(dir_leftnum1>10)obstacle_type=OBSTACLE_RIGHT_BEGIN;
            else
            {
                check_Right_Cross();
                if(far_conf1_max<30&&far_conf1_max>3)//圆环标志，远线是弧线，角度最大不超过30，但判断太过严格，容易判不到
                {
                    circle_type = CIRCLE_RIGHT_BEGIN;
                    none_right_line = 0;
                    have_right_line = 0;
                }

            }
            circle_obstacle_flag=0;
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
    check_Lcircle_Lobstacle();
    check_Rcircle_Robstacle();
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
                if (have_left_line >= 1 && !Lpt0_found)
                {
                    circle_type = CIRCLE_LEFT_IN;
                    none_left_line = 0;
                    have_left_line = 0;
                }
                break;
            case CIRCLE_LEFT_IN:
                track_type = TRACK_RIGHT;
                check_Left_Cross();
                if(far_Lpt0_found&&ipts0_num<120)
                {
                    //check_Right_Cross();
                    Integral_vel_flag=1;
                    farline_type=1;//切寻远线
                }
                if(total_distance>3000)
                {
                    circle_type = CIRCLE_LEFT_RUNNING;
                    farline_type=0;
                    none_right_line = 0;
                    Integral_vel_flag=0;
                }
                break;
            case CIRCLE_LEFT_RUNNING:
                track_type = TRACK_RIGHT;
                if (Lpt0_found) rpts0s_num = rptsc0_num = Lpt0_rpts0s_id-10;//截断
                begin_y=110;//近线起始点拉低，防止丢线
                if (Lpt1_found && Lpt1_rpts1s_id < 50)//右角点足够靠下
                {
                    circle_type = CIRCLE_LEFT_OUT;
                    begin_y=96;
                }

                break;
            case CIRCLE_LEFT_OUT:
                track_type = TRACK_LEFT;
                if (is_straight1)
                    circle_type = CIRCLE_LEFT_END;
                break;

            case CIRCLE_LEFT_END:
                track_type = TRACK_RIGHT;
                Integral_vel_flag=1;
                begin_y=110;//近线起始点拉低，防止丢线
                if (total_distance>=3000)
                {
                    aim_distance=440;
                    circle_type = CIRCLE_NONE;
                    begin_y=96;
                    Integral_vel_flag=0;
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
                if (have_right_line > 1 && !Lpt1_found)
               {
                   circle_type = CIRCLE_RIGHT_IN;
                   none_right_line = 0;
                   have_right_line = 0;
               }
                break;
                //寻右线，左线丢线后有线
            case CIRCLE_RIGHT_IN:
                track_type = TRACK_LEFT;
                check_Right_Cross();

                if(far_Lpt1_found&&ipts1_num<120)
                {
                    //check_Left_Cross();
                    Integral_vel_flag=1;
                    farline_type=1;//切寻远线
                }
                if(total_distance>3000)//左边近处线寻到
                {
                    circle_type = CIRCLE_RIGHT_RUNNING;
                    farline_type=0;
                    none_left_line = 0;
                    Integral_vel_flag=0;
                }
                break;
            case CIRCLE_RIGHT_RUNNING:
                track_type = TRACK_LEFT;
                if (Lpt1_found) rpts1s_num = rptsc1_num = Lpt1_rpts1s_id-10; //截断
                begin_y=110;//近线起始点拉低，防止丢线
                if (Lpt0_found && Lpt0_rpts0s_id < 50)//左角点足够靠下
                {
                    circle_type = CIRCLE_RIGHT_OUT;
                    begin_y=96;
                }
                break;

            case CIRCLE_RIGHT_OUT:
                track_type = TRACK_RIGHT;
                if (is_straight0)
                    circle_type = CIRCLE_RIGHT_END;
                break;
            case CIRCLE_RIGHT_END:
                track_type = TRACK_LEFT;
                Integral_vel_flag=1;
                begin_y=110;//近线起始点拉低，防止丢线
                if (total_distance>3000)
                {
                    aim_distance=440;
                    circle_type = CIRCLE_NONE;
                    begin_y=96;
                    Integral_vel_flag=0;
                }
                break;
        }

}
