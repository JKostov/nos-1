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
