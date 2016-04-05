#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/signal.h>
#include <unistd.h>
void Custom()
{
  printf("\nSIGUSR1 Custom handler\n");
	return;
}
void custom2()
{
printf("\nSIGUSR2 Custom Handler\n");
return;
}
int main()
{
  int pid, ret;
  switch(pid = fork())
  {
    case 0:
      signal(SIGUSR1, Custom);
	sleep(100);
      printf("Child process %d\n",getpid());
      break;
    default:
      printf("Parent process %d \n",getpid());
 	signal(SIGUSR1,Custom);
	signal(SIGUSR2,custom2);
      //ret = kill(pid, SIGUSR1);
      sleep(1000);
	printf("kill returned %d\n", ret);
      sleep(50);
      break;
  }
  return 0;
}
