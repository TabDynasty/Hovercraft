#ifndef CIRCLE_H_
#define CIRCLE_H_


enum circle_type_e {
    CIRCLE_NONE = 0,
    CIRCLE_LEFT_BEGIN, CIRCLE_RIGHT_BEGIN,
    CIRCLE_LEFT_IN, CIRCLE_RIGHT_IN,
    CIRCLE_LEFT_RUNNING, CIRCLE_RIGHT_RUNNING,
    CIRCLE_LEFT_OUT, CIRCLE_RIGHT_OUT,
    CIRCLE_LEFT_END, CIRCLE_RIGHT_END,
};

/*================================ 接口函数 ==================================*/
void check_circle();
void run_Lcircle();
void run_Rcircle();

/*================================ 全局变量 ==================================*/
extern enum circle_type_e circle_type;
extern bool circle_obstacle_flag;
extern int LcircleIn_thred;
extern int RcircleIn_thred;

#endif /* CIRCLE_H_ */
