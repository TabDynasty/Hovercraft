#ifndef W25QXX_H_
#define W25QXX_H_
#include "zf_common_headfile.h"
//////////////////////////////////////////////////////////////////////////////////
//@Author czh
//根据正点原子程序移植
//通用版
//仅需提供硬件接口
//@version 1.1
//////////////////////////////////////////////////////////////////////////////////




//W25X系列/Q系列芯片列表
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17

#define NM25Q80     0X5213
#define NM25Q16     0X5214
#define NM25Q32     0X5215
#define NM25Q64     0X5216
#define NM25Q128    0X5217
#define NM25Q256    0X5218

extern uint16 W25QXX_TYPE;                  //定义W25QXX芯片型号


////////////////////////////////////////////////////////////////////////////

//指令表
#define W25X_WriteEnable        0x06
#define W25X_WriteDisable       0x04
#define W25X_ReadStatusReg      0x05
#define W25X_WriteStatusReg     0x01
#define W25X_ReadData           0x03
#define W25X_FastReadData       0x0B
#define W25X_FastReadDual       0x3B
#define W25X_PageProgram        0x02
#define W25X_BlockErase         0xD8
#define W25X_SectorErase        0x20
#define W25X_ChipErase          0xC7
#define W25X_PowerDown          0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID           0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID      0x9F

//1页=256字节1扇区(sectors)=4K字节1块(block)=16扇区=64K字节

#define SECTOR_SIZE                 (2048)
#define FLASH_SECTOR_NUM            (128)                   //扇区数
#define FLASH_ALIGN_ADDR            8                       //地址对齐整数倍  一个数据占8byte
#define BOOTSECTOR    (FLASH_SECTOR_NUM-5)     //存档起点
typedef uint32                     FLASH_WRITE_TYPE;       //flash_write 函数写入 的数据类型

//-------------------------------------------------------------------------------------------------------------------
//  @brief      使用宏定义对flash进行数据读取
//  @param      SectorNum       需要写入的扇区编号
//  @param      offset          地址偏移
//  @param      type            读取的数据类型
//  @return                     返回给定地址的数据
//  @since      v1.0
//  Sample usage:               flash_read(20,0,uint32);//读取20号扇区偏移0数据类型为uint32
//-------------------------------------------------------------------------------------------------------------------
#define     flash_read(SectorNum,offset,type)        (*(type *)((uint32)(((SectorNum)*SECTOR_SIZE) + (offset))))

uint8 Flash_SPI_ReadWriteByte(uint8 data);
void W25QXX_Init(void);
uint16  W25QXX_ReadID(void);                //读取FLASH ID
uint8    W25QXX_ReadSR(void);               //读取状态寄存器
void W25QXX_Write_SR(uint8 sr);             //写状态寄存器
void W25QXX_Write_Enable(void);         //写使能
void W25QXX_Write_Disable(void);        //写保护
void W25QXX_Write_NoCheck(uint8* pBuffer,uint32 WriteAddr,uint16 NumByteToWrite);
void W25QXX_Read(uint8* pBuffer,uint32 ReadAddr,uint16 NumByteToRead);   //读取flash
void W25QXX_Write(uint8* pBuffer,uint32 WriteAddr,uint16 NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);           //整片擦除
void W25QXX_Erase_Sector(uint32 Dst_Addr);  //扇区擦除
void W25QXX_Wait_Busy(void);            //等待空闲
void W25QXX_PowerDown(void);            //进入掉电模式
void W25QXX_WAKEUP(void);               //唤醒


//读写数据类型操作uint8/int8/uint16/int16/floatd的单个数据以及数组
//---------------------------------------------------------------
void W25QXX_write_int32(int16 a,uint32 startadr,uint16 ranking);
int32 W25QXX_read_int32(uint32 startadr,uint16 ranking);
void W25QXX_write_int32_buf (int32 *a,uint32 startadr,uint16 ranking,uint16 num);
void W25QXX_read_int32_buf(int32 *a,uint32 startadr,uint16 ranking,uint16 num);
void W25QXX_write_uint32(uint32 a,uint32 startadr,uint16 ranking);
uint32 W25QXX_read_uint32(uint32 startadr,uint16 ranking);
void  W25QXX_write_uint32_buf(uint32 *a,uint32 startadr,uint16 ranking,uint16 num);
void  W25QXX_read_uint32_buf(uint32 *a,uint32 startadr,uint16 ranking,uint16 num);
void W25QXX_write_float(float a,uint32 startadr,uint16 ranking);
float W25QXX_read_float(uint32 startadr,uint16 ranking);
void W25QXX_write_float_buf(float *buf,uint16 data_len ,uint32 startadr);
void W25QXX_read_float_buf(float *addr_buf,uint16 data_len ,uint32 startadr);
void W25QXX_write_int16(int16 a,uint32 startadr,uint16 ranking);
int16 W25QXX_read_int16(uint32 startadr,uint16 ranking);
void W25QXX_write_int16_buf(int16 *buf,uint16 data_len ,uint32 startadr);
void W25QXX_read_int16_buf(int16 *addr_buf,uint16 data_len ,uint32 startadr);
void W25QXX_write_uint16(uint16 a,uint32 startadr,uint16 ranking);
uint16 W25QXX_read_uint16(uint32 startadr,uint16 ranking);
void W25QXX_write_uint16_buf(uint16 *buf,uint16 data_len ,uint32 startadr);
void W25QXX_read_uint16_buf(uint16 *addr_buf,uint16 data_len ,uint32 startadr);
void W25QXX_write_int8(int8 a,uint32 startadr,uint16 ranking);
int8 W25QXX_read_int8(uint32 startadr,uint16 ranking);
void W25QXX_write_int8_buf(int8 *buf,uint16 data_len ,uint32 startadr);
void W25QXX_read_int8_buf(int8 *addr_buf,uint16 data_len ,uint32 startadr);
void W25QXX_write_uint8(uint8 a,uint32 startadr,uint16 ranking);
uint8 W25QXX_read_uint8(uint32 startadr,uint16 ranking);
void W25QXX_write_uint8_buf(uint8 *buf,uint16 data_len ,uint32 startadr);
void W25QXX_read_uint8_buf(uint8 *addr_buf,uint16 data_len ,uint32 startadr);
//---------------------------------------------------------------
//



#endif /* W25QXX_H_ */
