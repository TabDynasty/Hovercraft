#include "key.h"
#include "zf_common_headfile.h"
#include "utils.h"


#define KB1     gpio_get_level(KEY_UP_PORT)
#define KB2     gpio_get_level(KEY_DOWN_PORT)
#define KB3     gpio_get_level(KEY_LEFT_PORT)
#define KB4     gpio_get_level(KEY_RIGHT_PORT)
#define KB5     gpio_get_level(KEY_MID_PORT)

void Key_Init(void)
{
    //gpio_init(KEY_GND_PORT,GPO,1,GPI_FLOATING_IN);
    gpio_init(KEY_UP_PORT,GPI,0,GPI_PULL_DOWN);
    gpio_init(KEY_DOWN_PORT,GPI,0,GPI_PULL_DOWN);
    gpio_init(KEY_LEFT_PORT,GPI,0,GPI_PULL_DOWN);
    gpio_init(KEY_RIGHT_PORT,GPI,0,GPI_PULL_DOWN);
    gpio_init(KEY_MID_PORT,GPI,0,GPI_PULL_DOWN);

}

//五项按键
int Key_Scan(void)
{
    static _Bool key_up=1;

    if(key_up&&(KB1||KB2||KB3||KB4||KB5))
    {
        system_delay_ms(10);//去抖动
        key_up=0;
        if     (KB1)
            return KEY_UP;
        else if(KB2)
            return KEY_DOWN;
        else if(KB3)
            return KEY_LEFT;
        else if(KB4)
            return KEY_RIGHT;
        else if(KB5)
            return KEY_MID;
    }
    else if(!KB1&&!KB2&&!KB3&&!KB4&&!KB5)
    {
        key_up=1;
    }
    return 0;//无按键按下
}

//学习板四独立按键
int Key_Scan4()
{
    int key1,key2,key3,key4;
    key_scanner();

    key1=key_get_state(KEY_1);
    key2=key_get_state(KEY_2);
    key3=key_get_state(KEY_3);
    key4=key_get_state(KEY_4);

    if(key1 == 1)
    {
        return KEY_UP;
    }

    if(key2 == 1)
    {
        return KEY_DOWN;
    }
    if(key3 == 1)
    {
        return KEY_LEFT;
    }
    if(key4 == 1)
    {
        return KEY_RIGHT;
    }
    if(key4 == 2)
    {
        return KEY_MID;
    }

    return 0;
}

