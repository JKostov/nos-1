#include <linux/signal_types.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/syscalls.h>
#include <linux/timekeeping.h>

SYSCALL_DEFINE2(print_state_changes, pid_t, pid, int, milliseconds)
{
  struct task_struct *p;
  printk("SYSCALL print state changes for process with pid: %d, for the last %d miliseconds\n", pid, milliseconds);

  p = find_task_by_vpid(pid);

  if (p == NULL)
  {
    printk("Unable to find process with pid: %d\n", pid);
    return -1;
    
  }

  struct state_change *state_changes;
  u64 time_ns = ktime_get_ns();
  u64 max_time = time_ns - ((unsigned int)milliseconds * 1000000);

  printk("CURR: %ld\n", time_ns);
  printk("MAX: %ld\n", max_time);

  printk("Process states: ");
	list_for_each_entry(state_changes, &p->state_changes, list)
  {
    // if (state_changes->time < max_time)
    // {
    //   return;
    // }

    printk("State: %ld\t, Time: %ld\t Is: %d\t, Unsig: %ld\n", state_changes->state, state_changes->time, state_changes->time < max_time, ((unsigned long)state_changes->time) < ((unsigned long)max_time));
  }

  printk("\n");

  return 0;
}