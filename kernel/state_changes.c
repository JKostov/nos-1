#include <linux/kernel.h>
#include <include/linux/types.h>

asmlinkage long sys_state_changes(pid_t pid, unsigned int tsec)
{
  printk("SYS CALL TEST\r\n");
  printk("PID: %d, TSEC: %d\r\n", pid, tsec);
  return 0;
}