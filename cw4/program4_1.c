#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>

// Uzyj: gcc program4_1.c -pthread -o program4_1

#define PISZ 0
#define CZYTAJ 1

int semafor;
char bufor[1];

static void utworz_nowy_semafor(void);
static void ustaw_semafor(int semNum);
static void semafor_p(int semNum);
static void semafor_v(int semNum);
static void usun_semafor(void);
static void produkuj();
static void * konsumuj(void *);
static void przygotuj_pliki();

int main(int argc, char **argv)
{

	utworz_nowy_semafor();
	ustaw_semafor(PISZ);
	ustaw_semafor(CZYTAJ);

	przygotuj_pliki();

	pthread_t thread1;
	int tresult = pthread_create(&thread1, NULL, konsumuj, NULL);
	if(tresult)
	{
		printf("ERROR\n");
		exit(1);
	}

	produkuj();

	pthread_join(thread1, NULL);
}

static void produkuj()
{
	FILE* z = fopen("zrodlo", "r");
	fseek(z, 0, SEEK_SET);
	char znak;
	do
	{
		znak = fgetc(z);

		semafor_p(CZYTAJ);
		
		bufor[0] = znak;

		semafor_v(PISZ);

	} while(znak != EOF);
}

static void * konsumuj(void * ptr)
{
	FILE* w = fopen("wynik", "w");
	char znak;
	while(1)
	{
		semafor_v(CZYTAJ);
		semafor_p(PISZ);

		znak = bufor[0];

		if(znak == EOF)
		{
			break;
		}

		printf("%s\n", bufor);

		fputc(znak, w);
	}

	fclose(w);
}

static void przygotuj_pliki()
{
	FILE* z = fopen("zrodlo", "w");
	FILE* w = fopen("wynik", "w");
	fputs("Wiadomosc\n", z);

	fclose(z);
	fclose(w);
}

static void utworz_nowy_semafor(void)
{
	semafor=semget(10,2,0777|IPC_CREAT);
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

static void ustaw_semafor(int semNum)
{
	int ustaw_sem;
	ustaw_sem=semctl(semafor,semNum,SETVAL,0);
	if (ustaw_sem==-1)
	{
		printf("Nie mozna ustawic semafora.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("%d Semafor zostal ustawiony.\n", semNum);
	}
}

static void semafor_p(int semNum)
{
	int zmien_sem;
	struct sembuf bufor_sem;
	bufor_sem.sem_num=semNum;
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
		printf("%d Semafor zostal zamkniety.\n", semNum);
	}
}

static void semafor_v(int semNum)
{
	int zmien_sem;
	struct sembuf bufor_sem;
	bufor_sem.sem_num=semNum;
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
		printf("%d Semafor zostal otwarty.\n", semNum);
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

