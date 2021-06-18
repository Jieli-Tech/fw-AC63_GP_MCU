#ifndef __RTC_H__
#define __RTC_H__

#include "typedef.h"

struct sys_time {
    u16 crc;
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
};

//	RTC初始化参数配置结构体
struct rtc_data {
    struct sys_time *default_sys_time;			//初始默认时间
    struct sys_time *default_alarm;				//初始默认闹钟时间
    void (*cbfun)(u8);							//闹钟唤醒回调函数,若不用可填NULL
};


/*----------------------------------------------------------------------------*/
/**@brief	初始化rtc
   @param 	arg:rtc参数结构体
   @return  0
   @note	int rtc_init(struct rtc_data *arg)
*/
/*----------------------------------------------------------------------------*/
int rtc_init(struct rtc_data *arg);

/*----------------------------------------------------------------------------*/
/**@brief	开关闹钟
   @param 	set_alarm: 0->关	1->开
   @return  NULL
   @note	void rtc_set_alarm_ctrl(u8 set_alarm)
*/
/*----------------------------------------------------------------------------*/
void rtc_set_alarm_ctrl(u8 set_alarm);

/*----------------------------------------------------------------------------*/
/**@brief	写入rtc时间
   @param 	curr_time:写入的目标时间
   @return  NULL
   @note	void rtc_write_time(struct sys_time *curr_time)
*/
/*----------------------------------------------------------------------------*/
void rtc_write_time(struct sys_time *curr_time);

/*----------------------------------------------------------------------------*/
/**@brief	读出rtc时间
   @param 	curr_time:读出当前时间
   @return  NULL
   @note	void rtc_read_time(struct sys_time *curr_time)
*/
/*----------------------------------------------------------------------------*/
void rtc_read_time(struct sys_time *curr_time);

/*----------------------------------------------------------------------------*/
/**@brief	写入闹钟时间
   @param 	alarm_time:写入的目标闹钟时间
   @return  NULL
   @note	void rtc_write_alarm(struct sys_time *alarm_time)
*/
/*----------------------------------------------------------------------------*/
void rtc_write_alarm(struct sys_time *alarm_time);

/*----------------------------------------------------------------------------*/
/**@brief	读取闹钟时间
   @param 	alarm_time:读出的闹钟时间
   @return  NULL
   @note	void rtc_read_alarm(struct sys_time *alarm_time)
*/
/*----------------------------------------------------------------------------*/
void rtc_read_alarm(struct sys_time *alarm_time);


/*----------------------------------------------------------------------------*/
/**@brief	当前日期换成星期几
   @param 	time:当前日期
   @return  星期几
   @note	u8 rtc_caculate_weekday_by_time(struct sys_time *r_time)
*/
/*----------------------------------------------------------------------------*/
u8 rtc_caculate_weekday_by_time(struct sys_time *r_time);

/*----------------------------------------------------------------------------*/
/**@brief	打印当前时间
   @param 	NULL
   @return  NULL
   @note	void rtc_time_dump(void)
*/
/*----------------------------------------------------------------------------*/
void rtc_time_dump(void);

#endif // __RTC_API_H__
