#include "led_manager.h"
#include "red_led.h"
#include "blink_led_strategy.h"
#include "solid_led_strategy.h"

#include "log.h"

#include <linux/slab.h>
#include <linux/kernel.h>


struct led_manager_impl {
	struct lbi_led_manager base;
	struct lbi_led *red_led;
	struct lbi_led_strategy *led_strategy;
	char is_on;
};


static const unsigned blink_led_period_ms = 500;


static void led_manager_on(struct lbi_led_manager *manager)
{
	struct led_manager_impl *this = (struct led_manager_impl*) manager;
	this->is_on = 1;
	this->led_strategy->perform(this->led_strategy, this->red_led);
}


static void led_manager_off(struct lbi_led_manager *manager)
{
	struct led_manager_impl *this = (struct led_manager_impl*) manager;
	this->is_on = 0;
	this->led_strategy->stop(this->led_strategy, this->red_led);
}


static void set_strategy(struct led_manager_impl *manager, struct lbi_led_strategy *strategy)
{
	if (!strategy)
		return;

	manager->led_strategy->dtor(manager->led_strategy);
	manager->led_strategy = strategy;

	if (manager->is_on)
		manager->led_strategy->perform(manager->led_strategy, manager->red_led);
}


static void led_manager_set_mode(struct lbi_led_manager *manager, lbi_led_mode mode)
{
	struct led_manager_impl *this = (struct led_manager_impl*) manager;
	set_strategy(this, (LBI_LED_MODE_BLINK == mode)
		? create_blink_led_strategy(blink_led_period_ms) : create_solid_led_strategy());
}


static void led_manager_destroy(struct lbi_led_manager *manager)
{
	struct led_manager_impl *this = (struct led_manager_impl*) manager;
	if (this->led_strategy)
		this->led_strategy->dtor(this->led_strategy);
	if (this->red_led)
		this->red_led->dtor(this->red_led);
}


struct lbi_led_manager* lbi_create_led_manager(void)
{
	struct led_manager_impl *manager = kmalloc(sizeof(struct led_manager_impl), GFP_KERNEL);
	if (!manager) {
		LOG(KERN_ERR, "failed to allocate memory for led manager");
		return NULL;
	}

	do {
		manager->red_led = create_red_led();
		if (!manager->red_led)
			break;

		manager->led_strategy = create_solid_led_strategy();
		if (!manager->led_strategy)
			break;

		manager->base.on = &led_manager_on;
		manager->base.off = &led_manager_off;
		manager->base.set_mode = &led_manager_set_mode;
		manager->base.dtor = &led_manager_destroy;

		return (struct lbi_led_manager*) manager;
	} while (0);

	led_manager_destroy((struct lbi_led_manager*) manager);
	return NULL;
}
