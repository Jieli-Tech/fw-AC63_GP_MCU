#include "asm/includes.h"


#define uart_clk    clk_get("uart")

static u32 debug_baud;
static JL_UART_TypeDef *debug = NULL;

int debug_uart_init(const struct uart_platform_data *data)
{
    debug_baud = data->baudrate;

    debug = JL_UART2;
    debug->CON0 = BIT(13) | BIT(12) | BIT(10);

    if (data->tx_pin < IO_PORT_MAX) {
        //crossbar
        gpio_direction_output(data->tx_pin, 1);
        gpio_set_fun_output_port(data->tx_pin, FO_UART2_TX, 1, 1);
    }
    if (data->rx_pin < IO_PORT_MAX) {
        //crossbar
        gpio_direction_input(data->rx_pin);
        gpio_set_pull_up(data->rx_pin, 1);
        gpio_set_die(data->rx_pin, 1);
        gpio_set_fun_input_port(data->rx_pin, PFI_UART2_RX);
        debug->CON0 |= BIT(1);
    }

    debug->BAUD = (uart_clk / data->baudrate) / 4 - 1;
    debug->CON0 |= BIT(0);

    return 0;
}


void putbyte(char a)
{
    if (!debug) {
        return;
    }

    if (a == '\r') {
        return;
    }
    if (a == '\n') {
        debug->BUF = '\r';
        __asm__ volatile("csync");
        while ((debug->CON0 & BIT(15)) == 0);    //TX IDLE
        debug->CON0 |= BIT(13);  //清Tx pending
    }

    debug->BUF = a;
    __asm__ volatile("csync");
    while ((debug->CON0 & BIT(15)) == 0);    //TX IDLE
    debug->CON0 |= BIT(13);  //清Tx pending
}

int getbyte(char *c)
{
    if (!debug) {
        return 0;
    }
    if ((debug->CON0 & BIT(14))) {
        *c = debug->BUF;
        debug->CON0 |= BIT(12);
        return 1;
    }
    return 0;
}


static void clock_critical_enter(void)
{

}

static void clock_critical_exit(void)
{
    if (debug == NULL) {
        return;
    }
    debug->OTCNT = uart_clk / 1000;
    debug->BAUD = (uart_clk / debug_baud) / 4 - 1;
}
CLOCK_CRITICAL_HANDLE_REG(debug_uart, clock_critical_enter, clock_critical_exit)

