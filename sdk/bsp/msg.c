#include "asm/includes.h"
#include "msg.h"


//消息池大小
#define   MAX_POOL   10

//临界区管理
#define   MSG_ENTER_CRITICAL()    local_irq_disable()
#define   MSG_EXIT_CRITICAL()     local_irq_enable()

u16 msg_pool[MAX_POOL];             //消息池
u16 msg_read = 0;                   //消息读位置
u16 msg_write = 0;                  //消息写位置
u16 msg_pool_residue = MAX_POOL;    //消息池剩余大小



/*----------------------------------------------------------------------------*/
/**@brief   先进先出的方式将消息放入消息池
   @param   void
   @return  0:消息放入消息池成功
            -1：消息放放入消息池不成功
   @note
*/
/*----------------------------------------------------------------------------*/
u8 msg_put_fifo(u16 msg)
{
    MSG_ENTER_CRITICAL();
    if (msg_pool_residue == 0) {
        MSG_EXIT_CRITICAL();
        puts("err:msg pool is full!\n");
        return -1;
    }
    msg_pool[msg_write] = msg;
    msg_write++;
    if (msg_write == MAX_POOL) {
        msg_write = 0;
    }
    msg_pool_residue--;
    MSG_EXIT_CRITICAL();
    return 0;
}


/*----------------------------------------------------------------------------*/
/**@brief   后进先出的方式将消息放入消息池
   @param   void
   @return  0:消息放入消息池成功
            -1：消息放放入消息池不成功
   @note
*/
/*----------------------------------------------------------------------------*/
u8 msg_put_lifo(u16 msg)
{
    MSG_ENTER_CRITICAL();
    if (msg_pool_residue < 2) {
        MSG_EXIT_CRITICAL();
        puts("err:msg pool less than two\n");
        return -1;
    }
    msg_read--;
    if (msg_read == 0xffff) {
        msg_read = MAX_POOL - 1;
    }
    msg_pool[msg_read] = msg;
    msg_pool_residue--;
    MSG_EXIT_CRITICAL();
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   获取一个消息
   @param   void
   @return  消息值
   @note
*/
/*----------------------------------------------------------------------------*/
u16 get_msg(void)
{
    u16 msg = NO_MSG;

    MSG_ENTER_CRITICAL();
    if (msg_pool_residue < MAX_POOL) {
        msg = msg_pool[msg_read];
        msg_read++;
        if (msg_read == MAX_POOL) {
            msg_read = 0;
        }
        msg_pool_residue++;
    }
    MSG_EXIT_CRITICAL();
    return msg;
}


/*----------------------------------------------------------------------------*/
/**@brief   清空消息池
   @param   void
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void flush_all_msg(void)
{
    MSG_ENTER_CRITICAL();
    msg_read = 0;
    msg_write = 0;
    msg_pool_residue = MAX_POOL;
    memset(msg_pool, 0xff, sizeof(msg_pool));
    MSG_EXIT_CRITICAL();
}

/*----------------------------------------------------------------------------*/
/**@brief   消息机制初始化
   @param   void
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void msg_init(void)
{
    if (MAX_POOL < 3) {
        while (1) {
            puts("err:max pool less than 3\n");
        }
    }
    flush_all_msg();
}

