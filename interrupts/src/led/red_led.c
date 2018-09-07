#include "red_led.h"

#include "log.h"

#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/slab.h>


static const unsigned red_led_pin = 15;


static void red_led_on(struct lbi_led *led)
{
	if (!gpio_direction_output(red_led_pin, 1)) {
		LOG(KERN_ERR, "failed to switch on red led on pin %u", red_led_pin);
	}
}


static void red_led_off(struct lbi_led *led)
{
	if (gpio_direction_output(red_led_pin, 0)) {
		LOG(KERN_ERR, "failed to switch off red led on pin %u", red_led_pin);
	}
}


static void red_led_destructor(struct lbi_led *led)
{
	red_led_off(led);
	gpio_unexport(red_led_pin);
	gpio_free(red_led_pin);

	if (led)
		kfree(led);

	LOG(KERN_INFO, "unexport red led gpio %u", red_led_pin);
}


struct lbi_led* create_red_led(void)
{
	struct lbi_led *led;

	do {
		if (!gpio_is_valid(red_led_pin)) {
			LOG(KERN_ERR, "invalid red led input gpio %u", red_led_pin);
			break;
		}

		if (gpio_request(red_led_pin, MODULE_NAME)) {
			LOG(KERN_ERR, "can't obtain red led gpio %u", red_led_pin);
			break;
		}

		if (gpio_direction_output(red_led_pin, 0)) {
			LOG(KERN_ERR, "can't configure red led gpio %u", red_led_pin);
			break;
		}

		if (gpio_export(red_led_pin, false)) {
			LOG(KERN_ERR, "can't export red led gpio %u", red_led_pin);
			break;
		}

		led = (struct lbi_led*) kmalloc(sizeof(struct lbi_led), GFP_KERNEL);

		if (!led) {
			LOG(KERN_ERR, "failed to allocate memory for red led");
			red_led_destructor(led);
			break;
		}

		led->on = &red_led_on;
		led->off = &red_led_off;
		led->dtor = &red_led_destructor;
		LOG(KERN_INFO, "red led is initialized on pin %u", red_led_pin);

		return led;

	} while (false);

	return NULL;
}
