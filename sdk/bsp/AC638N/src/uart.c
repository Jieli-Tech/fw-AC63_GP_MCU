#include "asm/includes.h"


#define uart_clk            clk_get("uart")
#define UART_RXBUF_SIZE     128


struct uart_handle {
    u8 irq_num;
    u8 irq_priority;
    u32 output_func_idx;
    u32 input_func_idx;
    JL_UART_TypeDef *reg;
    void (*irq_handler)();
};

static void uart0_irq_handler();
static void uart1_irq_handler();

static u8 uart_rxbuf[2][UART_RXBUF_SIZE] __attribute__((aligned(32)));
static void (*tx_callback[2])();
static void (*irq_user_handler[2])(u8 *rxbuf, u16 len) = {NULL, NULL};

static const struct uart_handle uart_hdl[2] = {
    [0] = {
        .irq_num            = IRQ_UART0_IDX,
        .irq_priority       = 0,
        .output_func_idx    = FO_UART0_TX,
        .input_func_idx     = PFI_UART0_RX,
        .reg                = JL_UART0,
        .irq_handler        = uart0_irq_handler,
    },

    [1] = {
        .irq_num            = IRQ_UART1_IDX,
        .irq_priority       = 0,
        .output_func_idx    = FO_UART1_TX,
        .input_func_idx     = PFI_UART1_RX,
        .reg                = JL_UART1,
        .irq_handler        = uart1_irq_handler,
    },
};


static void uart_irq_handler(int id)
{
    JL_UART_TypeDef *reg = uart_hdl[id].reg;

    // RX irq
    if ((reg->CON0 & BIT(5)) && (reg->CON0 & BIT(11))) {
        reg->CON0 |= BIT(12) | BIT(10) | BIT(7);
        __asm__ volatile("csync");

        if (irq_user_handler[id]) {
            irq_user_handler[id](uart_rxbuf[id], reg->HRXCNT);
        }

        reg->RXSADR  = (u32)uart_rxbuf[id];
        reg->RXEADR  = (u32)(uart_rxbuf[id] + UART_RXBUF_SIZE);
        reg->RXCNT = UART_RXBUF_SIZE;
    }

    // TX irq
    if ((reg->CON0 & BIT(2)) && (reg->CON0 & BIT(15))) {
        reg->CON0 |= BIT(13);
        if (tx_callback[id]) {
            tx_callback[id]();
        }
    }
}

___interrupt
static void uart0_irq_handler()
{
    uart_irq_handler(0);
}

___interrupt
static void uart1_irq_handler()
{
    uart_irq_handler(1);
}




int uart_init(const struct uart_platform_data *data)
{
    static u8 uart_inited = 0;
    int id = data->id;
    const struct uart_handle *hdl = NULL;

    if (uart_inited == 0) {
        uart_inited = 1;
        for (int i = 0; i < ARRAY_SIZE(uart_hdl); i++) {
            uart_hdl[i].reg->CON0 = 0;
        }
    }

    if (id < 2) {
        if (uart_hdl[id].reg->CON0 & BIT(0)) {
            return -EBUSY;
        }
        hdl = &uart_hdl[id];
    } else {
        for (id = 0; id < ARRAY_SIZE(uart_hdl); id++) {
            if (!(uart_hdl[id].reg->CON0 & BIT(0))) {
                hdl = &uart_hdl[id];
                break;
            }
        }
        if (!hdl) {
            return -ENODEV;
        }
    }

    JL_UART_TypeDef *reg = hdl->reg;

    reg->CON0 = BIT(13) | BIT(12) | BIT(10);
    reg->BAUD = (uart_clk / data->baudrate) / 4 - 1;

    request_irq(hdl->irq_num, hdl->irq_priority, hdl->irq_handler, 0);

    if (data->rx_pin < IO_PORT_MAX) {
        //crossbar
        gpio_direction_input(data->rx_pin);
        gpio_set_pull_up(data->rx_pin, 1);
        gpio_set_die(data->rx_pin, 1);
        gpio_set_fun_input_port(data->rx_pin, hdl->input_func_idx);

        reg->RXSADR  = (u32)uart_rxbuf[id];
        reg->RXEADR  = (u32)(uart_rxbuf[id] + UART_RXBUF_SIZE);
        reg->RXCNT = UART_RXBUF_SIZE;
        reg->OTCNT = clk_get("lsb") / 1000; //Time(ot)= Time(lsb) * UTx_OTCNT
        reg->CON0 |= BIT(5);   //OT中断
        reg->CON0 |= BIT(6);   //启动DMA
        reg->CON0 |= BIT(1);   //RX enable
    }

    if (data->tx_pin < IO_PORT_MAX) {
        //crossbar
        gpio_direction_output(data->tx_pin, 1);
        gpio_set_fun_output_port(data->tx_pin, hdl->output_func_idx, 1, 1);
        reg->CON0 |= BIT(0); //TX enable
    }

    return id;
}

void uart_set_rx_irq_handler(int id, void (*handler)(u8 *, u16))
{
    ASSERT(id < 2, "id = %d\n", id);

    irq_user_handler[id] = handler;
}

void uart_tx_byte(int id, u8 byte)
{
    JL_UART_TypeDef *reg = uart_hdl[id].reg;

    reg->CON0 &= ~BIT(2);       //关闭发送中断
    reg->BUF = byte;
    __asm__ volatile("csync");
    while ((reg->CON0 & BIT(15)) == 0);
    reg->CON0 |= BIT(13);  //清Tx pending
}

void uart_tx_buf(int id, u8 *txdata, u16 len)
{
    JL_UART_TypeDef *reg = uart_hdl[id].reg;

    reg->CON0 &= ~BIT(2);       //关闭发送中断
    reg->TXADR = (u32)txdata;
    reg->TXCNT = len;
    __asm__ volatile("csync");

    while ((reg->CON0 & BIT(15)) == 0);
    reg->CON0 |= BIT(13);  //清Tx pending
}

void uart_async_tx_buf(int id, u8 *txdata, u16 len, void (*callback)())
{
    JL_UART_TypeDef *reg = uart_hdl[id].reg;

    tx_callback[id] = callback;

    reg->CON0 |= BIT(13);  //清Tx pending
    reg->CON0 |= BIT(2);   //使能发送中断
    reg->TXADR = (u32)txdata;
    reg->TXCNT = len;
}


#if 0

static const struct uart_platform_data uart_config = {
    .id         = 0xff,
    .tx_pin     = IO_PORTA_02,
    .rx_pin     = IO_PORTA_03,
    .baudrate   = 115200,
};

static volatile u8 uart_test_rx_flag = 0;
static volatile u8 uart_test_tx_flag = 0;

static u8 uart_test_rxbuf[64 + 16];
static u8 uart_test_txbuf[64] __attribute__((aligned(4)));

static void uart_loop_rx_handler(u8 *rxdata, u16 len)
{
    memcpy(uart_test_rxbuf, rxdata, len);
    uart_test_rx_flag = 1;
}

static void loop_tx_callback()
{
    uart_test_tx_flag = 1;
}

void uart_loop_test()
{
    int id = uart_init(&uart_config);

    uart_set_rx_irq_handler(id, uart_loop_rx_handler);

    for (int i = 0; i < sizeof(uart_test_txbuf); i++) {
        uart_test_txbuf[i] = i;
    }

    puts("---------sync dma tx test --------\n");

    for (int j = 0; j < 100; j++) {
        uart_test_rx_flag = 0;
        uart_tx_buf(id, uart_test_txbuf, sizeof(uart_test_txbuf));

        while (1) {
            if (uart_test_rx_flag) {
                break;
            }
        }
        if (!memcmp(uart_test_rxbuf, uart_test_txbuf, sizeof(uart_test_txbuf))) {
            printf("sync loop test suss: %d\n", j);
            memset(uart_test_rxbuf, 0, sizeof(uart_test_rxbuf));
        } else {
            puts("sync loop test faild:\n");
            put_buf(uart_test_rxbuf, sizeof(uart_test_rxbuf));
            return;
        }
    }

    puts("---------async dma tx test --------\n");

    for (int j = 0; j < 100; j++) {
        uart_test_rx_flag = 0;
        uart_test_tx_flag = 0;
        uart_async_tx_buf(id, uart_test_txbuf, sizeof(uart_test_txbuf), loop_tx_callback);

        while (1) {
            if (uart_test_rx_flag && uart_test_tx_flag) {
                break;
            }
        }
        if (!memcmp(uart_test_rxbuf, uart_test_txbuf, sizeof(uart_test_txbuf))) {
            printf("async loop test suss: %d\n", j);
            memset(uart_test_rxbuf, 0, sizeof(uart_test_rxbuf));
        } else {
            puts("async loop test faild:\n");
            put_buf(uart_test_rxbuf, sizeof(uart_test_rxbuf));
            return;
        }
    }
}

#endif



