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
int BlokowanieOdblokowaniePeronu = 1;	// Zmienna warunkowo dzięki której zawiadowca na przemian blokuje i odblokowuje peron 

struct message PociagiGotowe = { .mtype = 1 }; 				// Proces Pociagi skonczyl dzialanie
struct message WjazdPociagu = { .mtype = 2 };				// Zawiadowca wysyla sygnal pociagowi ze moze wjechac
struct message PociagWjechal = { .mtype = 3 };				// Komunikat dla Zawiadowcy Stacji że pociąg wjechał na peron
struct message PociagOdjechal = { .mtype = 4 };				// Komunikat dla Zawiadowcy Stacji że pociąg opuścił peron
struct message DoPasazerow = { .mtype = 8 };				// Komunikat dla Procesu Pasazerow ktory przekazuje PID by potem otrzymać sygnal o koncu pracy 


void odjazdPociagu()
{
	pid_t pid = (pid_t)strtol(PociagWjechal.content, NULL, 10);
	if (kill(pid, SIGUSR1) == -1) 
    	perror("Nie udało się wysłać sygnału");
	
	PociagNieOdjechal = 0;
}

void signalJedenZawiadowcy_handler(int signal)
{
	printf("\033[1;31m[%d] Zawiadowca Stacji: Sygnał 1 - szybszy odjazd.\033[0m\n", getpid());
	odjazdPociagu();
}


void signalDwaZawiadowcy_handler(int signal)
{
    if (BlokowanieOdblokowaniePeronu)
        printf("\033[1;31m[%d] Zawiadowca Stacji: Sygnał 2 - nikt nie może już wejść.\033[0m\n", getpid());
    else
        printf("\033[1;31m[%d] Zawiadowca Stacji: Sygnał 2 - znowu mogą wchodzić.\033[0m\n", getpid());

    BlokowanieOdblokowaniePeronu = !(BlokowanieOdblokowaniePeronu);
	kill(-(PociagiGotowe.pid_grupy), SIGUSR2);
}

void koniecPracy_handler(int signal)
{
	PracaTrwa = 0; 
	odjazdPociagu();
}

int main()
{
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	snprintf(WjazdPociagu.content, sizeof(WjazdPociagu.content), "%d", getpid());
	snprintf(DoPasazerow.content, sizeof(DoPasazerow.content), "%d", getpid());	
	send_message(kolejowa_kolejka_komunikatow, &DoPasazerow, 0);
	recive_message(kolejowa_kolejka_komunikatow, &PociagiGotowe, 1, 0);	

	printf("\033[1;31m[%d] Zawiadowca Stacji rozpoczal prace.\033[0m\n", getpid());

	signal(SIGUSR1, signalJedenZawiadowcy_handler);
	signal(SIGUSR2, signalDwaZawiadowcy_handler);
	signal(SIGIO, koniecPracy_handler);


	while (PracaTrwa)
	{
		printf("\033[1;31m[%d] Zawiadowca Stacji: Pociag moze wjechac!\033[0m\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 0);
		while (recive_message(kolejowa_kolejka_komunikatow, &PociagWjechal, 3, 0))
			continue;

		PociagNieOdjechal = 1;
		
		while (PociagNieOdjechal)
		{
			for (int i = 0; i < T; i++)
			{
				if (!PociagNieOdjechal)
				{
					break;
				}
				sleep(1);
			}

			if (PociagNieOdjechal)
				odjazdPociagu();
		}
	
		while(recive_message(kolejowa_kolejka_komunikatow, &PociagOdjechal, 4, 0))
			continue;
		printf("\033[1;31m[%d] Zawiadowca Stacji: Dobra odjechał, następny.\033[0m\n", getpid());
	}

	printf("\033[1;31m[%d] Zawiadowca Stacji: Kończę pracę na dziś.\033[0m\n", getpid());
}