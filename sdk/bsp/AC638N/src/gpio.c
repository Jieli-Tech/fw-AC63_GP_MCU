#include "typedef.h"
#include "asm/includes.h"
#include "asm/gpio.h"



#define USB_DP_OUT(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(0)) | ((x & 0x1)<<0))
#define USB_DM_OUT(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(1)) | ((x & 0x1)<<1))
#define USB_DP_DIR(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(2)) | ((x & 0x1)<<2))
#define USB_DM_DIR(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(3)) | ((x & 0x1)<<3))
#define USB_DP_PU(x)    JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(4)) | ((x & 0x1)<<4))
#define USB_DM_PU(x)    JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(5)) | ((x & 0x1)<<5))
#define USB_DP_PD(x)    JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(6)) | ((x & 0x1)<<6))
#define USB_DM_PD(x)    JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(7)) | ((x & 0x1)<<7))
#define USB_DP_DIE(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(8)) | ((x & 0x1)<<8))
#define USB_DM_DIE(x)   JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(9)) | ((x & 0x1)<<9))
#define USB_DP_DIEH(x)  JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(10)) | ((x & 0x1)<<10))
#define USB_DM_DIEH(x)  JL_USB_IO->CON0 = ((JL_USB_IO->CON0 & ~BIT(11)) | ((x & 0x1)<<11))

#define USB_DP_IN()     (!!(JL_USB->CON0 & BIT(17)))
#define USB_DM_IN()     (!!(JL_USB->CON0 & BIT(16)))

#define USB_SET(gpio, reg_name, value)	\
	do { \
		if (IO_PORT_DP == (gpio)) { \
			USB_DP_##reg_name(value); \
		} \
		if (IO_PORT_DM == (gpio)) { \
			USB_DM_##reg_name(value); \
		} \
	} while (0)


static const u32 gpio_regs[] = {
    (u32) JL_PORTA,
    (u32) JL_PORTB,
    (u32) JL_PORTC,
    (u32) JL_PORTD,
    (u32) JL_PORTP,
};


#define __gpio_mask(gpio)   BIT((gpio) % IO_GROUP_NUM)

struct gpio_reg *gpio2reg(u32 gpio)
{
    if (gpio > IO_MAX_NUM) {
        return NULL;
    }
    return (struct gpio_reg *)gpio_regs[gpio / IO_GROUP_NUM];
}

__attribute__((always_inline_when_const_args))
u32 usb_set_direction(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio + 2);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio + 2);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_output(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_set_pull_up(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio + 4);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio + 4);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_set_pull_down(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio + 6);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio + 6);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_set_die(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio + 8);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio + 8);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_set_dieh(u32 gpio, u32 value)
{
    gpio -= IO_PORT_DP;
    if (value) {
        JL_USB_IO->CON0 |= BIT(gpio + 10);
    } else {
        JL_USB_IO->CON0 &= ~BIT(gpio + 10);
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 usb_read(u32 gpio)
{
    gpio -= IO_PORT_DP;
    u32 v = JL_USB->CON0 & (BIT(17 - gpio));
    return !!v;
}

__attribute__((always_inline_when_const_args))
void usb_iomode(u32 enable)
{
    if (enable) {
        JL_USB->CON0 = 0;
        JL_USB_IO->CON0 |= BIT(12) | BIT(14);
    } else {
        JL_USB_IO->CON0 &= ~(BIT(12) | BIT(14));
    }
}

__attribute__((always_inline_when_const_args))
int gpio_set_direction(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if ((gpio == IO_PORT_DP) || (gpio == IO_PORT_DM)) {
        usb_iomode(1);
        USB_SET(gpio, DIR, value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->dir |= mask;
    } else {
        g->dir &= ~mask;
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_set_output_value(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        USB_DP_OUT(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_OUT(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value == 1) {
        g->out |= mask;
    } else {
        g->out &= ~mask;
    }
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_read_input_value(u32 gpio)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        return USB_DP_IN();
    } else if (gpio == IO_PORT_DM) {
        return USB_DM_IN();
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    int v = !!(g->in & mask);
    return v;
}

__attribute__((always_inline_when_const_args))
int gpio_direction_input(u32 gpio)
{
    u32 mask;
    struct gpio_reg *g;
    if ((gpio == IO_PORT_DP) || (gpio == IO_PORT_DM)) {
        usb_iomode(1);
        USB_SET(gpio, DIR, 1);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    g->dir |= mask;
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_dir(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    dat = dat & (BIT(len) - 1);
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->dir;
    switch (op) {
    case GPIO_SET:
        SFR(sfr, start, len, dat);
        break;
    case GPIO_AND:
        sfr &= (dat << start);
        break;
    case GPIO_OR:
        sfr |= (dat << start);
        break;
    case GPIO_XOR:
        sfr ^= (dat << start);
        break;
    }
    g->dir = sfr;
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_direction_output(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;

    if ((gpio == IO_PORT_DP) || (gpio == IO_PORT_DM)) {
        usb_iomode(1);
        USB_SET(gpio, DIR, 0);
        USB_SET(gpio, OUT, value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value == 1) {
        g->out |= mask;
    } else if (value == 0) {
        g->out &= ~mask;
    }
    g->dir &= ~mask;
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_write(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        USB_DP_OUT(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_OUT(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value == 1) {
        g->out |= mask;
    } else {
        g->out &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_out(u32 gpio, u32 start, u32 len, u32 dat)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->out;
    SFR(sfr, start, len, dat);
    g->out = sfr;

    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_read(u32 gpio)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        return USB_DP_IN();
    } else if (gpio == IO_PORT_DM) {
        return USB_DM_IN();
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    int v = !!(g->in & mask);
    return v;
}
__attribute__((always_inline_when_const_args))
int gpio_in(u32 gpio)
{
    u32 mask;
    struct gpio_reg *g;
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    u32 v = g->in;
    return v;
}
__attribute__((always_inline_when_const_args))
int gpio_set_pull_up(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        USB_DP_PU(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_PU(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->pu |= mask;
    } else {
        g->pu &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_pu(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    dat = dat & (BIT(len) - 1);
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->pu;
    switch (op) {
    case GPIO_SET:
        SFR(sfr, start, len, dat);
        break;
    case GPIO_AND:
        sfr &= (dat << start);
        break;
    case GPIO_OR:
        sfr |= (dat << start);
        break;
    case GPIO_XOR:
        sfr ^= (dat << start);
        break;
    }
    g->pu = sfr;
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_pull_down(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        USB_DP_PD(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_PD(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->pd |= mask;
    } else {
        g->pd &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_pd(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    dat = dat & (BIT(len) - 1);
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->pd;
    switch (op) {
    case GPIO_SET:
        SFR(sfr, start, len, dat);
        break;
    case GPIO_AND:
        sfr &= (dat << start);
        break;
    case GPIO_OR:
        sfr |= (dat << start);
        break;
    case GPIO_XOR:
        sfr ^= (dat << start);
        break;
    }
    g->pd = sfr;
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_hd0(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->hd0 |= mask;
    } else {
        g->hd0 &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_hd(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->hd |= mask;
    } else {
        g->hd &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_die(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;
    if (gpio == IO_PORT_DP) {
        USB_DP_DIE(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_DIE(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->die |= mask;
    } else {
        g->die &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_set_dieh(u32 gpio, u32 value)
{
    u32 mask;
    struct gpio_reg *g;

    if (gpio == IO_PORT_DP) {
        USB_DP_DIEH(value);
        return 0;
    } else if (gpio == IO_PORT_DM) {
        USB_DM_DIEH(value);
        return 0;
    }
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    mask = __gpio_mask(gpio);
    if (value) {
        g->dieh |= mask;
    } else {
        g->dieh &= ~mask;
    }
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_die(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    dat = dat & (BIT(len) - 1);
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->die;
    switch (op) {
    case GPIO_SET:
        SFR(sfr, start, len, dat);
        break;
    case GPIO_AND:
        sfr &= (dat << start);
        break;
    case GPIO_OR:
        sfr |= (dat << start);
        break;
    case GPIO_XOR:
        sfr ^= (dat << start);
        break;
    }
    g->die = sfr;
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_dieh(u32 gpio, u32 start, u32 len, u32 dat, enum gpio_op_mode op)
{
    struct gpio_reg *g;
    start = start > gpio ? start - gpio : start;
    dat = dat & (BIT(len) - 1);
    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }
    u32 sfr = g->dieh;
    switch (op) {
    case GPIO_SET:
        SFR(sfr, start, len, dat);
        break;
    case GPIO_AND:
        sfr &= (dat << start);
        break;
    case GPIO_OR:
        sfr |= (dat << start);
        break;
    case GPIO_XOR:
        sfr ^= (dat << start);
        break;
    }
    g->dieh = sfr;
    return 0;
}

__attribute__((always_inline_when_const_args))
u32 get_gpio(const char *p)
{
    u32 port;
    if (p[0] == 'P') {
        if (p[1] >= 'A' && p[1] <= 'H') {
            port = p[1] - 'A';
            port *= IO_GROUP_NUM;
        } else if (p[1] == 'P') {
            return IO_PORTP_00;
        } else {
            return -EINVAL;
        }
        int mask = (p[2] - '0') * 10 + (p[3] - '0');
        if (mask < 16) {
            port += mask;
            return port;
        }
    }
    if (strcmp(p, "USBDP") == 0) {
        return  IO_PORT_DP;
    }
    if (strcmp(p, "USBDM") == 0) {
        return  IO_PORT_DM;
    }
    return -EINVAL;
}

//===================================================//
// BR30 Crossbar API
//===================================================//
static const u32 gpio_out_regs[] = {
    (u32) &(JL_OMAP->PA0_OUT),
    (u32) &(JL_OMAP->PB0_OUT),
    (u32) &(JL_OMAP->PC0_OUT),
    (u32) &(JL_OMAP->PD0_OUT),
    (u32) &(JL_OMAP->PP0_OUT),
};

static const u32 gpio_in_regs[] = {
    (u32) PA0_IN,
    (u32) PB0_IN,
    (u32) PC0_IN,
    (u32) PD0_IN,
    (u32) PP0_IN,
};

__attribute__((always_inline_when_const_args))
u32 *gpio2crossbar_outreg(u32 gpio)
{
    if (gpio >= IO_MAX_NUM) {
        if (gpio == IO_PORT_DP) {
            return (u32 *) & (JL_OMAP->USBDP_OUT);
        } else if (gpio == IO_PORT_DM) {
            return (u32 *) & (JL_OMAP->USBDM_OUT);
        } else {
            return NULL;
        }
    }
    u32 *reg;
    u32 group_index = gpio / IO_GROUP_NUM;
    u32 port_index = gpio % IO_GROUP_NUM;

    reg = (u32 *)gpio_out_regs[group_index];

    return reg + port_index;
}

__attribute__((always_inline_when_const_args))
u32 gpio2crossbar_inport(u32 gpio)
{
    if (gpio >= IO_MAX_NUM) {
        if (gpio == IO_PORT_DP) {
            return USBDP_IN;
        } else if (gpio == IO_PORT_DM) {
            return USBDM_IN;
        } else {
            return -1;
        }
    }
    u32 group_index = gpio_in_regs[gpio / IO_GROUP_NUM];
    u32 port_index = gpio % IO_GROUP_NUM;

    return group_index + port_index;
}

__attribute__((always_inline_when_const_args))
int gpio_set_fun_output_port(u32 gpio, u32 fun_index, u8 dir_ctl, u8 data_ctl)
{
    u32 *pout = gpio2crossbar_outreg(gpio);
    if (pout == NULL) {
        return -EINVAL;
    }
    u32 en = 0;
    if (dir_ctl) {
        en |= BIT(0);
    }
    if (data_ctl) {
        en |= BIT(1);
    }
    *pout = (fun_index << 2) | en;
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_disable_fun_output_port(u32 gpio)
{
    u32 *pout = gpio2crossbar_outreg(gpio);
    if (pout == NULL) {
        return -EINVAL;
    }
    *pout &= ~(BIT(1) | BIT(0));
    return 0;
}


__attribute__((always_inline_when_const_args))
int gpio_set_fun_input_port(u32 gpio, enum PFI_TABLE pfun)
{
    u32 *p_fun = (u32 *)pfun;
    if (p_fun == 0) {
        return -EINVAL;
    }
    u32 iport = gpio2crossbar_inport(gpio);
    if (iport == -1) {
        return -EINVAL;
    }
    *p_fun = iport;
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_disable_fun_input_port(enum PFI_TABLE pfun)
{
    u32 *p_fun = (u32 *)pfun;
    p_fun = 0;
    return 0;
}


