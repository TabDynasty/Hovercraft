#ifndef CONTROL_H_
#define CONTROL_H_
#include "zf_common_headfile.h"

/*================================ 接口函数 ==================================*/
void control_Init();
void check_all();

/*============================= 结构体联合体定义 =============================*/
enum track_type_e {
    TRACK_LEFT,
    TRACK_RIGHT,
};

/*================================ 全局变量 ==================================*/
extern uint32 aimSpeed;/**< 目标速度*/
extern int Speed_long_straight,Speed_short_straight,Speed_instraight,Speed_circle;
extern int times;
extern uint32 aim_distance;//除了1000
extern uint32 AIM_DISTANCE;/**< 直接用于计算*/
extern int aim_idx;
extern float angle;
extern enum track_type_e track_type;
extern uint32 aim_distance;
extern uint32 chasu_bili;
extern enum track_type_e track_type;
extern float cx;
extern float cy;
extern float error;
extern float pure_angle;
extern int upError;
extern float (*rpts)[2];/**< 中线*/
extern int rpts_num;
extern float rptsn[MT9V03X_W][2];/**< 归一化中线*/
extern int rptsn_num;
extern int obs_dx;

extern int circle_num,circle_switch,garage_switch;
extern int element_switch,element_num;
extern int element[20];

#endif /* CONTROL_H_ */
