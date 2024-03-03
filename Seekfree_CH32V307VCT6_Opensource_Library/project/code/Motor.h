#ifndef MOTOR_H_
#define MOTOR_H_

#include "zf_common_headfile.h"
#include "isr.h"
#include "PID.h"

//
//              @---@                  @---@
//              | 1 |                  | 2 |
//              @---@                  @---@                ^
//                                                         ^ ^
//4                                                       ^ | ^
//3                         up                              |   前     进
//down                      down                            |
//up                                                        |   方     向
//1                                                         |
//2             @---@                  @---@                |
//              | 3 |                  | 4 |                |
//              @---@                  @---@               /|\
//                                                         /|\
//                                                         /|\

/*============================= 宏定义/重定义 ================================*/
#define INIT_PWM   500                              //电调初始化所需pwm值
#define MOTOR_FREQ 50                               //电机控制频率
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
#endif /* MOTOR_H_ */
/*================================ 接口函数 ==================================*/
void Motor_Init(void);
void Speed_Set(void);
void Motor_Set(int speed, int spin);
int Stable_posture(float aim_angle_vel, int imu_anglevel_data);
int Vertical_circle(int aim_vel, int now_vel);
void pit_speed(void);
