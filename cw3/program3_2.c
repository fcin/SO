#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

static void semafor_p(void);
static void semafor_v(void);

int semafor;
int skCount;

int main(int argc, char **argv)
{
	if(argc < 1)
	{
		printf("Za malo argumentow\n");
		exit(1);
		return 1;
	}

	semafor = atoi(argv[1]);
	skCount = atoi(argv[2]);

	for(int i = 0; i < skCount; i++)
	{
		semafor_p();
		printf("%d wykonuje prace\n", getpid());
		sleep(1);
		printf("%d skonczyl prace\n", getpid());
		semafor_v();
	}
}

static void semafor_p(void)
{
	int zmien_sem;
	struct sembuf bufor_sem;
	bufor_sem.sem_num=0;
	bufor_sem.sem_op=-1;
	bufor_sem.sem_flg=SEM_UNDO;
	zmien_sem=semop(semafor,&bufor_sem,1);
	if (zmien_sem==-1) 
	{
		printf("Nie moglem zamknac semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("%d zamknal semafor\n", getpid());
	}
}

static void semafor_v(void)
{
	int zmien_sem;
	struct sembuf bufor_sem;
	bufor_sem.sem_num=0;
	bufor_sem.sem_op=1;
	bufor_sem.sem_flg=SEM_UNDO;
	zmien_sem=semop(semafor,&bufor_sem,1);
	if (zmien_sem==-1) 
	{
		printf("Nie moglem otworzyc semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("%d otworzyl semafor\n", getpid());
	}
}
