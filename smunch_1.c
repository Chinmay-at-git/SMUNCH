#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/types.h>

SYSCALL_DEFINE2(int pid, unsigned long bit_pattern)
{
	struct task_struct *pid_task; //Note the pointer		
	unsigned long flags;	
	int ret=-2;	
	rcu_read_lock();
		pid_task = pid_task(find_vpid(pid),PIDTYPE_PID);
	rcu_read_unlock();	
	
	if(!lock_task_sighand(pid_task,&flags))
	{
		//Process refuses to give the lock. Either absent/dead/dying (Should not be Zombie?)
		return -1;		
	}
			
	if(thread_group_empty(pid_task)!=0)
	{
		printk(KERN_ALERT "\nMULTI-Threaded Process, Exiting without processing");
		ret=-1; goto return_path;
	}
	//What to do if zombie!!
	if((pid_task->exit_state & EXIT_ZOMBIE) && (bit_pattern & (1UL<<(SIGKILL-1))))
	{ 
		printk(KERN_ALERT "SIGKILL present while Zombie, detaching it!!");
		unlock_task_sighand(pid_task,&flags);	 	
		detach_pid(pid_task, PIDTYPE_PID);
		return 0; 
	}	
	
	if(bit_pattern & (1UL<<(SIGKILL-1)))
	{
	// Signal is SIGKILL !!
		sigaddset(&pid_task->signal->shared_pending.signal,SIGKILL);
		signal_wake_up(pid_task);
		ret=0; goto return_path;
	}
	//Signal is not SIGKILL , Send ALL signals !!
	target->signal->shared_pending.signal.sig[0] = bit_pattern;
	signal_wake_up(pid_task);
	ret=0;		
return_path:
	unlock_task_sighand(pid_task,&flags);
	return ret; 			
}
