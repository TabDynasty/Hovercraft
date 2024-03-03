#ifndef GARAGE_H_
#define GARAGE_H_

/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"

/*================================== 枚举=====================================*/
enum garage_type_e {
    GARAGE_NONE = 0,                        // 非车库模式
    GARAGE_FOUND_LEFT, GARAGE_FOUND_RIGHT,  // 发现车库，即斑马线+单侧L角点(未使用)
    GARAGE_OUT_LEFT, GARAGE_OUT_RIGHT,      // 出库，陀螺仪转过45°，即出库完毕
};
extern enum garage_type_e garage_type;
void run_garage();
#endif /* GARAGE_H_ */
