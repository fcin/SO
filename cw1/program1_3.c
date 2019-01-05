#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	for(int i = 0; i < 3; i++)
	{
		switch(fork())
		{
			case -1: 
				fprintf(stdout,"ERROR\n");
				exit(-1);
				return -1;
			case 0:
				execl("program1_1", "program1_1", NULL);
				break;
			default:
				break;
		}
	}

	exit (1);
}
