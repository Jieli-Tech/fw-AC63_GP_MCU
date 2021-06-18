
.section .stack, "a"
         .space 0x600
         .global _stack
_stack:


.section .startup.text, "ax"
    .align 2
    .global _start
    .type _start,@function
    .org 0

_start:
cpu0_start:
	sp = _stack    	// for idle
    ssp = _stack   	// for interrupt

    call boot_info_init

//========================//
//     clear bss 		  //
//========================//
    r3 = bss_begin
    r1 = 0
    r2 = bss_size
    r2 >>= 2;
1:
    rep r2{
        [r3++] = r1
    }
    if(r2 != 0) goto 1b

//========================//
//     copy data 		  //
//========================//
    r4 = data_addr
    r1 = data_begin
    r2 = data_size
    r2 >>= 2;
1:
    rep r2{
        r3 = [r1++]
        [r4++] = r3
    }
    if(r2 != 0)goto 1b

//========================//
//     copy data_code     //
//========================//
    r4 = data_code_addr
    r1 = data_code_begin
    r2 = data_code_size
    r2 >>= 2;
1:
    rep r2{
        r3 = [r1++]
        [r4++] = r3
    }
    if(r2 != 0)goto 1b

//========================//
//     goto main 		  //
//========================//
	r2 = icfg
	r2 &= 0xfffffcff
	r2 |= 0x100
	icfg = r2

	.extern maskrom_init
    call maskrom_init

    .extern main
    r0= main
    goto r0


.extern debug_exception_analyze
// void exception_analyze(unsigned *sp)
.global exception_irq_handler
.type exception_irq_handler, @function
//.section .volatile_ram_code
exception_irq_handler:
	[--sp] = {reti, rete, retx, rets, psr, icfg, usp, ssp}
	[--sp] = {r15-r0}

	r0 = sp // 把当前 sp 的值作为参数传递给解析函数
    call debug_exception_analyze

1:
	goto 1b // 死循环

.size exception_irq_handler, . - exception_irq_handler





.global usb_upgrade_mode
.type usb_upgrade_mode, @function
.section .volatile_ram_code
usb_upgrade_mode:

    call debug_disable
    call hw_mmu_disable

    r0 = 0
	r1 = 0x28000
	r2 = 0x69e0
    r2 >>= 2
.L_ram_clr:
	rep r2 {
	[r1++] = r0
	}
    if(r2 > 0) goto .L_ram_clr

    ssp = 0x28c20
    usp = 0x29020
    sp = usp

    reti = 0x107716
    rti

.size usb_upgrade_mode, . - usb_upgrade_mode







