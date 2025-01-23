#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"
#include <pthread.h>

struct message DoPasazerow = { .mtype = 8 };						// Komunikat dla Procesu Pasazerow ktory przekazuje PID by potem otrzymać sygnal o koncu pracy 

int losuj_zero_jeden() 
{
    int los = rand() % 100;

    if (los < 70) 
        return 1;
    else if (los >= 70)
        return 0;
}

void* collectZombie()
{
	int zombieCounter = 0;
	while (zombieCounter != MaxGeneratedPassengersAmount)
	{
		while (wait(NULL) == -1);
		zombieCounter++;
		printf("Zebrałem zombie %d.\n", zombieCounter);
	}
}

int main()
{
	FILE *plik = fopen("RaportPasazerow.txt", "w");
	if (plik == NULL)
	{
		perror("Nie można otworzyć pliku");
		exit(43);
	}
	if (fclose(plik) != 0)
	{
		perror("Nie można zamknąć pliku");
		exit(43);
	}

	srand(time(NULL));
	pthread_t zombieCollector; 
	if (pthread_create(&zombieCollector, NULL, collectZombie, NULL) != 0)
	{
		perror("pthread_create error");
		exit(3);
	}


	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	int pidyKierownikowZawiadowcy[N + 1];
	for (int i = 0; i < N + 1; i++)
	{
		recive_message(kolejowa_kolejka_komunikatow, &DoPasazerow, 8, 0);
		pid_t pid = (pid_t)strtol(DoPasazerow.content, NULL, 10);
		pidyKierownikowZawiadowcy[i] = pid;
	}


	int iterator = 0;
	int los;
	while (iterator < MaxGeneratedPassengersAmount)
	{
		los = losuj_zero_jeden();
		switch (fork())
		{
			case -1:
				perror("Fork Problem");
				system("killall -9 Pasazer Pasazer_z_Rowerem");
				exit(2);

			
			case 0:
				sleep(1);
				switch (los)
				{
					case 1:
						execl("./Pasazer", "Pasazer", NULL);
						perror("Pasazer process error");
						exit(3);
					
					case 0:
						execl("./Pasazer_z_rowerem", "Pasazer_z_rowerem", NULL);
						perror("Pasazer_z_rowerem process error");
						exit(4);
				}
		}
		iterator++;
		usleep(PassengersGenerationLatency);
	}

	if (pthread_join(zombieCollector, NULL) != 0)
	{
		perror("pthread join error");
		exit(7);
	}

	printf("Wszyscy Pasażerowie już wsiedli.\n");

	for (int i = 0; i < N + 1; i++)
	{
		kill(pidyKierownikowZawiadowcy[i], SIGIO);
	}
	
	return 0;
}