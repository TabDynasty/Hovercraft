/*
 * menu.h
 *
 *  Created on: 2023年7月12日
 *      Author: 唐纪元
 */
/*=============================== 头文件包含 =================================*/
#include "zf_common_headfile.h"
#include "key.h"
/*============================= 结构体联合体定义 =============================*/
typedef struct{
        int x;
        int y;
}Site_t;

typedef struct{

    uint8 ExitMark;     // 退出菜单(0-不退出，1-退出)

    uint8 Cursor;       // 光标值(当前光标位置)

    uint8 PageNo;       // 菜单页(显示开始项)

    uint8 Index;        // 菜单索引(当前选择的菜单项)

    uint8 DispNum;      // 显示项数(每页可以现在菜单项)

    uint8 MaxPage;      // 最大页数(最大有多少种显示页)

}MENU_PRMT;      // 菜单参数


typedef struct{

    uint8 *MenuName;        // 菜单项目名称

    void(*ItemHook)(void);  // 要运行的菜单函数

    uint32 *DebugParam;     // 要调试的参数

}MENU_TABLE;     // 菜单执行
void Menu_Process(uint8 *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8 num);
void Menu_PrmtInit(MENU_PRMT *prmt, uint8 num, uint8 page);
void Menu_Display(MENU_TABLE *menuTable, uint8 pageNo, uint8 dispNum, uint8 cursor);
void adjustParam(Site_t site, uint32 *param, uint8 max_param_bit, uint16 Color, uint16 bkColor);
uint8 Menu_Move(MENU_PRMT *prmt, int key);
void menu_showFirst(void);
void Menu_Read_Flash();
void MainMenu_Set(void);
void Read_Load();
void Read_Flash1();
void Menu_Write_Flash();
void Write_Flash1();
void Update_Flash();
void Write_Flash2();
void Read_Flash2();
void Write_Flash3();
void Read_Flash3();

/*================================ 全局变量 ==================================*/
extern uint32 *Flash_Data[];
extern uint16 FLASHDATANUM;
extern uint32 mode_Flag;
extern uint32 motor_Flag;

extern void My_FlashWrite(int16 Boot);
extern void My_FlashRead(int16 Boot);
extern void read_param(void);

