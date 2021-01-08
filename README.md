# Linux kernel modification - Advanced operating systems

## Assignment
Show states of a process with a given PID for the last t seconds

## Process states

The state field of the process descriptor describes the current condition of the process Each process on the system is in exactly one of five different states. This value is represented by one of five flags:

- **TASK_RUNNING** - The process is runnable, and it is either currently running or on a runqueue waiting to run. This is the only possible state for a process executing in user-space and it can also apply to a process in kernel-space that is actively running

- **TASK_INTERRUPTIBLE** - The process is sleeping (it is blocked), waiting for some condition to exist. When this condition exists, the kernel sets the process’s state to TASK_RUNNING. The process also awakes prematurely and becomes runnable if it receives a signal

- **TASK_UNINTERRUPTIBLE** - This state is identical to TASK_INTERRUPTIBLE except that it does not wake up and become runnable if it receives a signal. This is used in situations where the process must wait without interruption or when the event is expected to occur quite quickly. Because the task does not respond to signals in this state, TASK_UNINTERRUPTIBLE is less often used than TASK_INTERRUPTIBLE

- **__TASK_TRACED** - The process is being traced by another process, such as a debugger, via ptrace

- **__TASK_STOPPED** - Process execution has stopped. The task is not running nor is it eligible to run. This occurs if the task receives the SIGSTOP, SIGTSTP, SIGTTIN, or SIGTTOU signal or if it receives any signal while it is being debugged

![Process state changes] (https://github.com/JKostov/nos-1/tree/master/docs/processStates.png)

In the [include/linux/sched.h#L83](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L83) file the process states are defined:
```c
#define TASK_RUNNING			0x0000
#define TASK_INTERRUPTIBLE		0x0001
#define TASK_UNINTERRUPTIBLE		0x0002
#define __TASK_STOPPED			0x0004
#define __TASK_TRACED			0x0008
```

## Manipulating the Current Process State

Kernel code often needs to change a process’s state.The preferred mechanism is using:

```c
set_current_state(state)
```

This function can be found in [include/linux/sched.h#L204](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L204).

# Modifications for the assignment

### task_struct extension

To be able to store the state changes the task struct is extended with list:

[include/linux/sched.h#L1286](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L1286)

```c
struct list_head state_changes;
```

### state_change structure
This structure keeps information about state changes and the time of the change
* state - process state
* time - time when the state was changed (time in seconds after booting the system)
* list - list_head node, keeping pointers to adjacent nodes in a list

[include/linux/sched.h#L2025](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L2025)
```c
struct state_change {
	long state;
	time64_t time;
	struct list_head list;
};
```

### Adding new state chenge in the list

As mentioned before the `set_current_state` function is used for changing the current process's state. This function is extended with the `add_new_state_in_state_changes()` function that adds a new state change for the current process:

[include/linux/sched.h#L204](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L204)

```c
#define __set_current_state(state_value) ({ current->state = (state_value); add_new_state_in_state_changes(); })
```
[include/linux/sched.h#L207](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L207)
```c
#define set_current_state(state_value) ({ smp_store_mb(current->state, (state_value)); add_new_state_in_state_changes(); })
```

[include/linux/sched.h#L2032](https://github.com/JKostov/nos-1/blob/81c9b61b9353d9b60183c0b7dfd3b6c3254e55a7/linux-5.4.9/include/linux/sched.h#L2032)
```c
static inline void add_state_change(void)
{
	struct task_struct *t = current;
	struct state_change *sc;
	sc = kmalloc(sizeof(*sc), GFP_ATOMIC);

	if(t == NULL || sc == NULL)
	{
		return;
	}

	sc->state = t->state;
	sc->time = ktime_get_seconds();

	list_add_rcu(&sc->list, &t->state_changes);
}

static inline void add_new_state_in_state_changes(void)
{
	struct task_struct *p = current;
	if (p == NULL)
	{
		return;
	}

	if (!is_idle_task(p))
	{
		p->number_of_state_changes = p->number_of_state_changes + 1;
		add_state_change();
	}
}
```

## List initialization and deallocation

When new process is created the list_head for the state changes is not initialized. The list initialization is done in the `copy_process` function:

[kernel/fork.c#L2236](https://github.com/JKostov/nos-1/blob/4c47feca4bc93465458f77ac9013745b1c9353dd/linux-5.4.9/kernel/fork.c#L2236)

```c
INIT_LIST_HEAD(&p->state_changes);
```

When process is terminated, the process descriptor for the process still exists, but the process is a zombie and is unable to run. After using the neccessary information for the terminated process, the task_struct of the process can be deallocated. At this point the state_changes list should be deallocated to. This is done in the `free_task_struct` function:

[kernel/fork.c#L172](https://github.com/JKostov/nos-1/blob/4c47feca4bc93465458f77ac9013745b1c9353dd/linux-5.4.9/kernel/fork.c#L172)

```c
struct state_change *p;
struct list_head *pos, *next;
list_for_each_safe(pos, next, &tsk->state_changes)
{
        p = list_entry(pos, struct state_change, list);
        list_del(pos);
        kfree(p);
}
```

# New System call for printing the process states

Accessing the process `task_struct` is done in kernel space, so for reading the state_chages new syscall must be implemented.

The new systemcall is defined in:

[kernel/state_changes.c](https://github.com/JKostov/nos-1/blob/master/linux-5.4.9/kernel/state_changes.c)

```c
#include <linux/signal_types.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/syscalls.h>
#include <linux/timekeeping.h>

SYSCALL_DEFINE2(print_state_changes, pid_t, pid, int, seconds)
{
  struct task_struct *p;
  printk("SYSCALL print state changes for process with pid: %d, for the last %d seconds\n", pid, seconds);

  p = find_task_by_vpid(pid);

  if (p == NULL)
  {
    printk("Unable to find process with pid: %d\n", pid);
    return -1;
    
  }

  struct state_change *state_changes;
  time64_t time_ns = ktime_get_seconds();
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
```

`SYSCALL_DEFINE2` macro is used for definition of a syscall with 2 parameters. The parameters sent are the PID and the seconds.

## Module for printing the process state changes

Module is implemented as a device to whom we can access through file sistem. Access is the same as the access to proc file system.

The module implementation:

[module/states_module.c](https://github.com/JKostov/nos-1/blob/master/module/states_module.c)

```c
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
```

Modules are used for kernel extension without changing the kernel code. They can be loaded at runtime.
