#ifndef _CHARGE_H_
#define _CHARGE_H_

#include "typedef.h"

/*充满电电压选择 3.962V-4.611V*/
#define CHARGE_FULL_V_3962		0
#define CHARGE_FULL_V_4002		1
#define CHARGE_FULL_V_4044		2
#define CHARGE_FULL_V_4086		3
#define CHARGE_FULL_V_4130		4
#define CHARGE_FULL_V_4175		5
#define CHARGE_FULL_V_4222		6
#define CHARGE_FULL_V_4270		7
#define CHARGE_FULL_V_4308		8
#define CHARGE_FULL_V_4349		9
#define CHARGE_FULL_V_4391		10
#define CHARGE_FULL_V_4434		11
#define CHARGE_FULL_V_4472		12
#define CHARGE_FULL_V_4517		13
#define CHARGE_FULL_V_4564		14
#define CHARGE_FULL_V_4611		15
#define CHARGE_FULL_V_MAX       16

/*充满电电流选择 2mA-30mA*/
#define CHARGE_FULL_mA_2		0
#define CHARGE_FULL_mA_5		1
#define CHARGE_FULL_mA_7	 	2
#define CHARGE_FULL_mA_10		3
#define CHARGE_FULL_mA_15		4
#define CHARGE_FULL_mA_20		5
#define CHARGE_FULL_mA_25		6
#define CHARGE_FULL_mA_30		7

/*
 	充电电流选择
	恒流：20-220mA
*/
#define CHARGE_mA_20			0
#define CHARGE_mA_30			1
#define CHARGE_mA_40			2
#define CHARGE_mA_50			3
#define CHARGE_mA_60			4
#define CHARGE_mA_70			5
#define CHARGE_mA_80			6
#define CHARGE_mA_90			7
#define CHARGE_mA_100			8
#define CHARGE_mA_110			9
#define CHARGE_mA_120			10
#define CHARGE_mA_140			11
#define CHARGE_mA_160			12
#define CHARGE_mA_180			13
#define CHARGE_mA_200			14
#define CHARGE_mA_220			15

/*
 	充电口下拉选择
	电阻 50k ~ 200k
*/
#define CHARGE_PULLDOWN_50K     0
#define CHARGE_PULLDOWN_100K    1
#define CHARGE_PULLDOWN_150K    2
#define CHARGE_PULLDOWN_200K    3


#define CHARGE_CCVOL_V			300		//最低充电电流档转向用户设置充电电流档的电压转换点(AC693X无涓流充电，电池电压低时采用最低电流档，电池电压大于设置的值时采用用户设置的充电电流档)

struct charge_platform_data {
    u8 charge_en;	        //内置充电使能
    u8 charge_poweron_en;	//开机充电使能
    u8 charge_full_V;	    //充满电电压大小
    u8 charge_full_mA;	    //充满电电流大小
    u8 charge_mA;	        //充电电流大小
    u8 vpwr_pulldown_en;    //下拉使能位
    u8 vpwr_pulldown_lvl;	//vpwr的下拉电阻配置项,若充电舱需要更大的负载才能检测到插入时，请将该变量置为对应阻值
    u8 vpwr_pulldown_keep;  //下拉电阻在softoff时是否保持,vpwr_pulldown_en=1时有效
    u16 vpwr_off_filter;	//vpwr<0.6 拔出过滤值，过滤时间 = (filter*2)ms
    u16 vpwr_on_filter;     //vpwr>vbat 插入过滤值,电压的过滤时间 = (filter*2)ms
    u16 vpwr_keep_filter;   //0.6V<vpwr<vbat 维持电压过滤值,过滤时间= (filter*2)ms
    u16 charge_full_filter; //充满过滤值,连续检测充满信号恒为1才认为充满,过滤时间 = (filter*2)ms
    u8 pwr_sel;             //电源模式
};

/*----------------------------------------------------*/
/**@brief    获取VPWR在线状态
   @param    无
   @return   0:不在线 1:在线
   @note     VPWR电压大于插入电压（约0.6V）则返回值为1
*/
/*----------------------------------------------------*/
extern u8 charge_get_online_flag(void);
/*----------------------------------------------------*/
/**@brief    开始充电
   @param    无
   @return   无
   @note     当VPWR电压大于电池电压时,可进行充电
*/
/*----------------------------------------------------*/
extern void charge_start(void);
/*----------------------------------------------------*/
/**@brief    停止充电
   @param    无
   @return   无
   @note     当VPWR电压小于于电池电压时,可停止充电
*/
/*----------------------------------------------------*/
extern void charge_close(void);
/*----------------------------------------------------*/
/**@brief    获取配置的恒流充电电流档位
   @param    无
   @return   参考charge.h充电电流档位
   @note     该接口用于获取配置的档位
*/
/*----------------------------------------------------*/
extern u8 charge_get_mA_config(void);
/*----------------------------------------------------*/
/**@brief    设置恒流充电电流档位
   @param    参考charge.h充电电流档位
   @return   无
   @note     该接口用于改变充电电流档位,使用场景有:
             1、电池电压小时(<3V),用小电流充电(20mA)
                电池电压大时(>3V),用配置的电流充电(charge_mA)
             2、温度升高时,降低充电电流,减小发热
*/
/*----------------------------------------------------*/
extern void charge_set_mA(u8 charge_mA);
/*----------------------------------------------------*/
/**@brief    soft power off时,判定是否需要维持下拉电阻
   @param    无
   @return   0:关闭下拉电阻 1:使能下拉电阻
   @note     该接口由软关机函数调用
*/
/*----------------------------------------------------*/
extern u8 charge_get_vpwr_pulldown_en(void);
/*----------------------------------------------------*/
/**@brief    soft power off时,获取下拉电阻档位
   @param    无
   @return   0:50k 1:100k 2:150k 3:200k
   @note     该接口由软关机函数调用
*/
/*----------------------------------------------------*/
extern u8 charge_get_vpwr_pulldown_res(void);
/*----------------------------------------------------*/
/**@brief    获取VPWR和插入电压（约0.6V）比的大小
   @param    无
   @return   0:VPWR<0.6 1:VPWR>0.6
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
extern u8 charge_get_vpwr_online_hw(void);
/*----------------------------------------------------*/
/**@brief    获取VPWR和VBAT比的大小
   @param    无
   @return   0:VPWR<VBAT 1:VPWR>VBAT
   @note     该接口直接读取寄存器,没有经过软件滤波
*/
/*----------------------------------------------------*/
extern u8 charge_get_lvcmp_det(void);
/*----------------------------------------------------*/
/**@brief    内置充电初始化
   @param    配置结构体
   @return   无
   @note     设置充电相关寄存器
*/
/*----------------------------------------------------*/
extern void charge_init(void *arg);

#endif    //_CHARGE_H_
