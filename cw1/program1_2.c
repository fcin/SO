#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	int parentpid = getpid();
	for(int i = 0; i < 3; i++)
	{
		switch(fork())
		{
			case -1: 
				fprintf(stdout,"ERROR\n");
				exit(-1);
				return -1;
			case 0:
				fprintf(stdout,"UID: %d\n", getuid());
				fprintf(stdout,"GID: %d\n", getgid());
				fprintf(stdout,"PID: %d\n", getpid());
				fprintf(stdout,"PPID: %d\n", getppid());
				break;
			default:
				break;
		}
	}

	if(parentpid == getpid())
	{
		char pid[10];
		sprintf(pid, "%d", parentpid);
		execl("/bin/pstree", "pstree", "-p", pid, NULL);
	}

	exit (1);
}
