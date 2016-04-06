#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
SYSCALL_DEFINE2(smunch,int,pid,unsigned long,bit_pattern)
{
	unsigned long flags;	
	struct task_struct *task; 		
	int ret;	
	rcu_read_lock();
	task = pid_task(find_vpid(pid),PIDTYPE_PID);
	rcu_read_unlock();	
	if(!task) return -1;	 // Process not present
	if(!lock_task_sighand(task,&flags))
	{
		//Process refuses to give the lock. Either dead/dying 
		unlock_task_sighand(task,&flags);
		return -1;		
	}		
	printk(KERN_ALERT "\nExit State:%XH,State=%XH ptrace=%d\n",task->exit_state,task->state,task->ptrace);	//Info to user
	if(!thread_group_empty(task) ||  task->ptrace)
	{
		printk(KERN_ALERT "\nMULTI-Threaded Process or being Traced, exiting without processing");
		ret=-1; 
		goto return_path;
	}
	if(bit_pattern & (1UL<<(SIGKILL-1)) && (task->exit_state & EXIT_ZOMBIE))
	{ 
		printk(KERN_ALERT "\nSIGKILL present while Process is Zombie, releasing task!!");		
		unlock_task_sighand(task,&flags);	 	
		release_task(task);  // detach_pid is called from release_task()
		return 0; 
	}
	printk(KERN_ALERT "!SIGKILL || (ordinary process) || DeepSleep, sending all signals!");
			/* It is Users responsility to note that signals will get handled from 1-64 order*/	
	task->signal->shared_pending.signal.sig[0] = bit_pattern;
	set_tsk_thread_flag(task,TIF_SIGPENDING);	
	if(task->state & TASK_UNINTERRUPTIBLE)
	{	printk(KERN_ALERT "\nProcess is in Uniterruptible Wait-DeepSleep!!"); // Info to User	
		wake_up_process(task); //wake_up_state(task,TASK_INTERRUPTIBLE); does not work
		ret=0; 
		goto return_path; 
	}
	signal_wake_up(task,1);
	ret=0;		
	return_path:
	unlock_task_sighand(task,&flags);
	return ret;
}
