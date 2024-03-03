#include "filters.h"
/*=============================  滤波器结构体定义  ================================*/
LowPassFilter imu_dataz;        //z轴角速度
LowPassFilter imu_acc_x;        //x轴线加速度
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化滤波器
// 参数说明     void
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void Filters_Init(void)
{
    initLowPassFilter(&imu_dataz, 0.35 , 0);
    initLowPassFilter(&imu_acc_x, 0.35 , 0);
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化卡尔曼滤波器
// 参数说明     *state         卡尔曼滤波器结构体
// 参数说明     init_x         待测量的初始值，如有中值一般设成中值（如陀螺仪）
// 参数说明     init_p         后验状态估计值误差的方差的初始值
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void kalman1_init(kalman_state *state, float init_x, float init_p)
{
    state->x = init_x;
    state->p = init_p;
    //A，H分别为预测，观测系数
    state->A = 1;
    state->H = 1;
    //q,r分别为预测（过程）噪声协方差，测量（观测）噪声协方差，服从正态分布
    //r：以陀螺仪为例，测试方法是：保持陀螺仪不动，统计一段时间内的陀螺仪输出数据。数据会近似正态分布，按3σ原则，取正态分布的(3σ)^2作为r的初始化值。
    state->q = 2e2;//10e-6;
    state->r = 5e2;//10e-5;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     一维卡尔曼滤波器
// 参数说明     *state         卡尔曼滤波器结构体
// 参数说明     z_measure      需要滤波的参数
// 返回参数     滤波输出
//-------------------------------------------------------------------------------------------------------------------
float kalman1_filter(kalman_state *state, float z_measure)
{
    /* 先验预测值 */
    state->x = state->A * state->x;
    state->p = state->A * state->A * state->p + state->q;  /* p(n|n-1)=A^2*p(n-1|n-1)+q */

    /* 观测值 */
    state->gain = state->p * state->H / (state->p * state->H * state->H + state->r);
    state->x = state->x + state->gain * (z_measure - state->H * state->x);
    state->p = (1 - state->gain * state->H) * state->p;

    /*后验预测值*/
    return state->x;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    初始化一阶低通滤波器
// 参数说明     *filter         一阶低通滤波器结构体
// 参数说明     alpha           滤波系数
// 参数说明     initialOutput   初始值
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
void initLowPassFilter(LowPassFilter *filter, float alpha, float initialOutput)
{
    filter->alpha = alpha;
    filter->y_prev = initialOutput;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     一阶低通滤波器
// 参数说明     *filter         一阶低通滤波器结构体
// 参数说明     input           需要滤波的参数
// 返回参数     滤波输出
//-------------------------------------------------------------------------------------------------------------------
float LowPass_Filter(LowPassFilter *filter, float input)
{
    float output;

    // 差分方程
    output = filter->alpha * input + (1 - filter->alpha) * filter->y_prev;

    // 更新上一时刻的输出值
    filter->y_prev = output;

    return output;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    滑动滤波
// 参数说明     Data_Reserved  保存参数的数组
// 参数说明     Input_Data     输入参数
// 参数说明     Offset_Data    输入偏置
// 返回参数     void
//-------------------------------------------------------------------------------------------------------------------
short int Sliding_Filter(Sliding_Data* Data_Reserved,unsigned short int Input_Data,short int Data_Offset)
{
    unsigned long int sum=0;
    /* 接收数据，存放在数组里面 */
    Data_Reserved->filter_arry[Data_Reserved->count]=Input_Data;
    /* 限幅滑动参数 */
    Data_Reserved->count++;
    if(Data_Reserved->count==16) Data_Reserved->count=0;
    /* 求和 */
    for(int i=0;i<16;i++)
    {
        sum+= Data_Reserved->filter_arry[i];
    }
    return (short int)((sum>>4)-Data_Offset);
}
