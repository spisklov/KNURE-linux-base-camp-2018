#ifndef __LBI_LOG_H__
#define __LBI_LOG_H__

#include <linux/kern_levels.h>

#define MODULE_NAME KBUILD_MODNAME
#define LOG(level, fmt, ...) printk(level MODULE_NAME ": " fmt "\n", ##__VA_ARGS__)

#endif // __LBI_LOG_H__
