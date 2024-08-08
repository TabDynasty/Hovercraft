#include "PID.h"
#include "control.h"
#include "utils.h"
#include "motor.h"
#include "zf_common_headfile.h"


PID Angle_PID,Angle_vel_PID,Speed_PID,Side_PID;         // 角度环， 角速度环，速度环,侧向环PID误差参数结构体
//下面的各项参数都除了100，方便flash的读取
int Angle_vel[4]       = {250, 0, 10 , 1000};    //  error大时角速度环PID系数
int Angle_vel_vel[4]       = {0,0,0,1000};       //  error小时角速度环PID系数
int Angle_0[4]           = {40, 0, 10 , 1000};    // 角度环PID弯道系数
int Angle_1[4]           = {40, 0, 10 , 1000};    // 角度环PID直道系数
int Speed[4]           = {0, 0, 0 ,   1000};    // 速度环PID系数
int Side[4]            = {100,5,0,1000};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    PID各个环初始化
// 参数说明      void
// 返回参数      void
//-------------------------------------------------------------------------------------------------------------------
void PID_Init(void)
{
    PID_Parameter_Init(&Angle_PID);
    PID_Parameter_Init(&Angle_vel_PID);
    PID_Parameter_Init(&Speed_PID);
    PID_Parameter_Init(&Side_PID);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    PID各项误差清零初始化
// 参数说明    *sptr      PID各项误差的结构体
// 返回参数      void
//-------------------------------------------------------------------------------------------------------------------
void PID_Parameter_Init(PID *sptr)
{
    sptr->SumError  = 0;
    sptr->LastError = 0;    //Error[-1]
    sptr->PrevError = 0;    //Error[-2]
    sptr->LastData  = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    位置式PID
// 参数说明    *sptr      PID各项误差的结构体
// 参数说明    *PID       PID各项系数的数组
// 参数说明      NowData    当前值
// 参数说明      Point      目标值
// 返回参数      Realize    反馈输出值
//-------------------------------------------------------------------------------------------------------------------
float PID_Realize(PID *sptr, int *PID, float NowData, float Point)
{

     float iError,   // 当前误差
          Realize;   // 最后得出的实际输出
     float kp_t,ki_t,kd_t;
     iError = Point - NowData;   // 计算当前误差


     kp_t=(float)(PID[KP]/100.0);
     kd_t=(float)(PID[KD]/100.0);
     ki_t=(float)(PID[KI]/1000.0);

     //积分限幅
     if(ki_t)
     {
         sptr->SumError +=  iError; // 误差积分
         if (sptr->SumError*ki_t >= PID[KT])
         {
             sptr->SumError = PID[KT]/ki_t;
         }
         else if (sptr->SumError*ki_t <= -PID[KT])
         {
             sptr->SumError = -PID[KT]/ki_t;
         }
     }
     Realize = kp_t * iError
             + ki_t * sptr->SumError
             + kd_t * (iError - sptr->LastError);

     sptr->LastError = iError;           // 更新上次误差
     sptr->LastData  = NowData;          // 更新上次数据

     return Realize; // 返回实际值
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    位置式PID
// 参数说明    *sptr      PID各项误差的结构体
// 参数说明    *PID       PID各项系数的数组
// 参数说明      NowData    当前值
// 参数说明      Point      目标值
// 返回参数      Realize    反馈输出值
//-------------------------------------------------------------------------------------------------------------------
extern int angle_thred;
float PID_Realize_Inner(PID *sptr, int *PID, float NowData, float Point)
{

     float iError,   // 当前误差
          Realize;   // 最后得出的实际输出
     float kp_t,ki_t,kd_t;
     iError = Point - NowData;   // 计算当前误差


     kp_t=(float)(PID[KP]/100.0);
     kd_t=(float)(PID[KD]/100.0);
     ki_t=(float)(PID[KI]/10000.0);

     //积分限幅
     if(ki_t)
     {
         sptr->SumError +=  iError; // 误差积分
         if (sptr->SumError*ki_t >= PID[KT])
         {
             sptr->SumError = PID[KT]/ki_t;
         }
         else if (sptr->SumError*ki_t <= -PID[KT])
         {
             sptr->SumError = -PID[KT]/ki_t;
         }
     }
     //积分分离
//     if(fabs(pure_angle)> abs(angle_thred2))
//     {
         //sptr->SumError = 0; // 误差积分
         Realize = kp_t * iError
             + ki_t * sptr->SumError
             + kd_t * (iError - sptr->LastError);
//     }else {
//         sptr->SumError +=  iError; // 误差积分
//         Realize = kp_t * iError
//              //+ ki_t * sptr->SumError
//              + kd_t * (iError - sptr->LastError);
//    }
     sptr->LastError = iError;           // 更新上次误差
     sptr->LastData  = NowData;          // 更新上次数据

     return Realize; // 返回实际值
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    增量式PID
// 参数说明    *sptr      PID各项误差的结构体
// 参数说明    *PID       PID各项系数的数组
// 参数说明      NowData    当前值
// 参数说明      Point      目标值
// 返回参数      Increase   反馈输出增量
//-------------------------------------------------------------------------------------------------------------------
int32 PID_Increase(PID *sptr, float *PID, int32 NowData, int32 Point)
{
    int32 iError,   //当前误差
          Increase; //最后得出的实际增量

    iError = Point - NowData;   // 计算当前误差

    Increase =  PID[KP] * (iError - sptr->LastError)
              + PID[KI] * iError
              + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差

    return Increase;    // 返回增量
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    快速增量式PID，用于减少运算的计算量
// 参数说明    *sptr      PID各项误差的结构体
// 参数说明    *PID       PID各项系数的数组
// 参数说明      NowData    当前值
// 参数说明      Point      目标值
// 返回参数      Increase   反馈输出增量
//-------------------------------------------------------------------------------------------------------------------
int32 PID_Increase_Quick(PID_Quick *sptr, int32 *PID, int32 NowData, int32 Point)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    int32 iError,   //当前误差
        Increase;   //最后得出的实际增量

    iError = Point - NowData;   // 计算当前误差
    Increase =  PID[KP] * (iError - sptr->LastError)/64
              + PID[KI] * iError/128
              + PID[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);

    sptr->PrevError = sptr->LastError;  // 更新前次误差
    sptr->LastError = iError;           // 更新上次误差
    sptr->LastData  = NowData;          // 更新上次数据

    return Increase;    // 返回增量
}


