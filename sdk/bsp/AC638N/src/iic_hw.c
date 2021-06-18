#include "asm/iic_hw.h"
#include "asm/gpio.h"
#include "asm/clock.h"
#include "asm/cpu.h"

/*
 *     [[  注意!!!  ]]
       * 适用于带cfg_done的硬件IIC，另一种硬件IIC另作说明
       * 硬件IIC的START / ACK(NACK)必须在发送或接收字节cfg_done前设置，且不能
         接cfg_done单独发送；而STOP则应在发送或接收字节cfg_done后设置，必须接
         cfg_done单独发送
 */

static JL_IIC_TypeDef *const iic_regs[IIC_HW_NUM] = {
    JL_IIC,
};

#define iic_get_id(iic)         (iic)

#define iic_info_port(iic, x)   (hw_iic_cfg[iic_get_id(iic)].port[x])
#define iic_info_baud(iic)      (hw_iic_cfg[iic_get_id(iic)].baudrate)
#define iic_info_hdrive(iic)    (hw_iic_cfg[iic_get_id(iic)].hdrive)
#define iic_info_io_filt(iic)   (hw_iic_cfg[iic_get_id(iic)].io_filter)
#define iic_info_io_pu(iic)     (hw_iic_cfg[iic_get_id(iic)].io_pu)
#define iic_info_role(iic)      (hw_iic_cfg[iic_get_id(iic)].role)

static inline u32 iic_get_scl(hw_iic_dev iic)
{
    u8 port = iic_info_port(iic, 0);
    return port;
}

static inline u32 iic_get_sda(hw_iic_dev iic)
{
    u8 port = iic_info_port(iic, 1);
    return port;
}

static int iic_port_init(hw_iic_dev iic)
{
    u32 reg;
    int ret = 0;
    u8 id = iic_get_id(iic);
    u32 scl, sda;
    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    if (id == 0) {
        gpio_set_fun_output_port(scl, FO_IIC_SCL, 1, 1);
        gpio_set_fun_output_port(sda, FO_IIC_SDA, 1, 1);
        gpio_set_fun_input_port(scl, PFI_IIC_SCL);
        gpio_set_fun_input_port(sda, PFI_IIC_SDA);
        if (scl >= IO_PORT_DP || sda >= IO_PORT_DP) {
            usb_iomode(1);
        }
        if (iic_info_hdrive(iic)) {
            gpio_set_hd(scl, 1);
            gpio_set_hd(sda, 1);
        } else {
            gpio_set_hd(scl, 0);
            gpio_set_hd(sda, 0);
        }
        if (iic_info_io_pu(iic)) {
            gpio_set_pull_up(scl, 1);
            gpio_set_pull_down(scl, 0);
            gpio_set_pull_up(sda, 1);
            gpio_set_pull_down(sda, 0);
        } else {
            gpio_set_pull_up(scl, 0);
            gpio_set_pull_down(scl, 0);
            gpio_set_pull_up(sda, 0);
            gpio_set_pull_down(sda, 0);
        }
    } else {
        ret = -EINVAL;
    }
    return ret;
}

int hw_iic_set_baud(hw_iic_dev iic, u32 baud)
{
    //f_iic = f_sys / (IIC_BAUD * 2)
    //=> IIC_BAUD = f_sys / (2 * f_iic)
    u32 sysclk;
    u8 id = iic_get_id(iic);

    sysclk = clk_get("lsb");
    /* printf("lsb clk:%d",sysclk); */
    if (sysclk < 2 * baud) {
        return -EINVAL;
    }
    iic_baud_reg(iic_regs[id]) = sysclk / (2 * baud);
    return 0;
}

static void hw_iic_set_die(hw_iic_dev iic, u8 en)
{
    u8 id = iic_get_id(iic);
    u32 scl, sda;
    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    if (id == 0) {
        gpio_set_die(scl, en);
        gpio_set_die(sda, en);
    } else {
        //undefined
    }
}

void hw_iic_suspend(hw_iic_dev iic)
{
    hw_iic_set_die(iic, 0);
}

void hw_iic_resume(hw_iic_dev iic)
{
    hw_iic_set_die(iic, 1);
}

int hw_iic_init(hw_iic_dev iic)
{
    int ret;
    u8 id = iic_get_id(iic);

    if ((ret = iic_port_init(iic))) {
        printf("invalid hardware iic port\n");
        return ret;
    }
    hw_iic_set_die(iic, 1);
    if (iic_info_role(iic) == IIC_MASTER) {
        iic_host_mode(iic_regs[id]);
        if ((ret = hw_iic_set_baud(iic, iic_info_baud(iic)))) {
            printf("iic baudrate is invalid\n");
            return ret ;
        }
    } else {
        iic_slave_mode(iic_regs[id]);
        iic_slave_scl_pull_down_enble(iic_regs[id]); //在收到/在发送数据时把SCL拉低
        iic_dir_in(iic_regs[id]);
    }
    if (iic_info_io_filt(iic)) {
        iic_isel_filter(iic_regs[id]);
    } else {
        iic_isel_direct(iic_regs[id]);
    }

    iic_auto_ack(iic_regs[id]);
    iic_int_disable(iic_regs[id]);
    iic_pnding_clr(iic_regs[id]);
    iic_enable(iic_regs[id]);
    /* iic_disable(iic_regs[id]); */
#if 0
    printf("info->scl = %d\n", iic_get_scl(iic));
    printf("info->sda = %d\n", iic_get_sda(iic));
    printf("info->baudrate = %d\n", iic_info_baud(iic));
    printf("info->hdrive = %d\n", iic_info_hdrive(iic));
    printf("info->io_filter = %d\n", iic_info_io_filt(iic));
    printf("info->io_pu = %d\n", iic_info_io_pu(iic));
    printf("info->role = %d\n", iic_info_role(iic));
    printf("IIC_CON0 0x%04x\n", iic_regs[id]->CON0);
    printf("IIC_CON1 0x%04x\n", iic_regs[id]->CON1);
    printf("IIC_BAUD 0x%02x\n", iic_regs[id]->BAUD);
    //printf("IIC_BUF %02x\n", iic_regs[id]->BUF);
    printf("IOMC1 0x%08x\n", JL_IOMAP->CON1);
#endif
    return 0;
}

void hw_iic_uninit(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);
    u32 scl, sda;

    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    hw_iic_set_die(iic, 0);
    if (id == 0) {
        gpio_set_hd(scl, 0);
        gpio_set_hd(sda, 0);
        gpio_set_pull_up(scl, 0);
        gpio_set_pull_up(sda, 0);
        if (scl >= IO_PORT_DP || sda >= IO_PORT_DP) {
            usb_iomode(0);
        }
    }
    iic_disable(iic_regs[id]);
}

static u8 start_signal = 0;
static u8 end_signal = 0;
void hw_iic_start(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);
    /* iic_enable(iic_regs[id]); */
    start_signal |= BIT(7);
    start_signal++;
    if ((start_signal & 0x7f) == 2) {
        start_signal &= ~BIT(7);
    }
}

void hw_iic_stop(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);
    /* iic_disable(iic_regs[id]); */
    iic_host_send_stop(iic_regs[id]); //stop singal
    start_signal = 0;
    while (!iic_host_is_stop_pending(iic_regs[id]));
}

u8 hw_iic_tx_byte(hw_iic_dev iic, u8 byte)
{
    u8 ack = 0;

    u8 id = iic_get_id(iic);

    iic_dir_out(iic_regs[id]);

    if ((start_signal & 0x7f) >= 2) { //连续两次起始信号则进入发送地址并接收模式
        iic_dir_in(iic_regs[id]);
        iic_host_receive_continue_byte(iic_regs[id]);
        iic_restart(iic_regs[id]);
        /* printf("rst/\n"); */
    }

    iic_buf_reg(iic_regs[id]) = byte;

    if (start_signal & BIT(7)) {
        iic_kick_start(iic_regs[id]); //kick start
        start_signal &= ~BIT(7);
        /* printf("st/\n"); */
    }

    while (!iic_is_pnding(iic_regs[id]));

    if (!iic_host_send_is_ack(iic_regs[id])) {
        ack = 1;
    }
    iic_pnding_clr(iic_regs[id]);


    return ack;
}

u8 hw_iic_rx_byte(hw_iic_dev iic, u8 ack)
{
    u8 data = 0;
    u8 id = iic_get_id(iic);
    iic_dir_in(iic_regs[id]);

    iic_host_receive_continue_byte(iic_regs[id]);

    iic_host_read_kick_start(iic_regs[id]);

    if (ack) {
    } else {
        iic_host_nack_auto_stop(iic_regs[id]); //硬件检测到nack(这个nack是我们硬件发出的), 自动长生stop信号
        iic_host_receive_continue_byte_stop(iic_regs[id]); //最后1byte, 完成后自动nack;
    }
    while (!iic_is_pnding(iic_regs[id]));
    data = iic_buf_reg(iic_regs[id]);
    iic_pnding_clr(iic_regs[id]);

    return data;
}

int hw_iic_write_buf(hw_iic_dev iic, const void *buf, int len)
{
    u8 id = iic_get_id(iic);
    int i = 0;

    if (!buf || !len) {
        return -1;
    }
    iic_dir_out(iic_regs[id]);

    for (i = 0; i < len; i++) {
        iic_buf_reg(iic_regs[id]) = ((u8 *)buf)[i];
        /* if (i == 0) { */
        /*     iic_kick_start(iic_regs[id]); //kick start */
        /* } */
        while (!iic_is_pnding(iic_regs[id]));

        if (iic_host_send_is_ack(iic_regs[id])) {
            iic_pnding_clr(iic_regs[id]);
            break;
        }
        iic_pnding_clr(iic_regs[id]);
    }


    return i;
}

int hw_iic_read_buf(hw_iic_dev iic, void *buf, int len)
{
    u8 id = iic_get_id(iic);
    int i;
    if (!buf || !len) {
        return -1;
    }
    iic_dir_in(iic_regs[id]);
    iic_host_receive_continue_byte(iic_regs[id]);

    for (i = 0; i < len; i++) {
        iic_host_read_kick_start(iic_regs[id]);
        if (i == len - 1) {
            iic_host_nack_auto_stop(iic_regs[id]); //硬件检测到nack(这个nack是我们硬件发出的), 自动长生stop信号
            iic_host_receive_continue_byte_stop(iic_regs[id]); //最后1byte, 完成后自动nack;
        }
        while (!iic_is_pnding(iic_regs[id]));
        iic_pnding_clr(iic_regs[id]);

        ((u8 *)buf)[i] = iic_buf_reg(iic_regs[id]);
    }
    return len;
}






/*================================= IIC slave mode ============================*/
//测试：需两个，一个下载host_test程序,一个下载slave_test程序.宏:IIC_SLAVE_BYTE_MODE_EN控制从机接收方式
static int cur_iic = 0;
#define IIC_SLAVE_BYTE_MODE_EN 1 //1:iic slave byte rx ; 0:iic slave dma rx
#define IIC_SLAVE_wADDR 		0xB0
#define IIC_SLAVE_rADDR 		0xB1

enum {
    IIC_STATE_START = 0,
    IIC_STATE_RECEIVE,
    IIC_STATE_SEND,
};
#define RX_LEN 80
u8 rx_data[RX_LEN];

___interrupt
static void hw_iic_slave_irq(void)
{
#if IIC_SLAVE_BYTE_MODE_EN //byte mode test
    static u8 iic_slave_state = IIC_STATE_START;
    static u8 receive_cnt = 0;
    /*========= 1.从机单byte 发送和接收 ============*/
    u8 id = iic_get_id(cur_iic);
    iic_pnding_clr(iic_regs[id]);
    if (iic_slave_state == IIC_STATE_START) {
        if (iic_slave_is_required_send(iic_regs[id])) {
            iic_dir_out(iic_regs[id]);
            iic_slave_state = IIC_STATE_SEND;
            /* printf("Send"); */
            goto _slave_isr_send;
        } else {
            iic_dir_in(iic_regs[id]);
            iic_slave_state = IIC_STATE_RECEIVE;
            /* printf("Receive"); */
            receive_cnt = 0;
            goto _slave_isr_end;
        }
    }

    /*========= 1-1.从机单byte 接收 ============*/
#define REV_CNT 	RX_LEN
    if (iic_slave_state == IIC_STATE_RECEIVE) {
        rx_data[receive_cnt++] = iic_buf_reg(iic_regs[id]);
        /* HOW To Know END (stop singal)*/
        if (receive_cnt < REV_CNT - 1) {
        } else if (receive_cnt < REV_CNT) {
            iic_force_nack(iic_regs[id]); //从机收到最后1byte，会nack
            /* printf("nack\n"); */
        } else {
            iic_auto_ack(iic_regs[id]);
            iic_slave_state = IIC_STATE_START;
            put_buf(rx_data, receive_cnt); //接受完成
        }
    }
    /*========= 1-2.从机单byte 发送 ============*/
_slave_isr_send:
    if (iic_slave_state == IIC_STATE_SEND) {
        static u8 send_byte = 0x0;
        if (iic_slave_send_is_end(iic_regs[id])) { //主机回了nack
            iic_slave_state = IIC_STATE_START;
            send_byte = 0;
            printf("host nack\n");
        } else {
            /* printf(" 	slave send: 0x%x", send_byte); */
            iic_buf_reg(iic_regs[id]) = send_byte++;
        }
    }
#else //DMA mode test
    /*========= 3.从机dma 接收 ============*/
    /*========= pending条件 addr + dma buf full ============*/
    u8 id = iic_get_id(cur_iic);
    iic_pnding_clr(iic_regs[id]);
    if (iic_slave_dma_is_enable(iic_regs[id])) {
        u32 buf_len = iic_slave_dma_get_buf_len(iic_regs[id]);
        /* iic_slave_dma_disable(iic_regs[id]); */
        iic_force_nack(iic_regs[id]);
        iic_auto_ack(iic_regs[id]);
        printf("dma end. rx len:%d\n", buf_len);
        put_buf((u8 *)iic_slave_dma_buf(iic_regs[id]), buf_len);
    }

#endif
_slave_isr_end:
    /*iic_pnding_clr(iic_regs[id]);*/
    iic_slave_scl_pull_down_release(iic_regs[id]);
}

void hw_iic_set_slave_addr(hw_iic_dev iic, u8 slave_addr)
{
    u8 id = iic_get_id(iic);
    iic_baud_reg(iic_regs[id]) = slave_addr;
}

int hw_iic_slave_init(hw_iic_dev iic, u8 dma_en, u32 *buf, u32 buf_len)
{
    int ret;
    u8 id = iic_get_id(iic);
    hw_iic_init(iic);//结构体成员: role=IIC_SLAVE

    iic_disable(iic_regs[id]);
//设置从机地址
    hw_iic_set_slave_addr(iic, IIC_SLAVE_wADDR); //从机地址
    iic_dir_in(iic_regs[id]);
//从机dma
    if (dma_en) {
        if (buf) {
            /*iic_slave_dma_big_endian(iic_regs[id]);*/
            iic_slave_dma_little_endian(iic_regs[id]);
            iic_slave_dma_buf(iic_regs[id]) = (u32)buf;
            if (((u32)buf % 4) != 0) {
                printf("dma buf not align 4");
            }
            iic_slave_dma_buf_depth(iic_regs[id]) = buf_len ;//单位: 32bit
            iic_slave_dma_enable(iic_regs[id]);
        } else {
            printf("IIC SLAVE DMA BUF NULL");
        }
    } else {
        iic_slave_dma_disable(iic_regs[id]);
    }
//中断
    request_irq(IRQ_IIC_IDX, 1, hw_iic_slave_irq, 0);
    iic_int_enable(iic_regs[id]); //中断使能

    cur_iic = iic;

    iic_enable(iic_regs[id]);
    iic_kick_start(iic_regs[id]);
#if 0
    printf("info->scl = %d\n", iic_get_scl(iic));
    printf("info->sda = %d\n", iic_get_sda(iic));
    printf("info->baudrate = %d\n", iic_info_baud(iic));
    printf("info->hdrive = %d\n", iic_info_hdrive(iic));
    printf("info->io_filter = %d\n", iic_info_io_filt(iic));
    printf("info->io_pu = %d\n", iic_info_io_pu(iic));
    printf("IIC_CON0 0x%04x\n", iic_regs[id]->CON0);
    printf("IIC_BAUD 0x%02x\n", iic_regs[id]->BAUD);
#endif
    return 0;
}

/*================iic slave int test================*/
static u32 dma_buf[20] __attribute__((aligned(4)));
int hw_iic_slave_test(void)
{
    int tmp = 0;
//结构体成员: role=IIC_SLAVE
#if IIC_SLAVE_BYTE_MODE_EN //byte mode test
    hw_iic_slave_init(0, 0, NULL, 0);
    printf("\niic slave byte test:\t");
#else //dma mode test
    hw_iic_slave_init(0, 1, dma_buf, sizeof(dma_buf) / 4);
    printf("\niic slave dma test:\t");
#endif
    __asm__ volatile("%0 =icfg" : "=r"(tmp)); //读
    printf("%s icfg = 0x%x\n", __func__, tmp);
#if 0
    u32 con2 = 0;
    u32 *addr = NULL;
    addr = (u8 *)iic_slave_dma_buf(iic_regs[0]);
    con2 = iic_slave_dma_buf_depth(iic_regs[0]);
    while (1) {
        if (con2 != iic_slave_dma_buf_depth(iic_regs[0])) {
            asm("trigger");
            con2 = iic_slave_dma_buf_depth(iic_regs[0]);
            printf("dma con2: 0x%x, data = 0x%x @ 0x%x", con2, *(addr + (con2 >> 16)), addr + (con2 >> 16));
        }
        delay(1000);
    };
#endif

    delay(10000);

    return 0;
}

/*================iic host int test================*/
volatile u8 flag = 0;
___interrupt
static void hw_iic_host_irq(void)
{
    u8 id = iic_get_id(0);
    /* printf("%s", __func__); */
    iic_pnding_clr(iic_regs[id]);
    iic_host_send_stop(iic_regs[id]); //stop singal
    while (!iic_host_is_stop_pending(iic_regs[id]));
    flag = 0 ;
}

static void hw_iic_host_int_test(void)
{
    u8 id = iic_get_id(0);
    iic_disable(iic_regs[id]);
    request_irq(IRQ_IIC_IDX, 1, hw_iic_host_irq, 0);
    iic_int_enable(iic_regs[id]);

    iic_enable(iic_regs[id]);

    iic_dir_out(iic_regs[id]);
    printf("%s in\n", __func__);
    for (int i = 0; i < 50; i++) {
        flag = 1;
        iic_buf_reg(iic_regs[id]) = IIC_SLAVE_wADDR;
        iic_kick_start(iic_regs[id]); //kick start
        printf("%s run\n", __func__);
        while (flag);
    }
    printf("%s out\n", __func__);
    while (1);
}

/*================iic host test================*/
static void hw_iic_host_test_process(hw_iic_dev iic)//与从机接收对应
{
    const u8 test_len = RX_LEN;
    u8 retry = 50;
    u8 id = iic_get_id(iic);
    u8 ack = 0;
    int i;
//主发
__retry_tx:
    hw_iic_start(iic);
    ack = hw_iic_tx_byte(iic, IIC_SLAVE_wADDR);
    if (!ack && retry--) {
        hw_iic_stop(iic);
        delay(3000);
        goto __retry_tx;
    }
    u8 data = 'A';
    for (i = 0; i < test_len - 1; i++) {
        hw_iic_tx_byte(iic, data++);
    }
    ack = hw_iic_tx_byte(iic, data);
    hw_iic_stop(iic);
    delay(8000000);
//主收
__retry_rx:
    hw_iic_start(iic);
    ack = hw_iic_tx_byte(iic, IIC_SLAVE_rADDR);
    if (!ack && retry--) {
        hw_iic_stop(iic);
        delay(3000);
        goto __retry_rx;
    }
    u8 rx_data[50];
    for (i = 0; i < test_len - 1; i++) {
        rx_data[i] = hw_iic_rx_byte(iic, 1);
    }
    rx_data[i] = hw_iic_rx_byte(iic, 0);
    hw_iic_stop(iic);
    put_buf(rx_data, test_len);
}

int hw_iic_host_test(void)
{
    printf("%s\n", __func__);
    hw_iic_init(0);//结构体成员: role=IIC_MASTER
    /* hw_iic_host_int_test(); */
    hw_iic_host_test_process(0);

    return 0;
}











void iic_disable_for_ota()
{
    JL_IIC->CON0 = 0;
}

