#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <../include/linux/types.h>
#include <../include/linux/sched.h>
#include <../include/linux/pid.h>
#include <linux/timekeeping.h>

SYSCALL_DEFINE2(task_state_changes, unsigned int, pid, unsigned int, tsec)
{
  printk("SYS CALL TEST\r\n");
  printk("PID: %d, TSEC: %d\r\n", pid, tsec);

  struct task_struct *p = NULL;

  rcu_read_lock(); 
  p = find_task_by_vpid(find_vpid(pid));

  if(p == NULL)
  {
    rcu_read_unlock(); 
    printk("Process not found.\r\n");
    return 0;
  }

  u64 current_time = ktime_get_ns() + tsec * 1000000000;

  printk("Process %d - states: ", p->pid);
  struct state_change *ptr;
  list_for_each_entry_reverse(ptr, &p->state_changes, list) {
    if (ptr->time < current_time) {
      printk("%ld, ", ptr->state);
    }
  }
  printk("\r\n");
  rcu_read_unlock(); 

  return 0;
}