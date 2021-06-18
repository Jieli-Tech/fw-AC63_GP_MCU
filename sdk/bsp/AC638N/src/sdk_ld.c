// *INDENT-OFF*

/* #include "app_config.h" */

/* =================  BR34 SDK memory ========================
 _______________ ___ 0x2C000(176K)
|   isr base    |
|_______________|___ _IRQ_MEM_ADDR(size = 0x100)
|rom export ram |
|_______________|
|    update     |
|_______________|___ RAM_LIMIT_H
|     HEAP      |
|_______________|___ data_code_pc_limit_H
| audio overlay |
|_______________|
|   data_code   |
|_______________|___ data_code_pc_limit_L
|     bss       |
|_______________|
|     data      |
|_______________|
|   irq_stack   |
|_______________|
|   boot info   |
|_______________|
|     TLB       |
|_______________|0 RAM_LIMIT_L

 =========================================================== */

#include "maskrom_stubs.ld"

/* EXTERN( */
/* _start */
/* #include "sdk_used_list.c" */
/* ); */

//============ About RAM ================

UPDATA_SIZE     = 0x80;
UPDATA_BEG      = _MASK_MEM_BEGIN - UPDATA_SIZE;
UPDATA_BREDR_BASE_BEG = 0x2c000;

RAM_LIMIT_L     = 0;
RAM_LIMIT_H     = UPDATA_BEG;
PHY_RAM_SIZE    = RAM_LIMIT_H - RAM_LIMIT_L;

//from mask export
ISR_BASE       = _IRQ_MEM_ADDR;
ROM_RAM_SIZE   = _MASK_MEM_SIZE;
ROM_RAM_BEG    = _MASK_MEM_BEGIN;

RAM0_BEG 		= RAM_LIMIT_L;
RAM0_END 		= RAM_LIMIT_H;
RAM0_SIZE 		= RAM0_END - RAM0_BEG;


//============ About CODE ================
CODE_BEG = 0x1E00100;

MEMORY
{
	code0(rx)       : ORIGIN = CODE_BEG,        LENGTH = 256 * 1024
	ram0(rwx)       : ORIGIN = RAM0_BEG,        LENGTH = RAM0_SIZE
}


ENTRY(_start)

SECTIONS
{
    . = ORIGIN(code0);
    .text ALIGN(4):
    {
        PROVIDE(text_rodata_begin = .);

        *(.entry_text)
        *(.startup.text)

		*(.text)

        . = ALIGN(4);

		*(.LOG_TAG_CONST*)
        *(.rodata*)

		. = ALIGN(4); // must at tail, make rom_code size align 4
        PROVIDE(text_rodata_end = .);

        clock_critical_handler_begin = .;
        KEEP(*(.clock_critical_txt))
        clock_critical_handler_end = .;

        . = ALIGN(4);
        __VERSION_BEGIN = .;
        KEEP(*(.sys.version))
        __VERSION_END = .;

        *(.noop_version)

		. = ALIGN(4);

        deepsleep_target_begin = .;
        KEEP(*(.deepsleep_target))
        deepsleep_target_end = .;

		. = ALIGN(4);
        timer_target_begin = .;
        KEEP(*(.timer_target))
        timer_target_end = .;

        CLOCK_CODE_START = .;
        *(.clock_code)
        *(.clock_const)
        CLOCK_CODE_SIZE = ABSOLUTE(. - CLOCK_CODE_START);

        *(.power_code)
        *(.power_const)

		/********maskrom arithmetic ****/
        *(.opcore_table_maskrom)
        *(.bfilt_table_maskroom)
        *(.opcore_maskrom)
        *(.bfilt_code)
        *(.bfilt_const)
		/********maskrom arithmetic end****/
		. = ALIGN(4);

	  } > code0

    . = ORIGIN(ram0);
	//TLB 起始需要16K 对齐；
    .mmu_tlb ALIGN(0x4000):
    {
        *(.mmu_tlb_segment);
    } > ram0

	.boot_info ALIGN(32):
	{
		*(.boot_info)
        . = ALIGN(32);
	} > ram0

	.irq_stack ALIGN(32):
    {
		*(.stack_magic)
        _cpu0_sstack_begin = .;
        PROVIDE(cpu0_sstack_begin = .);
        *(.stack)
        _cpu0_sstack_end = .;
        PROVIDE(cpu0_sstack_end = .);
    	_stack_end = . ;
		*(.stack_magic0)
		. = ALIGN(4);

    } > ram0

	.data :
	{
		//cpu start
        . = ALIGN(4);
        *(.data_magic)

        *(.data*)
        *(.power_data)

		. = ALIGN(4);

	} > ram0

	.data_code :
	{
        . = ALIGN(4);
        data_code_pc_limit_begin = .;
		*(.bank_critical_code)
        . = ALIGN(4);

		*(.flushinv_icache)
        *(.cache)
        *(.volatile_ram_code)

        *(.os_critical_code)
		*(.os_code)
		*(.os_str)

        _SPI_CODE_START = . ;
        *(.spi_code)
        _SPI_CODE_END = . ;
		. = ALIGN(4);

		. = ALIGN(4);
		lp_target_begin = .;
		*(.lp_target)
		lp_target_end = .;
		. = ALIGN(4);

        data_code_pc_limit_end = .;

    } > ram0

	.bss ALIGN(32):
    {
        *(.bss)

        . = ALIGN(4);

        *(.clock_bss)
        *(.power_bss)

        *(.os_bss)
        *(.volatile_ram)
		*(.btstack_pool)

        *(.src_filt)
        *(.src_dma)
        *(.mem_heap)
		*(.memp_memory_x)

        . = ALIGN(4);
		*(.non_volatile_ram)

        . = ALIGN(32);

    } > ram0


    .common ALIGN(32):
    {
        //common bank code addr
        *(.common*)


       . = ALIGN(4);
    } > ram0


	_HEAP_BEGIN = . ;
	_HEAP_END = RAM0_END;
}

//================== Section Info Export ====================//
text_begin  = ADDR(.text);
text_size   = SIZEOF(.text);
text_end    = text_begin + text_size;

bss_begin = ADDR(.bss);
bss_size  = SIZEOF(.bss);
bss_end    = bss_begin + bss_size;

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size =  SIZEOF(.data);

/* moveable_slot_addr = ADDR(.moveable_slot); */
/* moveable_slot_begin = data_begin + data_size; */
/* moveable_slot_size =  SIZEOF(.moveable_slot); */

data_code_addr = ADDR(.data_code);
data_code_begin = data_begin + data_size;
data_code_size =  SIZEOF(.data_code);

//================ BANK ==================//
bank_code_load_addr = data_code_begin + data_code_size;
/* bank_code_load_addr = aac_begin + aac_size; */

/* moveable_addr = ADDR(.overlay_moveable) ; */
/* moveable_size = SIZEOF(.overlay_moveable) ; */
//===================== HEAP Info Export =====================//
PROVIDE(HEAP_BEGIN = _HEAP_BEGIN);
PROVIDE(HEAP_END = _HEAP_END);
_MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);

/* ASSERT(MALLOC_SIZE  >= 0x8000, "heap space too small !") */

//============================================================//
//=== report section info begin:
//============================================================//
report_text_beign = ADDR(.text);
report_text_size = SIZEOF(.text);
report_text_end = report_text_beign + report_text_size;

report_mmu_tlb_begin = ADDR(.mmu_tlb);
report_mmu_tlb_size = SIZEOF(.mmu_tlb);
report_mmu_tlb_end = report_mmu_tlb_begin + report_mmu_tlb_size;

report_boot_info_begin = ADDR(.boot_info);
report_boot_info_size = SIZEOF(.boot_info);
report_boot_info_end = report_boot_info_begin + report_boot_info_size;

report_irq_stack_begin = ADDR(.irq_stack);
report_irq_stack_size = SIZEOF(.irq_stack);
report_irq_stack_end = report_irq_stack_begin + report_irq_stack_size;

report_data_begin = ADDR(.data);
report_data_size = SIZEOF(.data);
report_data_end = report_data_begin + report_data_size;

report_bss_begin = ADDR(.bss);
report_bss_size = SIZEOF(.bss);
report_bss_end = report_bss_begin + report_bss_size;

report_data_code_begin = ADDR(.data_code);
report_data_code_size = SIZEOF(.data_code);
report_data_code_end = report_data_code_begin + report_data_code_size;

report_heap_beign = _HEAP_BEGIN;
report_heap_size = _HEAP_END - _HEAP_BEGIN;
report_heap_end = _HEAP_END;

BR34_PHY_RAM_SIZE = PHY_RAM_SIZE;
BR34_SDK_RAM_SIZE = report_mmu_tlb_size + \
					report_boot_info_size + \
					report_irq_stack_size + \
					report_data_size + \
					report_bss_size + \
					report_data_code_size + \
					report_heap_size;
//============================================================//
//=== report section info end
//============================================================//

