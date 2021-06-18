#include "asm/includes.h"
#include "asm/spi.h"

/*
   [[[ README ]]]
   1. 本spi测试demo提供了spi.c的API使用例程，测试方式为两个spi的环回测试。
   spi1设置为主机模式，spi2设置为从机模式，spi1发送数据到spi2，然后接收spi2
   原样返回的数据，然后比较发送出去的数据与接收的数据是否一致，一致则说明
   验证通过。
   2. 本demo涉及BYTE收发测试及DMA收发测试，通过宏SPI_TEST_BYTE_MODE_EN选择(
   1:byte, 0:dma)。另外demo还涉及到spi中断中调用可用于中断的spi API的使用。
   3. spi.c的API不包含CS引脚，CS由API以外控制。
   4. 配置结构体定义在board_xxx.c中，例如用到spi1，需要定义spi1_p_data，
   否则编译出错。
   5. spi的DMA地址需要4字节对齐。
   6. 虽然spi.c的API带有spi0，但在有挂spi flash的芯片上使用可能会出问题，
   避免使用spi0。

*/

static u8 slave_dir = 1;
static u8 spi1_send_buf[100] __attribute__((aligned(4)));
static u8 spi1_recv_buf[100] __attribute__((aligned(4)));
static u8 spi2_send_buf[100] __attribute__((aligned(4)));
static u8 spi2_recv_buf[100] __attribute__((aligned(4)));

static spi_dev spi1_hdl = 1;
static spi_dev spi2_hdl = 2;

#define SPI_TEST_BYTE_MODE_EN         1//测试模式选择: 1:byte   0:dma

static void spi_cs_init(u32 gpio, u8 dir)//dir:1输入，0：输出
{
    if (dir) {
        gpio_set_direction(gpio, 1);
    } else {
        gpio_set_direction(gpio, 0);
    }
    gpio_set_die(gpio, 1);
    gpio_set_pull_up(gpio, 0);
    gpio_set_pull_down(gpio, 0);
}


__attribute__((interrupt("")))//中断函数，需以下特殊声明
static void spi2_isr()
{
    static int i = 0;
    if (spi_get_pending(spi2_hdl)) {
        spi_clear_pending(spi2_hdl);
        if (gpio_read(IO_PORTA_03)) {
            return;
        }
#if SPI_TEST_BYTE_MODE_EN
        if (slave_dir == 1) {
            spi2_recv_buf[i] = spi_recv_byte_for_isr(spi2_hdl);
            spi_send_byte_for_isr(spi2_hdl, spi2_recv_buf[i]);
            i >= 100 ? i = 0 : i++;
            slave_dir = 0;
        } else {
            slave_dir = 1;
        }
#else
        if (slave_dir == 1) {
            spi_dma_set_addr_for_isr(spi2_hdl, spi2_recv_buf, 100, 0);
            slave_dir = 0;
        } else {
            slave_dir = 1;
        }
#endif
    }
}


void spi_test_main()
{
    int i;
    int err;

    spi_open(spi1_hdl);
    spi_open(spi2_hdl);
    spi_set_ie(spi2_hdl, 1);
    request_irq(IRQ_SPI2_IDX, 3, spi2_isr, 0);//配置中断优先级，中断函数
//spi cs init
    spi_cs_init(IO_PORTB_04, 0); //spi1 cs out
    gpio_write(IO_PORTB_04, 1);
    spi_cs_init(IO_PORTA_03, 1); //spi2 cs in
    for (i = 0; i < 100; i++) {
        spi1_send_buf[i] = i % 26 + 'A';
        spi1_recv_buf[i] = 0;
    }
    puts(">>> spi test start\n");
#if SPI_TEST_BYTE_MODE_EN
    puts(">>> spi byte test\n");
    gpio_write(IO_PORTB_04, 0);
    for (i = 0; i < 100; i++) {
        err = spi_send_byte(spi1_hdl, spi1_send_buf[i]);
        if (err) {
            puts("spi1 byte send timeout\n");
            break;
        }
        delay(100);
        spi1_recv_buf[i] = spi_recv_byte(spi1_hdl, &err);
        if (err) {
            puts("spi1 byte recv timeout\n");
            break;
        }
        delay(100);
    }
    gpio_write(IO_PORTB_04, 1);
#else
    puts(">>> spi dma test\n");
    spi_dma_set_addr_for_isr(spi2_hdl, spi2_recv_buf, 100, 1);
    gpio_write(IO_PORTB_04, 0);
    err = spi_dma_send(spi1_hdl, spi1_send_buf, 100);
    if (err < 0) {
        puts("spi1 dma send timeout\n");
        goto __out_dma;
    }

    err = spi_dma_recv(spi1_hdl, spi1_recv_buf, 100);//delay(100);
    if (err < 0) {
        puts("spi1 dma recv timeout\n");
        goto __out_dma;
    }

__out_dma://delay(100);
    gpio_write(IO_PORTB_04, 1);
#endif
    puts("<<< spi test end\n");

    puts("\nspi master receivce buffer:\n");
    put_buf(spi1_recv_buf, 100);

    if (!memcmp(spi1_send_buf, spi1_recv_buf, 100)) {
        puts("\nspi test pass\n");
    } else {
        puts("\nspi test fail\n");
    }

    spi_close(spi1_hdl);
    spi_close(spi2_hdl);
}

