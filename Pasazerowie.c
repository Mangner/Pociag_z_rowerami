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


int generate_passengers = 1;


void end_generation_handler(int signal)
{
	generate_passengers = 0;
}

int losuj_zero_jeden() 
{
    int los = rand() % 100;

    if (los < 70) 
        return 1;
    else 
        return 0;
}


int main()
{
	srand(time(NULL));
	if (signal(SIGUSR1, end_generation_handler) == SIG_ERR )
	{
        perror("Nie można ustawić handlera dla SIGUSR1");
        exit(1);
    }


	int iterator = 0;
	while (generate_passengers || iterator >= MaxGeneratedPassengersAmount)
	{
		switch (fork())
		{
			case -1:
				perror("Fork Problem");
				exit(2);
			
			case 0:
				switch (losuj_zero_jeden())
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
		sleep(PassengersGenerationLatency);
		iterator++;
	}

	while (iterator > 0)
	{
		wait(NULL);
		iterator--;
	}

	return 0;
}