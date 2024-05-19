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
/*=============================  电机引脚定义  ================================*/
#define PWM_UP_PIN         TIM4_PWM_MAP1_CH4_D15
#define PWM_DOWN_PIN       TIM4_PWM_MAP1_CH1_D12
#define PWM_1_PIN          TIM4_PWM_MAP1_CH2_D13
#define PWM_2_PIN          TIM5_PWM_MAP0_CH2_A1
#define PWM_3_PIN          TIM5_PWM_MAP0_CH1_A0
#define PWM_4_PIN          TIM4_PWM_MAP1_CH3_D14
///*============================= 4路电机的起转pwm值  ================================*
#define MOTOR_PWM_START      540
/*================================ 全局变量 ==================================*/
SPEED_st Motor;          /* 电机结构体*/
//标志位
bool Integral_vel_flag = 0;
bool motorflag=0   ;

int centripetal_p_straight,centripetal_p_instraight = 0;
int Speed_now = 0;
int total_distance = 0;
int aim_signal = 0;
int angle_thred;
int anti_coefficient;
int break_coefficient;  //刹车系数
extern bool slow_start_flag;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     速度设置，在中断调用
// 参数说明     void
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void Speed_Set(void)
{
    int ang_gain = Stable_posture(angle,mpu6050_gyro_z);
    int vel_gain = Vertical_circle(aimSpeed, Speed_now);
    float centripetal_gain = (float)Speed_now * abs((int)pure_angle)/1000;
    debug_show_int("ang", ang_gain, 1);
    debug_show_int("vel", vel_gain, 3);
    //控制方向的4个风扇， 分别进行速度和角度的闭环
    Motor_Set(vel_gain, ang_gain, centripetal_gain);
    //上下两个风扇,船浮起来
    if(slow_start_flag == true)
       {
        pwm_set_duty(PWM_UP_PIN,   620);
        pwm_set_duty(PWM_DOWN_PIN, 620);
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

    Motor.PWM_fan_up = 590;
    Motor.PWM_fan_down = 590;
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
void Motor_Set(int speed, int spin ,float force)
{

    int pwm1=0,pwm2=0,pwm3=0,pwm4=0;
    /******给速度用**********/
    if(speed>=0)
    {
        pwm3+=speed;
        pwm4+=speed;
    }else{
        pwm1+=speed;
        pwm2+=speed;
    }
    //*********转向用
    if(spin>=0){
        pwm1+=spin;
        pwm4+=spin;
    }else {
        pwm2+=-spin;
        pwm3+=-spin;
    }
    /**************给侧推防止漂移用*******************/
    if(is_straight0 == 1 && is_straight1 == 1)//直道防侧滑
    {
        if(angle> angle_thred){
            pwm1+=force * centripetal_p_straight;
            pwm3+=force * centripetal_p_straight;
        }
        if(angle<angle_thred * (-1)){
            pwm2+=force * centripetal_p_straight;
            pwm4+=force * centripetal_p_straight;
        }
    }else{                                    //弯道防甩出去
        if(angle> angle_thred){
            pwm1+=force * centripetal_p_instraight;
            pwm3+=force * centripetal_p_instraight*anti_coefficient/100; //过弯由于电机线性差会加速，给侧推时候将后面电机乘以一个衰减系数

        }
        if(angle<angle_thred * (-1)){
            pwm2+=force * centripetal_p_instraight;
            pwm4+=force * centripetal_p_instraight*anti_coefficient/100;//过弯由于电机线性差会加速，给侧推时候将后面电机乘以一个衰减系数
        }
    }
    /*********************直道入弯****************************/
    if(straight_road_type == STRAIGHT_OUT)
    {
        pwm1  +=Speed_now * break_coefficient/10;
        pwm2  +=Speed_now * break_coefficient/10;
    }
    /*********************限幅防止越界***************************/
    if(pwm1 > 250)
        pwm1 = 250;
    if(pwm2 > 250)
        pwm2 = 250;
    if(pwm3 > 250)
        pwm3 = 250;
    if(pwm4 > 250)
        pwm4 = 250;

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
    int increment_max = 200;
    float data = mpu6050_gyro_transition(imu_angle_vel_data-off_setz);
    data = LowPass_Filter(&imu_dataz,(float)data); //对采集到的imu值进行滤波

    debug_show_float("imuz",data,0);

    int spin_Increment =(int)PID_Realize_Inner(&Angle_vel_PID, Angle_vel, data,aim_angle_vel);   //pid内环

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
    int increment_max = 100;

     int vel_Increment = (int)PID_Realize(&Speed_PID, Speed, (float)now_vel, (float)aim_vel);
     //输出限幅
     if(vel_Increment > increment_max)
         vel_Increment = increment_max;
     if(vel_Increment < -increment_max)
         vel_Increment = -increment_max;
     return vel_Increment;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取当前速度，可选择对其积分
// 参数说明     void
// 返回参数     void
// 使用示例     pit_speed();
//-------------------------------------------------------------------------------------------------------------------
void pit_speed(void)
{
    Speed_now = encoder_get_count(TIM3_ENCOEDER);                              // 获取编码器计数
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
