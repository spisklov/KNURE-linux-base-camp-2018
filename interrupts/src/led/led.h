#ifndef __LBI_LED_LED_H__
#define __LBI_LED_LED_H__

struct lbi_led;

struct lbi_led {
	void (*on)(struct lbi_led *led);
	void (*off)(struct lbi_led *led);
	void (*dtor)(struct lbi_led *led);
};

#endif // __LBI_LED_LED_H__
