#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

typedef void(*timer_handler_t)(void);

struct timer_target {
    timer_handler_t timer_handle;
};

#define REGISTER_TIMER_TARGET(target) \
        const struct timer_target target sec(.timer_target)

extern const struct timer_target timer_target_begin[];
extern const struct timer_target timer_target_end[];

#define list_for_each_timer_target(p) \
    for (p = timer_target_begin; p < timer_target_end; p++)


int timer1_init(void);


void udelay(u32 us);
void mdelay(u32 ms);



#endif

