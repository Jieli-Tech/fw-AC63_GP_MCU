#include "typedef.h"
#include "asm/hwi.h"
#include "asm/csfr.h"

extern int ISR_BASE;
#define ISR_ENTRY  (u32)&ISR_BASE


void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id)
{
    unsigned char icfg_wdat;
    unsigned int icfg_num, icfg_bit;
    unsigned int *israddr = (unsigned int *)ISR_ENTRY;

    israddr[index] = (u32)handler;

    icfg_num =  index / 8;
    icfg_bit = (index % 8) * 4;
    icfg_wdat = ((priority & 0x7) << 1) | BIT(0);
    unsigned int icfg_clr = ~(0xf << icfg_bit);
    unsigned int icfg_set = icfg_wdat << icfg_bit;

    local_irq_disable();

    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    icfg_ptr[icfg_num] = (icfg_ptr[icfg_num] & icfg_clr) | icfg_set;

    local_irq_enable();
}


void interrupt_init()
{
    int i ;

    local_irq_disable();

    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    for (i = 0 ; i < 32 ; i++) {
        icfg_ptr[i] = 0;
    }

    local_irq_enable();
}

//------------------------------------------------------//
// interrupt ip
//------------------------------------------------------//
void reg_set_ip(unsigned char index, unsigned char priority)
{
    unsigned char icfg_wdat;
    unsigned int icfg_num, icfg_bit;

    icfg_num =  index / 8;
    icfg_bit = (index % 8) * 4;
    icfg_wdat = (priority & 0x7) << 1;
    unsigned int icfg_clr = ~(0xe << icfg_bit);
    unsigned int icfg_set = icfg_wdat << icfg_bit;

    local_irq_disable();

    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    icfg_ptr[icfg_num] = (icfg_ptr[icfg_num] & icfg_clr) | icfg_set;

    local_irq_enable();
}

//------------------------------------------------------//
// interrupt ie
//------------------------------------------------------//

void bit_set_ie(unsigned char index)
{
    unsigned int icfg_num, icfg_bit;

    icfg_num =  index / 8;
    icfg_bit = (index % 8) * 4;

    local_irq_disable();

    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    icfg_ptr[icfg_num] |= BIT(icfg_bit);

    local_irq_enable();
}

void bit_clr_ie(unsigned char index)
{
    unsigned int icfg_num, icfg_bit;

    icfg_num =  index / 8;
    icfg_bit = (index % 8) * 4;

    local_irq_disable();
    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    icfg_ptr[icfg_num] &= ~BIT(icfg_bit);
    local_irq_enable();
}

bool irq_read(u32 index)
{
    unsigned int icfg_num =  index / 8;
    unsigned int icfg_bit = (index % 8) * 4;

    volatile unsigned int *icfg_ptr = &(q32DSP(0)->ICFG00);
    return (icfg_ptr[icfg_num] & BIT(icfg_bit)) ? 1 : 0;
}

__attribute__((always_inline_when_const_args))
sec(.volatile_ram_code)
u32 irq_check_ipnd(u32 index)
{
    if (index == -1) { //all irq
        return q32DSP(0)->IPND0 | q32DSP(0)->IPND1;
    } else {
        unsigned int ipnd_num =  index / 32;
        unsigned int ipnd_bit = (index % 32);

        const volatile unsigned int *ipnd_ptr = &(q32DSP(0)->IPND0);
        return (ipnd_ptr[ipnd_num] & BIT(ipnd_bit)) ? 1 : 0;
    }
}

void unrequest_irq(u8 index)
{
    bit_clr_ie(index);
}


static const u8 usb_upgrade_mode_magic[16] = {
    0x33, 0xEA, 0xBB, 0x74, 0x85, 0x71, 0x8F, 0x32, 0xA7, 0x3C, 0xF0, 0x18, 0x4D, 0x20, 0x08, 0xF7
};

void check_upgrade()
{
    char *magic_str = (char *)ISR_ENTRY;
    magic_str += IRQ_SOFT0_IDX * 4;

    if (memcmp(magic_str, usb_upgrade_mode_magic, sizeof(usb_upgrade_mode_magic)) == 0) {
        memset(magic_str, 0, sizeof(usb_upgrade_mode_magic));
        void usb_upgrade_mode();
        usb_upgrade_mode();
    }
}
void chip_reboot_entry_usb_upgrade_mode()
{
    char *magic_str = (char *)ISR_ENTRY;
    magic_str += IRQ_SOFT0_IDX * 4;
    memcpy(magic_str, usb_upgrade_mode_magic, sizeof(usb_upgrade_mode_magic));
    cpu_reset();
}


























