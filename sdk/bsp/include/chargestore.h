#ifndef __CHARGESTORE_H__
#define __CHARGESTORE_H__

#include "typedef.h"

struct chargestore_platform_data {
    u32 baudrate;//波特率
    u32 io_port; //对应IO
    void (*init)(u32 baudrate, u32 io_port);//链路层初始化接口
    void (*open)(u8 mode);//链路层打开接口
    void (*close)(void);//链路层关闭接口
    void (*write)(u8 *, u8);//链路层写数据接口
    void (*set_callback)(void (*callback)(u8 cmd, u8 *data, u8 len));//链路层设置数据回调函数接口
    void (*set_update)(void);//设置进入升级接口
};

/*----------------------------------------------------*/
/**@brief    关闭数据接口
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_api_close(void);

/*----------------------------------------------------*/
/**@brief    发送数据
   @param    无
   @return   无
   @note     只有收到数据才能发送数据,是被动的
*/
/*----------------------------------------------------*/
int chargestore_api_write(u8 *buf, u8 len);

/*----------------------------------------------------*/
/**@brief    初始化升级协议层
   @param    无
   @return   无
   @note     只有收到数据才能发送数据,是被动的
*/
/*----------------------------------------------------*/
void chargestore_api_init(const struct chargestore_platform_data *arg);

/*----------------------------------------------------*/
/**@brief    等待数据发送完成
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_api_wait_complete(void);

/*----------------------------------------------------*/
/**@brief    设置数据接收回调函数
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_api_set_data_callback(void (*callback)(u8 *data, u8 len));

#endif
