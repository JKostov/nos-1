#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/pid.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Julije Kostov");
MODULE_DESCRIPTION("Simple module to print states of a process.");
MODULE_VERSION("1.0.0");

#define DEVICE_NAME "states_module"
#define PROCFS_NAME "stt_mod"

static struct proc_dir_entry *module_proc_file;

static struct task_struct *p;

static int pid = 0;
module_param(pid, int, 0660);
MODULE_PARM_DESC(pid, "Process ID for printing the state changes");

static int seconds = -1;
module_param(seconds, int, 0660);
MODULE_PARM_DESC(seconds, "Number of seconds");

static void print_process_states(struct task_struct *p)
{
  
}

static ssize_t module_read(struct file *flip, char __user *ubuf, size_t count, loff_t *ppos)
{
  if (pid == 0)
  {
    printk("Process PID not passed.\n");
    return 0;
  }

  if (seconds == -1)
  {
    printk("Process PID not passed.\n");
    return 0;
  }

  time64_t time_ns = ktime_get_seconds();

  p = pid_task(find_vpid(pid), PIDTYPE_PID);
  if (p == NULL)
  {
    printk("Process with PID: %d not found\n", pid);
    return 0;
  }

  printk("MODULE print state changes for process with pid: %d, for the last %d seconds\n", pid, seconds);

  struct state_change *state_changes;
  time64_t max_time = time_ns - seconds;

  printk("Process states: ");
  list_for_each_entry(state_changes, &p->state_changes, list)
  {
    if (state_changes->time < max_time)
    {
      return;
    }

    printk("State: %ld\t, Time: %ld\n", state_changes->state, state_changes->time);
  }

  printk("\n");

  return 0;
}

static ssize_t module_write(struct file *flip, const char __user *ubuf, size_t count, loff_t *ppos)
{
  char buffer[100];
  int scaned_num;

  if (raw_copy_from_user(buffer, ubuf, count))
  {
    return -EINVAL;
  }

  scaned_num = sscanf(buffer, "%d %d", &pid, &seconds);

  if (scaned_num != 2)
  {
    printk("Error while parsing pid\n");
    return -EINVAL;
  }

  printk("SCANNED: %d\n", scaned_num);
  printk("PID: %d, SEC: %d\n", pid, seconds);

  int c = strlen(buffer);
  *ppos = c;
  return c;
}

static const struct file_operations file_ops = {
    .read = module_read,
    .write = module_write,
};

static int __init init_states_module(void)
{
  module_proc_file = proc_create(PROCFS_NAME, 0664, NULL, &file_ops);
  if (module_proc_file == NULL)
  {
    proc_remove(module_proc_file);
    printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROCFS_NAME);
    return -ENOMEM;
  }

  printk("STATES MODULE init\n");

  return 0;
}

static void __exit exit_states_module(void)
{
  proc_remove(module_proc_file);
  printk("STATES MODULE exit\n");
}

module_init(init_states_module);
module_exit(exit_states_module);