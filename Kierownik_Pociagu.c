#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int PracaTrwa = 1;					// Zmienna warunkowa podczas ktorej kierownik pociagu pracuje, konczy sie gdzy zostana rozwiezieni wszyscy pasazerowie
int PociagNieOdjechal = 0;			// Zmienna warunkowa podczas której pociąg stoi na peronie
int PasazerowieMogaWchodzic = 0;	// Zmienna warunkowa podczas której pasażerowie mogą wchodzić do pociągu, zmienia się po sygnale 2 zawiadowcy



struct message WjazdPociagu = { .mtype = 2 };						// Kierownik czeka na sygnal Zawiadowcy ze moze wjechac
struct message PociagWjechal = { .mtype = 3 };						// Komunikat dla Zawiadowcy Stacji że pociąg wjechał na peron
struct message PociagOdjechal = { .mtype = 4 };						// Komunikat dla Zawiadowcy Stacji że pociąg opuścił peron
struct message RodzajPasazera = { .mtype = 5 };						// Komunikat dla Kierownika od Pasażera który mówi czy pasażer ma rower czy nie
struct message LosPasazera = { .mtype = 6 };						// Komunikat dla pasażera który mówi czy wchodzi czy wraca do kolejki na peronie 
struct message KoniecPasazera = { .mtype = 7 };						// Komunikat dla Kierowcy Pociągu że ten proces pasażerski się wpisał albo wrócił do kolejki 


void odjazdPociagu_handler(int signal)
{
	printf("[%d] Kierownik Pociagu: Otrzymalem Sygnal SIGURS1.\n", getpid());
	PociagNieOdjechal = 0;
}	

int main()
{
	printf("[%d] Kierownik Pociagu: Pociag gotowy do pracy.\n", getpid());
	
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	snprintf(PociagWjechal.content, sizeof(PociagWjechal.content), "%d", getpid());			// Treśc tego komunikatu to pid tego procesu żeby Zawiadowca mógł mu wysyłać sygnały

	int semafory_pociagu = create_semafor(".", 'C', 4, IPC_CREAT | 0600);					// Semafory pociagu
	initialize_semafor(semafory_pociagu, 0, 1);												// Semafor ktory podnosi się gdy pasażerowie mogą wchodzić
	initialize_semafor(semafory_pociagu, 1, 0);												// Semafor który podnosci się gdy jakiś pasażer chce wejść
	initialize_semafor(semafory_pociagu, 2, 0);												// Semafor odpowiadający za kontrole pasażera czyli sprawdzanie miejsca dla niego

	size_t rozmiar_pamieci_pociagu = P + R + 2;
	int IndexWolnegoMiejsca = P + R;
	int IndexWolnegoMiejscaRowerowego = P + R + 1;

	int shm_ID = create_shared_memory(".", 'B', sizeof(int) * rozmiar_pamieci_pociagu, IPC_CREAT | 0600);
	int* pamiec_dzielona_pociagu = (int*)attach_shared_memory(shm_ID, NULL, 0);

	signal(SIGUSR1, odjazdPociagu_handler);

	while (PracaTrwa)
	{
		recive_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 2, 0);
		printf("[%d] Kierownik Pociagu: Pociag wjechal na peron!\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &PociagWjechal, 0);

		PociagNieOdjechal = 1;
		PasazerowieMogaWchodzic = 1;
		pamiec_dzielona_pociagu[IndexWolnegoMiejsca] = 0;
		pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego] = 0;



		while (PociagNieOdjechal)
		{
			
		}

		printf("[%d] Kierownik Pociągu: Odjazd!\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &PociagOdjechal, 0);

	}

	free_semafor(semafory_pociagu);
	detach_shared_memory(pamiec_dzielona_pociagu, shm_ID);
	free_shared_memory(shm_ID);

	return 0;
}