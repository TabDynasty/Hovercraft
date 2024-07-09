#include "image.h"
#include "utils.h"
#include "zf_common_headfile.h"

char data_name_int[8][4];
char data_name_float[8][4];
int debug_data_int[8]={0};
float debug_data_float[8]={0};

int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}

float fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}

int myy_min(int x,int y)
{
    if(x<y)return x;
    else return y;
}

float myy_fmin(float x,float y)
{
    if(x<y)return x;
    else return y;
}

void lcd_Show_fLine(int line_Num,float line[][2],int color)
{
    for(int i=0;i<line_Num;i++)
        for(int y=0;y<1;y++)
            if(1<((line[i][0]+y)/x_Zoom)&&((line[i][0]+y)/x_Zoom)<show_X&&1<(line[i][1]/y_Zoom)&&(line[i][1]/y_Zoom)<show_Y)
                tft180_draw_point((int)(line[i][0]+y)/x_Zoom,(int)line[i][1]/y_Zoom,color);
}

void lcd_Show_Line(int line_Num,int line[][2],int color)
{
        for(int i=0;i<line_Num;i++)
            for(int y=0;y<1;y++)
                if(1<((line[i][0]+y)/x_Zoom)&&((line[i][0]+y)/x_Zoom)<show_X&&1<(line[i][1]/y_Zoom)&&(line[i][1]/y_Zoom)<show_Y)
                    tft180_draw_point((int)(line[i][0]+y)/x_Zoom,(int)line[i][1]/y_Zoom,color);
}

void lcd_Show_inv_Line(int num,float matrix[][2],uint16 color)
{
    float inv_matrix[num][2];
    for(int i=0;i<num;i++)
    {
        inv_matrix[i][0]=Cal_inv_rot_x(matrix[i][0],matrix[i][1]);
        inv_matrix[i][1]=Cal_inv_rot_y(matrix[i][0],matrix[i][1]);
    }
    lcd_Show_fLine(num,inv_matrix,color);
}


float Cal_rot_x(float x,float y){
    float rot_x;
    rot_x = (rot[1][0]*y+rot[1][1]*x+rot[1][2])/(rot[2][0]*y+rot[2][1]*x+rot[2][2]);
    return rot_x;
}

float Cal_rot_y(float x,float y){
    float rot_y;
    rot_y = (rot[0][0]*y+rot[0][1]*x+rot[0][2])/(rot[2][0]*y+rot[2][1]*x+rot[2][2]);
    return rot_y;
}

float Cal_inv_rot_x(float x,float y){
    float inv_rot_x;
    inv_rot_x = (inv_rot[1][0]*y + inv_rot[1][1]*x + inv_rot[1][2])/(inv_rot[2][0]*y+inv_rot[2][1]*x+1);
    return inv_rot_x;
}

float Cal_inv_rot_y(float x,float y){
    float inv_rot_y;
    inv_rot_y = (inv_rot[0][0]*y + inv_rot[0][1]*x + inv_rot[0][2])/(inv_rot[2][0]*y+inv_rot[2][1]*x+1);
    return inv_rot_y;
}

/******************************************************************************
* FunctionName   : patch_Line()
* Description    : 两点连直线 直线第一个点x为最小值
* EntryParameter : x1,y1-第一个点坐标 x2,y2-第二个点坐标 *num 返回数组长度
* ReturnValue    : 返回直线数组,[0]存x,[1]存y
*******************************************************************************/
int patch_Line (int pts[][2], int startX, int startY, int endX, int endY, int num, int pts_num)
{
    int i = 0, j = 0;
    // 直线 x = ky + b
    float k = 0.0f, b = 0.0f;

    k = (float) ((float) endX - (float) startX) / (float) ((float) endY - (float) startY);
    b = (float) startX - (float) startY * k;

    for (i = startY, j = num; i > endY; i--, j++)
    {
        if (j >= pts_num)
            break;
        pts[j][0] = (int) (k * i + b);
        pts[j][1] = (int) i;

    }

    return j - 1;//返回最后一个点的位置

}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     用于显示函数内部整型局部变量，想要显示的变量会列在一个独立的专门用来显示需要debug的变量的区域
// 参数说明     data_name[]        可以为你想要显示的变量取一个不超过4个字符的名字
// 参数说明     debug_data         要显示的整型变量
// 返回参数     index              你想要显示的序号(0~7)
//-------------------------------------------------------------------------------------------------------------------
void debug_show_int(char data_name[], int debug_data , int index)
{
    debug_data_int[index] = debug_data;
    for (int j = 0 ;j <= 3;j++)
    {
        data_name_int[index][j]  = data_name[j];
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     用于显示函数内部浮点型局部变量，想要显示的变量会列在一个独立的专门用来显示需要debug的变量的区域
// 参数说明     data_name[]        可以为你想要显示的变量取一个不超过4个字符的名字
// 参数说明     debug_data         要显示的浮点型变量
// 返回参数     index              你想要显示的序号 (0~7)
//-------------------------------------------------------------------------------------------------------------------
void debug_show_float(char data_name[], float debug_data , int index)
{
    debug_data_float[index] = debug_data;
    for (int j = 0 ;j <= 3;j++)
    {
        data_name_float[index][j]  = data_name[j];
    }
}


