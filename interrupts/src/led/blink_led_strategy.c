#include "blink_led_strategy.h"

#include "log.h"

#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/timer.h>


struct blink_led_strategy;

struct blink_timer {
	struct timer_list timer;
	struct lbi_led *led;
	char is_on;
	struct blink_led_strategy *strategy;
};

struct blink_led_strategy {
	struct lbi_led_strategy base;
	struct blink_timer *timer;
	unsigned blink_period_ms;
};


static void timer_callback(struct timer_list *timer)
{
	struct blink_timer *this_timer = (struct blink_timer*) timer;

	if (this_timer->is_on) {
		this_timer->led->off(this_timer->led);
		this_timer->is_on = 0;
	} else {
		this_timer->led->on(this_timer->led);
		this_timer->is_on = 1;
	}

	mod_timer(timer, jiffies + msecs_to_jiffies(this_timer->strategy->blink_period_ms));
}


static void blink_led_strategy_perform(struct lbi_led_strategy* strategy, struct lbi_led *led)
{
	struct blink_led_strategy *this = (struct blink_led_strategy*) strategy;

	struct blink_timer *timer = kmalloc(sizeof(struct blink_timer), GFP_KERNEL);
	if (!timer) {
		LOG(KERN_ERR, "failed to allocate memeory for timer");
		return;
	}

	timer_setup((struct timer_list*) timer, &timer_callback, 0);
	led->on(led);
	this->timer->strategy = this;
	this->timer->led = led;
	this->timer->is_on = 1;
	mod_timer((struct timer_list*)&this->timer, jiffies + msecs_to_jiffies(this->blink_period_ms));
}


static void blink_led_strategy_stop(struct lbi_led_strategy* strategy, struct lbi_led *led)
{
	struct blink_led_strategy *this = (struct blink_led_strategy*) strategy;

	if (this->timer) {
		del_timer((struct timer_list*)&this->timer);
		this->timer = NULL;
	}

	led->off(led);
}


static void blink_led_strategy_destroy(struct lbi_led_strategy* strategy)
{
	struct blink_led_strategy *this = (struct blink_led_strategy*) strategy;

	if (this->timer)
		del_timer((struct timer_list*)&this->timer);
	kfree(strategy);
}


struct lbi_led_strategy* create_blink_led_strategy(unsigned blink_period_ms)
{
	struct blink_led_strategy *strategy = kmalloc(sizeof(struct blink_led_strategy), GFP_KERNEL);
	if (!strategy) {
		LOG(KERN_ERR, "failed to allocate memory for blink led strategy");
		return NULL;
	}

	strategy->base.perform = &blink_led_strategy_perform;
	strategy->base.stop = &blink_led_strategy_stop;
	strategy->base.dtor = &blink_led_strategy_destroy;
	strategy->timer = NULL;
	strategy->blink_period_ms = blink_period_ms;

	return (struct lbi_led_strategy*) strategy;
}
