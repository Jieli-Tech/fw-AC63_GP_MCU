#ifndef _CTMU_H_
#define _CTMU_H_

#include "typedef.h"
#include "asm/includes.h"


#define CTMU_CH_PA03    0
#define CTMU_CH_PB07    1
#define CTMU_CH_PA05    2
#define CTMU_CH_PA06    3
#define CTMU_CH_PC02    4
#define CTMU_CH_PC03    5
#define CTMU_CH_PB04    6
#define CTMU_CH_PB06    7


struct ctmu_key_port {
    u32 press_delta;    //按下判决的阈值
    u8 ch_io; 			//触摸通道有固定的8个IO可选，比如填CTMU_CH_PA03，CTMU_CH_PA04，，，，
    u8 key_value; 		//按键返回值
};

struct ctmu_touch_key_platform_data {
    u8 num; 	        //触摸按键个数
    const struct ctmu_key_port *port_list;
};

#define CTMU_PLATFORM_DATA_BEGIN(data) \
    const struct ctmu_touch_key_platform_data data = {

#define CTMU_PLATFORM_DATA_END()  \
    };


/*
 * @brief  ctmu 总初始化函数
 * @param pdata 初始化的参数结构体地址 ： struct ctmu_touch_key_platform_data *
 * @return  0,成功  非0,失败
*/
int ctmu_init(void *pdata);

/*
 * @brief 获取注册的触摸按键的序号
 * @return  按键序号
*/
u8 get_ctmu_value(void);

/*
 * @brief sdk提供的参考函数
*/
void ctmu_test(void);


#endif /* #ifndef _CTMU_H_ */

