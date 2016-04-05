#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <stdlib.h>
#define smunch(a,b) syscall(326,a,b)  // ...or whatever number is next
void msg(void)
{
	printf("Usage: superkill pid signals_tosend");
	
}
int main(int argc,char *argv[])
{
	unsigned long bit_pattern=0;
	int pid,sig,i;
	if(argc<3)
	{
	msg(); return -1;
	}
	pid= atoi(argv[1]);
	for(i=2;i<argc;i++)
	{

	sig = atoi(argv[i]);
	bit_pattern=bit_pattern | 1UL << (sig-1);
	}		
	printf("Killing PID=%d with BitPattern %XH\n",pid,bit_pattern);
	sig= smunch(pid,bit_pattern);
	printf("Smunch Returns %d",sig);
}
