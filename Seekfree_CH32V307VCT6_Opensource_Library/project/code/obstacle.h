#ifndef OBSTACLE_H_
#define OBSTACLE_H_
/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"
enum obstacle_type_e
{
    OBSTACLE_NONE,
    OBSTACLE_LEFT_BEGIN,
    OBSTACLE_RIGHT_BEGIN,
    OBSTACLE_LEFT_IN,
    OBSTACLE_RIGHT_IN,
    OBSTACLE_LEFT_OUT,
    OBSTACLE_RIGHT_OUT,
};
/*================================ 全局变量 ==================================*/
extern enum obstacle_type_e  obstacle_type;

#endif /* OBSTACLE_H_ */
