#ifndef MOTOR_H_
#define MOTOR_H_

#include "zf_common_headfile.h"
#include "isr.h"
#include "PID.h"

//
//              @---@                  @---@
//              | 3 |                  | 1 |
//              @---@                  @---@                ^
//                                                         ^ ^
//4                                                       ^ | ^
//3                         up                              |   前     进
//down                      down                            |
//up                                                        |   方     向
//1                                                         |
//2             @---@                  @---@                |
//              | 4 |                  | 2 |                |
//              @---@                  @---@               /|\
//                                                         /|\
//                                                         /|\

/*============================= 宏定义/重定义 ================================*/
#define INIT_PWM   485                             //电调初始化所需pwm值
#define MOTOR_FREQ 50                              //电机控制频率
/*============================= 联合体/结构体 ================================*/
typedef struct {

        //六路风扇分别输出的PWM值
        int32 PWM_fan_up   ;
        int32 PWM_fan_down ;

        int32 nowspeed;
        int32 aimspeed;
}SPEED_st;

/*================================ 全局变量 ==================================*/
extern SPEED_st Motor;/**< 速度结构体*/
extern bool motorflag;
extern bool Integral_vel_flag;
extern int speed_now;
extern int centripetal_p_straight,centripetal_p_instraight;
extern int total_distance;
extern int aim_signal;
extern int anti_coefficient;
extern int break_coefficient;  //刹车系数
extern int max_output;//最终输出限制幅度
extern int circle_slow;//圆环降速
extern int start_pwm;
extern int bottom_Speed_Max;
extern int bottom_Speed_Min;
extern int angle_thred1;
extern int angle_thred2;
extern int speed_up_conf;
extern int slow_down_conf;
extern int Speed_now ;
extern int ang_gain,vel_gain;
#endif /* MOTOR_H_ */
/*================================ 接口函数 ==================================*/
void Motor_Init(void);
void Speed_Set(void);
void Motor_Set(int speed, int spin, float force);
int Stable_posture(float aim_angle_vel, int imu_anglevel_data);
int Vertical_circle(int aim_vel, int now_vel);
void pit_speed(void);
void Stop_Set(void);
