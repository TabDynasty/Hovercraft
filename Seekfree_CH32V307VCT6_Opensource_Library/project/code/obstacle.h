#ifndef OBSTACLE_H_
#define OBSTACLE_H_
/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"
enum obstacle_type_e
{
    OBSTACLE_NONE,
    OBSTACLE_BEGIN,
    OBSTACLE_IN,
    OBSTACLE_OUT,
};
/*================================ 全局变量 ==================================*/
extern enum obstacle_type_e  obstacle_type;

#endif /* OBSTACLE_H_ */
