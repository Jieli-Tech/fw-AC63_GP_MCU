#include "asm/includes.h"

struct chargestore_handle {
    JL_UART_TypeDef *UART;
    void (*data_callback)(u8 cmd, u8 *data, u8 len);
    u32 baudrate;
    u32 io_port;
    u32 fo_uart;
    u32 pfi_uart;
};

#define DMA_ISR_LEN 64
#define DMA_BUF_LEN 64
#define __this  (&hdl)
static struct chargestore_handle hdl;
static u8 uart_dma_buf[DMA_BUF_LEN] __attribute__((aligned(4)));
extern void nvram_set_boot_state(u32 state);

//串口时钟和串口超时时钟是分开的
#define UART_SRC_CLK    clk_get("uart")
#define UART_OT_CLK     clk_get("lsb")

enum {
    UPGRADE_NULL = 0,
    UPGRADE_USB_HARD_KEY,
    UPGRADE_USB_SOFTKEY,
    UPGRADE_UART_SOFT_KEY,
    UPGRADE_UART_ONE_WIRE_HARD_KEY,
};

/*----------------------------------------------------*/
/**@brief    设置升级RAM标记
   @param    无
   @return   无
   @note     设置标记后复位系统进入串口升级
*/
/*----------------------------------------------------*/
void chargestore_set_update_ram(void)
{
    int tmp;
    __asm__ volatile("%0 =icfg" : "=r"(tmp));
    tmp &= ~(3 << 8);
    __asm__ volatile("icfg = %0" :: "r"(tmp));
    local_irq_disable();
    nvram_set_boot_state(UPGRADE_UART_SOFT_KEY);
    cpu_reset();
}

/*----------------------------------------------------*/
/**@brief    链路层接收中断
   @param    无
   @return   无
   @note     用于接收串口升级命令
*/
/*----------------------------------------------------*/
___interrupt
static void uart_isr(void)
{
    u16 i;
    u32 rx_len = 0;
    if ((__this->UART->CON0 & BIT(2)) && (__this->UART->CON0 & BIT(15))) {
        __this->UART->CON0 |= BIT(13);
        if (__this->data_callback) {
            __this->data_callback(CMD_COMPLETE, NULL, 0);
        }
    }
    if ((__this->UART->CON0 & BIT(3)) && (__this->UART->CON0 & BIT(14))) {
        __this->UART->CON0 |= BIT(12);//清RX PND
        if (__this->data_callback) {
            __this->data_callback(CMD_RECVDATA, uart_dma_buf, DMA_ISR_LEN);
        }
        memset((void *)uart_dma_buf, 0, sizeof(uart_dma_buf));
        __this->UART->RXSADR = (u32)uart_dma_buf;
        __this->UART->RXEADR = (u32)(uart_dma_buf + DMA_BUF_LEN);
        __this->UART->RXCNT = DMA_ISR_LEN;
    }
    if ((__this->UART->CON0 & BIT(5)) && (__this->UART->CON0 & BIT(11))) {
        //OTCNT PND
        __this->UART->CON0 |= BIT(7);//DMA模式
        __this->UART->CON0 |= BIT(10);//清OTCNT PND
        asm volatile("nop");
        rx_len = __this->UART->HRXCNT;//读当前串口接收数据的个数
        __this->UART->CON0 |= BIT(12);//清RX PND(这里的顺序不能改变，这里要清一次)
        if (__this->data_callback) {
            __this->data_callback(CMD_RECVDATA, uart_dma_buf, rx_len);
        }
        memset((void *)uart_dma_buf, 0, sizeof(uart_dma_buf));
        __this->UART->RXSADR = (u32)uart_dma_buf;
        __this->UART->RXEADR = (u32)(uart_dma_buf + DMA_BUF_LEN);
        __this->UART->RXCNT = DMA_ISR_LEN;
    }
}

/*----------------------------------------------------*/
/**@brief    链路层写数据
   @param    无
   @return   无
   @note     用于回复命令
*/
/*----------------------------------------------------*/
void chargestore_write(u8 *data, u8 len)
{
    u32 data_addr = (u32)data;
    if (data_addr % 4) {//4byte对齐
        ASSERT(0, "%s: unaligned accesses!", __func__);
    }
    __this->UART->TXADR = data_addr;
    __this->UART->TXCNT = len;
}

/*----------------------------------------------------*/
/**@brief    链路层打开数据通路
   @param    0:接收模式 1:发送模式
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_open(u8 mode)
{
    __this->UART->CON0 = BIT(13) | BIT(12) | BIT(10);
    if (mode == MODE_RECVDATA) {
        gpio_direction_input(__this->io_port);
        gpio_set_die(__this->io_port, 1);
        __this->UART->CON1 &= ~BIT(4);
        gpio_set_fun_input_port(__this->io_port, __this->pfi_uart);
        memset((void *)uart_dma_buf, 0, sizeof(uart_dma_buf));
        __this->UART->RXSADR = (u32)uart_dma_buf;
        __this->UART->RXEADR = (u32)(uart_dma_buf + DMA_BUF_LEN);
        __this->UART->RXCNT = DMA_ISR_LEN;
        __this->UART->CON0 |= BIT(6) | BIT(5) | BIT(3);
    } else {
        gpio_direction_output(__this->io_port, 1);
        gpio_set_hd(__this->io_port, 1);
        gpio_set_hd0(__this->io_port, 1);
        __this->UART->CON1 |= BIT(4);
        gpio_set_fun_output_port(__this->io_port, __this->fo_uart, 1, 1);
        __this->UART->CON0 |= BIT(2);
    }
    __this->UART->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);
}

/*----------------------------------------------------*/
/**@brief    链路层关闭数据通路
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_close(void)
{
    __this->UART->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(0);
    gpio_set_pull_down(__this->io_port, 0);
    gpio_set_pull_up(__this->io_port, 0);
    gpio_set_die(__this->io_port, 1);
    gpio_set_hd(__this->io_port, 0);
    gpio_set_hd0(__this->io_port, 0);
    gpio_direction_input(__this->io_port);
    memset((void *)uart_dma_buf, 0, sizeof(uart_dma_buf));
}

/*----------------------------------------------------*/
/**@brief    链路层设置通信波特率
   @param    baudrate:传入波特率值
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_set_baudrate(u32 baudrate)
{
    u32 uart_timeout;
    __this->baudrate = baudrate;
    uart_timeout = 20 * 1000000 / __this->baudrate;
    __this->UART->OTCNT = uart_timeout * (UART_OT_CLK / 1000000);
    __this->UART->BAUD = (UART_SRC_CLK / __this->baudrate) / 4 - 1;
}

/*----------------------------------------------------*/
/**@brief    链路层设置接收到数据处理回调函数
   @param    callback:回调函数
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_set_data_callback(void (*callback)(u8 cmd, u8 *data, u8 len))
{
    __this->data_callback = callback;
}

/*----------------------------------------------------*/
/**@brief    链路层初始化
   @param    baudrate:波特率 io_port:升级IO的编号
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_init(u32 baudrate, u32 io_port)
{
    u32 uart_timeout;
    if (!(JL_UART0->CON0 & BIT(0))) {
        JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
        request_irq(IRQ_UART0_IDX, 2, uart_isr, 0);
        __this->UART = JL_UART0;
        __this->fo_uart = FO_UART0_TX;
        __this->pfi_uart = PFI_UART0_RX;
    } else if (!(JL_UART1->CON0 & BIT(0))) {
        JL_UART1->CON0 = BIT(13) | BIT(12) | BIT(10);
        request_irq(IRQ_UART1_IDX, 2, uart_isr, 0);
        __this->UART = JL_UART1;
        __this->fo_uart = FO_UART1_TX;
        __this->pfi_uart = PFI_UART1_RX;
    } else if (!(JL_UART2->CON0 & BIT(0))) {
        JL_UART2->CON0 = BIT(13) | BIT(12) | BIT(10);
        request_irq(IRQ_UART2_IDX, 2, uart_isr, 0);
        __this->UART = JL_UART2;
        __this->fo_uart = FO_UART2_TX;
        __this->pfi_uart = PFI_UART2_RX;
    } else {
        ASSERT(0, "uart all used!\n");
    }
    uart_timeout = 20 * 1000000 / baudrate;
    __this->UART->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);//占用该串口,不被其他模块使用
    __this->UART->OTCNT = uart_timeout * (UART_OT_CLK / 1000000);
    __this->UART->BAUD = (UART_SRC_CLK / baudrate) / 4 - 1;
    __this->baudrate = baudrate;
    __this->io_port = io_port;
    gpio_set_pull_down(__this->io_port, 0);
    gpio_set_pull_up(__this->io_port, 0);
    gpio_set_die(__this->io_port, 1);
    gpio_direction_input(__this->io_port);
}

