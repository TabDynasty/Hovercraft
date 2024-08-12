/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library 即（CH32V307VCT6 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是CH32V307VCT6 开源库的一部分
*
* CH32V307VCT6 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MounRiver Studio V1.8.1
* 适用平台          CH32V307VCT6
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期                                      作者                             备注
* 2022-09-15        大W            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#include "image.h"
#include "utils.h"
#include "motor.h"
#include "control.h"
#include "cross.h"
#include "circle.h"
#include "Key.h"
#include "garage.h"
#include "obstacle.h"
#include "PID.h"
#include "menu.h"
#include "W25QXX.h"
#include "dual.h"
#include "filters.h"
#include "straight_road.h"
#include "optical_flow.h"
#include "led_show.h"
/*================================无线图传部分====
 *
 * =============================*/
#define WIFI_SSID_TEST          "RAQUEL"
#define WIFI_PASSWORD_TEST      "987654321" // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL
#define WIFI_SPI_SHOW            (1)        // 如果要启用无线图传,则置为1
uint8 image_copy[MT9V03X_H][MT9V03X_W];     // 图像备份数组，在发送前将图像备份再进行发送，这样可以避免图像出现撕裂的问题
/*================================ 全局变量 ==================================*/
uint8 show_Img[MT9V03X_H][MT9V03X_W];/**< 用来展示图片*/
uint16 Ostu_Thres;/**< 大津法后的阈值*/
/**< Robert阈值*/
uint32 th_edge=11;
uint32 sobelThres=100;//sobel阈值
float inv_rptsn[MT9V03X_H][2];/**< 变换前中线的位置*/
float inv_aim_idx[2];
float ANGLE=0;//实际传入的角度
int farline_type=0;//右圆环寻左远线的标志位，左圆环同理
int Count_ang_Flag=0;//开陀螺仪标志位
int count_break_obs=0;//对路障，短路计数，用的同一种判别方法
uint8 if_lost_left_line = 0,if_lost_right_line = 0;//是否丢线
int tof_flag=0;//tof标志位
int c_g_ciecleflag=0;
int total_angel_z;
int start=0;
int launch_car=1;
float off_setz =0;    //z轴角速度偏置，去零飘用
float power_level = 0;
image_t img_raw = DEF_IMAGE(NULL, MT9V03X_W, MT9V03X_H);
//按键调参的标志
int8 show_pagex,show_pagey,key_pos;
int frame_vote;
bool slow_start_flag = true;
extern int8 flash_num;
bool protect_flag=0;

/********此区域debug用*********/



/********此区域debug用*********/

/********************************图显函数**************************************/
void Pimage_show(void);//显示透视后的图像
void cross_circle_Show();//在原图上展示十字的各种东西
void draw_Show();//原图划线
void change_show_page();//翻页
void Init_all();
void select_section();
void distort_show(uint8*);
void data_show();
void get_offset_and_reset_flag();
void show_power();
int key=0;
char strff[8];
char *newchar;

int main (void)
{
    Init_all();     //初始化所有
    select_section();
    if(flash_num>=0 && flash_num<=1)
    {
        Read_Load();    //加载菜单
        MainMenu_Set(); //进入菜单
        My_FlashWrite(flash_num); //读取菜单
    }else if(flash_num == 2){
        read_param_slow();   //菜单被刷时用
    }else{
        read_param_fast();   //菜单被刷时用
    }

    Motor_Init();
    tft180_clear(RGB565_WHITE);

    while(1)
    {
        frame_vote+=1;//计算帧率用
        change_show_page();
        Led_show_all();
        if(mt9v03x_finish_flag)
        {
            //tft180_clear(RGB565_BLUE);
            img_raw.data = mt9v03x_image[0];
            if(cross_type==0)
                Ostu_Thres = otsuThreshold(mt9v03x_image[0],MT9V03X_W,30,begin_y-20);
            else
                Ostu_Thres = otsuThreshold(mt9v03x_image[0],MT9V03X_W,120,0);

            process_image();
            find_corners();
             check_all();

            control_Init();//中线处理


            //屏显
            data_show();
//            tft180_displayimage03x((const uint8 *)mt9v03x_image, show_X,show_Y);
            /********此区域debug用*********/
            //Integral_vel_flag = 1;
            // debug_show_float("serr",Angle_PID.SumError*0.1 , 2);
            /********此区域debug用*********/

            // 无线图传发送图像
            #if (WIFI_SPI_SHOW == 1)
            seekfree_assistant_oscilloscope_data.data[0] = Speed_now;
            seekfree_assistant_oscilloscope_data.data[1] = pure_angle;
            seekfree_assistant_oscilloscope_data.data[2] = ang_gain;
            seekfree_assistant_oscilloscope_data.data[3] = vel_gain;
            seekfree_assistant_oscilloscope_data.data[4] = imu_data;
            seekfree_assistant_oscilloscope_data.data[5] = Motor.PWM_fan_up;
            seekfree_assistant_oscilloscope_data.data[6] = Motor.PWM_fan_down;
            seekfree_assistant_oscilloscope_data.data[7] = farline_type;
            // 设置本次需要发送几个通道的数据
            seekfree_assistant_oscilloscope_data.channel_num = 8;
            // 最大支持8通道
            seekfree_assistant_oscilloscope_send(&seekfree_assistant_oscilloscope_data);
            system_delay_ms(20);
            seekfree_assistant_data_analysis();
        #endif
            mt9v03x_finish_flag = 0;
        }
    }
}

//翻页
void change_show_page()
{
    key =Key_Scan();
    if(key == KEY_RIGHT)
    {
        show_pagex++;
        tft180_clear(RGB565_WHITE);
    }

    if(key==KEY_LEFT)
    {
        show_pagex--;
        tft180_clear(RGB565_WHITE);
    }

    if(key == KEY_UP)
    {
        show_pagey++;
        tft180_clear(RGB565_WHITE);
    }
    if(key == KEY_DOWN)
    {
        show_pagey--;
        tft180_clear(RGB565_WHITE);
    }

    if(show_pagex>4)
    {
        show_pagex = 0;
    }
    else if(show_pagex<0)
    {
        show_pagex= 3;
    }

}
//展示各种参数
void data_show(void)
{

    switch (show_pagey) {
        case 0:
            //正常显示区域
            if(show_pagex==0){
                    draw_Show();
                    //测定偏置以及获得电池电量
                    get_offset_and_reset_flag();
                    show_power();
                    //第1列存放各种标志位
                    tft180_show_int   (1,64,mpu6050_gyro_z,5,RGB565_RED,RGB565_WHITE);
                    tft180_show_float (1, 80, off_setz, 2,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (1,96,error,4,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1,112,pure_angle,4,RGB565_RED,RGB565_WHITE);
                    //第2列存放近角点
                    tft180_show_int (35, 64,circle_obstacle_flag,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (35, 80,cross_type,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (35,96,ipts0_num,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (35,112,ipts1_num,3,RGB565_RED,RGB565_WHITE);
                    //第3列存放远角点
                    tft180_show_int (70, 64,garage_type,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (70, 80,total_distance,4,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (70, 96,Lpt0_found,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (70, 112,Lpt1_found,1,RGB565_RED,RGB565_WHITE);

                    //第4列下半存放远近边线长度
                    tft180_show_int   (105,32,conf0_max,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,48,conf1_max,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,64,Lpt0_rpts0s_id,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,80,Lpt1_rpts1s_id,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,96,far_Lpt0_found, 3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,112,far_Lpt1_found, 3,RGB565_RED,RGB565_WHITE);

                    tft180_show_int (140, 32,lose_count,3,RGB565_RED,RGB565_WHITE);
//                    tft180_show_int (140, 32,dir_rightnum0,2,RGB565_RED,RGB565_WHITE);
//                    tft180_show_int (140, 48,dir_leftnum1,2,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (140, 64,is_longstraight0,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (140, 80,is_longstraight1,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (140, 96,is_straight0,1,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (140, 112,is_straight1,1,RGB565_RED,RGB565_WHITE);
                }
                else if(show_pagex==1)
                {
                    //tft180_show_int (1, 0,gain,4,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1, 16,Speed_now,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1, 32,imu_data,4,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1, 48,Ostu_Thres,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1, 64,(int)inv_far_Lpt1[0],3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1, 80,(int)inv_far_Lpt1[1],3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1,96,Lpt0_found,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int (1,112,Lpt1_found,3,RGB565_RED,RGB565_WHITE);
                    //第二列
                    tft180_show_int   (35,96,(int)dir_leftnum1,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (35,112,(int)dir_rightnum0,3,RGB565_RED,RGB565_WHITE);
                    //第三列
                    tft180_show_int   (70,64,mpu6050_gyro_x,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (70,80,mpu6050_gyro_y,3,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (70,96,mpu6050_gyro_z,3,RGB565_RED,RGB565_WHITE);
                    //第四列
                    tft180_show_int   (105,64,is_straight0, 2,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,80,is_straight1, 2,RGB565_RED,RGB565_WHITE);
                    tft180_show_int   (105,96,pure_angle, 2,RGB565_RED,RGB565_WHITE);
                    tft180_show_float(105, 112, angle, 2,1,RGB565_RED,RGB565_WHITE);
                }
                else if(show_pagex==2)
                {
                    static bool flag = 0;
                    if(garage_type==GARAGE_STOP)motorflag=0;
                    else motorflag=1;
                    if(flag == 0)
                    {
                        system_delay_ms(1000);
                        flag = 1;
                        slow_start_flag = false;
                    }
                }
                else if(show_pagex==3)
               {
                    //关电机
                  motorflag=0;
               }
            break;
        case 1:
            //debug data 区域
            for(int i = 0; i <= 7; i ++)
            {
                tft180_show_string(0, i*16, data_name_int[i],RGB565_BLUE,RGB565_WHITE);
                tft180_show_int (45, i*16 ,debug_data_int[i],4,RGB565_RED,RGB565_WHITE);
            }

            for(int i = 0; i <= 7; i++)
            {
               // tft180_show_string(80, i*16, data_name_float[i],RGB565_BLUE,RGB565_WHITE);
                tft180_show_float (115, i*16,debug_data_float[i],3,3,RGB565_RED,RGB565_WHITE);
            }
            break;
        case 2:
            //图像debug区域
            if(show_pagex==0){
                //显示原图，画边线
                draw_Show();

                //第1列
                tft180_show_int (1, 64,ipts0[0][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 80,ipts1[0][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 96,obstacle_type,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 112,total_distance,3,RGB565_RED,RGB565_WHITE);
                //第2列
                tft180_show_int (35, 64,ipts0[ipts0_num][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35, 80,ipts1[ipts1_num][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,96,rpts0s[rpts0s_num][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,112,rpts0s[rpts1s_num][0],3,RGB565_RED,RGB565_WHITE);

                //第3列
                tft180_show_int (70, 64,Lpt0_s_rpts0s_id,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70, 80,Lpt1_s_rpts1s_id,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70,96,Lpt0_s_found,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70,112,Lpt1_s_found,3,RGB565_RED,RGB565_WHITE);

                //第4列下半存放远近边线长度
                tft180_show_int   (105,32,Lpt0_found,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int   (105,48,Lpt1_found,3,RGB565_RED,RGB565_WHITE);

                tft180_show_int   (105,64,conf0_max,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int   (105,80,conf1_max,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int   (105,96,far_conf0_max, 3,RGB565_RED,RGB565_WHITE);
                tft180_show_int   (105,112,far_conf1_max, 3,RGB565_RED,RGB565_WHITE);

                tft180_show_int (140, 32,dir_rightnum0,2,RGB565_RED,RGB565_WHITE);
                tft180_show_int (140, 48,dir_leftnum1,2,RGB565_RED,RGB565_WHITE);
                tft180_show_int (140, 64,is_longstraight0,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (140, 80,is_longstraight1,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (140, 96,is_straight0,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (140, 112,is_straight1,1,RGB565_RED,RGB565_WHITE);

            }
            else if(show_pagex==1)
            {
                //全图sobel
//                uint8 show_Img[MT9V03X_H][MT9V03X_W];
//                sobelThreshold(img_raw.data,show_Img,img_raw.width,img_raw.height,sobelThres);
//                tft180_displayimage03x((const uint8 *)show_Img, show_X,show_Y);
                Pimage_show();
                //边线
                lcd_Show_Line(ipts0_num,ipts0,RGB565_RED);
                lcd_Show_Line(ipts1_num,ipts1,RGB565_BLUE);
                //lcd_Show_inv_Line(rptsn_num,rptsn,RGB565_PURPLE);
                //起始点
                tft180_draw_point(  (int)((ipts0[0][0])/x_Zoom) , (int)(ipts0[0][1]/y_Zoom) , RGB565_YELLOW   );
                tft180_draw_point(  (int)((ipts1[0][0])/x_Zoom) , (int)(ipts1[0][1]/y_Zoom) , RGB565_YELLOW   );

                //第1列
                tft180_show_int (1, 64,ipts0[0][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 80,ipts1[0][0],3,RGB565_RED,RGB565_WHITE);
                //第2列
                tft180_show_float (35, 64,sobel0,3,1,RGB565_RED,RGB565_WHITE);
                tft180_show_float (35, 80,sobel1,3,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,96,ipts0_num,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,112,ipts1_num,3,RGB565_RED,RGB565_WHITE);
            }
            else if(show_pagex==2)
            {
                //大津法二值化
                uint8 show_Img[MT9V03X_H][MT9V03X_W];
                for(int i=0;i<MT9V03X_H;i++)
                    for(int j=0;j<MT9V03X_W;j++)
                        show_Img[i][j]=mt9v03x_image[i][j] <= Ostu_Thres ? 0 : 0xffff;
                tft180_displayimage03x((const uint8 *)show_Img, show_X,show_Y);
                //边线
                lcd_Show_Line(ipts0_num,ipts0,RGB565_RED);
                lcd_Show_Line(ipts1_num,ipts1,RGB565_BLUE);
                tft180_draw_point(  (int)((zebraL_x)/x_Zoom) , (int)(ipts0[1][1]/y_Zoom) , RGB565_PURPLE   );
                tft180_draw_point(  (int)((zebraR_x)/x_Zoom) , (int)(ipts1[1][1]/y_Zoom) , RGB565_PURPLE   );
                //lcd_Show_in1v_Line(rptsn_num,rptsn,RGB565_PURPLE);
                //起始点
//                tft180_draw_point(  (int)((ipts0[0][0])/x_Zoom) , (int)(ipts0[0][1]/y_Zoom) , RGB565_YELLOW   );
//                tft180_draw_point(  (int)((ipts1[0][0])/x_Zoom) , (int)(ipts1[0][1]/y_Zoom) , RGB565_YELLOW   );


//                tft180_show_int (1,80,Ostu_Thres,3,RGB565_RED,RGB565_WHITE);
//                tft180_show_int (1,96,mt9v03x_image[ipts0[0][0]][ipts0[0][1]],3,RGB565_RED,RGB565_WHITE);
//                tft180_show_int (1,112,mt9v03x_image[ipts1[0][0]][ipts1[0][1]],3,RGB565_RED,RGB565_WHITE);
                //第1列
                tft180_show_int (1, 64,ipts0[0][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 80,ipts1[0][0],3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 96,motorflag,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (1, 112,total_distance,3,RGB565_RED,RGB565_WHITE);
                //第2列
//                tft180_show_float (35, 64,sobel0,3,1,RGB565_RED,RGB565_WHITE);
//                tft180_show_float (35, 80,sobel1,3,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,96,ipts0_num,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (35,112,ipts1_num,3,RGB565_RED,RGB565_WHITE);

                tft180_show_int (70, 64,garage_type,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70, 80,total_distance,4,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70, 96,zebra_L_flag,1,RGB565_RED,RGB565_WHITE);
                tft180_show_int (70, 112,zebra_R_flag,1,RGB565_RED,RGB565_WHITE);

                tft180_show_int (105, 96,zebraL_x,3,RGB565_RED,RGB565_WHITE);
                tft180_show_int (105, 112,zebraR_x,3,RGB565_RED,RGB565_WHITE);
            }


            break;
        default:
            break;
    }

}

void show_power()
{
    power_level = (float)Sliding_Filter(&Power_level,adc_convert(ADC1_IN9_B1),0)* 0.1578*0.893;
    tft180_show_float (105, 16, power_level , 3,1,RGB565_BLACK,RGB565_WHITE);
    tft180_show_char (140, 16, 'V' , RGB565_BLACK,RGB565_WHITE);
}

//展示圆环，十字
void cross_circle_Show(void)
{
    if(cross_type!=CROSS_NONE||circle_type==CIRCLE_LEFT_IN||circle_type==CIRCLE_RIGHT_IN||circle_obstacle_flag)
    {
                tft180_draw_point((int)((inv_Lpt0[0])/x_Zoom),(int)(inv_Lpt0[1]/y_Zoom),RGB565_PURPLE   );//左角点
                tft180_draw_point(  (int)(far_Show_x0/x_Zoom) , (int)(far_Show_y0/y_Zoom) , RGB565_YELLOW   );//左远起始点
                tft180_draw_point((int)((inv_far_Lpt0[0])/x_Zoom),(int)(inv_far_Lpt0[1]/y_Zoom),RGB565_WHITE   );//左远角点
                lcd_Show_Line(far_ipts0_num,far_ipts0,RGB565_RED);//左远线

                tft180_draw_point((int)((inv_Lpt1[0])/x_Zoom),(int)(inv_Lpt1[1]/y_Zoom),RGB565_PURPLE   );//右角点
                tft180_draw_point(  (int)(far_Show_x1/x_Zoom) , (int)(far_Show_y1/y_Zoom) , RGB565_RED   );//右远起始点
                tft180_draw_point((int)((inv_far_Lpt1[0])/x_Zoom),(int)(inv_far_Lpt1[1]/y_Zoom),RGB565_GREEN   );//右远角点
                lcd_Show_Line(far_ipts1_num,far_ipts1,RGB565_BLUE);//右远线
    }
}

//透视变换展示
void Pimage_show(void)
{
  int X;
  int Y;
    for(int i=0;i<MT9V03X_H;i++){
        for(int j=0;j<MT9V03X_W;j++)
        {
             X=round(((i*rot[0][0]+j*rot[0][1]+rot[0][2]))/(i*rot[2][0]+j*rot[2][1]+rot[2][2]));
             Y=round((i*rot[1][0]+j*rot[1][1]+rot[1][2])/(i*rot[2][0]+j*rot[2][1]+rot[2][2]));
            if(0<=X&&X<MT9V03X_H&&0<=Y&&Y<MT9V03X_W)
            {
                show_Img[X][Y]=mt9v03x_image[i][j];
            }
        }
    }
    tft180_displayimage03x((const uint8 *)show_Img, show_X,show_Y);
}
//画线
void draw_Show()
{
    inv_aim_idx[0] = (inv_rot[1][0]*rptsn[aim_idx][1]+inv_rot[1][1]*rptsn[aim_idx][0]+inv_rot[1][2])/(inv_rot[2][0]*rptsn[aim_idx][1]+inv_rot[2][1]*rptsn[aim_idx][0]+inv_rot[2][2]);
    inv_aim_idx[1] = (inv_rot[0][0]*rptsn[aim_idx][1]+inv_rot[0][1]*rptsn[aim_idx][0]+inv_rot[0][2])/(inv_rot[2][0]*rptsn[aim_idx][1]+inv_rot[2][1]*rptsn[aim_idx][0]+inv_rot[2][2]);

    tft180_displayimage03x((const uint8 *)mt9v03x_image, show_X,show_Y);

    //边线
    lcd_Show_Line(ipts0_num,ipts0,RGB565_RED);
    lcd_Show_Line(ipts1_num,ipts1,RGB565_BLUE);
    lcd_Show_inv_Line(rptsn_num,rptsn,RGB565_PURPLE);

    //起始点
    tft180_draw_point(  (int)((img_raw.width / 2 - begin_x)/x_Zoom) , (int)(begin_y/y_Zoom) , RGB565_YELLOW   );
    tft180_draw_point(  (int)((img_raw.width / 2 + begin_x)/x_Zoom) , (int)(begin_y/y_Zoom) , RGB565_YELLOW   );


    if((inv_aim_idx[0])/x_Zoom<show_X&&inv_aim_idx[1]/y_Zoom<show_Y)
    tft180_draw_point((int)((inv_aim_idx[0])/x_Zoom) , (int)(inv_aim_idx[1]/y_Zoom) ,     RGB565_YELLOW   );//预瞄点
    cross_circle_Show();
     //circle_Show();
}


void get_offset_and_reset_flag(void)
{

    if(gpio_get_level(D9)==1){
        system_delay_ms(5);
        while(gpio_get_level(D9));
          //获取陀螺仪偏置
          for(uint8 i=0;i<100;i++)        //采集100次
          {
              //mpu6050_get_gyro();//获取陀螺仪角速度
              off_setz += mpu6050_gyro_z;
              system_delay_ms(5);   //采样周期
          }
          off_setz /= 100;

          /******所有元素标志清零*******/
          //garage_type = GARAGE_NONE;
          circle_type = CIRCLE_NONE;
          cross_type = CROSS_NONE;
          obstacle_type = OBSTACLE_NONE;
          straight_road_type = STRAIGHT_NONE;
    }
}

void select_section()
{
    while(1){
        key=Key_Scan();
        tft180_show_string(0, 0, "The current version ",RGB565_RED,RGB565_WHITE);
        power_level =  adc_convert(ADC1_IN9_B1);
        tft180_show_float (120, 112, (float)power_level * 0.1578*0.893, 3,1,RGB565_BLACK,RGB565_WHITE);
        tft180_show_char (150, 112, 'V' , RGB565_BLACK,RGB565_WHITE);
        switch (flash_num) {
            case 0:
                tft180_show_string(65,70 , "slow",RGB565_BLUE,RGB565_WHITE);
                  break;
            case 1:
                tft180_show_string(65,70 , "fast",RGB565_BLUE,RGB565_WHITE);
                  break;
            case 2:
                tft180_show_string(45,70 , "etc_slow",RGB565_BLUE,RGB565_WHITE);
                  break;
            case 3:
                tft180_show_string(45,70 , "etc_fast",RGB565_BLUE,RGB565_WHITE);
                  break;
            default:
                tft180_show_string(0,70 , "error,switch back",RGB565_RED,RGB565_WHITE);
                break;
        }


        if(key == KEY_RIGHT)
        {
            flash_num++;
            tft180_clear(RGB565_WHITE);
        }
        if(key == KEY_LEFT)
        {
            flash_num--;
            tft180_clear(RGB565_WHITE);

        }
        if(key == KEY_MID)
        {
            break;
        }
    }
}
//初始化所有
void Init_all(void)
{
    /*系统及各外设初始化*/
    clock_init(SYSTEM_CLOCK_144M);  // 初始化芯片时钟 工作频率为 144MHz
    debug_init();                   // 初始化默认 Debug UART
    mt9v03x_init();
    dual_com_init();
    /*无线图传初始化*/
    #if (WIFI_SPI_SHOW == 1)
    while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
        {
            printf("\r\n connect wifi failed. \r\n");
            system_delay_ms(100);                                                   // 初始化失败 等待 100ms
        }

        printf("\r\n module version:%s",wifi_spi_version);                          // 模块固件版本
        printf("\r\n module mac    :%s",wifi_spi_mac_addr);                         // 模块 MAC 信息
        printf("\r\n module ip     :%s",wifi_spi_ip_addr_port);                     // 模块 IP 地址

        // zf_device_wifi_spi.h 文件内的宏定义可以更改模块连接(建立) WIFI 之后，是否自动连接 TCP 服务器、创建 UDP 连接、创建 TCP 服务器等操作
        if(1 != WIFI_SPI_AUTO_CONNECT)                                              // 如果没有开启自动连接 就需要手动连接目标 IP
        {
            while(wifi_spi_socket_connect(                                          // 向指定目标 IP 的端口建立 TCP 连接
                "TCP",                                                              // 指定使用TCP方式通讯
                WIFI_SPI_TARGET_IP,                                                 // 指定远端的IP地址，填写上位机的IP地址
                WIFI_SPI_TARGET_PORT,                                               // 指定远端的端口号，填写上位机的端口号，通常上位机默认是8080
                WIFI_SPI_LOCAL_PORT))                                               // 指定本机的端口号
            {
                // 如果一直建立失败 考虑一下是不是没有接硬件复位
                printf("\r\n Connect TCP Servers error, try again.");
                system_delay_ms(100);                                               // 建立连接失败 等待 100ms
            }
        }
        // 逐飞助手初始化 数据传输使用高速WIFI SPI
        seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
        // 发送总钻风图像信息(仅包含原始图像信息)
#endif

    tft180_set_dir(TFT180_CROSSWISE_180 );//屏幕设置方向
    tft180_init ();                       //屏幕初始化
    Key_Init();
    mpu6050_init ();
    //W25QXX_Init();
    gpio_init(D8,GPI,0,GPI_FLOATING_IN);

    gpio_init(E12,GPO,0,GPO_PUSH_PULL);
    gpio_init(E13,GPO,0,GPO_PUSH_PULL);
    gpio_init(E14,GPO,0,GPO_PUSH_PULL);
    gpio_init(E15,GPO,0,GPO_PUSH_PULL);
    gpio_set_level(E12, 1);
    adc_init(ADC1_IN9_B1, ADC_8BIT);
    adc_convert(ADC1_IN9_B1);
    encoder_dir_init(TIM3_ENCOEDER, TIM3_ENCOEDER_MAP3_CH1_C6, TIM3_ENCOEDER_MAP3_CH2_C7);


    /*中断初始化*/
    pit_ms_init(TIM6_PIT,10);
    pit_ms_init(TIM7_PIT,1);
    pit_ms_init(TIM8_PIT,1000);
    /*软件初始化*/
    PID_Init();
    Filters_Init();

}

