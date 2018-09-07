#ifndef __LBI_FS_SYSFS_H__
#define __LBI_FS_SYSFS_H__

#include "led/led_mode.h"

#include <linux/device.h>


struct lbi_sysfs_handlers {
	lbi_led_mode (*on_get_mode)(void);
	void (*on_set_mode)(lbi_led_mode mode);
	unsigned (*on_get_count)(void);
	void (*on_start)(void);
	void (*on_stop)(void);
	void (*on_clear)(void);
};


int lbi_create_sysfs(struct lbi_sysfs_handlers *handlers);
void lbi_destroy_sysfs(void);

#endif // __LBI_FS_SYSFS_H__
