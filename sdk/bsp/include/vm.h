#ifndef _VM_H_
#define _VM_H_

#include "ioctl.h"
#include "typedef.h"

#define IOCTL_SET_VM_INFO               _IOW('V', 1, 1)
#define IOCTL_GET_VM_INFO               _IOW('V', 2, 1)

typedef u16 vm_hdl;

typedef enum _vm_err {
    VM_ERR_NONE = 0,
    VM_INDEX_ERR = -0x100,
    VM_INDEX_EXIST,     //0xFF
    VM_DATA_LEN_ERR,    //0xFE
    VM_READ_NO_INDEX,   //0xFD
    VM_READ_DATA_ERR,   //0xFC
    VM_WRITE_OVERFLOW,  //0xFB
    VM_NOT_INIT,
    VM_INIT_ALREADY,
    VM_DEFRAG_ERR,
    VM_ERR_INIT,
    VM_ERR_PROTECT
} VM_ERR;


void vm_check_all(u8 level);    //level : default 0
VM_ERR vm_init(void *dev_hdl, u32 vm_addr, u32 vm_len, u8 vm_mode);

//sdk 已默认初始化VM请不要修改,使用顺序： vm_write--->vm_read(直接读未写过的id会返回空id的错误)
//VM的id请统一在vm_id.h里定义，避免混乱
//使用注意，不要在中断里使用vm_write 接口，也不要频繁使用vm_write接口
//vm_write有关中断操作

/*----------------------------------------------------------------------------*/
/**@brief   vm数据全部清除
   @param   void
   @return  VM_ERR 错误值
   @note
*/
/*----------------------------------------------------------------------------*/
VM_ERR vm_eraser(void);

/*----------------------------------------------------------------------------*/
/**@brief   vm读函数，读取对应id的存储值
   @param   hdl      : id号
            data_buf : 读取数据存放buf
            len:     :读取长度，要和写进去的长度一致
   @return  读取的长度值：与len一致表示读取成功
            其他错误返回值请看VM_ERR
   @note
*/
/*----------------------------------------------------------------------------*/
s32 vm_read(vm_hdl hdl, u8 *data_buf, u16 len);


/*----------------------------------------------------------------------------*/
/**@brief   vm写函数，读取对应id的存储值
   @param   hdl      : id号
            data_buf : 要写入数据的存放buf
            len:     :写长度
   @return  写入的长度值：与len一致表示写入成功
            其他错误返回值请看VM_ERR
   @note
*/
/*----------------------------------------------------------------------------*/
s32 vm_write(vm_hdl hdl, u8 *data_buf, u16 len);

#endif  //_VM_H_

