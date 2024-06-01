/*
 * image.C
 *
 *  Created on:
 *  2023年5月14日      两个“extern“没有移，移好了基本的图像代码，还有部分定义未解决-->工作到22：23
 *  2023年5月17日      基本图像代码移好，十字代码开始动工-->工作到22：16
 *  2023年5月17日      解决下载问题-->工作到22：00多
 *  2023年5月22日      摄像头图传，tft屏幕硬件搭好，软件测试可用-->工作到22：50
 *  2023年5月24日      屏幕显示大津法，sobel，按键写好，拍了透视变换矩阵，透视在屏幕显示不太行，问了孙哥,有点摸鱼-->工作到23：04
 *  2023年5月25日      原来是没有线性插值，图像显示就是这样，解决了一些十字的疑难问题（问孙哥），开会，移了部分电机，可以屏幕画左右边线，中线差点-->工作到23：09
 *  2023年5月26日     完善了编码器的代码，大无语事件，新板子刚用就烧了，但是焊一下应该很快，又用上了学习板，拒绝飞线，TX,RX,居然搞反了，下午还问了龚哥的，还好查出来了，回到原点，摸了好长时间的鱼，休息一下-->工作到23：06
 *  2023年5月28日      motor里面初始化问题解决，中线问题解决，今天9点才到-->工作到23：09
 *      Author: 唐纪元
 */


#include "image.h"
#include "zf_common_headfile.h"
#include "utils.h"
#include "cross.h"
/*============================= 宏定义/重定义 ================================*/
#define AT                  AT_IMAGE
#define MAX_COLS MT9V03X_W /*图像水平分辨率*/
#define MAX_ROWS MT9V03X_H /*图像竖直分辨率*/
/*================================ 全局变量 ==================================*/
const int dir_front[4][2] = {{0,  -1},
                             {1,  0},
                             {0,  1},
                             {-1, 0}};
const int dir_frontleft[4][2] = {{-1, -1},
                                 {1,  -1},
                                 {1,  1},
                                 {-1, 1}};
const int dir_frontright[4][2] = {{1,  -1},
                                  {1,  1},
                                  {-1, 1},
                                  {-1, -1}};

//float distort[3][3] = //去畸变矩阵
//{{63.620673740948250 , 0, 92.509352427546370},
//{0 , 63.589641618875400 , 56.622635035447324},
//{0 , 0 , 1}};

float distort[3][3] = //去畸变矩阵
{{60,    0,   94.5000000000000},

{0,   60,  60.5000000000000},
{0,   0,   1}};

float k1=-0.005011653958470,k2=-0.017472367034198;//径向畸变系数，切向畸变p1、p2为0

//float rot[3][3] =
//{{5.66080000000000  ,  -0.0666000000000000, -89.1297000000000},
//{3.09110000000000  ,  1.45190000000000 ,   -97.7337000000000},
//{0.0356000000000000 , -0.00180000000000000 ,   0.484200000000000}};
//
//float inv_rot[3][3] =
//{{0.0630000000000000,  0.0225000000000000,  16.1326000000000},
//{-0.590500000000000,  0.701800000000000 ,  32.9692000000000},
//{-0.00680000000000000 ,   0.000900000000000000,    1}};
float rot[3][3] =
{{5.90940000000000,  0.209600000000000,   -86.4731000000000},
{3.63760000000000,    1.83580000000000,    -116.950300000000},
{0.0392000000000000,  0.00160000000000000, 0.417700000000000}};

float inv_rot[3][3] =
{{0.0946606151582685,    -0.0224171494997392, 13.3203363215301},
{-0.605698814988820,  0.581310078192423,   37.3658937975044},
{-0.00656350972042618,    -0.000122920432650466,   1.00085560478816}};

uint32 clip_value=2;
uint32 adaptive_Block=11;

int16 delta_y= 0;
int16 delta_x= 0;

uint32 begin_x=15;
uint32 begin_y=MT9V03X_H*0.8;
uint32 BEGIN_Y;//用于位于坡道时切换

uint32 start_x=8;/**< 用于找边线的起始点*/
uint32 start_y=70;/**< 用于找边线的起始点*/
uint32 end_x=8;/**< 用于找边线的起始点*/
uint32 end_y=70;/**< 用于找边线的起始点*/

int16 line_blur_kernel  =11;
extern uint16 Ostu_Thres;

int pixel_per_meter     =100;
float ROAD_WIDTH        =0.45;
float sample_dist       =0.02;
float angle_dist        =0.20;
bool origin_flag = 0;/**< 初始帧标志*/

int ipts0[LINE_LENTH][2];/**< 原图左边线*/
int ipts1[LINE_LENTH][2];/**< 原图右边线*/
int ipts0_num, ipts1_num;

int dipts0[LINE_LENTH][2];/**< 去畸变左边线*/
int dipts1[LINE_LENTH][2];/**< 去畸变右边线*/
int dipts0_num, dipts1_num;

float rpts0[LINE_LENTH][2];/**< 透视变换后左边线*/
float rpts1[LINE_LENTH][2];/**< 透视变换后右边线*/
int rpts0_num, rpts1_num;

float rpts0b[LINE_LENTH][2];/**< 左边线滤波*/
float rpts1b[LINE_LENTH][2];/**< 右边线滤波*/
int rpts0b_num, rpts1b_num;

float rpts0s[LINE_LENTH][2];/**< 左边线等距采样*/
float rpts1s[LINE_LENTH][2];/**< 右边线等距采样*/
int rpts0s_num, rpts1s_num;

float rpts0a[LINE_LENTH];/**< 左边线局部角度变化率*/
float rpts1a[LINE_LENTH];/**< 右边线局部角度变化率*/
int rpts0a_num, rpts1a_num;

float rpts0an[LINE_LENTH];/**< 非极大抑制后的左边线局部角度变化率*/
float rpts1an[LINE_LENTH];/**< 非极大抑制后的边线局部角度变化率*/
int rpts0an_num, rpts1an_num;

float inv_rptsc0[LINE_LENTH][2];/**< 左中线*/
float inv_rptsc1[LINE_LENTH][2];/**< 右中线*/


float rptsc0[LINE_LENTH][2];/**< 透视变换后左中线*/
float rptsc1[LINE_LENTH][2];/**< 透视变换后右中线*/
int rptsc0_num, rptsc1_num;

int dir_f0[LINE_LENTH];/**< 原图左边线每个点的方向*/
int dir_f1[LINE_LENTH];/**< 原图右边线每个点的方向*/
int dir_backnum0,dir_backnum1;/**<统计左右边线的向下点的个数*/
int dir_rightnum0,dir_leftnum1;/**<统计左边线的向右点，和右边线向左的个数*/

// L角点
int Lpt0_rpts0s_id, Lpt1_rpts1s_id;
int Lpt0_s_rpts0s_id, Lpt1_s_rpts1s_id;
int inv_Lpt0_rpts0s_id,inv_Lpt1_rpts1s_id;
bool Lpt0_found, Lpt1_found;
bool Lpt0_s_found, Lpt1_s_found;

// 长直道
bool is_straight0, is_straight1,is_straight_far_0,is_straight_far_1;
uint32 Lconf_Min=60;
uint32 Lconf_Max=120;/**< 直角阈值 */
bool is_longstraight0, is_longstraight1;
bool bend_flag=false;

float sobel0=0,sobel1=0;
float conf0_s,conf0,conf1_s,conf1,conf0_max,conf1_max;/**< max用来显示在屏幕*/
extern image_t img_raw ;
int x0,x1;
int find_type=0;//0表示使用大津法寻找起始点，1表示使用sobel寻找起始点
/*================================ 接口函数 ==================================*/
//Sobel函数
#define Sobel_Gx(addr,y,x,width)    (addr[(y-1)*width+x+1]+2*addr[y*width+x+1]+addr[(y+1)*width+x+1]-(addr[(y-1)*width+x-1]+2*addr[y*width+x-1]+addr[(y+1)*width+x-1]))
#define Sobel_Gy(addr,y,x,width)    (addr[(y-1)*width+x-1]+2*addr[(y-1)*width+x]+addr[(y-1)*width+x+1]-(addr[(y+1)*width+x-1]+2*addr[(y+1)*width+x]+addr[(y+1)*width+x+1]))
#define Sobel_G(addr,y,x,width)     (abs(Sobel_Gx(addr,y,x,width))+abs(Sobel_Gy(addr,y,x,width)))
void sobelThreshold(uint8* img_data, uint8* output_data ,int width, int height,int Threshold)
{
    for(int i=1;i<height-1;i++)
    {
        for(int j=1;j<width-1;j++)
        {
            output_data[j+i*width]=(Sobel_G(img_data,i,j,width) >= Threshold ? 0 : 0xffff);
        }
    }
}


//大津法
uint8 otsuThreshold(uint8 *image, uint16 width, uint16 height)
{
    #define GrayScale 256
    int pixelCount[GrayScale] = {0};//每个灰度值所占像素个数
    float pixelPro[GrayScale] = {0};//每个灰度值所占总像素比例
    int i,j;
    int Sumpix = width * height;   //总像素点
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针


    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
          //   pixelCount[(int)image[i][j]]++;    若不用指针用这个
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix;   //计算每个像素在整幅图像中的比例
        u += i * pixelPro[i];  //总平均灰度
    }


    float maxVariance=0.0;  //最大类间方差
    float w0 = 0, avgValue  = 0;  //w0 前景比例 ，avgValue 前景平均灰度
    for(int i = 0; i < 256; i++)     //每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
    {
        w0 += pixelPro[i];  //假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
        avgValue  += i * pixelPro[i];

        float variance = pow((avgValue/w0 - u), 2) * w0 /(1 - w0);    //类间方差
        if(variance > maxVariance)
        {
            maxVariance = variance;
            threshold = i;
        }
    }


    return threshold;

}

/*
    函数功能：在对OSTU二值化的图像进行Robert算子边缘增强
    参数说明：
        org_in：输入原图，一维数组
        ostu_out：输出图像，经大津法二值化并进行边缘增强后的图像
        th_ostu: 大津法阈值
        th_edge：边缘增强阈值，建议在11左右调试
        start_rows: 边缘强化起始行，从0开始计数
        end_row：边缘强化结束行，包括改行
*/
void Ostu_Robert(unsigned char *org_in, unsigned char *ostu_out, unsigned char th_ostu, unsigned int th_edge, unsigned int start_rows, unsigned int end_rows)
{
  unsigned int i, col, h, v;
  unsigned int start_pixel, end_pixel;
  unsigned int pix0, pix1, pix2, pix3;

  col = 0;
  start_pixel = start_rows * MAX_COLS;
  end_pixel = (end_rows + 1) * MAX_COLS;

  for (i = start_pixel; i < end_pixel - MAX_COLS; i++)
  {
    col++;

    if (org_in[i] >= th_ostu) /*等号是否需要根据小车情况进行处理*/
    {
      ostu_out[i - start_pixel] = 255; /*置白*/
    }
    else
    {
      ostu_out[i - start_pixel] = 0; /*置黑*/
    }

    if (col == MAX_COLS) //右边界像素无法用Robert算子，不增强，仅更新列索引
    {
      col = 0;
    }
    else //Robert算子边缘增强
    {
      pix0 = (unsigned int)org_in[i] & 0xff;
      pix1 = (unsigned int)org_in[i + 1] & 0xff;
      pix2 = (unsigned int)org_in[i + MAX_COLS] & 0xff;
      pix3 = (unsigned int)org_in[i + 1 + MAX_COLS] & 0xff;

      if (pix0 > pix3)
      {
        h = pix0 - pix3;
      }
      else
      {
        h = pix3 - pix0;
      }

      if (pix1 > pix2)
      {
        v = pix1 - pix2;
      }
      else
      {
        v = pix2 - pix1;
      }

      h = h + v;

      if (h >= th_edge)
      {
        ostu_out[i - start_pixel] = 0; // 强制置黑
      }
    }
  }
}

//全图自适应，用于屏幕显示
void adaptiveThreshold(uint8* img_data, uint8* output_data, int width, int height, int block, uint8 clip_value)
{
  //assert(block % 2 == 1); // block必须为奇数
  int half_block = block / 2;
  // 先遍历y后遍历x比较cache-friendly
  for(int y=half_block; y<height-half_block; y++){
    for(int x=half_block; x<width-half_block; x++){
      // 计算局部阈值
      int thres = 0;
      for(int dy=-half_block; dy<=half_block; dy++){
        for(int dx=-half_block; dx<=half_block; dx++){
          thres += img_data[(x+dx)+(y+dy)*width];
        }
      }
      thres = thres / (block * block) - clip_value;
      // 进行二值化
      output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
    }
  }
}

void findline_lefthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && half < x && x < img->width - half - 1 && 0 < y && y < img->height - half - 1 && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 1) % 4;
            turn++;
        } else if (frontleft_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        } else {
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }
    *num = step;
}


// 左手迷宫巡线
void findline_lefthand_sobel(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {

    int half = 0;
    int step = 0, dir = 0, turn = 0;
    while (step < *num &&
        half < x &&                                     //保证block  不出界
        x < img->width - half - 1  &&                   //保证block  不出界
        half < y && y < img->height - half - 1 &&       //保证block  不出界
        turn < 4)
    {
        int local_thres = 1;
        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);

        if( (x==1 && y < img->height -20) || x== img->width - 2 || y==1 )     //到上左右边界就停下
        {
//            if(x==1)  touch_boundary0  = 1;                //左边界是因为到最左边才停下来的，触碰到最左边，可能是环岛，十字等，

            break;
        }
        if (front_value < local_thres)
        {                        //前进方向为黑色，即墙        ->转弯
            dir = (dir + 1) % 4;
            turn++;
        }
        else if (frontleft_value < local_thres) {             //可以前进，且左前方为黑，  ->直走
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            AT(img, x, y) = 0x10;

        }
        else
        {                                                //可以前进，且左前方也可前进，为白，    ->左前方
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }

    }
    *num = step;
}

//左手寻线生长方向版
void findline_lefthand_adaptive01(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num,int dir_[],int *num0,int *num1){
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0,back_num = 0,right_num=0;
    while (step < *num && half < x && x < img->width - half - 1 && 0 < y && y < img->height - half && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontleft_value = AT(img, x + dir_frontleft[dir][0], y + dir_frontleft[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 1) % 4;//此处为转向，生长方向还没有固定
            turn++;

        }
        else if (frontleft_value < local_thres)
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            dir_[step]=dir;
            if(dir==2 && step <=80){
                    back_num++;//dir==2判断向下生长
            }
            if(dir==1 && step <=80){
                    right_num++;//dir==1判断向右生长
                       }
            step++;
            turn = 0;
        } else {
            dir_[step]=dir;
            if(dir==2 && step <=80){
                     back_num++;//dir==2判断向下生长
            }
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            dir = (dir + 3) % 4;
            pts[step][0] = x;
            pts[step][1] = y;

            step++;
            turn = 0;
        }
    }
    *num = step;
    *num0 = back_num;
    *num1 = right_num;
}

void findline_righthand_adaptive(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width -half- 1 && 0 < y && y < img->height - 1 && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
        if (front_value < local_thres) {
            dir = (dir + 3) % 4;
            turn++;
        } else if (frontright_value < local_thres) {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        } else {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
        }
    }

    *num = step;
}

// 右手迷宫巡线
void findline_righthand_sobel(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num) {

    //int half = 1;
    int step = 0, dir = 0, turn = 0;
    while (step < *num && 0 < x && x < img->width - 1 && 0 < y && y < img->height - 1 && turn < 4)
    {
        int local_thres = 1;

        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);


        if( (x==img->width - 2  && y < img->height -20)   || x==1 || y==1)
        {
//            if(x==img->width - 2)  touch_boundary1  = 1;                //右边界是因为到最右边才停下来的，触碰到最右边，可能是环岛，十字等，
            break;
        }

        if (front_value < local_thres)
        {
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres)
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;
            AT(img, x, y) = 0x20;


        } else
        {
            x += dir_frontright[dir][0];
            y += dir_frontright[dir][1];
            dir = (dir + 1) % 4;
            pts[step][0] = x;
            pts[step][1] = y;
            step++;
            turn = 0;

            AT(img, x, y) = 0x20;
        }
    }
    *num = step;
}

//右手巡线可记录生长方向版
void findline_righthand_adaptive01(image_t *img, int block_size, int clip_value, int x, int y, int pts[][2], int *num,int dir_[],int *num0,int  *num1) {
    int half = block_size / 2;
    int step = 0, dir = 0, turn = 0,back_num=0,left_num=0;
    while (step < *num && half < x && x < img->width -half- 1 && 0 < y && y < img->height -half && turn < 4) {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++) {
            for (int dx = -half; dx <= half; dx++) {
                local_thres += AT(img, x + dx, y + dy);
            }
        }
        local_thres /= block_size * block_size;
        local_thres -= clip_value;

        //int current_value = AT(img, x, y);
        int front_value = AT(img, x + dir_front[dir][0], y + dir_front[dir][1]);
        int frontright_value = AT(img, x + dir_frontright[dir][0], y + dir_frontright[dir][1]);
            if (front_value < local_thres) {
                dir = (dir + 3) % 4;
                turn++;//此处为转向，生长方向还没有固定
            } else if (frontright_value < local_thres) {
                x += dir_front[dir][0];
                y += dir_front[dir][1];
                pts[step][0] = x;
                pts[step][1] = y;
                dir_[step]=dir;
                if(dir==2 && step <=80){
                back_num++;//dir==2判断向下生长
                }
                if(dir==3 && step <=80){
                left_num++;//dir==3判断向左生长
                }
                step++;
                turn = 0;
            } else {
                dir_[step]=dir;
                if(dir==2 && step <=80){
                back_num++;//dir==2判断向下生长
                }
                x += dir_frontright[dir][0];
                y += dir_frontright[dir][1];
                dir = (dir + 1) % 4;
                pts[step][0] = x;
                pts[step][1] = y;

                step++;
                turn = 0;
            }

    }

    *num = step;
    *num0 = back_num;
    *num1 = left_num;
}

//大津法扫线（防丢线）
void findpoint_otsu(uint16 Ostu_Thres)
{
/*===============================================提取左边线==================================================*/

    if(origin_flag==0){
        x0 = img_raw.width / 2 - begin_x;
        for (; x0 > 0; x0--) if (AT_IMAGE(&img_raw, x0 - 1, begin_y) < Ostu_Thres) {
            break;}
    }
    else {
        int count0=1;
        while(x0-count0>0 || x0+count0<img_raw.width){
            if(AT_IMAGE(&img_raw,clip(x0-count0,0,img_raw.width-1),begin_y) < Ostu_Thres && AT_IMAGE(&img_raw,clip(x0-count0+1,1,img_raw.width),begin_y) >= Ostu_Thres){//向左寻找，左黑右白
                    x0=x0-count0+1;
                    break;}
            if(AT_IMAGE(&img_raw,clip(x0+count0-1,0,img_raw.width-1),begin_y) < Ostu_Thres && AT_IMAGE(&img_raw,clip(x0+count0,1,img_raw.width),begin_y) >= Ostu_Thres){//向右寻找，左黑右白
                    x0=x0+count0;
                    break;}
            count0++;}
    }

    /*===============================================提取右边线==================================================*/
    if(origin_flag==0){
        x1 = img_raw.width / 2 + begin_x;
        for (; x1 < img_raw.width - 1; x1++) if (AT_IMAGE(&img_raw, x1 + 1, begin_y) < Ostu_Thres) {
            break;}
        origin_flag=1;//不使用防丢线时关闭
    }
    else {
        int count1=1;
        while(x1-count1>0 || x1+count1<img_raw.width){
            if(AT_IMAGE(&img_raw,clip(x1-count1,0,img_raw.width-1),begin_y) >= Ostu_Thres && AT_IMAGE(&img_raw,clip(x1-count1+1,1,img_raw.width),begin_y) < Ostu_Thres){//向左寻找，左白右黑
                    x1=x1-count1;
                    break;}
            if(AT_IMAGE(&img_raw,clip(x1+count1-1,0,img_raw.width-1),begin_y) >= Ostu_Thres && AT_IMAGE(&img_raw,clip(x1+count1,1,img_raw.width),begin_y) < Ostu_Thres){//向右寻找，左白右黑
                    x1=x1+count1-1;
                    break;}
            count1++;}
    }

}

//sobel扫线（防丢线）
void findpoint_sobel()
{
/*===============================================提取左边线==================================================*/
    float grad_sum0=0;
    if(origin_flag==0){
        x0 = img_raw.width / 2 - begin_x;
        for (; x0 > 0; x0--){
            if(Sobel_G(img_raw.data,begin_y,clip(x0,1,img_raw.width-1),img_raw.width)>=sobelThres)
                for (int i=0;i<3;i++)
                    grad_sum0=grad_sum0+Sobel_G(img_raw.data,begin_y,clip(x0-i,1,img_raw.width-1),img_raw.width);
            if(grad_sum0>=sobelThres*1.5){
                break;}}
    }
    else {
        int count0=1;
        while(x0-count0>0 || x0+count0<img_raw.width){
            sobel0=0;
            grad_sum0=0;
            if(Sobel_G(img_raw.data,begin_y,clip(x0-count0+1,1,img_raw.width-1),img_raw.width)>=sobelThres
                    && (AT_IMAGE(&img_raw,clip(x0-count0+1,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x0-count0,1,img_raw.width-1),begin_y)>0)){//sobel跳变且右像素点为白色
                for(int i=0;i<3;i++)
                    grad_sum0=grad_sum0+Sobel_G(img_raw.data,begin_y,clip(x0-count0+1-i,1,img_raw.width-1),img_raw.width);//求左侧三个像素点sobel之和，过滤噪点
                if(grad_sum0>=sobelThres*1.5){
                    x0=x0-count0+1;
                    break;}}
            if(Sobel_G(img_raw.data,begin_y,clip(x0+count0,1,img_raw.width-1),img_raw.width)>=sobelThres
                    && (AT_IMAGE(&img_raw,clip(x0+count0,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x0+count0-1,1,img_raw.width-1),begin_y)>0)){
                for(int i=0;i<3;i++)
                    grad_sum0=grad_sum0+Sobel_G(img_raw.data,begin_y,clip(x0+count0-i,1,img_raw.width-1),img_raw.width);
                if(grad_sum0>=sobelThres*1.5){
                    x0=x0+count0;
                    break;}}
            count0++;}
    }
    sobel0=Sobel_G(img_raw.data,begin_y,clip(x0,1,img_raw.width-1),img_raw.width);

    /*===============================================提取右边线==================================================*/
    float grad_sum1=0;
    if(origin_flag==0){
        x1 = img_raw.width / 2 + begin_x;
        for (; x1 > 0; x1--){
            if(Sobel_G(img_raw.data,begin_y,clip(x1,1,img_raw.width-1),img_raw.width)>=sobelThres)
                for (int i=0;i<3;i++)
                    grad_sum1=grad_sum1+Sobel_G(img_raw.data,begin_y,clip(x1+i,1,img_raw.width-1),img_raw.width);
            if(grad_sum1>=sobelThres*1.5)
                break;}
        origin_flag=1;//不使用防丢线时关闭
    }
    else {
        int count1=1;
        while(x1-count1>0 || x1+count1<img_raw.width){
            sobel1=0;
            grad_sum1=0;
            if(Sobel_G(img_raw.data,begin_y,clip(x1-count1,1,img_raw.width-1),img_raw.width)>=sobelThres
                    && (AT_IMAGE(&img_raw,clip(x1-count1,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x1-count1+1,1,img_raw.width-1),begin_y)>0)){//sobel跳变且左像素点为白色
                for(int i=0;i<3;i++)
                    grad_sum1=grad_sum1+Sobel_G(img_raw.data,begin_y,clip(x1-count1+i,1,img_raw.width-1),img_raw.width);//求右侧三个像素点sobel之和，过滤噪点
                if(grad_sum1>=sobelThres*1.5){
                    x1=x1-count1;
                    break;}}
            if(Sobel_G(img_raw.data,begin_y,clip(x1+count1-1,1,img_raw.width-1),img_raw.width)>=sobelThres
                    && (AT_IMAGE(&img_raw,clip(x1+count1-1,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x1+count1,1,img_raw.width-1),begin_y)>0)){
                for(int i=0;i<3;i++)
                    grad_sum1=grad_sum1+Sobel_G(img_raw.data,begin_y,clip(x1+count1-1+i,1,img_raw.width-1),img_raw.width);
                if(grad_sum1>=sobelThres*1.5){
                    x1=x1+count1-1;
                    break;}}
            count1++;}
    }
    sobel1=Sobel_G(img_raw.data,begin_y,clip(x1,1,img_raw.width-1),img_raw.width);
}

void distort_img_process(int pts_in[][2], int num, int pts_out[][2])
{
    float fx=distort[0][0],fy=distort[1][1],cx=distort[0][2],cy=distort[1][2];
    for (int i = 0; i < num; i++){
        //根据公式计算去畸变图像上点(ipts0[i][0],ipts0[i][1])对应在畸变图像的坐标(distorted_0,distorted_1)，建立对应关系
        double x = (pts_in[i][0] - cx) / fx;
        double y = (pts_in[i][1] - cy) / fx;
        double r = sqrt(x * x + y * y);
        //double x_distorted = x*(1+k1*r*r+k2*r*r*r*r)+2*p1*x*y+p2*(r*r+2*x*x);
        //double y_distorted = y*(1+k1*r*r+k2*r*r*r*r)+2*p2*x*y+p1*(r*r+2*y*y);
        double x_distorted = x*(1+k1*r*r+k2*r*r*r*r);//p1、p2为0，将上式的p1p2省略
        double y_distorted = y*(1+k1*r*r+k2*r*r*r*r);
        double distorted_0 = fx * x_distorted + cx;
        double distorted_1 = fy * y_distorted + cy;

        //将畸变图像上点的坐标，赋值到去畸变图像中（最近邻插值）
        if (distorted_0 >= 0 && distorted_1 >=0 && distorted_1 < img_raw.height && distorted_0 < img_raw.width){
            pts_out[i][0] = (int)distorted_0;
            pts_out[i][1] = (int)distorted_1;
        }else{
            pts_out[i][0] = 0;
            pts_out[i][1] = 0;
        }
    }

}

void rot_img_process()
{
    for(int i=0;i<dipts0_num;i++)
    {
        rpts0[i][0] = (rot[1][0]*dipts0[i][1]+rot[1][1]*dipts0[i][0]+rot[1][2])/(rot[2][0]*dipts0[i][1]+rot[2][1]*dipts0[i][0]+rot[2][2]);
        rpts0[i][1] = (rot[0][0]*dipts0[i][1]+rot[0][1]*dipts0[i][0]+rot[0][2])/(rot[2][0]*dipts0[i][1]+rot[2][1]*dipts0[i][0]+rot[2][2]);
    }
    for(int i=0;i<dipts1_num;i++)
    {
        rpts1[i][0] = (rot[1][0]*dipts1[i][1]+rot[1][1]*dipts1[i][0]+rot[1][2])/(rot[2][0]*dipts1[i][1]+rot[2][1]*dipts1[i][0]+rot[2][2]);
        rpts1[i][1] = (rot[0][0]*dipts1[i][1]+rot[0][1]*dipts1[i][0]+rot[0][2])/(rot[2][0]*dipts1[i][1]+rot[2][1]*dipts1[i][0]+rot[2][2]);
    }
    rpts0_num = dipts0_num;
    rpts1_num = dipts1_num;
}

void blur_points(float pts_in[][2], int num, float pts_out[][2], int kernel){
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - fabs(j));
            pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - fabs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}

void resample_points(float pts_in[][2], int num1, float pts_out[][2], int *num2, float dist){
    int remain = 0, len = 0;
    for(int i=0; i<num1-1 && len < *num2; i++){
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float dx = pts_in[i+1][0] - x0;
        float dy = pts_in[i+1][1] - y0;
        float dn = sqrt(dx*dx+dy*dy);
        dx /= dn;
        dy /= dn;

        while(remain < dn && len < *num2){
            x0 += dx * remain;
            pts_out[len][0] = x0;
            y0 += dy * remain;
            pts_out[len][1] = y0;

            len++;
            dn -= remain;
            remain = dist;
        }
        remain -= dn;
    }
    *num2 = len;
}

void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist){
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}

void nms_angle(float angle_in[], int num, float angle_out[], int kernel){
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] - dy * dist;
        pts_out[i][1] = pts_in[i][1] + dx * dist;
    }
}

void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist){
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] + dy * dist;
        pts_out[i][1] = pts_in[i][1] - dx * dist;
    }
}


void process_image()
{
    //寻找起始点，提取边线
    switch(find_type){
    case 0:
        findpoint_otsu(Ostu_Thres);
        ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);
        ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
        if (AT_IMAGE(&img_raw, x0, begin_y) >= Ostu_Thres && AT_IMAGE(&img_raw, x0 - 1, begin_y) < Ostu_Thres)
            if(x0<x1||(x0>x1&&abs(x0-img_raw.width/2)<=abs(x1-img_raw.width/2)))
                findline_lefthand_adaptive01(&img_raw, adaptive_Block, clip_value, x0, begin_y, ipts0, &ipts0_num,dir_f0,&dir_backnum0,&dir_rightnum0);//只有此处使用了左手巡线新版
            else ipts0_num = 0;
        else ipts0_num = 0;
        if (AT_IMAGE(&img_raw, x1, begin_y) >= Ostu_Thres&&AT_IMAGE(&img_raw, x1 + 1, begin_y) < Ostu_Thres)
            if(x0<x1||(x0>x1&&abs(x0-img_raw.width/2)>abs(x1-img_raw.width/2)))
                findline_righthand_adaptive01(&img_raw, adaptive_Block, clip_value, x1, begin_y, ipts1, &ipts1_num,dir_f1,&dir_backnum1,&dir_leftnum1);//只有此处使用了右手巡线新版
            else ipts1_num = 0;
        else ipts1_num = 0;
        break;
    case 1:
        findpoint_sobel();
        ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);
        ipts1_num = sizeof(ipts1) / sizeof(ipts1[0]);
        if(AT_IMAGE(&img_raw,clip(x0,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x0-1,1,img_raw.width-1),begin_y)>0)
            findline_lefthand_adaptive01(&img_raw, adaptive_Block, clip_value, x0, begin_y, ipts0, &ipts0_num,dir_f0,&dir_backnum0,&dir_rightnum0);//只有此处使用了左手巡线新版
        else ipts0_num = 0;
        if(AT_IMAGE(&img_raw,clip(x1,1,img_raw.width-1),begin_y)- AT_IMAGE(&img_raw,clip(x1+1,1,img_raw.width-1),begin_y)>0)
            findline_righthand_adaptive01(&img_raw, adaptive_Block, clip_value, x1, begin_y, ipts1, &ipts1_num,dir_f1,&dir_backnum1,&dir_leftnum1);//只有此处使用了右手巡线新版
        else ipts1_num = 0;
        break;
    default:
        break;
    }

    //去畸变
    distort_img_process(ipts0, ipts0_num, dipts0);
    dipts0_num = ipts0_num;
    distort_img_process(ipts1, ipts1_num, dipts1);
    dipts1_num = ipts1_num;

//    for(int i=0;i<ipts0_num;i++)
//    {
//        dipts0[i][0] = (distort[1][0]*ipts0[i][1]+distort[1][1]*ipts0[i][0]+distort[1][2])/(distort[2][0]*ipts0[i][1]+distort[2][1]*ipts0[i][0]+distort[2][2]);
//        dipts0[i][1] = (distort[0][0]*ipts0[i][1]+distort[0][1]*ipts0[i][0]+distort[0][2])/(distort[2][0]*ipts0[i][1]+distort[2][1]*ipts0[i][0]+distort[2][2]);
//    }
//    for(int i=0;i<ipts1_num;i++)
//    {
//        dipts1[i][0] = (distort[1][0]*ipts1[i][1]+distort[1][1]*ipts1[i][0]+distort[1][2])/(distort[2][0]*ipts1[i][1]+distort[2][1]*ipts1[i][0]+distort[2][2]);
//        dipts1[i][1] = (distort[0][0]*ipts1[i][1]+distort[0][1]*ipts1[i][0]+distort[0][2])/(distort[2][0]*ipts1[i][1]+distort[2][1]*ipts1[i][0]+distort[2][2]);
//    }
//    dipts0_num = ipts0_num;
//    dipts1_num = ipts1_num;

    //透视变换
    rot_img_process();

    // 边线滤波
    blur_points(rpts0, rpts0_num, rpts0b, (int) round(line_blur_kernel));
    rpts0b_num = rpts0_num;
    blur_points(rpts1, rpts1_num, rpts1b, (int) round(line_blur_kernel));
    rpts1b_num = rpts1_num;

    // 边线等距采样
    rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);
    resample_points(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter);
    rpts1s_num = sizeof(rpts1s) / sizeof(rpts1s[0]);
    resample_points(rpts1b, rpts1b_num, rpts1s, &rpts1s_num, sample_dist * pixel_per_meter);

    // 边线局部角度变化率，采用我们的算法可以得到稳定且等距的边线前提下，直接取每个点前后20cm构成一个三点，
    //利用向量的角度计算公式算实际角度即可。为方便判断，
    local_angle_points(rpts0s, rpts0s_num, rpts0a, (int) round(angle_dist / sample_dist));
    rpts0a_num = rpts0s_num;
    local_angle_points(rpts1s, rpts1s_num, rpts1a, (int) round(angle_dist / sample_dist));//angle_dist / sample_dist
    rpts1a_num = rpts1s_num;

    // 角度变化率非极大抑制
    //我们又进行了角度的非极大抑制，保证只留下角度最大的点，即角点。这样算出来的角度很稳定，跟实际也很接近。
    nms_angle(rpts0a, rpts0a_num, rpts0an, (int) round(angle_dist / sample_dist) * 2 + 1);//angle_dist / sample_dist
    rpts0an_num = rpts0a_num;
    nms_angle(rpts1a, rpts1a_num, rpts1an, (int) round(angle_dist / sample_dist) * 2 + 1);//angle_dist / sample_dist
    rpts1an_num = rpts1a_num;

    // 左右中线跟踪
    track_leftline(rpts0s, rpts0s_num, rptsc0, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);//
    rptsc0_num = rpts0s_num;
    track_rightline(rpts1s, rpts1s_num, rptsc1, (int) round(10.0), pixel_per_meter * ROAD_WIDTH / 2);//
    rptsc1_num = rpts1s_num;

//    if(Lpt0_found)
//       {
//            inv_Lpt0_found[0]=Cal_inv_rot_x(rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][0],rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][1]);
//            inv_Lpt0_found[1]=Cal_inv_rot_y(rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][0],rpts0s[clip(Lpt0_rpts0s_id,0,rpts0s_num-1)][1]);
//
//            inv_Back_Lpt0[0]=Cal_inv_rot_x((rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][0]),rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][1]);
//            inv_Back_Lpt0[1]=Cal_inv_rot_y((rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][0]),rpts0s[clip(Lpt0_rpts0s_id+back_Position,0,rpts0s_num-1)][1]);
//       }
//
//    if(Lpt1_found)
//       {
//           inv_Lpt1_found[0]=Cal_inv_rot_x((rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][1]);
//           inv_Lpt1_found[1]=Cal_inv_rot_y((rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id,0,rpts1s_num-1)][1]);
//
//           inv_Back_Lpt1[0]=Cal_inv_rot_x((rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][1]);
//           inv_Back_Lpt1[1]=Cal_inv_rot_y((rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][0]),rpts1s[clip(Lpt1_rpts1s_id+back_Position,0,rpts1s_num-1)][1]);
//       }

}


/******************************************************************************
* FunctionName   : find_corners()
* Description    : 判断L角点和长直道标志位
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void find_corners() {
    Lpt0_found = Lpt1_found = false;
    Lpt0_s_found = Lpt1_s_found = false;
    conf0_max =conf1_max = 0;
    is_straight0 = rpts0s_num > 1.4 / sample_dist;
    is_straight1 = rpts1s_num > 1.4 / sample_dist;
    is_longstraight0 = rpts0s_num > 1.4 / sample_dist;
    is_longstraight1 = rpts1s_num > 1.4 / sample_dist;
    //计算左线的conf0和判断左线的长直道
    for (int i = 0; i <rpts0s_num; i++)
    {
        //if (rpts0an[i] == 0) continue;
        //小角点，用于长直道及路障判断
        int im0_s = clip(i - (int) round(0.14 / sample_dist), 0, rpts0s_num - 1);//向前取一个点
        int ip0_s = clip(i + (int) round(0.14 / sample_dist), 0, rpts0s_num - 1);//向后取一个点

        conf0_s  = fabs(rpts0a[i]) - (fabs(rpts0a[im0_s]) + fabs(rpts0a[ip0_s])) / 2;
        conf0_s=conf0_s*180/PI;

        //长直道

        if(conf0_s>15&&i< 1.8/sample_dist&&!bend_flag) is_longstraight0 = false; //长直道入弯
        //路障
        if (Lpt0_s_found == false&&Lconf_Min<conf0_s&&conf0_s<Lconf_Max&&(i<1.5/(sample_dist)))
        {
            Lpt0_s_rpts0s_id = i;
            Lpt0_s_found = true;
        }

        //大角点，用于十字及圆环判断
        int im0 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);//向前取一个点
        int ip0 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts0s_num - 1);//向后取一个点

        conf0  = fabs(rpts0a[i]) - (fabs(rpts0a[im0]) + fabs(rpts0a[ip0])) / 2;
        conf0=conf0*180/PI;

        if(conf0>10&&bend_flag) is_longstraight0 = false; //弯入长直道

        //十字及圆环
        if (rpts0an[i]!=0&&Lpt0_found == false&&Lconf_Min<conf0&&conf0<Lconf_Max&&(i<1.5/(sample_dist)))
        {
            Lpt0_rpts0s_id = i;
            Lpt0_found = true;
        }
        if(conf0>15&&i< 1.6/sample_dist) is_straight0 = false; //只要中间有大角度，就不是直道

        if(conf0_s>conf0_max)conf0_max = conf0_s;//用于图显
        if (Lpt0_found==true) break;//只找第一个角点
    }

    //计算右线的conf0和判断右线的长直道
    for (int i = 0; i <rpts1s_num; i++)
    {
        //if (rpts1an[i] == 0) continue;
        //小角点，用于长直道及路障判断
        int im1_s = clip(i - (int) round(0.14 / sample_dist), 0, rpts1s_num - 1);//向前取一个点
        int ip1_s = clip(i + (int) round(0.14 / sample_dist), 0, rpts1s_num - 1);//向后取一个点

        conf1_s  = fabs(rpts1a[i]) - (fabs(rpts1a[im1_s]) + fabs(rpts1a[ip1_s])) / 2;
        conf1_s=conf1_s*180/PI;

        //长直道

        if(conf1_s>15&&i< 1.8/sample_dist&&!bend_flag) is_longstraight1 = false;//长直道入弯
        //路障
        if (Lpt1_s_found == false&&Lconf_Min<conf1_s&&conf1_s<Lconf_Max&&(i<1.5/(sample_dist)))//限距离，限尖峰
        {
            Lpt1_s_rpts1s_id = i;
            Lpt1_s_found = true;
        }

        //大角点，用于十字及圆环判断
        int im1 = clip(i - (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);//向前取一个点
        int ip1 = clip(i + (int) round(angle_dist / sample_dist), 0, rpts1s_num - 1);//向后取一个点

        conf1  = fabs(rpts1a[i]) - (fabs(rpts1a[im1]) + fabs(rpts1a[ip1])) / 2;
        conf1=conf1*180/PI;

        if(conf1>10&&bend_flag) is_longstraight1 = false; //弯入长直道

        //十字及圆环
        if (rpts1an[i]!=0&&Lpt1_found == false&&Lconf_Min<conf1&&conf1<Lconf_Max&&(i<1.5/(sample_dist)))//限距离，限尖峰
        {
            Lpt1_rpts1s_id = i;
            Lpt1_found = true;
        }
        if(conf1>15&&i< 1.6/sample_dist) is_straight1 = false; //只要中间有大角度，就不是长直道

        if(conf1_s>conf1_max)conf1_max = conf1_s;//用于图显
        if (Lpt1_found==true) break;//只找第一个角点
    }

    if(!is_longstraight0 || !is_longstraight1)bend_flag = true;
    else bend_flag = false;
}
