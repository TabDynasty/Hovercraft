#include <stdint.h>
#include <string.h>
#include "Dual.h"
#include "zf_driver_uart.h"

#define _UART_INDEX        UART_7                                      // 无线串口对应使用的串口号
#define _UART_BAUDRATE     115200                                      // 无线串口对应使用的串口波特率
#define _UART_TX_PIN       UART7_MAP3_TX_E12                           // 无线串口对应模块的 TX 要接到单片机的 RX
#define _UART_RX_PIN       UART7_MAP3_RX_E13                           // 无线串口对应模块的 RX 要接到单片机的 TX

#define _UART_BUFFER_MAX   64

#define NO_POWER_STR            "Low pwr\r\n"
#define CHARGE_FINISH_STR       "Chg OK\r\n"


static DUAL_CAR_EVENT   msg_event = 0;
static uint8_t          uart_get_flag = 0;
static uint8_t          uart_get_data[_UART_BUFFER_MAX] = {0};                 // 串口接收数据缓冲区
static uint32_t         uart_data_count = 0;               // 数据个数



/*================================ 函数 ==================================*/
/* init system uart which will be used to communicate  */
static int sys_uart_init(void)
{
    uart_init(_UART_INDEX, _UART_BAUDRATE, _UART_TX_PIN, _UART_RX_PIN);             // 初始化编码器模块与引脚 正交解码编码器模式
    uart_rx_interrupt(_UART_INDEX, 1);
    return 0;
}

static uint8_t sys_uart_read_byte(void)
{
    return uart_read_byte(_UART_INDEX);
}

static void sys_uart_write_str(const char *str)
{
    uart_write_string(_UART_INDEX, str);
}

/* 解析字符串并获得串口传输的事件 */
static DUAL_CAR_EVENT get_uart_event(uint8_t *buff)
{
    if(strstr((const char*)buff, NO_POWER_STR)!= NULL)
    {
        return REC_NO_POWER;
    }
    else if(strstr((const char*)buff, CHARGE_FINISH_STR)!= NULL)
    {
        return REC_CHARGE_FINISH;
    }
    else
    {
        return REC_NO_MSG;
    }

}



/*================================ 接口函数 ==================================*/

int dual_com_init(void)
{
    return sys_uart_init();
}

int dual_com_deinit(void)
{
    return 0;
}

int dual_com_read(void)
{
    int event = msg_event;
    msg_event = REC_NO_MSG;
    uart_get_flag = 0;
    return event;
}

int dual_com_write(DUAL_CAR_EVENT event)
{
    const char *str = NULL;
    if (event)
    {
        if(event == REC_NO_POWER)
        {
            str = NO_POWER_STR;
        }
        else if(event == REC_CHARGE_FINISH)
        {
            str = CHARGE_FINISH_STR;
        }
        else
        {
            return -1;
        }
        sys_uart_write_str(str);
        return 0;
    }
    else return -1;

}
/* PLEASE PUT THIS IN TO SYSTEM UART IRQ */
void dual_com_irq_handler(void)
{
    uint8_t data = sys_uart_read_byte();
    /* if not get full msg */
    if(!uart_get_flag)
    {
        uart_get_data[uart_data_count++] = data;
        if(uart_data_count == _UART_BUFFER_MAX){uart_data_count = 0;}
        else{
            if(uart_get_data[uart_data_count-1] == '\n'&&uart_get_data[uart_data_count-2] == '\r')
            {                            // 输出测试信息
                uart_get_data[uart_data_count] = '\0';
                msg_event = get_uart_event(uart_get_data);
                uart_data_count = 0;
                uart_get_flag = 1;
            }
        }

    }
}
