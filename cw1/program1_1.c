#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	fprintf(stdout,"UID: %d\n", getuid());
	fprintf(stdout,"GID: %d\n", getgid());
	fprintf(stdout,"PID: %d\n", getpid());
	fprintf(stdout,"PPID: %d\n", getppid());
	exit (1);
}


