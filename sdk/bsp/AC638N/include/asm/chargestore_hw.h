#ifndef __CHARGESTORE_HW_H__
#define __CHARGESTORE_HW_H__

enum {
    CMD_COMPLETE,
    CMD_RECVDATA,
};

enum {
    MODE_RECVDATA,
    MODE_SENDDATA,
};

/*----------------------------------------------------*/
/**@brief    链路层打开数据通路
   @param    0:接收模式 1:发送模式
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_open(u8 mode);
/*----------------------------------------------------*/
/**@brief    链路层关闭数据通路
   @param    无
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_close(void);
/*----------------------------------------------------*/
/**@brief    链路层写数据
   @param    无
   @return   无
   @note     用于回复命令
*/
/*----------------------------------------------------*/
void chargestore_write(u8 *data, u8 len);
/*----------------------------------------------------*/
/**@brief    链路层初始化
   @param    baudrate:波特率 io_port:升级IO的编号
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_init(u32 baudrate, u32 io_port);
/*----------------------------------------------------*/
/**@brief    设置升级RAM标记
   @param    无
   @return   无
   @note     设置标记后复位系统进入串口升级
*/
/*----------------------------------------------------*/
void chargestore_set_update_ram(void);
/*----------------------------------------------------*/
/**@brief    链路层设置通信波特率
   @param    baudrate:传入波特率值
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_set_baudrate(u32 baudrate);
/*----------------------------------------------------*/
/**@brief    链路层设置接收到数据处理回调函数
   @param    callback:回调函数
   @return   无
   @note     无
*/
/*----------------------------------------------------*/
void chargestore_set_data_callback(void (*callback)(u8 cmd, u8 *data, u8 len));

#endif
