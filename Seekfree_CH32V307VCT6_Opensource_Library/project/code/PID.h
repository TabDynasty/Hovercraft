#ifndef PID_H_
#define PID_H_
#include "zf_common_headfile.h"
/*---------------------PID各项系数数组--------------------*/

extern int Angle[4],Angle_vel[4],Speed[4];

/*-------------------PID各项误差结构体定义初始化--------------*/
typedef struct PID
{
    float SumError;     /* 累计误差                     */
    float LastError;    /* 上次误差                     */
    int32 PrevError;    /* 前次误差                     */
    int32 LastData;     /* 上次数据                     */
} PID;

typedef struct _PID_Quick
{
    int32 SumError;     //误差累计
    int32 LastError;    //Error[-1]
    int32 PrevError;    //Error[-2]
    int32 LastData;     //Speed[-1]
} PID_Quick;

extern PID  Angle_PID,Angle_vel_PID,Speed_PID;
/*================================ 接口函数 ==================================*/
void PID_Init(void);
void PID_Parameter_Init(PID *sptr);
float PID_Realize(PID *sptr, int *PID, float NowData, float Point);
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point);
int32 PID_Increase_Quick(PID_Quick *sptr, int32 *PID, int32 NowData, int32 Point);
int32 PID_Realize_Quick(PID_Quick *sptr, int32 *PID_Val, int32 NowData, int32 Point);

/*============================= 宏定义/重定义 ================================*/
#define KP 0
#define KI 1
#define KD 2
#define KT 3
#define KB 4
#define KF 5
#endif /* PID_H_ */
