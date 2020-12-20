#include <linux/signal_types.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(print_state_changes, pid_t, pid)
{
  printk("SYSCALL print state changes for porcess pid: %d\n", pid);
  return 0;
}