#include <linux/module.h>
#include <linux/timer.h>
#include <linux/ctype.h>
#include <linux/init.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maksim Holikiv  golikov.mo@gmail.com");
MODULE_DESCRIPTION("Modul for test system calls");
MODULE_VERSION("0.1");


#define MAX_VALUE_OF_FEBONACHY  20



static unsigned long currJiff;
       u32   timerInterval = 1000;
static u8    counterToOff  = 9;


struct timer_list workTimer;


static void  timerCallback(struct timer_list *workTimer);
       u32   Febonachy(u32 value);


static int __init initial_func(void) {
    currJiff = get_jiffies_64();
    printk("current value of jiff is %d \n", currJiff);
	
    __init_timer(&workTimer, timerCallback, 0);
    mod_timer( &workTimer, jiffies + msecs_to_jiffies(timerInterval));
   	
    printk("timer was initialuzed. Time to wake up  %d \n", workTimer.expires);

return 0;
}


void  timerCallback(struct timer_list *workTimer){
    currJiff = get_jiffies_64();    

    printk("current value of jiff is %d \n", currJiff);
    
    if(counterToOff){
      counterToOff --;
      mod_timer( workTimer, jiffies + msecs_to_jiffies(timerInterval) );
    }

 }
u32   Febonachy(u32 value){
	if(value < MAX_VALUE_OF_FEBONACHY){

	}	
}


static void __exit unloading(void) {
    del_timer( &workTimer );

    u64 cval = get_jiffies_64() - currJiff; 
    printk("current value of jiff is %d \n", cval);
    printk("timer modul unload sucsessfully\n");
}

module_init(initial_func);
module_exit(unloading);
