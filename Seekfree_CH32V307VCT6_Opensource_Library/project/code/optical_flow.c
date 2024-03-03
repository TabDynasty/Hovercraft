#include "zf_common_headfile.h"
#include "optical_flow.h"

#define UART_INDEX              (UART_8)
#define UART_BAUDRATE           (19200)
#define UART_TX_PIN             (UART8_MAP3_TX_E14  )
#define UART_RX_PIN             (UART8_MAP3_RX_E15  )


uint8   get_data = 0;                                                       // 接收数据变量
int16_t opt_flow_speed_x,opt_flow_speed_y;
uint8   UartRxOpticalFlow[9];
static int buf_size =0;


void Optical_flow_init(void)
{
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // 初始化编码器模块与引脚 正交解码编码器模式
    uart_rx_interrupt(UART_INDEX, ZF_ENABLE);
}


void Optical_flow_uart_rx(void)
{
    //static int buf_size =0;
    uint8_t Check_sum = 0;
    static int16_t flow_x,flow_y;
    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
    UartRxOpticalFlow[buf_size++]=get_data;
    if  (buf_size >= 1 && buf_size <=10)
    {
         if (UartRxOpticalFlow[0] == 0xfe) //接收到帧头
         {
             Check_sum=(uint8_t)(UartRxOpticalFlow[2]+UartRxOpticalFlow[3]+UartRxOpticalFlow[4]+UartRxOpticalFlow[5]);
             if(Check_sum == UartRxOpticalFlow[6] && UartRxOpticalFlow[6]!=0)//校验和正确
                     {
                         flow_x = UartRxOpticalFlow[2] + (UartRxOpticalFlow[3] << 8);
                         flow_y = UartRxOpticalFlow[4] + (UartRxOpticalFlow[5] << 8);
                         opt_flow_speed_x = flow_x;
                         opt_flow_speed_y = flow_y;
                     }
             if (UartRxOpticalFlow[8] == 0xaa )
                 {
                        //此处为数据包处理逻辑
                        buf_size = 0;
                        memset(UartRxOpticalFlow,0,10);
                 }
         }
         else
         {
             buf_size  =  0;
             memset(UartRxOpticalFlow,0,10);
         }
    }else{
        buf_size  =  0;
        memset(UartRxOpticalFlow,0,10);
    }

}
