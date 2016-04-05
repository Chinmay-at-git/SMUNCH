#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/signal.h>
#include <unistd.h>

#define deepsleep() syscall(325)  // ...or whatever number is next
void custom1()
{
printf("Deepsleep USR1");
return;
}
void custom2()
{
printf("Deepsleep USR2");
return;
}
int main()
{
	signal(SIGUSR1,custom1);
signal(SIGUSR2,custom2);
	printf("goodnight, Irene\n");
	deepsleep();
	printf("oops ... woke up!\n");
	sleep(1000);
}
