#include "sysfs.h"

#include "log.h"

#include <linux/device.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/sysfs.h>


typedef ssize_t (*show_callback_t)(struct class *class, struct class_attribute *attr, char *buf);
typedef ssize_t (*store_callback_t)(struct class *class, struct class_attribute *attr, const char *buf, size_t count);


struct sysfs_impl {
	struct lbi_sysfs_handlers handlers;
	struct class *fs_class;
	struct class_attribute mode_attr;
	struct class_attribute count_attr;
	struct class_attribute start_attr;
	struct class_attribute stop_attr;
	struct class_attribute clear_attr;
};


struct led_mode {
	lbi_led_mode mode;
	const char const* name;
	size_t size;
};


static struct sysfs_impl sysfs = {
	.fs_class = NULL,
};

static const struct led_mode modes[] = {
	{
		.mode = LBI_LED_MODE_BLINK,
		.name = "blink",
		.size = 5
	},
	{
		.mode = LBI_LED_MODE_SOLID,
		.name = "solid",
		.size = 5
	},
};

static const size_t modes_size = sizeof(modes) / sizeof(struct led_mode);


static ssize_t mode_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	unsigned i;
	if (!sysfs.handlers.on_set_mode)
		return -ENAVAIL;

	for (i = 0; i != modes_size; ++i) {
		if (modes[i].size != count) {
			continue;
		} else if (0 == strncmp(modes[i].name, buf, count)) {
			sysfs.handlers.on_set_mode(modes[i].mode);
			return count;
		}
	}

	return -EINVAL;
}


static ssize_t mode_show(struct class *class, struct class_attribute *attr, char *buf)
{
	int i;
	lbi_led_mode led_mode;
	if (!sysfs.handlers.on_get_mode)
		return -ENAVAIL;

	led_mode = sysfs.handlers.on_get_mode();
	for (i = 0; i != modes_size; ++i) {
		if (modes[i].mode == led_mode) {
			strcpy(buf, modes[i].name);
			return modes[i].size + 1;
		}
	}

	return -EIO;
}


static ssize_t count_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (!sysfs.handlers.on_get_count)
		return -ENAVAIL;

	return sprintf(buf, "%u", sysfs.handlers.on_get_count());
}


static ssize_t start_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (!sysfs.handlers.on_start)
		return -ENAVAIL;

	sysfs.handlers.on_start();

	return sprintf(buf, "started");
}


static ssize_t stop_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (!sysfs.handlers.on_stop)
		return -ENAVAIL;

	sysfs.handlers.on_stop();

	return sprintf(buf, "stop");
}


static ssize_t clear_show(struct class *class, struct class_attribute *attr, char *buf)
{
	if (!sysfs.handlers.on_stop)
		return -ENAVAIL;

	sysfs.handlers.on_clear();

	return sprintf(buf, "clear");
}


static int create_sysfs_file(struct class* class, const char *name, show_callback_t show, store_callback_t store, int mode, struct class_attribute *attr)
{
	int res;

	attr->attr.mode = mode;
	attr->attr.name = name;
	attr->show = show;
	attr->store = store;

	res = class_create_file(class, attr);
	if (res)
		LOG(KERN_ERR, "failed to create sysfs file %s", name);

	return res;
}


void lbi_destroy_sysfs(void)
{
	if (sysfs.fs_class) {
		class_remove_file(sysfs.fs_class, &sysfs.mode_attr);
		class_remove_file(sysfs.fs_class, &sysfs.count_attr);
		class_remove_file(sysfs.fs_class, &sysfs.start_attr);
		class_remove_file(sysfs.fs_class, &sysfs.stop_attr);
		class_remove_file(sysfs.fs_class, &sysfs.clear_attr);

		class_destroy(sysfs.fs_class);
		sysfs.fs_class = NULL;
	}
}


int lbi_create_sysfs(struct lbi_sysfs_handlers *handlers)
{
	int result = 0;

	do {
		if (sysfs.fs_class) {
			LOG(KERN_WARNING, "sysfs class already exists");
			return -EEXIST;
		}

		sysfs.fs_class = class_create(THIS_MODULE, MODULE_NAME);
		if (IS_ERR(sysfs.fs_class)) {
			LOG(KERN_ERR, "failed to create sysfs class");
			return -EPERM;
		}

		memcpy(&sysfs.handlers, handlers, sizeof(struct lbi_sysfs_handlers));

		result = create_sysfs_file(sysfs.fs_class, "mode", &mode_show, &mode_store, (S_IRUGO | S_IWUGO), &sysfs.mode_attr);
		if (result)
			break;

		result = create_sysfs_file(sysfs.fs_class, "count", &count_show, NULL, S_IRUGO, &sysfs.count_attr);
		if (result)
			break;

		result = create_sysfs_file(sysfs.fs_class, "start", &start_show, NULL, S_IRUGO, &sysfs.start_attr);
		if (result)
			break;

		result = create_sysfs_file(sysfs.fs_class, "stop", &stop_show, NULL, S_IRUGO, &sysfs.stop_attr);
		if (result)
			break;

		result = create_sysfs_file(sysfs.fs_class, "clear", &clear_show, NULL, S_IRUGO, &sysfs.clear_attr);
		if (result)
			break;

		return result;
	} while (0);

	lbi_destroy_sysfs();
	return result;
}
