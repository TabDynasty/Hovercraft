#ifndef GARAGE_H_
#define GARAGE_H_
#include "zf_common_headfile.h"

/*============================= 结构体联合体定义 =============================*/
enum garage_type_e {
    GARAGE_NONE,    // 非车库模式
    GARAGE_FOUND,   // 发现车库，即斑马线
    GARAGE_STOP,    // 停车
    GARAGE_OUT,     // 出库
};

/*================================ 接口函数 ==================================*/
void check_garage();
void run_garage();

/*================================ 全局变量 ==================================*/
extern enum garage_type_e garage_type;
extern bool zebra_L_flag,zebra_R_flag;
extern int zebraL_x,zebraR_x;
extern int out_distance,stop_distance;

#endif /* GARAGE_H_ */
