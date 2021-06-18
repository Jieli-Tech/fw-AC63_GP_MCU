#ifndef ASM_UART_H
#define ASM_UART_H

#include "typedef.h"


struct uart_platform_data {
    u8  id;             //串口ID号, 0xff表示自动分配
    u8  tx_pin;         //发送IO, 0xff表示不使能发送
    u8  rx_pin;         //接收IO, 0xff表示不使能接收
    u32 baudrate;       //波特率
};

/*
 * 调试串口初始化, id无效, 接收不支持DMA, 只能按byte接收
 */
extern int debug_uart_init(const struct uart_platform_data *data);


/*
 *串口初始化, 支持DMA接收, 返回串口ID号
 */
extern int uart_init(const struct uart_platform_data *);

/*
 * 设置接收中断处理函数, 此函数需要尽快将数据移走, 否则可能会挡住下次接收
 */
extern void uart_set_rx_irq_handler(int id, void (*handler)(u8 *, u16));

/*
 * 发送一个byte
 */
extern void uart_tx_byte(int id, u8 byte);

/*
 * 同步方式DMA发送批量数据
 */
extern void uart_tx_buf(int id, u8 *txdata, u16 len);

/*
 * 异步方式DMA发送批量数据, 发送完成后回调callback函数
 */
extern void uart_async_tx_buf(int id, u8 *txdata, u16 len, void (*callback)());

#endif


