#ifndef CROSS_H_
#define CROSS_H_
#include "zf_common_headfile.h"
/*================================ 全局变量 ==================================*/

enum cross_type_e {
    CROSS_NONE = 0,     // 非十字模式
    CROSS_BEGIN,        // 找到上下两个L角点
    CROSS_IN,           // 近处L角点全丢了，固定搜线
};
extern enum cross_type_e cross_type;


extern float inv_Lpt0[2],inv_Lpt1[2];/**< 左右L角点对应原图的点*/
extern float inv_Back_Lpt0[2],inv_Back_Lpt1[2];/**< 左右L角点后的一个点*/
extern int far_x0,far_y0,far_x1,far_y1;/**< 远处搜线起始点*/
extern int far_Show_x0,far_Show_y0,far_Show_x1,far_Show_y1;/**< 图显远处搜线起始点*/
extern float inv_far_Lpt0[2],inv_far_Lpt1[2];/**< 远处左右L角点*/

extern int far_ipts0[MT9V03X_H][2];/**< 原图左边线*/
extern int far_ipts1[MT9V03X_H][2];/**< 远处右边线*/
extern int far_ipts0_num, far_ipts1_num;

extern float far_rpts0[MT9V03X_H][2];/**< 透视变换后左边线*/
extern float far_rpts1[MT9V03X_H][2];/**< 透视变换后右边线*/
extern int far_rpts0_num, far_rpts1_num;

extern float far_rpts0b[MT9V03X_H][2];/**< 左边线滤波*/
extern float far_rpts1b[MT9V03X_H][2];/**< 右边线滤波*/
extern int far_rpts0b_num, far_rpts1b_num;

extern float far_rpts0s[MT9V03X_H][2];/**< 左边线等距采样*/
extern float far_rpts1s[MT9V03X_H][2];/**< 右边线等距采样*/
extern int far_rpts0s_num, far_rpts1s_num;

extern float far_rpts0a[MT9V03X_H];/**< 左边线局部角度变化率*/
extern float far_rpts1a[MT9V03X_H];/**< 右边线局部角度变化率*/
extern int far_rpts0a_num, far_rpts1a_num;

extern float far_rpts0an[MT9V03X_H];/**< 非极大抑制后的左边线局部角度变化率*/
extern float far_rpts1an[MT9V03X_H];/**< 非极大抑制后的边线局部角度变化率*/
extern int far_rpts0an_num, far_rpts1an_num;

extern float far_rptsc0[MT9V03X_H][2];/**< 透视变换后远左中线*/
extern float far_rptsc1[MT9V03X_H][2];/**< 透视变换后远右中线*/
extern int far_rptsc0_num, far_rptsc1_num;

extern bool far_Lpt0_found, far_Lpt1_found;/**< 左右角点标志位*/
extern int far_Lpt0_rpts0s_id, far_Lpt1_rpts1s_id;/**< 左右角点坐标*/

extern uint32 far_begin_x;/**< 远线搜寻的起始点*/
extern uint32 far_begin_y;/**< 远线搜寻的起始点*/

extern float far_conf0_max;/**< 用于图显*/
extern float far_conf1_max;/**< 用于图显*/
/*================================ 接口函数 ==================================*/

void check_Cross();
void check_Left_Cross();
void check_Right_Cross();
void find_far_L0();
void find_far_L1();
#endif /* CROSS_H_ */
