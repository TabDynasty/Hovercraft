#ifndef CONTROL_H_
#define CONTROL_H_
#include "zf_common_headfile.h"
enum track_type_e {
    TRACK_LEFT,
    TRACK_RIGHT,
};
extern uint32 aimSpeed;/**< 目标速度*/
extern uint32 Speed_straight;
extern uint32 Speed_instraight;
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
#endif /* CONTROL_H_ */
