#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semafor;

static void utworz_nowy_semafor(void);
static void ustaw_semafor(void);
static void semafor_p(void);
static void semafor_v(void);
static void usun_semafor(void);

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Za malo argumentow\n");
		exit(1);
		return 1;
	}

	utworz_nowy_semafor();
	ustaw_semafor();
	semafor_v();

	char* programName = argv[1];

	int processCount = atoi(argv[2]);
	char semaforName[10];
	sprintf(semaforName, "%d", semafor);

	int skCount = atoi(argv[3]);
	char skCountString[10];
	sprintf(skCountString, "%d", skCount);

	for(int i = 0; i < processCount; i++)
	{
		switch(fork())
		{
			case -1:
				printf("Za malo argumentow\n");
				exit(1);
				return 1;
			case 0:
				execl("program3_2", "program3_2", semaforName, skCountString, NULL);
				break;
			default:
			{
				int a, status;
				while((a = wait(&status)) == 0);
			}
		}
	}



	usun_semafor();
}

static void utworz_nowy_semafor(void)
{
	semafor=semget(10,1,0777|IPC_CREAT);
	if (semafor==-1) 
	{
		printf("Nie moglem utworzyc nowego semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Semafor zostal utworzony : %d\n",semafor);
	}
}

static void ustaw_semafor(void)
{
	int ustaw_sem;
	ustaw_sem=semctl(semafor,0,SETVAL,0);
	if (ustaw_sem==-1)
	{
		printf("Nie mozna ustawic semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Semafor zostal ustawiony.\n");
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
		printf("Semafor zostal zamkniety.\n");
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
		printf("Semafor zostal otwarty.\n");
	}
}

static void usun_semafor(void)  
{
	int sem;
	sem=semctl(semafor,0,IPC_RMID);
	if (sem==-1)
	{
		printf("Nie mozna usunac semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Semafor zostal usuniety : %d\n",sem);
	}
}

