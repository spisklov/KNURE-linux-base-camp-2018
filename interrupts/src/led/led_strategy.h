#ifndef __LBI_LED_LED_STRATEGY_H__
#define __LBI_LED_LED_STRATEGY_H__

#include "led.h"


struct lbi_led_strategy;

struct lbi_led_strategy {
	void (*perform)(struct lbi_led_strategy* strategy, struct lbi_led *led);
	void (*stop)(struct lbi_led_strategy* strategy, struct lbi_led *led);
	void (*dtor)(struct lbi_led_strategy* strategy);
};

#endif // __LBI_LED_LED_STRATEGY_H__
