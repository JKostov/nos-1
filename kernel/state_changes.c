#include <linux/kernel.h>

asmlinkage long sys_state_changes(unsigned int pid, unsigned int tsec)
{
  printk("SYS CALL TEST\r\n");
  printk("PID: %d, TSEC: %d\r\n", pid, tsec);
  return 0;
}