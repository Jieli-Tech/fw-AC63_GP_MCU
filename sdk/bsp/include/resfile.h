#ifndef _RESFILE_H_
#define _RESFILE_H_

#include "typedef.h"

/********* resfile 文件头 **********/


enum resfile_err_code {
    RESFILE_DIR_NOT_EXIST = -0xFF,
    RESFILE_FILE_NOT_EXIST,
    RESFILE_MALLOC_ERR,
    RESFILE_VM_NOT_FIND,
    RESFILE_DATA_CRC_ERR,
    RESFILE_WRITE_AREA_NEED_ERASE_ERR,
    RESFILE_SUSS = 0,
    RESFILE_END,
};




int resfile_init(void);

void *resfile_open(const char *path);

int resfile_read(void *fp, void *buf, u32 len);

int resfile_seek(void *fp, u32 offset, u32 fromwhere);

int resfile_get_pos(void *fp);

u32 resfile_get_addr(void *fp);

int resfile_get_len(void *fp);

int resfile_get_name(void *fp, u8 *name, int len);

int resfile_close(void *fp);


#endif

