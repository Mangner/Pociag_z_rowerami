#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


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
		wait(NULL);
		zombieCounter++;
	}
}


int main()
{
	FILE *plik = fopen("RaportPasazerow.txt", "w");
	if (plik == NULL)
	{
		perror("Nie można otworzyć pliku");
		exit(1);
	}
	if (fclose(plik) != 0)
	{
		perror("Nie można zamknąć pliku");
		exit(2);
	}

	srand(time(NULL));
	pthread_t zombieCollector; 
	if (pthread_create(&zombieCollector, NULL, collectZombie, NULL) != 0)
	{
		perror("pthread_create error");
		exit(3);
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
				exit(4);
							
			case 0:
				switch (los)
				{
					case 1:
						execl("./Pasazer", "Pasazer", NULL);
						perror("Pasazer process error");
						exit(5);
					
					case 0:
						execl("./Pasazer_z_rowerem", "Pasazer_z_rowerem", NULL);
						perror("Pasazer_z_rowerem process error");
						exit(6);
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

	return 0;
}