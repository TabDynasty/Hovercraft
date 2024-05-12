#include "W25QXX.h"
#include "utils.h"
#include "zf_common_headfile.h"
#include "menu.h"
//////////////////////////////////////////////////////////////////////////////////
//@Author czh
//根据正点原子程序移植
//通用版
//仅需提供硬件接口
//@version 1.1
//////////////////////////////////////////////////////////////////////////////////
uint16 W25QXX_TYPE=0;  //默认是W25Q128
//uint16 W25QXX_TYPE=0;  //默认是W25Q128
//需要注意的是本文件中需实现uint16，uint16等数据类型的声明
////------------------------------需要实现的硬件接口----------------------------------------------

#define W25QXX_CS_PIN A15                                                                      //片选引脚
#define W25QXX_CS(x)    ((x == 0)?(gpio_set_level(W25QXX_CS_PIN,0)):(gpio_set_level(W25QXX_CS_PIN,1))) //片选引脚高低电平操作（*）

/*外部变量声明*/
void flash_spi_cs_init(void)//初始化片选引脚
{
    gpio_init(A15,GPO,0, GPO_PUSH_PULL);
}

void flash_spi_init(void)//通信用SPI初始化
{
    //spi_init(TFT180_SPI, SPI_MODE0, TFT180_SPI_SPEED, TFT180_SCL_PIN, TFT180_SDA_PIN, TFT180_SDA_PIN_IN, SPI_CS_NULL);
      spi_init(SPI_3,  SPI_MODE0, 15*1000*1000,SPI3_MAP0_SCK_B3,SPI3_MAP0_MOSI_B5,SPI3_MAP0_MISO_B4,A15 );//硬件SPI初始化//8*1000*1000//SPI1_CS5_P11_2
    //  spi_init(SPI_1,  SPI_MODE0, 5*1000*1000,SPI1_MAP1_SCK_B3,SPI1_MAP1_MOSI_B5,SPI1_MAP1_MISO_B4,A15 );
     //  spi_init(TFT_SPIN, TFT_SCL, TFT_SDA, SPI_MISO_NULL, SPI_NSS_NULL, 0, SystemCoreClock/12);
}

uint8 Flash_SPI_ReadWriteByte(uint8 data)//spi读写函数接口
{
  uint8 rec_buf;//接收数组
  spi_mosi(SPI_3,&data,&rec_buf,1);//调用逐飞通信接口
  return rec_buf;//返回接收数据
}

void delay_us(uint16 time)//延时函数
{
    system_delay_us(time);
}
//------------------------------------------------------------------------------------------------

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector
//初始化SPI FLASH的IO口

void W25QXX_Init(void)
{
    flash_spi_cs_init();
    W25QXX_CS(1);               //SPI FLASH不选中
    flash_spi_init();           //初始化SPI
    W25QXX_TYPE=W25QXX_ReadID();
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8 W25QXX_ReadSR(void)
{
    uint8 byte=0;
    W25QXX_CS(0);                            //使能器件
    Flash_SPI_ReadWriteByte(W25X_ReadStatusReg); //发送读取状态寄存器命令
    byte=Flash_SPI_ReadWriteByte(0Xff);          //读取一个字节
    W25QXX_CS(1);                            //取消片选
    return byte;
}

//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(uint8 sr)
{
    W25QXX_CS(0);                            //使能器件
    Flash_SPI_ReadWriteByte(W25X_WriteStatusReg);//发送写取状态寄存器命令
    Flash_SPI_ReadWriteByte(sr);                //写入一个字节
    W25QXX_CS(1);                            //取消片选
}
//W25QXX写使能
//将WEL置位
void W25QXX_Write_Enable(void)
{
    W25QXX_CS(0);                           //使能器件
    Flash_SPI_ReadWriteByte(W25X_WriteEnable);  //发送写使能
    W25QXX_CS(1);                               //取消片选
}
//W25QXX写禁止
//将WEL清零
void W25QXX_Write_Disable(void)
{
    W25QXX_CS(0);                            //使能器件
    Flash_SPI_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令
    W25QXX_CS(1);                            //取消片选
}
//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
uint16 W25QXX_ReadID(void)
{
    uint16 Temp = 0;
    W25QXX_CS(0);
    Flash_SPI_ReadWriteByte(0x90);//发送读取ID命令
    Flash_SPI_ReadWriteByte(0x00);
    Flash_SPI_ReadWriteByte(0x00);
    Flash_SPI_ReadWriteByte(0x00);
    Temp|=Flash_SPI_ReadWriteByte(0xFF)<<8;
    delay_us(5);
    Temp|=Flash_SPI_ReadWriteByte(0xFF);
    W25QXX_CS(1);
    return Temp;
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX_Read(uint8* pBuffer,uint32 ReadAddr,uint16 NumByteToRead)
{
    uint16 i;
    W25QXX_CS(0);                               //使能器件
    Flash_SPI_ReadWriteByte(W25X_ReadData);             //发送读取命令
    Flash_SPI_ReadWriteByte((uint8)((ReadAddr)>>16));   //发送24bit地址
    Flash_SPI_ReadWriteByte((uint8)((ReadAddr)>>8));
    Flash_SPI_ReadWriteByte((uint8)ReadAddr);
    for(i=0;i<NumByteToRead;i++)
    {
        pBuffer[i]=Flash_SPI_ReadWriteByte(0XFF);       //循环读数
    }
    W25QXX_CS(1);
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX_Write_Page(uint8* pBuffer,uint32 WriteAddr,uint16 NumByteToWrite)
{
    uint16 i;
    W25QXX_Write_Enable();                      //SET WEL
    W25QXX_CS(0);                               //使能器件
    Flash_SPI_ReadWriteByte(W25X_PageProgram);          //发送写页命令
    Flash_SPI_ReadWriteByte((uint8)((WriteAddr)>>16));  //发送24bit地址
    Flash_SPI_ReadWriteByte((uint8)((WriteAddr)>>8));
    Flash_SPI_ReadWriteByte((uint8)WriteAddr);
    for(i=0;i<NumByteToWrite;i++)Flash_SPI_ReadWriteByte(pBuffer[i]);//循环写数
    W25QXX_CS(1);                               //取消片选
    W25QXX_Wait_Busy();                         //等待写入结束
}
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8* pBuffer,uint32 WriteAddr,uint16 NumByteToWrite)
{
    uint16 pageremain;
    pageremain=256-WriteAddr%256; //单页剩余的字节数
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
    while(1)
    {
        W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
        if(NumByteToWrite==pageremain)break;//写入结束了
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;

            NumByteToWrite-=pageremain;           //减去已经写入了的字节数
            if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
            else pageremain=NumByteToWrite;       //不够256个字节了
        }
    };
}

//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
uint8 W25QXX_BUFFER[4096];
void W25QXX_Write(uint8* pBuffer,uint32 WriteAddr,uint16 NumByteToWrite)
{
    uint32 secpos;
    uint16 secoff;
    uint16 secremain;
    uint16 i;
    uint8 * W25QXX_BUF;
    W25QXX_BUF=W25QXX_BUFFER;
    secpos=WriteAddr/4096;//扇区地址
    secoff=WriteAddr%4096;//在扇区内的偏移
    secremain=4096-secoff;//扇区剩余空间大小
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
    while(1)
    {
        W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
        for(i=0;i<secremain;i++)//校验数据
        {
            if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除
        }
        if(i<secremain)//需要擦除
         {
            W25QXX_Erase_Sector(secpos);        //擦除这个扇区
            for(i=0;i<secremain;i++)            //复制
            {
                W25QXX_BUF[i+secoff]=pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区

        }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(NumByteToWrite==secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff=0;//偏移位置为0

            pBuffer+=secremain;                 //指针偏移
            WriteAddr+=secremain;               //写地址偏移
            NumByteToWrite-=secremain;          //字节数递减
            if(NumByteToWrite>4096)secremain=4096;//下一个扇区还是写不完
            else secremain=NumByteToWrite;      //下一个扇区可以写完了
        }
    };
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();                      //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS(0);                               //使能器件
    Flash_SPI_ReadWriteByte(W25X_ChipErase);            //发送片擦除命令
    W25QXX_CS(1);                               //取消片选
    W25QXX_Wait_Busy();                         //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(uint32 Dst_Addr)
{
    //监视falsh擦除情况,测试用
//  printf("fe:%x\r\n",Dst_Addr);
      Dst_Addr*=4096;
    W25QXX_Write_Enable();                      //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS(0);                               //使能器件
    Flash_SPI_ReadWriteByte(W25X_SectorErase);          //发送扇区擦除指令
    Flash_SPI_ReadWriteByte((uint8)((Dst_Addr)>>16));   //发送24bit地址
    Flash_SPI_ReadWriteByte((uint8)((Dst_Addr)>>8));
    Flash_SPI_ReadWriteByte((uint8)Dst_Addr);
      W25QXX_CS(1);                             //取消片选
    W25QXX_Wait_Busy();                         //等待擦除完成
}
//等待空闲
void W25QXX_Wait_Busy(void)
{
    while((W25QXX_ReadSR()&0x01)==0x01);        // 等待BUSY位清空
}
//进入掉电模式
void W25QXX_PowerDown(void)
{
    W25QXX_CS(0);                                   //使能器件
    Flash_SPI_ReadWriteByte(W25X_PowerDown);        //发送掉电命令
      W25QXX_CS(1);                             //取消片选
    delay_us(3);                               //等待TPD
}
//唤醒
void W25QXX_WAKEUP(void)
{
    W25QXX_CS(0);                               //使能器件
    Flash_SPI_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
    W25QXX_CS(1);                             //取消片选
    delay_us(3);                                //等待TRES1
}

#define Start_Address 100//默认数据读取开始地址
                         //地址为24位，即从0到8M


//数据类型读写函数
//==================================================
//==================================================
union//联合体来实现数据类型转换
{
 float x;
 uint8 s[4];
}F_and_C;

//-------------------------------------------------------------------------------------------------------------------
// @brief       W25QXX写一个浮点数
// @param       a                 写入的浮点数
// @param       startadr    开始地址
// @param       ranking     计数用，用来表示写入的是第几个数据
// @return  void
// @since       v1.0
// Sample usage:            W25QXX_write_float(3.14f,10,1)
//-------------------------------------------------------------------------------------------------------------------
void W25QXX_write_float(float a,uint32 startadr,uint16 ranking)
{
      F_and_C.x=a;

            W25QXX_Write(F_and_C.s,startadr+4*ranking,4);
}
//-------------------------------------------------------------------------------------------------------------------
// @brief       W25QXX读取一个浮点数
// @param       startadr    开始地址
// @param       ranking     计数用，用来表示读取的是第几个数据
// @return  float
// @since       v1.0
// Sample usage:            data=W25QXX_read_float(10,1)
//-------------------------------------------------------------------------------------------------------------------
float W25QXX_read_float(uint32 startadr,uint16 ranking)
{
            W25QXX_Read(F_and_C.s,startadr+4*ranking,4);
            return F_and_C.x;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief       W25QXX写一个浮点数组
// @param       *buf            写入的浮点数组
// @param       data_len    写入数量
// @param       startadr    开始地址
// @return  void
// @since       v1.0
// Sample usage:            W25QXX_write_float_buf(write_data_buf,5,1000)
//-------------------------------------------------------------------------------------------------------------------
void W25QXX_write_float_buf(float *buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
          F_and_C.x=buf[i];
            W25QXX_Write(F_and_C.s,startadr+4*i,4);
        }
}
//-------------------------------------------------------------------------------------------------------------------
// @brief       W25QXX读取一个浮点数组
// @param       *buf            读取的浮点数组
// @param       data_len    读取数量
// @param       startadr    开始地址
// @return  void
// @since       v1.0
// Sample usage:            W25QXX_write_float_buf(read_data_buf,5,1000)
//-------------------------------------------------------------------------------------------------------------------
void W25QXX_read_float_buf(float *addr_buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
            W25QXX_Read(F_and_C.s,startadr+4*i,4);
            addr_buf[i]=F_and_C.x;
        }
}


//读写int32部分
//==================================================
//==================================================
union//联合体来实现数据类型转换
{
 int32 x;
 uint8 s[4];
}INT32_and_C;

void W25QXX_write_int32(int16 a,uint32 startadr,uint16 ranking)
{
      INT32_and_C.x=a;
      W25QXX_Write(INT32_and_C.s,startadr+4*ranking,4);
}

int32 W25QXX_read_int32(uint32 startadr,uint16 ranking)
{
      W25QXX_Read(INT32_and_C.s,startadr+4*ranking,4);
      return INT32_and_C.x;
}

void W25QXX_write_int32_buf (int32 *a,uint32 startadr,uint16 ranking,uint16 num)
{
    uint16 i;
    for(i=0;i<num;i++)
    {
        INT32_and_C.x=a[i];
        W25QXX_Write(INT32_and_C.s,startadr+4*ranking,4);
        ranking++;
    }
}

void W25QXX_read_int32_buf(int32 *a,uint32 startadr,uint16 ranking,uint16 num)
{
    uint16 i;
    for(i=0;i<num;i++)
    {
        W25QXX_Read(INT32_and_C.s,startadr+4*ranking,4);
        a[i]=INT32_and_C.x;
        ranking++;
    }
}

//读写uint32部分
//==================================================
//==================================================

union
{
    uint32 x;
    uint8 s[4];
}   UINT32_and_C;

void W25QXX_write_uint32(uint32 a,uint32 startadr,uint16 ranking)
{
    UINT32_and_C.x=a;
    W25QXX_Write(UINT32_and_C.s,startadr+4*ranking,4);
}

uint32 W25QXX_read_uint32(uint32 startadr,uint16 ranking)
{
    W25QXX_Read(UINT32_and_C.s,startadr+4*ranking,4);
    return UINT32_and_C.x;
}

void  W25QXX_write_uint32_buf(uint32 *a,uint32 startadr,uint16 ranking,uint16 num)
{
    uint16 i;
    for(i=0;i<num;i++)
    {
        UINT32_and_C.x=a[i];
        W25QXX_Write(UINT32_and_C.s,startadr+4*ranking,4);
        ranking++;
    }
}
void  W25QXX_read_uint32_buf(uint32 *a,uint32 startadr,uint16 ranking,uint16 num)
{
    uint16 i;
    for(i=0;i<num;i++)
    {
        W25QXX_Read(UINT32_and_C.s,startadr+4*ranking,4);
        a[i]=UINT32_and_C.x;
        ranking++;
    }
}

//==================================================
//==================================================
//下面的几个函数用法和浮点类型的用法相同
//就不再赘述
//:)
//读写int16部分
//==================================================
//==================================================
union//联合体来实现数据类型转换
{
 int16 x;
 uint8 s[2];
}INT16_and_C;


void W25QXX_write_int16(int16 a,uint32 startadr,uint16 ranking)
{
      INT16_and_C.x=a;
      W25QXX_Write(INT16_and_C.s,startadr+2*ranking,2);
}

int16 W25QXX_read_int16(uint32 startadr,uint16 ranking)
{
      W25QXX_Read(INT16_and_C.s,startadr+2*ranking,2);
      return INT16_and_C.x;
}

void W25QXX_write_int16_buf(int16 *buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
          INT16_and_C.x=buf[i];
            W25QXX_Write(INT16_and_C.s,startadr+2*i,2);
        }
}
void W25QXX_read_int16_buf(int16 *addr_buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
            W25QXX_Read(INT16_and_C.s,startadr+2*i,2);
            addr_buf[i]=INT16_and_C.x;
        }
}
//读写uint16部分
//==================================================
//==================================================
union
{
 uint16 x;
 uint8 s[2];
}UINT16_and_C;
void W25QXX_write_uint16(uint16 a,uint32 startadr,uint16 ranking)
{
      UINT16_and_C.x=a;
            W25QXX_Write(UINT16_and_C.s,startadr+2*ranking,2);
}

uint16 W25QXX_read_uint16(uint32 startadr,uint16 ranking)
{
            W25QXX_Read(UINT16_and_C.s,startadr+2*ranking,2);
            return UINT16_and_C.x;
}



void W25QXX_write_uint16_buf(uint16 *buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
          UINT16_and_C.x=buf[i];
            W25QXX_Write(UINT16_and_C.s,startadr+2*i,2);
        }
}
void W25QXX_read_uint16_buf(uint16 *addr_buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
            W25QXX_Read(UINT16_and_C.s,startadr+2*i,2);
            addr_buf[i]=UINT16_and_C.x;
        }
}


//读写int8部分
//==================================================
//==================================================
union
{
 int8 x;
 uint8 s[1];
}INT8_and_C;

void W25QXX_write_int8(int8 a,uint32 startadr,uint16 ranking)
{
      INT8_and_C.x=a;
            W25QXX_Write(INT8_and_C.s,startadr+1*ranking,1);
}

int8 W25QXX_read_int8(uint32 startadr,uint16 ranking)
{
            W25QXX_Read(INT8_and_C.s,startadr+1*ranking,1);
            return INT8_and_C.x;
}

void W25QXX_write_int8_buf(int8 *buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
          INT8_and_C.x=buf[i];
            W25QXX_Write(INT8_and_C.s,startadr+1*i,1);
        }
}
void W25QXX_read_int8_buf(int8 *addr_buf,uint16 data_len ,uint32 startadr)
{
     uint16 i;
     for(i=0;i<data_len;i++)
    {
            W25QXX_Read(INT8_and_C.s,startadr+1*i,1);
            addr_buf[i]=INT8_and_C.x;
        }
}






void W25QXX_write_uint8(uint8 a,uint32 startadr,uint16 ranking)
{
            W25QXX_Write(&a,startadr+1*ranking,1);
}

uint8 W25QXX_read_uint8(uint32 startadr,uint16 ranking)
{
        uint8 read_buf;
            W25QXX_Read(&read_buf,startadr+1*ranking,1);
            return read_buf;
}

void W25QXX_write_uint8_buf(uint8 *buf,uint16 data_len ,uint32 startadr)
{
            W25QXX_Write(buf,startadr,data_len);
}


void W25QXX_read_uint8_buf(uint8 *addr_buf,uint16 data_len ,uint32 startadr)
{
            W25QXX_Read(addr_buf,startadr,data_len);

}





//w25q64版本
//void My_FlashRead(int16 Boot)
//{
//    for(int i=0;i<FLASHDATANUM;i++)
//    {
//        *((FLASH_WRITE_TYPE*)(Flash_Data[i])) = (FLASH_WRITE_TYPE)W25QXX_read_uint32(Boot*200,i);
//    }
//}




