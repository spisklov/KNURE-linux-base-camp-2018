#include "log.h"

#include "button/button.h"
#include "fs/sysfs.h"
#include "led/led_manager.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergii Pisklov");
MODULE_DESCRIPTION("Led Button Interrupt");
MODULE_VERSION("0.1");


struct lbi {
	struct lbi_led_manager *led_manager;
	char button_inited;
	char sysfs_inited;
	char is_on;
	unsigned press;
	lbi_led_mode mode;
	char gather_statistic;
};


static struct lbi *lbi = NULL;


static void on_button(void)
{
	if (lbi->gather_statistic)
		++lbi->press;

	if (lbi->is_on) {
		lbi->led_manager->off(lbi->led_manager);
		lbi->is_on = 0;
	}
	else {
		lbi->led_manager->on(lbi->led_manager);
		lbi->is_on = 1;
	}
}


static lbi_led_mode get_mode(void)
{
	return lbi->mode;
}


static void set_mode(lbi_led_mode mode)
{
	lbi->led_manager->set_mode(lbi->led_manager, mode);
}


static unsigned get_interrupt_count(void)
{
	return lbi->press;
}


static void start_statistic(void)
{
	lbi->gather_statistic = 1;
}


static void stop_statistic(void)
{
	lbi->gather_statistic = 0;
}


static void clear_statistic(void)
{
	lbi->press = 0;
}


static void destroy_lbi(void)
{
	if (!lbi) {
		LOG(KERN_ERR, "nothig to destruct");
		return;
	}

	if (lbi->sysfs_inited)
		lbi_destroy_sysfs();

	if (lbi->button_inited)
		lbi_destroy_button();

	if (lbi->led_manager)
		lbi->led_manager->dtor(lbi->led_manager);

	kfree(lbi);
	lbi = NULL;
}


int lbi_probe(struct platform_device *device)
{
	int result = 0;
	struct lbi_sysfs_handlers sysfs_handlers = {
		&get_mode,
		&set_mode,
		&get_interrupt_count,
		&start_statistic,
		&stop_statistic,
		&clear_statistic
	};

	LOG(KERN_INFO, "%s", __FUNCTION__);

	if (lbi) {
		LOG(KERN_ERR, "already initialized");
		return -EALREADY;
	}

	lbi = kzalloc(sizeof(struct lbi), GFP_KERNEL);
	if (!lbi) {
		LOG(KERN_ERR, "failed to allocate memory for lbi");
		return -ENOMEM;
	}

	do {
		lbi->led_manager = lbi_create_led_manager();
		if (!lbi->led_manager) {
			result = -ENOMEM;
			break;
		}

		result = lbi_create_button(&on_button);
		if (result)
			break;
		lbi->button_inited = 1;

		result = lbi_create_sysfs(&sysfs_handlers);
		if (result)
			break;
		lbi->sysfs_inited = 1;

		lbi->press = 0;
		lbi->mode = LBI_LED_MODE_SOLID;
		lbi->gather_statistic = 1;
		return 0;

	} while(0);

	destroy_lbi();
	return result;
}


int lbi_remove(struct platform_device *device)
{
	LOG(KERN_INFO, "%s", __FUNCTION__);
	destroy_lbi();
	return 0;
}


static struct of_device_id lbi_match_table[] = {
	{
		.compatible = "Sepa,lbi",
	},
	{},
};

MODULE_DEVICE_TABLE(of, lbi_match_table);


static struct platform_driver lbi_platform_driver = {
	.probe = &lbi_probe,
	.remove = &lbi_remove,
	.driver = {
		.name = "lbi",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(lbi_match_table),
	},
};

module_platform_driver(lbi_platform_driver);
