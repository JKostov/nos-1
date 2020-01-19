#include <linux/kernel.h>

SYSCALL_DEFINE2(sys_state_changes, int, pid, int, tsec)
{
  printk("SYS CALL TEST\r\n");
  printk("PID: %d, TSEC: %d\r\n", pid, tsec);
  return 0;
}