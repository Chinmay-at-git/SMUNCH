#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
SYSCALL_DEFINE2(smunch,int,pid,unsigned long,bit_pattern)
{
	struct task_struct *pid_task1; //Note the pointer		
	unsigned long flags;	
	int ret=-2;	
	rcu_read_lock();
		pid_task1 = pid_task(find_vpid(pid),PIDTYPE_PID);
	rcu_read_unlock();	
	
	if(!lock_task_sighand(pid_task1,&flags))
	{
		//Process refuses to give the lock. Either absent/dead/dying (Should not be Zombie?)
		unlock_task_sighand(pid_task1,&flags);
		return -1;		
	}
			
	if(!thread_group_empty(pid_task1))
	{
		printk(KERN_ALERT "\nMULTI-Threaded Process, Exiting without processing");
		ret=-1; goto return_path;
	}
	//What to do if zombie!!
	if((pid_task1->exit_state & EXIT_TRACE) && (bit_pattern & (1UL<<(SIGKILL-1))))
	{ // EXIT_TRACE == EXIT_ZOMBIE||EXIT_DEAD 
		printk(KERN_ALERT "SIGKILL present while Process is Zombie/dead, releasing task!!");
		signal_wake_up(pid_task1,1);
		unlock_task_sighand(pid_task1,&flags);	 	
		release_task(pid_task1);  // detach_pid is called from release_task()
		return 0; 
	}	
	
	if(bit_pattern & (1UL<<(SIGKILL-1)))
	{
	// Signal is SIGKILL !!
		printk(KERN_ALERT "Ordinary Process and SIGKILL present");
		sigaddset(&pid_task1->signal->shared_pending.signal,SIGKILL);
		signal_wake_up(pid_task1,1);
		ret=0; goto return_path;
	}
	//Signal is not SIGKILL , Send ALL signals !!
	printk(KERN_ALERT "Ordinary Process and SIGKILL absent, sending all signals!");
	pid_task1->signal->shared_pending.signal.sig[0] = bit_pattern;
	signal_wake_up(pid_task1,1);
	ret=0;		
return_path:
	unlock_task_sighand(pid_task1,&flags);
	return(ret); 			
}
