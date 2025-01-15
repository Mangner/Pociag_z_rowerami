#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int PracaTrwa = 1;					// Zmienna warunkowa podczas ktorej zawiadowca pracuje, konczy sie gdzy zostana rozwiezieni wszyscy pasazerowie
int PociagNieOdjechal = 0;			// Zmienna warunkowa podczas ktorej zawiadowca czeka czas T by nastpenie rozkazać pociągowi odjazd

struct message PociagiGotowe = { .mtype = 1 }; 				// Proces Pociagi skonczyl dzialanie
struct message WjazdPociagu = { .mtype = 2 };				// Zawiadowca wysyla sygnal pociagowi ze moze wjechac
struct message PociagWjechal = { .mtype = 3 };				// Komunikat dla Zawiadowcy Stacji że pociąg wjechał na peron
struct message PociagOdjechal = { .mtype = 4 };				// Komunikat dla Zawiadowcy Stacji że pociąg opuścił peron


void odjazdPociagu()
{
	pid_t pid = (pid_t)strtol(PociagWjechal.content, NULL, 10);
	if (kill(pid, SIGUSR1) == -1) 
    	perror("Nie udało się wysłać sygnału");
	PociagNieOdjechal = 0;
}

void signalJedenZawiadowcy_handler(int signal)
{
	printf("[%d] Zawiadowca Stacji: Sygnał 1 - szybszy odjzad.\n", getpid());
	odjazdPociagu();
}

void signalDwaZawiadowcy_handler(int signal)
{
	printf("[%d] Zawiadowca Stacji: Sygnał 2 - nikt nie może już wejść.\n");
	pid_t pid = (pid_t)strtol(PociagWjechal.content, NULL, 10);
	if (kill(pid, SIGUSR2) == -1) 
    	perror("Nie udało się wysłać sygnału");
}


int main()
{
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	recive_message(kolejowa_kolejka_komunikatow, &PociagiGotowe, 1, 0);

 
	printf("[%d] Zawiadowca Stacji rozpoczal prace.\n", getpid());

	signal(SIGUSR1, signalJedenZawiadowcy_handler);
	signal(SIGUSR2, signalDwaZawiadowcy_handler);

	while (PracaTrwa)
	{
		printf("[%d] Zawiadowca Stacji: Pociag moze wjechac!\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 0);
		recive_message(kolejowa_kolejka_komunikatow, &PociagWjechal, 3, 0);
		PociagNieOdjechal = 1;
		
		while (PociagNieOdjechal)
		{
			for (int i = 0; i < T; i++)
			{
				if (!PociagNieOdjechal)
				{
					printf("Przerwal petle.\n");
					break;
				}
				sleep(1);
			}

			if (PociagNieOdjechal)
				odjazdPociagu();
		}

		recive_message(kolejowa_kolejka_komunikatow, &PociagOdjechal, 4, 0);
		printf("[%d] Zawiadowca Stacji: Dobra odjechał następny.\n", getpid());

	}

	delete_meesage_queue(kolejowa_kolejka_komunikatow);
}