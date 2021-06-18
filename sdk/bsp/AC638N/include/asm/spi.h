#ifndef _SPI_H_
#define _SPI_H_
#include "typedef.h"
#include "ioctl.h"

enum spi_mode {
    SPI_MODE_BIDIR_1BIT,    //支持SPIx(x=0,1,2)，全双工，di接收，do发送
    SPI_MODE_UNIDIR_1BIT,   //支持SPIx(x=0,1,2)，半双工，do分时发送/接收
    SPI_MODE_UNIDIR_2BIT,   //支持SPIx(x=0,1.2)，半双工，di & do共2bit分时发送/接收
    SPI_MODE_UNIDIR_4BIT,   //支持SPIx(x=0)，半双工，di & do共4bit分时发送/接收
};

enum {
    SPI0,  //SPI0系统已使用，SPI1 SPI2供用户使用
    SPI1,
    SPI2,
    SPI_MAX_HW_NUM,
};

#define SPI_MAX_IO_GROUP            3 //spi0:2组, spi1/2:3组

struct spi_io {
    u8 cs_pin;
    u8 di_pin;
    u8 do_pin;
    u8 clk_pin;
    u8 d2_pin;
    u8 d3_pin;
};

struct spi_io_mapping {
    u32 num;                                //可选端口数量
    struct spi_io io[SPI_MAX_IO_GROUP];
};

enum spi_role {
    SPI_ROLE_MASTER,
    SPI_ROLE_SLAVE,
};

struct spi_platform_data {
    u8 port[3];  //CLK, DO, DI
    u8 mode;     //模式，选项为enum spi_mode中的枚举常量
    u8 role;     //角色，选项为enum spi_role中的枚举常量
    u32 clk;     //波特率
};

extern const struct spi_platform_data spi1_p_data;
extern const struct spi_platform_data spi2_p_data;

typedef const int spi_dev;

/*
 * @brief 打开spi
 * @parm spi  spi句柄
 * @return 0 成功，< 0 失败
 */
int spi_open(spi_dev spi);
/*
 * @brief spi dma接收
 * @parm spi  spi句柄
 * @parm buf  接收缓冲区基地址
 * @parm len  期望接收长度
 * @return 实际接收长度，< 0表示失败
 */
int spi_dma_recv(spi_dev spi, void *buf, u32 len);
/*
 * @brief spi dma发送
 * @parm spi  spi句柄
 * @parm buf  发送缓冲区基地址
 * @parm len  期望发送长度
 * @return 实际发送长度，< 0表示失败
 */
int spi_dma_send(spi_dev spi, const void *buf, u32 len);
/*
 * @brief spi 配置dma，不等待pnd，用于中断
 * @parm spi  spi句柄
 * @parm buf  缓冲区基地址
 * @parm len  期望长度
 * @parm rw  1 接收 / 0 发送
 * @return null
 */
void spi_dma_set_addr_for_isr(spi_dev spi, void *buf, u32 len, u8 rw);
/*
 * @brief 中断使能
 * @parm spi  spi句柄
 * @parm en  1 使能，0 失能
 * @return null
 */
void spi_set_ie(spi_dev spi, u8 en);
/*
 * @brief 判断中断标志
 * @parm spi  spi句柄
 * @return 0 无png / 1 有png
 */
u8 spi_get_pending(spi_dev spi);
/*
 * @brief 清除中断标志
 * @parm spi  spi句柄
 * @return null
 */
void spi_clear_pending(spi_dev spi);
/*
 * @brief 设置spi[单向/双向，位数]模式
 * @parm spi  spi句柄
 * @parm mode  模式:
 *        SPI_MODE_BIDIR_1BIT,    //支持SPIx(x=0,1,2)，全双工，di接收，do发送
 *        SPI_MODE_UNIDIR_1BIT,   //支持SPIx(x=0,1,2)，半双工，do分时发送/接收
 *        SPI_MODE_UNIDIR_2BIT,   //支持SPIx(x=0)，半双工，di & do共2bit分时发送/接收
 *        SPI_MODE_UNIDIR_4BIT,   //支持SPIx(x=0)，半双工，di & do共4bit分时发送/接收
 * @return null
 */
void spi_set_bit_mode(spi_dev spi, int mode);
/*
 * @brief 接收1个字节
 * @parm spi  spi句柄
 * @parm err  返回错误信息，若err为非空指针，0 成功，< 0 失败，若为空指针，忽略
 * @return 接收的字节
 */
u8 spi_recv_byte(spi_dev spi, int *err);
/*
 * @brief 接收1个字节，不等待pnd，用于中断
 * @parm spi  spi句柄
 * @return 接收的字节
 */
u8 spi_recv_byte_for_isr(spi_dev spi);
/*
 * @brief 发送1个字节
 * @parm spi  spi句柄
 * @parm byte 发送的字节
 * @return 0 成功，< 0 失败
 */
int spi_send_byte(spi_dev spi, u8 byte);
/*
 * @brief 发送1个字节，不等待pnd，用于中断
 * @parm spi  spi句柄
 * @parm byte 发送的字节
 * @return null
 */
void spi_send_byte_for_isr(spi_dev spi, u8 byte);
/*
 * @brief 发送并接收1个字节，在8个时钟内完成，仅使用于SPI_MODE_BIDIR_1BIT
 * @parm spi  spi句柄
 * @parm byte  发送的字节
 * @parm err  返回错误信息，若err为非空指针，0 成功，< 0 失败，若为空指针，忽略
 * @return 接收的字节
 */
u8 spi_send_recv_byte(spi_dev spi, u8 byte, int *err);
/*
 * @brief 设置波特率
 * @parm spi  spi句柄
 * @parm baud  波特率
 * @return 0 成功，< 0 失败
 */
int spi_set_baud(spi_dev spi, u32 baud);
/*
 * @brief 获取波特率
 * @parm spi  spi句柄
 * @return  波特率
 */
u32 spi_get_baud(spi_dev spi);
/*
 * @brief 关闭spi
 * @parm spi  spi句柄
 * @return null
 */
void spi_close(spi_dev spi);

#endif

