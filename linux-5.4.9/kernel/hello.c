#include<linux/kernel.h>

asmlinkage long sys_hello(void)
{
  printk("Hello sys call!");
  return 0;
}