#ifndef UTILS_H_
#define UTILS_H_
#include "zf_common_headfile.h"
//#define x_Zoom (1.55)  /**< 188/121*/
//#define y_Zoom (1.519) /**< 120/79*/
//#define show_X (121)
//#define show_Y (79)
#define x_Zoom (1.86)  /**< 188/101*/
#define y_Zoom (1.875) /**< 120/64*/
#define show_X (101)
#define show_Y (64)

extern char data_name_int[8][4];
extern char data_name_float[8][4];
extern int debug_data_int[8];
extern float debug_data_float[8];

int clip(int x, int low, int up);
float fclip(float x, float low, float up);
int myy_min(int x,int y);
float myy_fmin(float x,float y);
void lcd_Show_Line(int line_Num,int line[][2],int color);
void lcd_Show_fLine(int line_Num,float line[][2],int color);
void lcd_Show_inv_Line(int num,float matrix[][2],uint16 color);
float Cal_rot_x(float x,float y);
float Cal_rot_y(float x,float y);
float Cal_inv_rot_x(float x,float y);
float Cal_inv_rot_y(float x,float y);
void debug_show_int(char data_name[], int debug_data , int index);
void debug_show_float(char data_name[], float debug_data , int index);
void section_Off(void);
void section_On_1(void);
void section_On_2(void);
void section_On_3(void);
void section_On_4(void);
#endif /* UTILS_H_ */
