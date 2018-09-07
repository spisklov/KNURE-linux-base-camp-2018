#ifndef __LBI_LED_LED_MANAGER_H__
#define __LBI_LED_LED_MANAGER_H__

#include "led_mode.h"


struct lbi_led_manager;

struct lbi_led_manager {
	void (*on)(struct lbi_led_manager* manager);
	void (*off)(struct lbi_led_manager* manager);
	void (*set_mode)(struct lbi_led_manager* manager, lbi_led_mode mode);
	void (*dtor)(struct lbi_led_manager* manager);
};

struct lbi_led_manager* lbi_create_led_manager(void);

#endif // __LBI_LED_LED_MANAGER_H__
