#include "solid_led_strategy.h"

#include "log.h"

#include <linux/slab.h>


static void solid_led_strategy_perform(struct lbi_led_strategy* strategy, struct lbi_led *led)
{
	led->on(led);
}


static void solid_led_strategy_stop(struct lbi_led_strategy* strategy, struct lbi_led *led)
{
	led->off(led);
}


static void solid_led_strategy_destroy(struct lbi_led_strategy* strategy)
{
	kfree(strategy);
}


struct lbi_led_strategy* create_solid_led_strategy(void)
{
	struct lbi_led_strategy *strategy = kmalloc(sizeof(struct lbi_led_strategy), GFP_KERNEL);
	if (!strategy) {
		LOG(KERN_ERR, "failed ot allocated memory for solid strategy");
		return NULL;
	}

	strategy->perform = &solid_led_strategy_perform;
	strategy->stop = &solid_led_strategy_stop;
	strategy->dtor = &solid_led_strategy_destroy;

	return strategy;
}
