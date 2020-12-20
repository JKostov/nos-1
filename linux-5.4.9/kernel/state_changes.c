#include <linux/signal_types.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/syscalls.h>
#include <linux/timekeeping.h>

static void print_state_changes(struct task_struct *p, int milliseconds)
{
  struct state_change *state_changes;
  u64 time_ns = ktime_get_ns();
  u64 max_time = time_ns - milliseconds * 1000000;
  int counter = 0;

  printk("In total there were %ld state changes for the process.", &p->number_of_state_changes);
  
  printk("Process states: ", &p->number_of_state_changes);
	list_for_each_entry(state_changes, &p->state_changes, list)
  {
    if (counter == 10)
    {
      return;
    }

    printk("%ld, ", state_changes->state);
    counter++;
  }

  printk("\n");
}

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

  print_state_changes(p, milliseconds);

  return 0;
}