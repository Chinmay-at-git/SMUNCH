#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/types.h>

SYSCALL_DEFINE2(int pid, unsigned long bit_pattern)
{
	struct task_struct *pid_task; //Note the pointer		
	rcu_read_lock();
	pid_task = pid_task(find_vpid(pid),PIDTYPE_PID);
	rcu_read_unlock();	

	if(thread_group_empty(pid_task)!=0)
	{
		printk(KERN_ALERT "\nMULTI-Threaded Process, Exiting without processing");
		return -1;
	}
	
	if(bit_pattern & (1UL<<(SIGKILL-1)))
	{
	// Signal is SIGKILL !!
		sigaddset(&pid_task->signal->shared_pending.signal,SIGKILL);
		signal_wake_up(pid_task);
		return 0;
	}
	//Signal is not SIGKILL
	target->signal->shared_pending.signal.sig[0] = bit_pattern;
	signal_wake_up(pid_task);
		
}
