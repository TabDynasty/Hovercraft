#ifndef OBSTACLE_H_
#define OBSTACLE_H_
#include "zf_common_headfile.h"

/*============================= 结构体联合体定义 =============================*/
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

/*================================ 接口函数 ==================================*/

void run_Lobstacle();
void run_Robstacle();

#endif /* OBSTACLE_H_ */
