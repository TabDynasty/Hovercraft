#ifndef GARAGE_H_
#define GARAGE_H_

/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"

/*================================== 枚举=====================================*/
enum garage_type_e {
    GARAGE_NONE = 0,                        // 非车库模式
    GARAGE_FOUND = 1,  // 发现车库，即斑马线
    GARAGE_OUT =2,      // 出库
};
extern enum garage_type_e garage_type;
void run_garage();
#endif /* GARAGE_H_ */
