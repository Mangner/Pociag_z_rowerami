#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int PracaTrwa = 1;						// Zmienna warunkowa podczas ktorej kierownik pociagu pracuje, konczy sie gdzy zostana rozwiezieni wszyscy pasazerowie
int PociagNieOdjechal = 0;				// Zmienna warunkowa podczas której pociąg stoi na peronie
int PasazerowieMogaWchodzic = 1;		// Zmienna warunkowa podczas której pasażerowie mogą wchodzić do pociągu, zmienia się po sygnale 2 zawiadowcy
int CzyRowerzyszciMogaWchodzic = 1;		// Zmienna ktora mowi czy jakis rowerzysta czeka na przyjazd kolejnego pociągu z miejscami rowerowymi

int semafory_pociagu;

struct message WjazdPociagu = { .mtype = 2 };						// Kierownik czeka na sygnal Zawiadowcy ze moze wjechac
struct message PociagWjechal = { .mtype = 3 };						// Komunikat dla Zawiadowcy Stacji że pociąg wjechał na peron
struct message PociagOdjechal = { .mtype = 4 };						// Komunikat dla Zawiadowcy Stacji że pociąg opuścił peron
struct message RodzajPasazera = { .mtype = 5 };						// Komunikat dla Kierownika od Pasażera który mówi czy pasażer ma rower czy nie
struct message LosPasazera = { .mtype = 6 };						// Komunikat dla pasażera który mówi czy wchodzi czy wraca do kolejki na peronie 
struct message KoniecPasazera = { .mtype = 7 };						// Komunikat dla Kierowcy Pociągu że ten proces pasażerski się wpisał albo wrócił do kolejki 
struct message DoPasazerow = { .mtype = 8 };						// Komunikat dla Procesu Pasazerow ktory przekazuje PID by potem otrzymać sygnal o koncu pracy 


void sygnalZawiadowcyJeden_handler(int signal)
{
	if (PociagNieOdjechal)
	{
		printf("\033[1;34m[%d] Kierownik Pociagu: Otrzymalem Sygnal SIGUSR1.\033[0m\n", getpid());
		PociagNieOdjechal = 0;
	}
}	

void sygnalZawiadowcyDwa_handler(int signal)
{
	printf("\033[1;34m[%d] Kierownik Pociagu: Otrzymalem Sygnal SIGUSR2.\033[0m\n", getpid());
	if (PasazerowieMogaWchodzic)
		PasazerowieMogaWchodzic = 0;
	else
	{
		PasazerowieMogaWchodzic = 1;
		if (isSemaphoreLowered(semafory_pociagu, 0))
			signal_semafor(semafory_pociagu, 0, 0);
	}
}
 
void koniecPracy_handler(int signal)
{
	PracaTrwa = 0;
}

int main()
{
	printf("\033[1;34m[%d] Kierownik Pociagu: Pociag gotowy do pracy.\033[0m\n", getpid());
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	snprintf(PociagWjechal.content, sizeof(PociagWjechal.content), "%d", getpid());			// Treśc tego komunikatu to pid tego procesu żeby Zawiadowca mógł mu wysyłać sygnały
	snprintf(DoPasazerow.content, sizeof(DoPasazerow.content), "%d", getpid());				// Potem pasazerowie jak sie skoncza wysla sygnlal by skonczyli prace 

	send_message(kolejowa_kolejka_komunikatow, &DoPasazerow, 0);

	semafory_pociagu = create_semafor(".", 'C', 6, IPC_CREAT | 0600);					// Semafory pociagu
	initialize_semafor(semafory_pociagu, 0, 1);												// Semafor ktory podnosi się gdy pasażerowie mogą wchodzić
	initialize_semafor(semafory_pociagu, 1, 0);												// Semafor który podnosci się gdy jakiś pasażer chce wejść
	initialize_semafor(semafory_pociagu, 2, 0);												// Semafor odpowiadający za kontrole pasażera czyli sprawdzanie miejsca dla niego
	initialize_semafor(semafory_pociagu, 3, 1);												// Semafor specjalny ktory kaze rowerzystom czekac jezeli nie ma miejsc na rowery
	initialize_semafor(semafory_pociagu, 4, 1);												// Semafor służący do synchronizacji zapisu przebiegu kursów , kto wsiadl
	initialize_semafor(semafory_pociagu, 5, N - 1);											// Semafor służacy do tego by tylko jeden proces zwalniał semafory itp.


	size_t rozmiar_pamieci_pociagu = P + R + 3;
	int IndexWolnegoMiejsca = P + R;
	int IndexWolnegoMiejscaRowerowego = P + R + 1;
	int IndexNumeruKursu = P + R + 2;
	
	int shm_ID = create_shared_memory(".", 'B', sizeof(int) * rozmiar_pamieci_pociagu, IPC_CREAT | 0600);
	int* pamiec_dzielona_pociagu = (int*)attach_shared_memory(shm_ID, NULL, 0);

	pamiec_dzielona_pociagu[IndexNumeruKursu] = 1;

	signal(SIGUSR1, sygnalZawiadowcyJeden_handler);
	signal(SIGUSR2, sygnalZawiadowcyDwa_handler);
	signal(SIGIO, koniecPracy_handler);


	while (PracaTrwa)
	{
		if (recive_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 2, 0))
		{
			if (!PracaTrwa)
				break;
			continue;
		}
		printf("\033[1;34m[%d] Kierownik Pociagu: Pociag wjechal na peron!\033[0m\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &PociagWjechal, 0);

		PociagNieOdjechal = 1;
		PasazerowieMogaWchodzic = 1;
		CzyRowerzyszciMogaWchodzic = 1;
		pamiec_dzielona_pociagu[IndexWolnegoMiejsca] = 0;
		pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego] = 0;

		if (isSemaphoreLowered)
			signal_semafor(semafory_pociagu, 3, 0);

		while (PociagNieOdjechal)
		{
			if (wait_semafor_no_wait(semafory_pociagu, 1))
			{
				signal_semafor(semafory_pociagu, 2, 0);
				while(recive_message(kolejowa_kolejka_komunikatow, &RodzajPasazera, 5, 0))
					continue;
				

				if (pamiec_dzielona_pociagu[IndexWolnegoMiejsca] >= P)
					{
						snprintf(LosPasazera.content, sizeof(LosPasazera.content), "%s", "Wracaj do Kolejki");
						send_message(kolejowa_kolejka_komunikatow, &LosPasazera, 0);
						pid_t pid = (pid_t)strtol(WjazdPociagu.content, NULL, 10);
						if (kill(pid, SIGUSR1) == -1) 
    						perror("Nie udało się wysłać sygnału");
						
						while (PociagNieOdjechal)
							pause();

					}

				else
				{
					if (strcmp(RodzajPasazera.content,"Z rowerem") == 0)
					{
						if (pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego] >= R)
						{
							snprintf(LosPasazera.content, sizeof(LosPasazera.content), "%s", "Wracaj do Kolejki");
							send_message(kolejowa_kolejka_komunikatow, &LosPasazera, 0);
							CzyRowerzyszciMogaWchodzic = 0;

						}
						else
						{
							snprintf(LosPasazera.content, sizeof(LosPasazera.content), "%s", "Wchodz do Pociagu");
							send_message(kolejowa_kolejka_komunikatow, &LosPasazera, 0);	
						}
					}
					else if (strcmp(RodzajPasazera.content, "Bez Rowera") == 0)
					{
						snprintf(LosPasazera.content, sizeof(LosPasazera.content), "%s", "Wchodz do Pociagu");
						send_message(kolejowa_kolejka_komunikatow, &LosPasazera, 0);						
					}
				}
				while(recive_message(kolejowa_kolejka_komunikatow, &KoniecPasazera, 7, 0))
					continue;
		
				printf("\033[1;34m[%d] Kierownik Pociągu: Proszę kolejny wsiadać!\033[0m\n", getpid());


				if (PasazerowieMogaWchodzic)
					signal_semafor(semafory_pociagu, 0, 0);
					

				if (CzyRowerzyszciMogaWchodzic)
					signal_semafor(semafory_pociagu, 3, 0);

			}
		}

	
		while(wait_semafor(semafory_pociagu, 4, 0))
			continue;
		

		FILE *plikPasazerow = fopen("RaportPasazerow.txt", "a");
		if (plikPasazerow == NULL)
		{
			perror("Nie można otworzyć pliku\n");
			exit(2137);
		}

		fprintf(plikPasazerow, "Pasazerowie kursu nr: %d\n", (int)pamiec_dzielona_pociagu[IndexNumeruKursu]);
		for (int ITERATOR = 0; ITERATOR < pamiec_dzielona_pociagu[IndexWolnegoMiejsca]; ITERATOR++)
			fprintf(plikPasazerow, "%d ", (int)pamiec_dzielona_pociagu[ITERATOR]);
		fprintf(plikPasazerow, "\n");
		fprintf(plikPasazerow, "Rowery pasażerów kursu nr: %d\n", (int)pamiec_dzielona_pociagu[IndexNumeruKursu]);
		for (int x = P; x < P + pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego]; x++)
			fprintf(plikPasazerow, "%d ", (int)pamiec_dzielona_pociagu[x]);
		fprintf(plikPasazerow, "\n");


		if (fclose(plikPasazerow) != 0)
		{
			perror("Nie można zamknąc pliku.\n");
			exit(2138);
		}
		

		signal_semafor(semafory_pociagu, 4, 0);

		printf("\033[1;34m[%d] Kierownik Pociągu: Odjazd!\033[0m\n", getpid());
		pamiec_dzielona_pociagu[IndexNumeruKursu] = pamiec_dzielona_pociagu[IndexNumeruKursu] + 1;
		send_message(kolejowa_kolejka_komunikatow, &PociagOdjechal, 0);

		for (int i = 0; i < T1; i++)
			sleep(1);

		printf("\033[1;34m[%d] Kierownik Pociagu: Pociag rozwiozl pasazerow.\033[0m\n", getpid());
	}

	printf("\033[1;34m[%d] Kierownik Pociagu: Koncze prace.\033[0m\n", getpid());

	if (wait_semafor_no_wait(semafory_pociagu, 5) == 0)
	{
		free_semafor(semafory_pociagu);
		detach_shared_memory(pamiec_dzielona_pociagu, shm_ID);
		free_shared_memory(shm_ID);
		delete_meesage_queue(kolejowa_kolejka_komunikatow);
	}
	return 0;
}