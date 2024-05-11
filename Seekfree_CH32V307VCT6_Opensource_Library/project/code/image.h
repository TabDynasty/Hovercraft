#ifndef IMAGE_H_
#define IMAGE_H_
#include "zf_common_headfile.h"

/*============================= 结构体联合体定义 =============================*/
typedef struct image {
    uint8 *data; /**< 图像数据*/
    uint32 width;
    uint32 height;
    uint32 step;  /**< 两行之间步长*/
} image_t;

/*============================= 宏定义/重定义 ================================*/

#define MT9V03X_W                (188)
#define MT9V03X_H                (120)

#define AT_IMAGE(img, x, y)          ((img)->data[(y)*(img)->step+(x)]) /**< 访问图片第X,Y位置的元素*/
#define AT_IMAGE_CLIP(img, x, y)     AT_IMAGE(img, clip(x, 0, (img)->width-1), clip(y, 0, (img)->height-1))
#define DEF_IMAGE(ptr, w, h)         {.data=ptr, .width=w, .height=h, .step=w}
#define ROI_IMAGE(img, x1, y1, w, h) {.data=&AT_IMAGE(img, x1, y1), .width=w, .height=h, .step=img.width}

/*================================ 接口函数 ==================================*/
void sobelThreshold(uint8* img_data, uint8* output_data ,int width, int height,int Threshold);
uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height);
void adaptiveThreshold(uint8* img_data, uint8* output_data, int width, int height, int block, uint8 clip_value);
void Ostu_Robert(unsigned char *org_in, unsigned char *ostu_out, unsigned char th_ostu, unsigned int th_edge, unsigned int start_rows, unsigned int end_rows);
//左手寻线
void findline_lefthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void findline_lefthand_sobel(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void findline_lefthand_adaptive01(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num,int dir_[],int *num0,int *num1);

//右手寻线
void findline_righthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void findline_righthand_sobel(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num);
void findline_righthand_adaptive01(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num,int dir_[],int *num0,int *num1);

void distort_img_process(int pts_in[][2], int num, int pts_out[][2]);
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);


void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel);
void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist);
void process_image();
void find_corners();

/*================================ 全局变量 ==================================*/
extern float distort[3][3];/**< 去畸变矩阵*/
extern float k1,k2;/**< 径向畸变系数*/
extern float rot[3][3];/**< 透视矩阵*/
extern float inv_rot[3][3];/**< 透视矩阵的逆矩阵*/
extern uint32 adaptive_Block;
extern uint32 clip_value;
extern float rot[3][3];/**< 透视矩阵*/
extern float inv_rot[3][3];/**< 透视矩阵的逆矩阵*/
extern uint32 back_Position ;/**< 左右L角点往后点的位置*/

extern int pixel_per_meter;/**< 像素和真实距离换算常数*/
extern float ROAD_WIDTH;/**< 赛道宽度*/
extern float sample_dist;/**< 采样距离*/
extern float angle_dist;/**< 角度采样距离*/

extern int ipts0[LINE_LENTH][2];/**< 原图左边线 0x1y*/
extern int ipts1[LINE_LENTH][2];/**< 原图右边线 0x1y*/
extern int ipts0_num, ipts1_num;

extern int dipts0[LINE_LENTH][2];/**< 去畸变左边线 0x1y*/
extern int dipts1[LINE_LENTH][2];/**< 去畸变右边线 0x1y*/
extern int dipts0_num, dipts1_num;

extern float rpts0[LINE_LENTH][2];/**< 透视变换后左边线 0x1y*/
extern float rpts1[LINE_LENTH][2];/**< 透视变换后右边线 0x1y*/
extern int rpts0_num, rpts1_num;

extern float inv_rptsc0[LINE_LENTH][2];/**< 左中线 0x1y*/
extern float inv_rptsc1[LINE_LENTH][2];/**< 右中线 0x1y*/

extern int16 line_blur_kernel;
extern float rpts0b[LINE_LENTH][2];/**< 左边线滤波*/
extern float rpts1b[LINE_LENTH][2];/**< 右边线滤波*/
extern int rpts0b_num, rpts1b_num;

extern float rpts0s[LINE_LENTH][2];/**< 左边线等距采样*/
extern float rpts1s[LINE_LENTH][2];/**< 右边线等距采样*/
extern int rpts0s_num, rpts1s_num;

extern float rpts0an[LINE_LENTH];/**< 左边线局部角度变化率*/
extern float rpts1an[LINE_LENTH];/**< 右边线局部角度变化率*/
extern int rpts0an_num, rpts1an_num;

extern float rpts0an[LINE_LENTH];/**< 非极大抑制后的左边线局部角度变化率*/
extern float rpts1an[LINE_LENTH];/**< 非极大抑制后的边线局部角度变化率*/
extern int rpts0an_num, rpts1an_num;

extern float rptsc0[LINE_LENTH][2];/**< 透视变换后左中线 0x1y*/
extern float rptsc1[LINE_LENTH][2];/**< 透视变换后右中线 0x1y*/
extern int rptsc0_num, rptsc1_num;

extern int dir_f0[LINE_LENTH];/**< 原图左边线每个点的方向*/
extern int dir_f1[LINE_LENTH];/*o*< 原图右边线每个点的方向*/
extern int dir_backnum0,dir_backnum1;/**<统计左右边线的向下点的个数*/
extern int dir_rightnum0,dir_leftnum1;/**<统计左边线的向右点，和右边线向左的个数*/

extern uint32 begin_x;
extern uint32 begin_y;

extern float conf0;
extern float conf1;
extern float conf0_max;
extern float conf1_max;
extern int x0,x1;

extern uint32 Lconf_Min,Lconf_Max;

extern int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
extern int Lpt0_s_rpts0s_id, Lpt1_s_rpts1s_id;
extern int inv_Lpt0_rpts0s_id,inv_Lpt1_rpts1s_id;
extern bool Lpt0_found,Lpt1_found;
extern bool Lpt0_s_found,Lpt1_s_found;
extern bool is_straight0, is_straight1,is_straight_far_0,is_straight_far_1;
extern bool is_longstraight0, is_longstraight1,bend_flag;

extern float sobel0,sobel1;
extern bool origin_flag;/**< 初始帧标志*/
extern int find_type;
#endif /* IMAGE_H_ */
