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
* 文件名称          zf_common_headfile
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

#ifndef __HEADFILE_H
#define __HEADFILE_H




#define TFT_X_MAX   160 //液晶X方宽度
#define TFT_Y_MAX   128 //液晶Y方宽度
#define LCD_W 160//液晶X方宽度
#define LCD_H 128//液晶Y方宽度
//===================================================C语言 函数库===================================================
#include "math.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

//===================================================芯片 SDK 底层===================================================
#include "ch32v30x_adc.h"
#include "ch32v30x_bkp.h"
#include "ch32v30x_can.h"
#include "ch32v30x_crc.h"
#include "ch32v30x_dac.h"
#include "ch32v30x_dbgmcu.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_exti.h"
#include "ch32v30x_flash.h"
#include "ch32v30x_fsmc.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_i2c.h"
#include "ch32v30x_iwdg.h"
#include "ch32v30x_pwr.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_rtc.h"
#include "ch32v30x_sdio.h"
#include "ch32v30x_spi.h"
#include "ch32v30x_tim.h"
#include "ch32v30x_usart.h"
#include "ch32v30x_wwdg.h"


//===================================================芯片 SDK 底层===================================================

//====================================================开源库公共层====================================================
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_common_interrupt.h"
#include "zf_common_fifo.h"
#include "zf_common_typedef.h"
//====================================================开源库公共层====================================================

//===================================================芯片外设驱动层===================================================
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_dvp.h"
#include "zf_driver_encoder.h"
#include "zf_driver_exti.h"
#include "zf_driver_flash.h"
#include "zf_driver_gpio.h"
//#include "zf_driver_iic.h"
#include "zf_driver_pit.h"
#include "zf_driver_pwm.h"
//#include "zf_driver_soft_iic.h"
//#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"
#include "zf_driver_timer.h"
#include "zf_driver_uart.h"
#include "zf_driver_usb_cdc.h"

//===================================================芯片外设驱动层===================================================

//===================================================外接设备驱动层===================================================
#include "zf_device_camera.h"
#include "zf_device_icm20602.h"
#include "zf_device_ips114.h"
#include "zf_device_tft180.h"
#include "zf_device_ips200.h"
#include "zf_device_mt9v03x_dvp.h"
#include "zf_device_mpu6050.h"
#include "zf_device_type.h"
#include "zf_device_wireless_uart.h"
#include "zf_device_oled.h"
#include "zf_device_scc8660_dvp.h"
#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_wireless_ch573.h"
#include "zf_device_wireless_uart.h"
#include "zf_device_virtual_oscilloscope.h"
#include "zf_device_w25q32.h"
#include "zf_device_k24c02.h"
#include "zf_device_aht20.h"
#include "zf_device_wifi_uart.h"
#include "zf_device_imu660ra.h"
#include "zf_device_imu963ra.h"
#include "zf_device_key.h"
#include "zf_device_gps_tau1201.h"
#include "zf_device_dl1a.h"
#include "zf_device_dm1xa.h"
#include "zf_device_wifi_spi.h"
//===================================================外接设备驱动层===================================================


//===================================================应用组件层===================================================
#include "seekfree_assistant.h"
#include "seekfree_assistant_interface.h"
//===================================================应用组件层===================================================

//===================================================用户自定义文件===================================================
#define MT9V03X_W2               (188)                                           // 用于边线x
#define MT9V03X_H2               (120)
#define PATCH_LENTH              (188)                                          // 用于边线y
#define LINE_LENTH               (120)
extern uint8 show_Img[MT9V03X_H][MT9V03X_W];/**< 用来展示图片*/
extern uint16 Ostu_Thres;/**< 大津法后的阈值*/
extern uint32 th_edge;/**< Robert阈值*/
extern uint32 sobelThres;
extern float inv_rptsn[MT9V03X_H][2];/**< 变换前中线的位置*/
extern float inv_aim_idx[2];
extern float ANGLE;
extern int Count_dis_Flag;
extern int Count_ang_Flag;
extern int count_break_obs;
extern int total_angel_z;
extern int start;
extern int total_distance;
extern int circle_garage_type;
extern int c_g_ciecleflag;
extern int farline_type;
extern int tof_flag;
extern int launch_car;
#define beepOn  gpio_set_level(E1,1)
#define beepOff gpio_set_level(E1,0)
//===================================================用户自定义文件===================================================
#endif

