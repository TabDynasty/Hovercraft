#include "zf_common_headfile.h"
#include "motor.h"
#include "control.h"
#include "garage.h"
#include "obstacle.h"
#include "PID.h"
#include "utils.h"
#include "filters.h"
#include "image.h"
#include "straight_road.h"
#include "circle.h"
/*=============================  电机引脚定义  ================================*/
#define PWM_UP_PIN         TIM4_PWM_MAP1_CH1_D12
#define PWM_DOWN_PIN       TIM4_PWM_MAP1_CH3_D14
#define PWM_1_PIN          TIM5_PWM_MAP0_CH1_A0
#define PWM_2_PIN          TIM4_PWM_MAP1_CH4_D15
#define PWM_3_PIN          TIM5_PWM_MAP0_CH2_A1
#define PWM_4_PIN          TIM4_PWM_MAP1_CH2_D13
///*============================= 4路电机的起转pwm值  ================================*
#define MOTOR_PWM_START      560
/*================================ 全局变量 ==================================*/
SPEED_st Motor;          /* 电机结构体*/
//标志位
bool Integral_vel_flag = 0;
bool motorflag=0;

int centripetal_p_straight,centripetal_p_instraight = 0;
int Speed_now = 0;
int total_distance = 0;
int aim_signal = 0;
int angle_thred1;
int angle_thred2;
int anti_coefficient;
int break_coefficient;  //刹车系数
int max_output;//最终输出限制幅度
int circle_slow;//圆环降速
int start_pwm;
int bottom_Speed_Max;
int bottom_Speed_Min;
int bottom_slow;
int speed_up_conf;
int slow_down_conf;
int ang_gain;
int vel_gain;

float imu_data;
extern bool slow_start_flag;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度设置，在中断调用
// 参数说明     void
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void Speed_Set(void)
{
    ang_gain = Stable_posture(angle,mpu6050_gyro_z);
    vel_gain = Vertical_circle(aimSpeed, Speed_now);
    float centripetal_gain = (float)Speed_now  * abs((int)pure_angle)/1000;

    //fabs((float)Side_circle(pure_angle,0));
//    debug_show_int("ang", ang_gain, 1);
//    debug_show_int("vel", vel_gain, 3);
//    debug_show_int("sid", centripetal_gain, 5);
    //控制方向的4个风扇， 分别进行速度和角度的闭环
    Motor_Set(vel_gain, ang_gain,  centripetal_gain);
    //上下两个风扇,船浮起来
    if(slow_start_flag == true)
       {
        pwm_set_duty(PWM_UP_PIN,   start_pwm);
        pwm_set_duty(PWM_DOWN_PIN, start_pwm);
       }else{
        pwm_set_duty(PWM_UP_PIN,   Motor.PWM_fan_up);
        pwm_set_duty(PWM_DOWN_PIN, Motor.PWM_fan_down);
       }
}
 //-------------------------------------------------------------------------------------------------------------------
// 函数简介     6路无刷电调以及电机各项参数初始化
// 参数说明     void
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void Motor_Init(void)
{
    //气垫船浮起所需的pwm

    pwm_init(PWM_UP_PIN,    MOTOR_FREQ, INIT_PWM);
    pwm_init(PWM_DOWN_PIN,  MOTOR_FREQ, INIT_PWM);
    pwm_init(PWM_1_PIN,     MOTOR_FREQ, INIT_PWM);
    pwm_init(PWM_2_PIN,     MOTOR_FREQ, INIT_PWM);
    pwm_init(PWM_3_PIN,     MOTOR_FREQ, INIT_PWM);
    pwm_init(PWM_4_PIN,     MOTOR_FREQ, INIT_PWM);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     4路控制姿态的pwm设置函数
// 参数说明     pwmn 对应引脚的pwm输入值
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
extern float power_level;
void Motor_Set(int speed, int spin ,float force)
{

    int pwm1=0,pwm2=0,pwm3=0,pwm4=0;
    static float power_conf;
    /******给速度用**********/
    if(speed>=0)
    {
        pwm3+=speed;
        pwm4+=speed;
    }else{
        pwm1+=speed;
        pwm2+=speed;
    }
    /*********转向用***********/
    if(spin>=0){
        pwm1+=spin;
        pwm4+=spin;
    }else {
        pwm2+=-1*spin;
        pwm3+=-1*spin;
    }
    /**************给侧推防止漂移用*******************/
    //弯道防甩出去
        if(angle> 0){
            pwm1+=force * centripetal_p_instraight;
            pwm3+=force * centripetal_p_instraight*anti_coefficient/100; //过弯由于电机线性差会加速，给侧推时候将后面电机乘以一个衰减系数

        }
        if(angle< 0){
            pwm2+=force * centripetal_p_instraight;
            pwm4+=force * centripetal_p_instraight*anti_coefficient/100;//过弯由于电机线性差会加速，给侧推时候将后面电机乘以一个衰减系数
        //}
    }

    /*********************直道提速****************************/
    if(straight_road_type == STRAIGHT_IN)
    {
        pwm3  +=(Speed_long_straight-Speed_now) * speed_up_conf/10;
        pwm4  +=(Speed_long_straight-Speed_now) * speed_up_conf/10;
    }
    /*********************直道入弯****************************/
    if(straight_road_type == STRAIGHT_OUT1 )
    {
        pwm1  +=Speed_now * break_coefficient/10;
        pwm2  +=Speed_now * break_coefficient/10;
    }


    /*********************圆环减速****************************/
    if((circle_type == CIRCLE_LEFT_BEGIN && none_left_line == 0)||( circle_type == CIRCLE_RIGHT_BEGIN&& none_right_line == 0 ))
    {
        pwm1  += circle_slow;
        pwm2  += circle_slow;
    }

    if( garage_type == GARAGE_FOUND)
       {
           pwm1  =200;
           pwm2  =200;
           pwm3 = 0;
           pwm4 = 0;

       }
    /***********************电池电压补偿*************************/
    power_conf = 12.8/power_level;
    pwm1 *= power_conf;
    pwm2 *= power_conf;
    pwm3 *= power_conf;
    pwm4 *= power_conf;
    /*********************限幅防止越界***************************/
    if(pwm1 > max_output)
        pwm1 = max_output;
    if(pwm2 > max_output)
        pwm2 = max_output;
    if(pwm3 > max_output)
        pwm3 = max_output;
    if(pwm4 > max_output)
        pwm4 = max_output;

    if(pwm1 <= 0)
        pwm1 = 0;
    if(pwm2 <=  0)
        pwm2 =  0;
    if(pwm3 <= 0)
        pwm3 = 0;
    if(pwm4 <= 0)
        pwm4 = 0;
    /***********************缓启动*******************************/
    if(slow_start_flag == true)
    {
        pwm1 = 0;
        pwm2 = 0;
        pwm3 = 0;
        pwm4 = 0;
    }


    /**********************最终赋值用*************************/
        pwm_set_duty(PWM_1_PIN, MOTOR_PWM_START+pwm1);
        pwm_set_duty(PWM_2_PIN, MOTOR_PWM_START+pwm2);
        pwm_set_duty(PWM_3_PIN, MOTOR_PWM_START+pwm3);
        pwm_set_duty(PWM_4_PIN, MOTOR_PWM_START+pwm4);

}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     内环角速度环，用于稳定姿态
// 参数说明     aim_angle_vel      目标角速度
// 参数说明     imu_angle_vel_data  陀螺仪当前测得的角速度值
// 返回参数     spin_Increment     加在对角两个电机上的pwm值，产生一个用于转向的力矩
//-------------------------------------------------------------------------------------------------------------------
extern float off_setz;

int Stable_posture(float aim_angle_vel, int imu_angle_vel_data)
{
    int increment_max = 300;
    int spin_Increment;
    imu_data = mpu6050_gyro_transition(imu_angle_vel_data-off_setz);
  if(fabs(imu_data) >5)
   {
       imu_data = LowPass_Filter(&imu_dataz,(float)imu_data); //对采集到的imu值进行滤波
   }else{
       imu_data = 0;
   }
    //debug_show_float("imuz",data,0);

    if(abs(pure_angle)<angle_thred2)
        {
            Angle_vel_PID.SumError = 0;
            spin_Increment =(int)PID_Realize_Inner(&Angle_vel_PID, Angle_vel, imu_data,aim_angle_vel);   //pid内环
        }else{
            spin_Increment =(int)PID_Realize_Inner(&Angle_vel_PID, Angle_vel_vel, imu_data,aim_angle_vel);   //pid内环
        }
    //输出限幅
    if(spin_Increment > increment_max)
        spin_Increment = increment_max;
    if(spin_Increment < -increment_max)
        spin_Increment = -increment_max;

    return spin_Increment;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度环
// 参数说明     aim_vel      目标速度
// 参数说明     now_vel      当前速度值
// 返回参数     vel_Increment 输出到前面两个或者后面两个电机上的pwm值
//-------------------------------------------------------------------------------------------------------------------
int Vertical_circle(int aim_vel, int now_vel)
{
    int increment_max = 300;

     int vel_Increment = (int)PID_Realize(&Speed_PID, Speed, (float)now_vel, (float)aim_vel);
     //输出限幅
     if(vel_Increment > increment_max)
         vel_Increment = increment_max;
     if(vel_Increment < -increment_max)
         vel_Increment = -increment_max;
     return vel_Increment;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     侧推环
// 参数说明     pure_angle      图像误差
// 参数说明     aim_angle       目标角度（将错就错为零）
// 返回参数     side_Increment  输出到侧边两个电机上的pwm值
//-------------------------------------------------------------------------------------------------------------------
int Side_circle(int now_angle, int aim_angle)
{
    int increment_max = 300;

     int side_Increment = (int)PID_Realize(&Side_PID, Side, now_angle, aim_angle);
     //输出限幅
     if(side_Increment > increment_max)
         side_Increment = increment_max;
     if(side_Increment < -increment_max)
         side_Increment = -increment_max;
     return side_Increment;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取当前速度，可选择对其积分
// 参数说明     void
// 返回参数     void
// 使用示例     pit_speed();
//-------------------------------------------------------------------------------------------------------------------
void pit_speed(void)
{
    Speed_now = -encoder_get_count(TIM3_ENCOEDER);                              // 获取编码器计数
    encoder_clear_count(TIM3_ENCOEDER);                                        // 清空编码器计数
    //flag置为1时，开始积分
    if(!Integral_vel_flag){
        total_distance = 0;}
    else{
        total_distance += Speed_now;
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     停车关闭所有电机，在中断调用
// 参数说明     void
// 返回参数     void
// 使用示例     stop_set();
//-------------------------------------------------------------------------------------------------------------------
void Stop_Set(void)
{
    pwm_set_duty(PWM_UP_PIN,   INIT_PWM);
    pwm_set_duty(PWM_DOWN_PIN, INIT_PWM);
    pwm_set_duty(PWM_1_PIN, INIT_PWM);
    pwm_set_duty(PWM_2_PIN, INIT_PWM);
    pwm_set_duty(PWM_3_PIN, INIT_PWM);
    pwm_set_duty(PWM_4_PIN, INIT_PWM);
}
