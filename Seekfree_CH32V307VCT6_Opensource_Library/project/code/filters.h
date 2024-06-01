#ifndef FILTERS_H_
#define FILTERS_H_

typedef struct _kalman_state//卡尔曼滤波器结构体
{
    float x;  /* 状态值 */
    float A;  /* x(n)=A*x(n-1)+u(n),u(n)~N(0,q) */
    float H;  /* z(n)=H*x(n)+w(n),w(n)~N(0,r)   */
    float q;  /* 预测（过程）噪声协方差   */
    float r;  /* 测量（观测）噪声协方差   */
    float p;  /* 估计误差协方差       */
    float gain; /* 卡尔曼增益     */
} kalman_state;


typedef struct _LowPassFilter// 一阶低通滤波器结构体
{
    float alpha;  // 滤波器系数
    float y_prev; // 上一时刻的输出值
} LowPassFilter;


typedef struct _Sliding_Data//滑行数据
{
    unsigned short int filter_arry[16];//滤波器阵列
    unsigned char      count;
} Sliding_Data;
#endif /* FILTERS_H_ */

extern LowPassFilter imu_dataz;        //z轴角速度
extern LowPassFilter imu_acc_x;        //x轴加速度
extern Sliding_Data Power_level;       //采集电压
/*================================ 接口函数 ==================================*/
extern  void Filters_Init(void);
extern float LowPass_Filter(LowPassFilter *filter, float input);
extern  void kalman1_init(kalman_state *state, float init_x, float init_p);
extern  float kalman1_filter(kalman_state *state, float z_measure);
extern  void initLowPassFilter(LowPassFilter *filter, float alpha, float initialOutput);
extern  float updateLowPassFilter(LowPassFilter *filter, float input);
extern  int Sliding_Filter(Sliding_Data* Data_Reserved,unsigned short int Input_Data,short int Data_Offset);
