
ifneq ($(KERNELRELEASE),)

MOD_TARGET := lbi
SRC_DIR := src

SRC_FILES := lbi.c button/button.c led/blink_led_strategy.c\
	led/led_manager.c led/red_led.c led/solid_led_strategy.c fs/sysfs.c

SRC := $(patsubst %, $(SRC_DIR)/%, $(SRC_FILES))

ccflags-y := -std=gnu99 -Wall -Werror -I$(M)/$(SRC_DIR)

obj-m := $(MOD_TARGET).o

$(MOD_TARGET)-y := $(SRC:.c=.o)

else

.PHONY: all clean
all:
	$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNEL_BUILD_DIR) M=$(CURDIR) clean

endif
