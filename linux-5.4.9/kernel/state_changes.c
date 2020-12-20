#include <linux/signal_types.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(print_state_changes, pid_t, pid, int, milliseconds)
{
  printk("SYSCALL print state changes for process with pid: %d\n int the last %d miliseconds", pid, milliseconds);
  return 0;
}