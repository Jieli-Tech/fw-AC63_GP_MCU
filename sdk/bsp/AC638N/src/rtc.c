#include "asm/includes.h"
#include "asm/rtc.h"
#include "asm/cpu.h"
#include "asm/timer.h"
#include "asm/power/p33.h"
#include "ioctl_cmds.h"
#if 0
static struct rtc_data *__this = NULL;

#define YEAR        2000U
#define MONTH       1
#define DAY         1

#define WRITE_ALARM     0
#define READ_ALARM      1

#define WRITE_RTC       2
#define READ_RTC        3

#define OS_ENTER_CRITICAL()  \
		CPU_CRITICAL_ENTER(); \

#define OS_EXIT_CRITICAL()  \
		CPU_CRITICAL_EXIT()



/*******************************************************
				CONST
********************************************************/
const u8 month_tab1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};		 ///<非闰年每月的天数
const u8 month_tab2[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};		 ///<闰年每月的天数

/*----------------------------------------------------------------------------*/
/**@brief	判断闰年
   @param 	year:年份
   @return  真假
   @note	bool leapyear(u16 year)
*/
/*----------------------------------------------------------------------------*/
static bool leapyear(u16 year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    } else {
        return false;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief	年份换算为天数
   @param 	year:年份
   @return  当年天数
   @note	u16 year_to_day(u16 year)
*/
/*----------------------------------------------------------------------------*/
static u16 year_to_day(u16 year)
{
    if (leapyear(year)) {
        return 366;
    } else {
        return 365;
    }
}

static u16 month_to_day(u16 year, u8 month)
{
    if (leapyear(year)) {
        return month_tab2[month];
    } else {
        return month_tab1[month];
    }
}

static void day_to_ymd(u16 day, struct sys_time *sys_time)
{
    u8 tmp = 0;
    while (day >= year_to_day(YEAR + tmp)) {
        day -= year_to_day(YEAR + tmp);
        tmp ++;
    }
    sys_time->year = YEAR + tmp;
    tmp = 0;
    while (day >= month_to_day(sys_time->year, MONTH + tmp)) {
        day -= month_to_day(sys_time->year, MONTH + tmp);
        tmp ++;
    }
    sys_time->month = MONTH + tmp;
    sys_time->day = DAY + day;
}

static u16 ymd_to_day(struct sys_time *time)
{
    u16 tmp;
    u16 tmp_day = 0;
    //年->日，当年不算在内，所以是<
    for (tmp = YEAR; tmp < time->year; tmp ++) {
        tmp_day += year_to_day(tmp);
    }
    for (tmp = MONTH; tmp < time->month; tmp ++) {
        tmp_day += month_to_day(time->year, tmp);
    }
    tmp_day += (time->day - 1);						//日->日,当日不算在内，所以日应该减1
    return tmp_day;
}

u8 caculate_weekday_by_time(struct sys_time *r_time)
{
    u16 y = r_time->year;
    u8 m = r_time->month;
    u8 d = r_time->day;
    if (m == 1 || m == 2) {
        m += 12;
        y--;
    }
    return ((d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7 + 1); //1~7  星期1 ~星期7
}

/*----------------------------------------------------------------------------*/
/**@brief 	读IRTC
   @param 	cmd：读指令 r_time：读出的日期
   @return  void
   @note  	void read_IRTC(u8 cmd, sstruct sys_time *r_time)
*/
/*----------------------------------------------------------------------------*/
static void read_IRTC(u8 cmd, struct sys_time *r_time)
{
    if (cmd == READ_RTC) {
        u16 r_day;
        /*read rtc need close irq*/
        OS_ENTER_CRITICAL();

        R3_RTC_CON0 |= BIT(4);
        while (!(R3_RTC_CON0 & BIT(5)));

        r_day = R3_RTC_DAT0;
        r_day <<= 8;
        r_day = R3_RTC_DAT1 | r_day;

        day_to_ymd(r_day, r_time);

        r_time->hour = R3_RTC_DAT2;
        r_time->min  = R3_RTC_DAT3;
        r_time->sec  = R3_RTC_DAT4;

        OS_EXIT_CRITICAL();

    } else if (cmd == READ_ALARM) {
        u16 r_day;
        r_day = R3_ALM_DAT0;
        r_day <<= 8;
        r_day = R3_ALM_DAT1 | r_day;

        day_to_ymd(r_day, r_time);

        r_time->hour = R3_ALM_DAT2;
        r_time->min  = R3_ALM_DAT3;
        r_time->sec  = R3_ALM_DAT4;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief 	写IRTC
   @param 	cmd：写指令 w_time:写入日期
   @return  void
   @note  	void write_IRTC(u8 cmd, sstruct sys_time *w_time)
*/
/*----------------------------------------------------------------------------*/
static void write_IRTC(u8 cmd, struct sys_time *w_time)
{
    if (cmd == WRITE_RTC) {
        u16 w_day;
        w_day = ymd_to_day(w_time);

        OS_ENTER_CRITICAL();

        R3_RTC_DAT0 = w_day >> 8;
        R3_RTC_DAT1 = w_day & 0xff;
        R3_RTC_DAT2 = w_time->hour;
        R3_RTC_DAT3 = w_time->min;
        R3_RTC_DAT4 = w_time->sec;

        OS_EXIT_CRITICAL();

    } else if (cmd == WRITE_ALARM) {
        u16 w_day;
        w_day = ymd_to_day(w_time);
        R3_ALM_DAT0 = w_day >> 8;
        R3_ALM_DAT1 = w_day & 0xff;
        R3_ALM_DAT2 = w_time->hour;
        R3_ALM_DAT3 = w_time->min;
        R3_ALM_DAT4 = w_time->sec;
    }
}

void rtc_set_alarm_ctrl(u8 set_alarm)
{
    if (set_alarm) {
        R3_ALM_CON |= BIT(0);
    } else {
        R3_ALM_CON &= ~BIT(0);
    }
}

void rtc_write_time(struct sys_time *curr_time)
{
    rtc_set_alarm_ctrl(0);
    write_IRTC(WRITE_RTC, curr_time);
    rtc_set_alarm_ctrl(1);
}

void rtc_read_time(struct sys_time *curr_time)
{
    read_IRTC(READ_RTC, curr_time);
}

void rtc_write_alarm(struct sys_time *alarm_time)
{
    rtc_set_alarm_ctrl(0);
    write_IRTC(WRITE_ALARM, alarm_time);
    rtc_set_alarm_ctrl(1);
}
void rtc_read_alarm(struct sys_time *alarm_time)
{
    read_IRTC(READ_ALARM, alarm_time);
}


void __attribute__((weak)) alm_wakeup_isr(void)
{
    if (__this->cbfun) {
        __this->cbfun(0);
    }
}

static void rtc_wakup_source()
{
    u8 sfr = R3_WKUP_SRC;
    /* printf("rtc_wkup_src is 0x%x\n", sfr); */
    if (sfr & BIT(R3_WKUP_SRC_ALM)) {
        rtc_set_alarm_ctrl(0);
        alm_wakeup_isr();
    }

    if (sfr & BIT(R3_WKUP_SRC_256HZ)) {
        TIME_256HZ_CPND(1);
    }

    if (sfr & BIT(R3_WKUP_SRC_64HZ)) {
        TIME_64HZ_CPND(1);
    }

    if (sfr & BIT(R3_WKUP_SRC_2HZ)) {
        TIME_2HZ_CPND(1);
    }

    if (sfr & BIT(R3_WKUP_SRC_1HZ)) {
        TIME_1HZ_CPND(1);
    }
}

void rtc_time_dump(void)
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));
    rtc_read_time(&tmp_time);
    printf("rtc_rtc_read_time: %d-%d-%d %d:%d:%d\n",
           tmp_time.year,
           tmp_time.month,
           tmp_time.day,
           tmp_time.hour,
           tmp_time.min,
           tmp_time.sec);
}

extern u8 power_reset_flag;
int rtc_init(struct rtc_data *arg)
{
    __this = arg;
    //32K_OSCO
    gpio_set_direction(IO_PORTA_03, 1);
    gpio_set_die(IO_PORTA_03, 0);
    gpio_set_dieh(IO_PORTA_03, 0);
    gpio_set_pull_up(IO_PORTA_03, 0);
    gpio_set_pull_down(IO_PORTA_03, 0);
    //32K_OSCI
    gpio_set_direction(IO_PORTA_04, 1);
    gpio_set_die(IO_PORTA_04, 0);
    gpio_set_dieh(IO_PORTA_04, 0);
    gpio_set_pull_up(IO_PORTA_04, 0);
    gpio_set_pull_down(IO_PORTA_04, 0);

    P3_OSL_CON |= (BIT(0));

    P33_CON_SET(P3_VLD_KEEP, 1, 1, 1);                              //连接到P33，否则会导致闹钟中断进不去
    /* if (power_reset_flag) { */
    rtc_write_time(__this->default_sys_time);
    rtc_write_alarm(__this->default_alarm);
    /* } */
    /* rtc_time_dump(); */

    return 0;
}


/*		RTC TEST DEMO		*/
#define RTC_TEST_ENABLE		0

#if RTC_TEST_ENABLE

struct sys_time def_sys_time = {  //初始一下当前时间
    .year = 2020,
    .month = 1,
    .day = 1,
    .hour = 0,
    .min = 0,
    .sec = 0,
};

struct sys_time test_sys_time = {  //初始一下当前时间
    .year = 2021,
    .month = 2,
    .day = 2,
    .hour = 12,
    .min = 12,
    .sec = 12,
};

struct sys_time def_alarm = {     //初始一下目标时间，即闹钟时间
    .year = 2050,
    .month = 1,
    .day = 1,
    .hour = 0,
    .min = 0,
    .sec = 0,
};

struct sys_time test_alarm = {     //初始一下目标时间，即闹钟时间
    .year = 2050,
    .month = 1,
    .day = 1,
    .hour = 18,
    .min = 18,
    .sec = 18,
};

struct rtc_data rtc_data = {
    .default_sys_time = &def_sys_time,
    .default_alarm = &def_alarm,
    .cbfun = NULL,                      //闹钟中断的回调函数,用户自行定义
};

void rtc_test_demo()
{
    struct sys_time tmp_time;
    memset((u8 *)&tmp_time, 0, sizeof(tmp_time));

    rtc_init(&rtc_data);				//初始化rtc

    rtc_read_time(&tmp_time);						//读当前rtc时间
    printf("rtc_rtc_read_time_before: %d-%d-%d %d:%d:%d\n", tmp_time.year, tmp_time.month, tmp_time.day, tmp_time.hour, tmp_time.min, tmp_time.sec);	//打印读取时间值

    rtc_write_time(&test_sys_time); 		//修改rtc时间
    rtc_read_time(&tmp_time); 				//读修改后rtc时间
    printf("rtc_rtc_read_time_after: %d-%d-%d %d:%d:%d\n", tmp_time.year, tmp_time.month, tmp_time.day, tmp_time.hour, tmp_time.min, tmp_time.sec);		//打印修改后时间值

    rtc_read_alarm(&tmp_time); 					//读当前alarm时间
    printf("rtc_read_alarm_before: %d-%d-%d %d:%d:%d\n", tmp_time.year, tmp_time.month, tmp_time.day, tmp_time.hour, tmp_time.min, tmp_time.sec);	//打印读取闹钟时间值

    rtc_write_alarm(&test_alarm); 	//修改alarm时间
    rtc_read_alarm(&tmp_time); 		//读修改后alarm时间
    printf("rtc_read_alarm_after: %d-%d-%d %d:%d:%d\n", tmp_time.year, tmp_time.month, tmp_time.day, tmp_time.hour, tmp_time.min, tmp_time.sec);		//打印修改后闹钟时间值
}


#endif	//RTC_TEST_ENABLE


#endif
