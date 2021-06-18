#ifndef  __EFUSE_H__
#define  __EFUSE_H__

u16 get_vbat_trim();
u16 get_vbg_trim();
u32 get_chip_version();
void efuse_init(void);

u32 get_lrc_pll_ds();
u8 get_sysdvdd_trim();
#endif  /*EFUSE_H*/
