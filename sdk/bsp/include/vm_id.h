#ifndef  __VM_ID_H__
#define  __VM_ID_H__


enum {
    VM_PMU_VOLTAGE = 1,
    //前20个id作保留位，请不要使用

    VM_USER_SART = 21,
    //用户区//


    //用户区//
    VM_USER_MAX = 127, //VM ID不能超过127
};



#endif  /*VM_ID_H*/
