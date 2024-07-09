#include "zf_common_headfile.h"
#include "menu.h"
#include "Key.h"
#include "W25QXX.h"
#include "Motor.h"
#include "image.h"
#include "control.h"
#include "PID.h"
#include "straight_road.h"
#include "circle.h"
/*============================= 宏定义/重定义 ================================*/
#define PAGE_DISP_NUM 6 /**< 定义LCD单页显示项目数*/
#define NUM 56 /**<数字坐标*/
/*================================ 全局变量 ==================================*/
uint8 ExitMenu_flag = 0;  /**< 菜单退出标志位*/
uint16  FLASHDATANUM;
int8 flash_num = 0;     /**< 扇区选择参数，选择不同扇区对应不同方案,每个扇区最多存50个数 */
uint32 mode_Flag=0;

uint32 *Flash_Data[] = {
                        &mode_Flag,
                        /*==图像==*/
                        &th_edge,
                        &begin_x,
                        &begin_y,
                        /*==控制==*/
                        &aimSpeed,
                        &aim_distance,
                        &centripetal_p_straight,
                        &centripetal_p_instraight,
                        /*==PID==*/
                        &Angle_vel[0],
                        &Angle_vel[1],
                        &Angle_vel[2],
//                        &Angle_vel[3],

                        &Angle_0[0],
                        &Angle_0[1],
                        &Angle_0[2],
                        //&Angle[3],
//
                        &Speed[0],
                        &Speed[1],
                        &Speed[2],
//                        &Speed[3],
                        &bottom_Speed_Max,
                        &bottom_Speed_Min,
                        &sobelThres,
                        &find_type,
                        &Speed_straight,
                        &Speed_instraight,
                        &aim_signal,

                        &Angle_1[0],
                        &Angle_1[1],
                        &Angle_1[2],

                        &Speed_circle,
                        &angle_thred1,
                        &anti_coefficient,
                        &break_dis,
                        &break_coefficient,

                        &check_straight_num,
                        &check_bend_num,
                        &check_angle_num,
                        &LcircleIn_thred,
                        &RcircleIn_thred,
                        &max_output,
                        &angle_thred2,
                        &circle_slow,
                        &start_pwm,
                        &speed_up_conf,
                        &slow_down_conf,
                       };
/*================================ 接口函数 ==================================*/
void beep_On();
void beep_Off();
void Menu_Null(void);
void Menu_BaseImg(void);
void Menu_ElemImg(void);
void Menu_Motor(void);
void Menu_Ctrl();
void Menu_PID();
//void Menu_Cross();
//void Menu_Circle();
//void Menu_Garage();
//void Menu_Ramp();
//void Menu_Beep();
//void Menu_BeepCircle();
//void Menu_BeepCross();
//void Menu_BeepGarage();
//void Menu_BeepRamp();


//----------------------------------   主菜单   -------------------------------

// 一级菜单

MENU_PRMT MainMenu_Prmt;

MENU_TABLE MainMenu_Table[] =
{
      {"0.mode_Flag",Menu_Null,&mode_Flag},
      {"1.BaseImg", Menu_BaseImg,NULL},
      {"2.ElemImg", Menu_ElemImg,NULL},
      {"3.MotorDebug",Menu_Motor,NULL},
      {"4.CtrlDebug",Menu_Ctrl,NULL},
      {"5.PIDDebug",Menu_PID,NULL},
};

//---------------------------------   二级菜单  -------------------------------\
// 二级菜单1  图像基础参数调节
MENU_PRMT BaseImg_Prmt;
MENU_TABLE BaseImg_MenuTable[] =
{
  {"0.th_edge",Menu_Null,&th_edge},
  {"1.begin_x",Menu_Null,&begin_x},
  {"2.begin_y",Menu_Null,&begin_y},
  {"3.sobelThres",Menu_Null,&sobelThres},
  {"4.find_type",Menu_Null,&find_type},
};

// 二级菜单2  元素基础参数调节
MENU_PRMT ElemImg_Prmt;
MENU_TABLE ElemImg_MenuTable[] =
{
    {"1.straight_fps",Menu_Null,&check_straight_num},
    {"2.bend_fps",Menu_Null,&check_bend_num},
    {"3.angle_fps",Menu_Null,&check_angle_num},
    {"4.LcircleIn",Menu_Null,&LcircleIn_thred},
    {"5.RcircleIn",Menu_Null,&RcircleIn_thred},
};

// 二级菜单3  电机基础参数调节
MENU_PRMT Motor_Prmt;
MENU_TABLE Motor_MenuTable[] =
{
        {"0.force_k_stra",Menu_Null,&centripetal_p_straight},
        {"1.force_k_in",Menu_Null,&centripetal_p_instraight},
        {"2.MAX",Menu_Null,&bottom_Speed_Max},
        {"3.MIN",Menu_Null,&bottom_Speed_Min},
        {"4.max_output",Menu_Null,&max_output},
        {"5.start_pwm",Menu_Null,&start_pwm},
};
// 二级菜单4  控制基础参数调节

MENU_PRMT Ctrl_Prmt;
MENU_TABLE Ctrl_MenuTable[] =
{
        {"0.S_stra",Menu_Null,&Speed_straight},
        {"1.S_instra",Menu_Null,&Speed_instraight},
        {"2.S_circle",Menu_Null,&Speed_circle},
        {"3.aim_dis",Menu_Null,&aim_distance},
        {"4.ang_thred1",Menu_Null,&angle_thred1},
        {"5.ang_thred2",Menu_Null,&angle_thred2},
        {"6.anti_coef",Menu_Null,&anti_coefficient},
        {"7.break_dis",Menu_Null,&break_dis},
        {"8.break_conf",Menu_Null,&break_coefficient},
        {"9.speed_up_conf",Menu_Null,&speed_up_conf},
        {"10.slow_down_conf",Menu_Null,&slow_down_conf},
        {"11.circ_slow",Menu_Null,&circle_slow},
};


// 二级菜单5  PID基础参数调节

MENU_PRMT PID_Prmt;
MENU_TABLE PID_MenuTable[] =
{
  {"Ang_vel_P",Menu_Null,&Angle_vel[0]},
  {"Ang_vel_I",Menu_Null,&Angle_vel[1]},
  {"Ang_vel_D",Menu_Null,&Angle_vel[2]},

  {"Ang0_P",Menu_Null,&Angle_0[0]},
  {"Ang0_I",Menu_Null,&Angle_0[1]},
  {"Ang0_D",Menu_Null,&Angle_0[2]},

  {"Ang1_P",Menu_Null,&Angle_1[0]},
  {"Ang1_I",Menu_Null,&Angle_1[1]},
  {"Ang1_D",Menu_Null,&Angle_1[2]},

  {"Speed_P",Menu_Null,&Speed[0]},
  {"Speed_I",Menu_Null,&Speed[1]},
  {"Spedd_D",Menu_Null,&Speed[2]},
};
// 二级菜单  选取Flash扇区 写存档
MENU_PRMT Read_Flash_Prmt;

MENU_TABLE Read_Flash_MenuTable[] =
{
    {"1.yunsu", Read_Flash1, NULL},
    {"2.biansu", Read_Flash2, NULL},
    {"3.biansuS", Read_Flash3, NULL},
};

// 二级菜单  读存档
MENU_PRMT Write_Flash_Prmt;

MENU_TABLE Write_Flash_MenuTable[] =
{
  {"1.yunsu", Write_Flash1, NULL},
  {"2.biansuF", Write_Flash2, NULL},
  {"3.biansuS", Write_Flash3, NULL},

};
/******************************************************************************
* FunctionName   : Menu_BaseImg()
* Description    : 基础图像设置
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Menu_BaseImg(void)
{
    tft180_clear(RGB565_BLACK);
    uint8 menuNum;
    menuNum = sizeof(BaseImg_MenuTable)/sizeof(BaseImg_MenuTable[0]);         // 菜单项数
    Menu_Process("-= BaseImg =-", &BaseImg_Prmt, BaseImg_MenuTable, menuNum);
}

/******************************************************************************
* FunctionName   : Menu_ElemImg()
* Description    : 元素设置
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Menu_ElemImg(void)
{
    tft180_clear(RGB565_BLACK);
    uint8 menuNum;
    menuNum = sizeof(ElemImg_MenuTable)/sizeof(ElemImg_MenuTable[0]);         // 菜单项数
    Menu_Process("-= ElemImg =-", &ElemImg_Prmt, ElemImg_MenuTable, menuNum);
}
/******************************************************************************
* FunctionName   : Menu_Motor()
* Description    : 控制设置
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Menu_Motor(void)
{
    tft180_clear(RGB565_BLACK);
    uint8 menuNum;
    menuNum = sizeof(Motor_MenuTable)/sizeof(Motor_MenuTable[0]);         // 菜单项数
    Menu_Process("-= MotorDebug =-", &Motor_Prmt, Motor_MenuTable, menuNum);
}

/******************************************************************************
* FunctionName   : Menu_Ctrl()
* Description    : 控制设置
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Menu_Ctrl(void)
{
    tft180_clear(RGB565_BLACK);
    uint8 menuNum;
    menuNum = sizeof(Ctrl_MenuTable)/sizeof(Ctrl_MenuTable[0]);         // 菜单项数
    Menu_Process("-= CtrlDebug =-", &Ctrl_Prmt, Ctrl_MenuTable, menuNum);
}

/******************************************************************************
* FunctionName   : Menu_PID()
* Description    : PID设置
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Menu_PID(void)
{
    tft180_clear(RGB565_BLACK);
    uint8 menuNum;
    menuNum = sizeof(PID_MenuTable)/sizeof(PID_MenuTable[0]);         // 菜单项数
    Menu_Process("-= PIDDebug =-", &PID_Prmt, PID_MenuTable, menuNum);
}

void Menu_Null(void)
{
   system_delay_ms(100);
}

/******************************************************************************
* FunctionName   : Menu_Process()
* Description    : 处理菜单项
* EntryParameter : menuName - 菜单名称，prmt - 菜单参数，table - 菜单表项, num - 菜单项数
* ReturnValue    : None
******************************************************************************/
void Menu_Process(uint8 *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8 num)
{
  int key;
  Site_t site;
  uint8 page;    //显示菜单需要的页数

  if(num - PAGE_DISP_NUM <= 0)
      page = 1;
  else
  {
      page = num - PAGE_DISP_NUM + 1;
      num = PAGE_DISP_NUM;
  }
  // 显示项数和页数设置
  Menu_PrmtInit(prmt, num, page);

  do
  {
      tft180_show_string(0, 0, menuName, RGB565_WHITE, RGB565_BLACK);    // 显示菜单标题
      // 显示分割线
//      for(uint8 y=0; y < LCD_H; y++)
//      {
//          lcd_showchar(LCD_W-1, y,'|', BLACK,WHITE);//要改
//      }
      // 显示菜单项
      Menu_Display(table, prmt->PageNo, prmt->DispNum, prmt->Cursor);

      key = Key_Scan();                       // 获取按键

      if ( Menu_Move(prmt, key) == 0 )           // 菜单移动，按下确认键
      {

        // 判断此菜单项有无需要调节的参数，有则进入参数调节
        if(table[prmt->Index].DebugParam != NULL && table[prmt->Index].ItemHook == Menu_Null)
        {
              site.x = LCD_W-NUM;   site.y = (1 + prmt->Cursor) * 16;
              tft180_show_uint(site.x,site.y, *(table[prmt->Index].DebugParam), 4,RGB565_WHITE, RGB565_BLUE);
              adjustParam(site, table[prmt->Index].DebugParam, 4, RGB565_RED, RGB565_BLUE);

        }
        // 不是参数调节的话就执行菜单函数
        else
        {
              table[prmt->Index].ItemHook();         // 执行相应项
        }
      }
  }while (prmt->ExitMark == 0 && ExitMenu_flag == 0);
  tft180_clear(RGB565_BLACK);
  system_delay_ms(50);
}


/******************************************************************************
* FunctionName   : Menu_PrmtInit()
* Description    : 初始化菜单参数
* EntryParameter : prmt - 菜单参数, num - 每页显示项数, page - 最大显示页数
* ReturnValue    : None
*******************************************************************************/
void Menu_PrmtInit(MENU_PRMT *prmt, uint8 num, uint8 page)
{
    prmt->ExitMark = 0;       //清除退出菜单标志

    prmt->Cursor   = 0;       //光标清零
    prmt->PageNo   = 0;       //页清零
    prmt->Index    = 0;       //索引清零
    prmt->DispNum  = num;     //页最多显示项目数
    prmt->MaxPage  = page;    //最多页数
}


/******************************************************************************
* FunctionName   : Menu_Display()
* Description    : 显示菜单项
* EntryParameter : menuTable - 显示页，dispNum - 每一页的显示项数，cursor - 光标位置
* ReturnValue    : None
*******************************************************************************/
void Menu_Display(MENU_TABLE *menuTable, uint8 pageNo, uint8 dispNum, uint8 cursor)
{
    uint8 i;
    Site_t site;
    for (i=0; i<dispNum; i++)
    {
      if (cursor == i)
      {
          /* 反白显示当前光标选中菜单项 */
          site.x = 0;    site.y = (i+1)*16;
          tft180_show_string(site.x,site.y,menuTable[pageNo+i].MenuName, RGB565_WHITE,RGB565_BLUE);
          /* 若此菜单项有需要调的参数，则显示该参数 */
          if(menuTable[pageNo+i].DebugParam != NULL)
          {
            site.x = LCD_W-NUM;  //数字坐标
            uint32 num_t = (*(menuTable[pageNo+i].DebugParam));
            tft180_show_uint(site.x,site.y,num_t,8,RGB565_WHITE, RGB565_BLUE);
          }
      }
      else
      {
          /* 正常显示其余菜单项 */
          site.x = 0;    site.y = (i+1)*16;
          tft180_show_string(site.x,site.y,menuTable[pageNo+i].MenuName, RGB565_WHITE,RGB565_BLACK);
          /* 若此菜单项有需要调的参数，则显示该参数 */
          if(menuTable[pageNo+i].DebugParam != NULL)
          {
              site.x = LCD_W-NUM;
              uint32 num_t = (*(menuTable[pageNo+i].DebugParam));
             // lcd_num(site, num_t, GREEN, WHITE);
              tft180_show_uint(site.x,site.y,num_t,8,RGB565_WHITE, RGB565_BLACK);
          }
      }
    }
}
/******************************************************************************
* FunctionName   : Menu_Move()
* Description    : 菜单移动
* EntryParameter :  prmt - 菜单参数, key - 按键值
* ReturnValue    : 有确认返回0，否则返回1
******************************************************************************/
uint8 Menu_Move(MENU_PRMT *prmt, int key)
{
  uint8 rValue = 1;
  switch (key)
  {
    case KEY_UP:                   // 向上
    {
      if (prmt->Cursor != 0)              // 光标不在顶端
      {
          prmt->Cursor--;                 // 光标上移
      }
      else                                // 光标在顶端
      {
          if (prmt->PageNo != 0)          // 页面没有到最小
          {
              prmt->PageNo--;             // 向上翻
          }
          else
          {
              prmt->Cursor = prmt->DispNum-1;    // 光标到底
              prmt->PageNo = prmt->MaxPage-1;    // 最后页
          }
      }
      tft180_clear(RGB565_BLACK);
      break;
    }

    case KEY_DOWN:                   // 向下
    {
      if (prmt->Cursor < prmt->DispNum-1)        // 光标没有到底，移动光标
      {
          prmt->Cursor++;                        // 光标向下移动
      }
      else                                       // 光标到底
      {
          if (prmt->PageNo < prmt->MaxPage-1)    // 页面没有到底，页面移动
          {
                prmt->PageNo++;                    // 下翻一页
          }
          else                                   // 页面和光标都到底，返回开始页
          {
                prmt->Cursor = 0;
                prmt->PageNo = 0;
          }
      }
      tft180_clear(RGB565_BLACK);
      break;
    }

    case KEY_MID:                   // 确认
    {
            prmt->Index = prmt->Cursor + prmt->PageNo;   //计算执行项的索引
            rValue = 0;
            break;
    }

    case KEY_LEFT:                   // 左键返回上级菜单
    {
            //prmt->Cursor = 0;
            //prmt->PageNo = 0;
            prmt->ExitMark = 1;
            break;
    }

    case KEY_RIGHT:                   // 右键跳到底部
    {
            prmt->Cursor = prmt->DispNum-1;             // 光标到底
            prmt->PageNo = prmt->MaxPage-1;             // 最后页
            tft180_clear(RGB565_BLACK);
                    break;
    }

    default:break;
  }

  return rValue;                    // 返回执行索引
}


/******************************************************************************
*  @brief  按键参数调整通用函数
******************************************************************************/
void adjustParam(Site_t site, uint32 *param, uint8 max_param_bit, uint16 Color, uint16 bkColor)
{
    uint8 key=-1;
    int add_INTS=1;
    while(1)
    {
        key=Key_Scan();
        if(add_INTS<=1)
            add_INTS=1;
        if(add_INTS>=1000)
            add_INTS=1000;
        switch(key)
        {
           case(KEY_UP):
              add_INTS*=10;
              //(*param)++;
              tft180_show_uint(site.x,site.y, (uint32)(*param), max_param_bit, Color, bkColor);
              break;
           case(KEY_DOWN):
              add_INTS/=10;
              //(*param)--;
              tft180_show_uint(site.x,site.y, (uint32)(*param), max_param_bit, Color, bkColor);
              break;
           case(KEY_LEFT):
               (*param)-=add_INTS;
              //(*param) -= 10;
              tft180_show_uint(site.x,site.y, (uint32)(*param), max_param_bit, Color, bkColor);
              break;
          case(KEY_RIGHT):
              (*param)+=add_INTS;
              //(*param) += 10;
              tft180_show_uint(site.x,site.y, (uint32)(*param), max_param_bit, Color, bkColor);
              break;
          case(KEY_MID):  //中间的键
               return ;
          default:
          break;
        }
        tft180_show_uint(128,0, (uint32)(add_INTS), max_param_bit, RGB565_WHITE, bkColor);
        if(*param <= 0)  *param = 0;
        if(*param >= 65535) *param = 0;
    }
}

void MainMenu_Set(void)
{
  tft180_clear(RGB565_BLACK);

  uint8 menuNum = sizeof(MainMenu_Table)/sizeof(MainMenu_Table[0]);    // 菜单项数

  Menu_Process(" -= Setting =- ", &MainMenu_Prmt, MainMenu_Table, menuNum);
//  mt9v03x_set_confing_buffer[0][1]=(int16)auto_exp;
//My_FlashWrite(1);
//Update_Flash();
  tft180_clear(RGB565_WHITE);
}

/******************************************************************************
* FunctionName   : Read_Load()
* Description    : 从flash中读取数据
* EntryParameter : None
* ReturnValue    : None
*******************************************************************************/
void Read_Load()
{
  //开机直接加载临时存档
  FLASHDATANUM =sizeof(Flash_Data)/sizeof(Flash_Data[0]); //计算Flash参数个数
  My_FlashRead(flash_num);
}

void Menu_Read_Flash(void)
{
    tft180_clear(RGB565_BLACK);

    uint8 menuNum;

    menuNum = sizeof(Read_Flash_MenuTable)/sizeof(Read_Flash_MenuTable[0]);        // 菜单项数

    Menu_Process("-=ChoseSectorR=-", &Read_Flash_Prmt, Read_Flash_MenuTable, menuNum);

}

//读不同扇区
void Read_Flash1()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 1;
  My_FlashRead(flash_num);

  tft180_show_string(5,50, "Rd1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}

//读不同扇区
void Read_Flash2()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 3;
  My_FlashRead(flash_num);

  tft180_show_string(5,50, "Rd1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}

//读不同扇区
void Read_Flash3()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 4;
  My_FlashRead(flash_num);

  tft180_show_string(5,50, "Rd1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}


void Menu_Write_Flash(void)
{
    tft180_clear(RGB565_BLACK);

    uint8 menuNum;

    menuNum = sizeof(Write_Flash_MenuTable)/sizeof(Write_Flash_MenuTable[0]);        // 菜单项数

    Menu_Process("-=ChoseSectorW=-", &Write_Flash_Prmt, Write_Flash_MenuTable, menuNum);
}

//写扇区
void Write_Flash1()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 1;
  My_FlashWrite(flash_num);

  tft180_show_string(5, 50, "Wr1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}

void Write_Flash2()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 3;
  My_FlashWrite(flash_num);

  tft180_show_string(5, 50, "Wr1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}


void Write_Flash3()
{
  tft180_clear(RGB565_BLACK);
  flash_num = 4;
  My_FlashWrite(flash_num);

  tft180_show_string(5, 50, "Wr1OK!", RGB565_RED,RGB565_WHITE);
  system_delay_ms(500);
  tft180_clear(RGB565_BLACK);
}

//EEROM版本
void My_FlashWrite(int16 Boot)
{
    //将参数读到缓冲区
    for(int i=0;i<FLASHDATANUM;i++)
    {
        flash_union_buffer[i].uint32_type = *((FLASH_WRITE_TYPE*)(Flash_Data[i])) ;
    }
    flash_write_page_from_buffer(46+Boot, 0);
    tft180_clear(RGB565_BLACK);
    tft180_show_string(0,56,"LoadOK!",RGB565_RED,RGB565_WHITE);
    system_delay_ms(1000);
}

//EEPROM版本
void My_FlashRead(int16 Boot)
{
    //将EEPOM中的数据读到数据缓冲区（256个数据）
    flash_read_page_to_buffer(46+Boot, 0);
    for(int i=0;i<FLASHDATANUM;i++)
    {
        *((FLASH_WRITE_TYPE*)(Flash_Data[i])) = flash_union_buffer[i].uint32_type;
    }
}
