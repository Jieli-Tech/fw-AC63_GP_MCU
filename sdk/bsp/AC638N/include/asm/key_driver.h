#ifndef _KEY_DRIVER_H_
#define _KEY_DRIVER_H_

/*************模块使能*****************/
/**************************************/
#define  USE_KEY_DRIVER   1
/**************************************/

#if (USE_KEY_DRIVER == 1)

struct key_driver_para {
    u8  last_key;  	         //上一次get_value按键值
//== 用于消抖类参数
    u8 filter_value; 		 //用于按键消抖
    u8 filter_cnt;  		 //用于按键消抖时的累加值
    u8 filter_time;	         //当filter_cnt累加到base_cnt值时, 消抖有效
//== 用于判定长按和HOLD事件参数
    u8 long_time;  	        //按键判定长按数量
    u8 hold_time;  	        //按键判定HOLD数量
    u8 press_cnt;  	        //与long_time和hold_time对比, 判断long和hold状态

    u8(*get_value)(void);                                  //按键值获取
    int (*key_init)(void *key_para);                       //按键初始化
    void (* key_process)(u8 key_status, u8 key_value);     //按键处理
    u8 key_init_ok;                                        //按键初始化完成标志
};


extern struct key_driver_para iokey_para;

/*----------------------------------------------------------------------------*/
/**@brief   IO按键值状态初始化
   @param   para:用户参数
   @return  0:初始化成功
   @note
*/
/*----------------------------------------------------------------------------*/
int io_key_init(void *para);

/*----------------------------------------------------------------------------*/
/**@brief   IO按键值获取
   @param   void
   @return  按键值
   @note
*/
/*----------------------------------------------------------------------------*/
u8 io_get_key_value(void);

/*----------------------------------------------------------------------------*/
/**@brief   按键处理函数
   @param   key_status:  按键状态(短按、长按、连按、抬起等)
   @param   key_value:   按键值
   @return  viod
   @note    注意按键处理时间不宜过长
*/
/*----------------------------------------------------------------------------*/
void io_key_process(u8 key_status, u8 key_value);

/*----------------------------------------------------------------------------*/
/**@brief   按键扫描函数,判断是否有按键按下，按下的时间状态
   @param   _scan_para:按键参数
   @return  viod
   @note
*/
/*----------------------------------------------------------------------------*/
void  key_driver_scan(void *_scan_para);

/*----------------------------------------------------------------------------*/
/**@brief   按键初始化
   @param   key_para:按键参数
   @return  -1:初始化错误
            0:初始化成功
   @note
*/
/*----------------------------------------------------------------------------*/
int key_driver_init(struct key_driver_para *key_para);

/*----------------------------------------------------------------------------*/
/**@brief   按键测试demo
   @param   void
   @return  void
   @note    按键与消息机制测试
*/
/*----------------------------------------------------------------------------*/
void key_msg_test_demo();

#endif

#endif
