#ifndef GARAGE_H_
#define GARAGE_H_

/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"

/*================================== 枚举=====================================*/
enum garage_type_e {
    GARAGE_NONE,    // 非车库模式
    GARAGE_FOUND,   // 发现车库，即斑马线
    GARAGE_STOP,    // 停车
    GARAGE_OUT,     // 出库
};
void check_garage();
void run_garage();
extern enum garage_type_e garage_type;
extern int zebra_L_flag;
extern int zebra_R_flag;
extern int zebraL_x;
extern int zebraR_x;

#endif /* GARAGE_H_ */
