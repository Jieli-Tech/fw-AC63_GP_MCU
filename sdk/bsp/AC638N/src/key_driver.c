#include "asm/includes.h"
#include "msg.h"


#if (USE_KEY_DRIVER == 1)

/*按键类型*/
#define  KEY_DRIVER_TYPE_IO   0x0


/*按键状态*/
#define KEY_SHORT_UP        0x0   //短按抬起(短按)
#define KEY_LONG            0x1   //长按
#define KEY_HOLD            0x2   //连按(按住不放)
#define KEY_LONG_HLOD_UP    0x3   //长按或连按的抬起

/*无按键 键值*/
#define NO_KEY 		        0xff


/*----------------------------------------------------------------------------*/
/*------------------------------io_key_start----------------------------------*/
/*----------------------------------------------------------------------------*/
extern u16 io_key_table[2];

/*----------------------------------------------------------------------------*/
/**@brief   IO按键值状态初始化
   @param   para:用户参数
   @return  0:初始化成功
   @note
*/
/*----------------------------------------------------------------------------*/
int io_key_init(void *para)
{
    struct key_driver_para *scan_para = (struct key_driver_para *)para;

    gpio_set_direction(io_key_table[0], 1);
    gpio_set_direction(io_key_table[1], 1);

    gpio_set_pull_down(io_key_table[0], 0);
    gpio_set_pull_down(io_key_table[1], 0);

    gpio_set_pull_up(io_key_table[0], 1);
    gpio_set_pull_up(io_key_table[1], 1);

    gpio_set_die(io_key_table[0], 1);
    gpio_set_die(io_key_table[1], 1);

    scan_para->key_init_ok = 1;  //标志置位
    puts("___io_key_init_ok___\n");
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   IO按键值获取
   @param   void
   @return  按键值
   @note
*/
/*----------------------------------------------------------------------------*/
u8 io_get_key_value(void)
{
    //按键接地为按下
    if (!gpio_read(io_key_table[0])) {
        return 0;
    } else if (!gpio_read(io_key_table[1])) {
        return 1;
    }
    return NO_KEY;
}

//io 按键消息表
u16 io_key_msg_table[2][4] = {
    //短按                  //长按                 //连按(hold)           //长、连按抬起
    [0] = {
        MSG_TEST_IO_KEY1_SHORT, MSG_TEST_IO_KEY1_LONG, MSG_TEST_IO_KEY1_HOLD, MSG_TEST_IO_KEY1_LONG_HOLD_UP
    },

    [1] = {
        MSG_TEST_IO_KEY2_SHORT, MSG_TEST_IO_KEY2_LONG, MSG_TEST_IO_KEY2_HOLD, MSG_TEST_IO_KEY2_LONG_HOLD_UP
    },
};

/*----------------------------------------------------------------------------*/
/**@brief   按键处理函数
   @param   key_status:  按键状态(短按、长按、连按、抬起等)
   @param   key_value:   按键值
   @return  viod
   @note    注意按键处理时间不宜过长
*/
/*----------------------------------------------------------------------------*/
void io_key_process(u8 key_status, u8 key_value)
{
    //按键处理，中断时间不宜过久，这里仅仅是作按键的区分
    //建议依靠消息机制把按键信息发送到主循环处理按键
    u16 key_msg;
    u8 err;
    key_msg = io_key_msg_table[key_value][key_status];
    err = msg_put_fifo(key_msg);
    if (err != 0) {
        puts("can not put msg\n");
    }
}

//注意扫描频率，这里供的时间默认为10ms扫描一次
//则对应的消抖时间为 4*10 =40ms,其他以此类推
struct key_driver_para iokey_para = {
    .last_key 		  = NO_KEY,  		    //上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 4,				    //按键消抖延时;
    .long_time 		  = 75,  			    //按键判定长按数量
    .hold_time 		  = (75 + 15),  	    //按键判定HOLD数量
    .get_value 		  = io_get_key_value,   //按键值获取
    .key_init         = io_key_init,        //按键状态初始化
    .key_process      = io_key_process,     //按键处理
    .key_init_ok      = 0,                  //按键初始化完成标志
};

/*----------------------------------------------------------------------------*/
/*------------------------------io_key_end------------------------------------*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/**@brief   按键扫描函数,判断是否有按键按下，按下的时间状态
   @param   _scan_para:按键参数
   @return  viod
   @note
*/
/*----------------------------------------------------------------------------*/
void  key_driver_scan(void *_scan_para)
{
    struct key_driver_para *scan_para = (struct key_driver_para *)_scan_para;

    u8 key_status = 0;
    u8 cur_key_value = NO_KEY;
    u8 key_value = NO_KEY;

    if (!scan_para->key_init_ok) {
        return;
    }

//===== 按键值获取
    cur_key_value = scan_para->get_value();
    /* if (cur_key_value != NO_KEY) { */
    /*     printf(">>>cur_key_value: %d\n", cur_key_value); */
    /* } */

//===== 按键消抖处理
    if (cur_key_value != scan_para->filter_value && scan_para->filter_time) {	//当前按键值与上一次按键值如果不相等, 重新消抖处理, 注意filter_time != 0;
        scan_para->filter_cnt = 0; 		            //消抖次数清0, 重新开始消抖
        scan_para->filter_value = cur_key_value;	//记录上一次的按键值
        return; 		                            //第一次检测, 返回不做处理
    }

    if (scan_para->filter_cnt < scan_para->filter_time) {//当前按键值与上一次按键值相等, filter_cnt开始累加;
        scan_para->filter_cnt++;
        return;
    }

//===== 按键类型判断
    if (cur_key_value == scan_para->last_key) {
        if (cur_key_value == NO_KEY) {                                     //没有按键按下
            return;
        }
        scan_para->press_cnt++;
        if (scan_para->press_cnt == scan_para->long_time) {                //长按
            key_status = KEY_LONG;
        } else if (scan_para->press_cnt == scan_para->hold_time) {         //连按(一直按着不放)
            key_status = KEY_HOLD;
            scan_para->press_cnt = scan_para->long_time;
        } else {
            return;//计数不做操作
        }
        key_value = cur_key_value;
    } else {
        if (cur_key_value == NO_KEY) {                                   //按键被抬起
            if (scan_para->press_cnt < scan_para->long_time) {           //短按
                key_status = KEY_SHORT_UP;
                key_value = scan_para->last_key;
            } else if (scan_para->press_cnt >= scan_para->long_time) {   //长按/HOLD状态之后按键抬起;
                key_status = KEY_LONG_HLOD_UP;
                key_value = scan_para->last_key;
            }
            scan_para->last_key = cur_key_value;
            scan_para->press_cnt = 0;
        } else {
            scan_para->last_key = cur_key_value;
            scan_para->press_cnt = 0;
            return;
        }
    }

//====== 按键处理
    //按键处理，中断时间不宜过久，这里仅仅是作按键的区分
    //建议依靠消息机制把按键信息发送到主循环处理按键
    scan_para->key_process(key_status, key_value);
}


/*----------------------------------------------------------------------------*/
/**@brief   按键初始化
   @param   key_para:按键参数
   @return  -1:初始化错误
            0:初始化成功
   @note
*/
/*----------------------------------------------------------------------------*/
int key_driver_init(struct key_driver_para *key_para)
{
    //初始化
    if (key_para->key_init) {
        return key_para->key_init(key_para);
    }

    return -1;
}

/*----------------------------------------------------------------------------*/
/**@brief   按键测试demo
   @param   void
   @return  void
   @note    按键与消息机制测试
*/
/*----------------------------------------------------------------------------*/
void key_msg_test_demo()
{
    /*******demo实现前提**********/
    //消息机制初始化: msg_init
    //按键初始化：    key_driver_init
    //定时器10ms扫描：key_driver_scan

    u16 msg;
    while (1) {
        msg = get_msg();
        if (msg == NO_MSG) {
            continue;
        }

        switch (msg) {
        case MSG_TEST_IO_KEY1_SHORT:
            puts("IO_KEY1_SHOURT\n");
            break;
        case MSG_TEST_IO_KEY1_LONG:
            puts("IO_KEY1___LONG\n");
            break;
        case MSG_TEST_IO_KEY1_HOLD:
            puts("IO_KEY1_HOLD\n");
            break;
        case MSG_TEST_IO_KEY1_LONG_HOLD_UP:
            puts("IO_KEY1_LONG_HOLD_UP\n");
            break;


        case MSG_TEST_IO_KEY2_SHORT:
            puts("IO_KEY2_SHOURT\n");
            break;
        case MSG_TEST_IO_KEY2_LONG:
            puts("IO_KEY2___LONG\n");
            break;
        case MSG_TEST_IO_KEY2_HOLD:
            puts("IO_KEY2_HOLD\n");
            break;
        case MSG_TEST_IO_KEY2_LONG_HOLD_UP:
            puts("IO_KEY2_LONG_HOLD_UP\n");
            break;

        default:
            printf("default msg:%x\n", msg);
            break;
        }
    }
}

#endif
