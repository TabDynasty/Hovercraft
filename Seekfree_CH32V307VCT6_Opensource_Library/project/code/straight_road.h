#ifndef STRAIGHT_ROAD_H_
#define STRAIGHT_ROAD_H_
/*============================= 结构体联合体定义 =============================*/
enum straigh_troad_type_e {
    STRAIGHT_NONE = 0,
    STRAIGHT_IN,
    STRAIGHT_OUT,
};
extern enum straigh_troad_type_e straight_road_type;
extern int break_dis;
extern int check_straight_thred;
extern int check_bend_thred;
extern int check_angle_thred;
#endif /* STRAIGHT_ROAD_H_ */
