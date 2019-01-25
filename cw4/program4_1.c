#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <wait.h>


#define PISZ 0
#define CZYTAJ 1
#define TRUE 1
#define FALSE 0

int semafor;
int pamiec;
char *adres;

static void utworz_nowy_semafor(void);
static void ustaw_semafor(int semNum);
static void semafor_p(int semNum);
static void semafor_v(int semNum);
static void usun_semafor(void);
static void produkuj();
static void konsumuj();
static void przygotuj_pliki();

static void pamiecStart()
{
    pamiec = shmget(9, 256, 0777|IPC_CREAT);
    if(pamiec == -1)
	{
		printf("Nie udalo sie utworzyc pamieci dzielonej\n");
		exit(1);
	}
	else
	{
		printf("Pamiec dzielona zostala utworzona, %d \n", pamiec);
	}
}

static void adresStart()
{
	adres = shmat(pamiec, 0, 0);
    if(*adres == -1)
	{
		printf("Nie udalo sie dolaczyc adresu\n");
		exit(1);
	}
	else
	{
		printf("Adres zostal dolaczony\n");
	}
}

static void adresStop(int fail)
{
    int status = shmdt(adres);
    if(status == -1 && fail != 0)
	{
		printf("Nie udalo sie odlaczyc adresu\n");
		exit(1);
	}
	else
	{
		printf("Adres zostal odlaczony\n");
	}
}

static void pamiecStop(int fail)
{
    int status = shmctl(pamiec, IPC_RMID, 0);
    if(status == -1 && fail != 0)
	{
		printf("Nie udalo sie odlaczyc pamieci dzielonej\n");
		exit(1);
	}
	else
	{
		printf("Pamiec dzielona zostala odlaczona\n");
	}
}

int main(int argc, char **argv)
{
	pamiecStop(FALSE);

	utworz_nowy_semafor();

	pamiecStart();
	adresStart();

	ustaw_semafor(PISZ);
	ustaw_semafor(CZYTAJ);

	przygotuj_pliki();

	switch(fork())
	{
		case -1:
			printf("ERROR ON FORK\n");
			exit(1);
			break;
		case 0:
			konsumuj();
			break;
		default:
			produkuj();
			{
				int status, w;
				w = wait(&status);

				pamiecStop(TRUE);
				adresStop(TRUE);
			}
			break;
	}
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
		
		adres[0] = znak;

		semafor_v(PISZ);

	} while(znak != EOF);
}

static void konsumuj()
{
	FILE* w = fopen("wynik", "w");
	char znak;
	while(1)
	{
		semafor_v(CZYTAJ);
		semafor_p(PISZ);

		znak = *adres;

		if(znak == EOF)
		{
			break;
		}

		printf("%c\n", *adres);

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

