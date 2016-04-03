#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <stdlib.h>
#define smunch(a,b) syscall(326,a,b)  // ...or whatever number is next
void msg(void)
{
	printf("Usage: superkill pid signal_tosend");
	
}
int main(int argc,char *argv[])
{
	unsigned long bit_pattern;
	int pid,sig;
	if(argc<3)
	{
	msg(); return -1;
	}
	pid= atoi(argv[1]);
	sig = atoi(argv[2]);
	bit_pattern=1UL << (sig-1);
	
	printf("Killing PID=%d with BitPattern %X\n",pid,bit_pattern);
	sig= smunch(pid,bit_pattern);
	printf("Smunch Returns %d",sig);
}
