#include "asm/includes.h"
#include "printf.h"
#include "resfile.h"
#include "msg.h"

static void user_msg_handler()
{
#if (USE_KEY_DRIVER == 1)

    int msg = get_msg();
    if (msg == NO_MSG) {
        return;
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
    default:
        break;
    }

#endif
}


int user_main()
{
#if (USE_KEY_DRIVER == 1)
    key_driver_init(&iokey_para);
#endif

    while (1) {
        wdt_clear();

        user_msg_handler();

        __asm__ volatile("idle");
    }

    return 0;
}
