#include "asm/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"


#define EEPROM_RADDR        0xa1
#define EEPROM_WADDR        0xa0

#define DELAY_CNT           0

#define IIC_TYPE  1//0:soft iic  1:hw iic
#if IIC_TYPE
#define iic_dev                             0
#define iic_init(iic)                       hw_iic_init(iic)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)
#else
#define iic_dev                             0
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)
#endif

static u8 eeprom_rbuf[512], eeprom_wbuf[512];
extern void delay(unsigned int cnt);

void eeprom_write(int iic, u8 *buf, u32 addr, u32 len)
{
    int i;
    u32 retry;
    int ret;
    u32 tx_len;
    u32 offset;
#if 1
    offset = 0;
    while (offset < len) {
        tx_len = len - offset > 8 ? 8 : len - offset;
        retry = 100;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            delay(DELAY_CNT);
            ret = iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            delay(DELAY_CNT);
            putchar('r');
            for (i = 0; i < tx_len - 1; i++) {
                ret = iic_tx_byte(iic, buf[offset + i]);
                if (!ret) {
                    if (--retry) {
                        continue;
                    } else {
                        goto __exit;
                    }
                }
                delay(DELAY_CNT);
            }
            putchar('i');
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            putchar('j');
            iic_stop(iic);
            delay(DELAY_CNT);
        } while (0);
        offset += tx_len;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            iic_stop(iic);
            if (ret) {
                break;
            }
            delay(DELAY_CNT);
        } while (1);
    }
__exit:
    iic_stop(iic);
#else
    offset = 0;
    while (offset < len) {
        retry = 100;
        tx_len = len - offset > 8 ? 8 : len - offset;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            putchar('h');
            ret = iic_write_buf(iic, buf + offset, tx_len);
            if (ret < tx_len) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            putchar('i');
            iic_stop(iic);
            delay(DELAY_CNT);
        } while (0);
        offset += tx_len;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            iic_stop(iic);
            if (ret) {
                break;
            }
            delay(DELAY_CNT);
        } while (1);
        putchar('j');
    }
__exit:
    iic_stop(iic);
#endif
}

void eeprom_read(int iic, u8 *buf, u32 addr, u32 len)
{
    int i;
    u32 retry = 100;
    int ret;
#if 1
    do {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        ret = iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        delay(DELAY_CNT);
        putchar('k');
        for (i = 0; i < len - 1; i++) {
            buf[i] = iic_rx_byte(iic, 1);
            delay(DELAY_CNT);
        }
        putchar('l');
        buf[len - 1] = iic_rx_byte(iic, 0);
        iic_stop(iic);
        delay(DELAY_CNT);
        putchar('m');
    } while (0);
#else
    do {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_read_buf(iic, buf, len);
        iic_stop(iic);
    } while (0);
#endif
}

void eeprom_test_main()
{
    int i = 0;
    u8 flag = 0;

    iic_init(iic_dev);
    for (i = 0; i < 512; i++) {
        eeprom_wbuf[i] = i % 26 + 'A';
        eeprom_rbuf[i] = 0;
    }
    puts(">>>> write in\n");
    eeprom_write(iic_dev, eeprom_wbuf, 0, 128);
    puts("<<<< write out\n");
    puts(">>>> read in\n");
    eeprom_read(iic_dev, eeprom_rbuf, 0, 128);
    puts("<<<< read out\n");

    for (i = 0; i < 128; i++) {
        if (eeprom_wbuf[i] != eeprom_rbuf[i]) {
            flag = 1;
            break;
        }
    }
    for (i = 0; i < 128; i++) {
        putchar(eeprom_rbuf[i]);
        putchar(' ');
        if (i % 16 == 15) {
            putchar('\n');
        }
    }
    putchar('\n');
    if (flag == 0) {
        puts("eeprom read/write test pass\n");
    } else {
        puts("eeprom read/write test fail\n");
    }
    iic_uninit(iic_dev);
}

