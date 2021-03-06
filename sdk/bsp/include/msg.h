#ifndef __MSG_H__
#define __MSG_H__

//定义空消息
#define   NO_MSG       0xffff


//消息表
enum {

    MSG_TEST_IO_KEY1_SHORT,
    MSG_TEST_IO_KEY1_LONG,
    MSG_TEST_IO_KEY1_HOLD,
    MSG_TEST_IO_KEY1_LONG_HOLD_UP,

    MSG_TEST_IO_KEY2_SHORT,
    MSG_TEST_IO_KEY2_LONG,
    MSG_TEST_IO_KEY2_HOLD,
    MSG_TEST_IO_KEY2_LONG_HOLD_UP,


    MSG_MAX = NO_MSG,
};


/*----------------------------------------------------------------------------*/
/**@brief   先进先出的方式将消息放入消息池
   @param   void
   @return  0:消息放入消息池成功
            -1：消息放放入消息池不成功
   @note
*/
/*----------------------------------------------------------------------------*/
u8 msg_put_fifo(u16 msg);

/*----------------------------------------------------------------------------*/
/**@brief   后进先出的方式将消息放入消息池
   @param   void
   @return  0:消息放入消息池成功
            -1：消息放放入消息池不成功
   @note
*/
/*----------------------------------------------------------------------------*/
u8 msg_put_lifo(u16 msg);

/*----------------------------------------------------------------------------*/
/**@brief   获取一个消息
   @param   void
   @return  消息值
   @note
*/
/*----------------------------------------------------------------------------*/
u16 get_msg(void);

/*----------------------------------------------------------------------------*/
/**@brief   清空消息池
   @param   void
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void flush_all_msg(void);

/*----------------------------------------------------------------------------*/
/**@brief   消息机制初始化
   @param   void
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void msg_init(void);


#endif


