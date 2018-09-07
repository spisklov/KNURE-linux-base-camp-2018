#include "button.h"

#include "log.h"

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/slab.h>


struct button {
	unsigned btn_gpio;
	unsigned btn_irq;
	lbi_on_button_callback_t callback;
};

static struct button *btn = NULL;


static irqreturn_t gpio_interrupt_handler(int irq, void *dev_id)
{
	if (btn && btn->callback) {
		btn->callback();
	}

	return IRQ_HANDLED;
}


int lbi_create_button(lbi_on_button_callback_t callback)
{
	int result = 0;

	btn = kmalloc(sizeof(struct button), GFP_KERNEL);
	if (!btn) {
		LOG(KERN_ERR, "failed to allocate memory for button");
		return -ENOMEM;
	}

	btn->btn_gpio = 7; //PA6
	btn->callback = callback;

	result = gpio_request(btn->btn_gpio, "sysfs");
	result |= gpio_direction_input(btn->btn_gpio);
	result |= gpio_export(btn->btn_gpio, false);

	LOG(KERN_INFO, "button gpio is set up, res %d", result);

	btn->btn_irq = gpio_to_irq(btn->btn_gpio);
	LOG(KERN_INFO, "button gpio input is mapped to %u irq", btn->btn_irq);
	result = request_irq(btn->btn_irq, &gpio_interrupt_handler, IRQF_TRIGGER_RISING, "gpio button interrupt handler", NULL);
	return result;
}


void lbi_destroy_button(void)
{
	if (!btn) {
		LOG(KERN_WARNING, "nothing to destroy in button");
		return;
	}

	free_irq(btn->btn_irq, NULL);
	gpio_unexport(btn->btn_gpio);
	gpio_free(btn->btn_gpio);

	kfree(btn);
}
