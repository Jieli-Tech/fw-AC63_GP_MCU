#ifndef	_IIC_SOFT_H_
#define _IIC_SOFT_H_

#include "typedef.h"

typedef const int soft_iic_dev;

struct soft_iic_config {
    int scl;
    int sda;
    u32 delay;
    u8 io_pu;
};

extern const struct soft_iic_config soft_iic_cfg[];

/*
 * @brief 初始化软件iic
 * @parm iic  iic句柄
 * @return 无意义
 */
int soft_iic_init(soft_iic_dev iic);
/*
 * @brief 关闭软件iic
 * @parm iic  iic句柄
 * @return null
 */
void soft_iic_uninit(soft_iic_dev iic);
/*
 * @brief 挂起软件iic
 * @parm iic  iic句柄
 * @return null
 */
void soft_iic_suspend(soft_iic_dev iic);
/*
 * @brief 恢复软件iic
 * @parm iic  iic句柄
 * @return null
 */
void soft_iic_resume(soft_iic_dev iic);
/*
 * @brief 发送iic结束位
 * @parm iic  iic句柄
 * @return null
 */
void soft_iic_start(soft_iic_dev iic);
/*
 * @brief 发送iic结束位
 * @parm iic  iic句柄
 * @return null
 */
void soft_iic_stop(soft_iic_dev iic);
/*
 * @brief 发送1个字节
 * @parm iic  iic句柄
 * @parm byte  发送的字节
 * @return 1 收到应答，0 未收到应答
 */
u8 soft_iic_tx_byte(soft_iic_dev iic, u8 byte);
/*
 * @brief 接收1个字节
 * @parm iic  iic句柄
 * @parm ack  1 接收字节后回复应答，0不回复应答
 * @return 接收的字节
 */
u8 soft_iic_rx_byte(soft_iic_dev iic, u8 ack);
/*
 * @brief 接收len个字节
 * @parm iic  iic句柄
 * @parm buf  接收缓冲区基地址
 * @parm len  期望接收长度
 * @return 实际接收长度，< 0表示失败
 */
int soft_iic_read_buf(soft_iic_dev iic, void *buf, int len);
/*
 * @brief 发送len个字节
 * @parm iic  iic句柄
 * @parm buf  发送缓冲区基地址
 * @parm len  期望发送长度
 * @return 实际发送长度，< 0表示失败
 */
int soft_iic_write_buf(soft_iic_dev iic, const void *buf, int len);
#endif

