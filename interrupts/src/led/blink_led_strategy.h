#ifndef __LBI_LED_BLINK_LED_STRATEGY_H__
#define __LBI_LED_BLINK_LED_STRATEGY_H__

#include "led_strategy.h"

struct lbi_led_strategy* create_blink_led_strategy(unsigned blink_period_ms);

#endif // __LBI_LED_BLINK_LED_STRATEGY_H__
