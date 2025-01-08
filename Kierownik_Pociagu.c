#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int PracaTrwa = 1;					// Zmienna warunkowa podczas ktorej kierownik pociagu pracuje, konczy sie gdzy zostana rozwiezieni wszyscy pasazerowie

struct message WjazdPociagu;
WjazdPociagu.type = 2;				// Kierownik czeka na sygnal Zawiadowcy ze moze wjechac

int brak_odjazdu = 1;				

int main()
{
	printf("[%d] Kierownik Pociagu: Pociag gotowy do pracy.\n", getpi());

	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'A', IPC_CREAT | 0600);
	
	size_t rozmiar_pamieci_pociagu = P + R + 2;
	int IndexWolnegoMiejsca = P + R;
	int IndexWolnegoMiejscaRowerowego = P + R + 1;

	int shm_ID = create_shared_memory(".", 'B', IPC_CREAT | 0600);
	int* pamiec_dzielona_pociagu = (int*)attach_shared_memory(shm_ID, NULL, 0);

	
	while (PracaTrwa)
	{
		recive_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 2, 0);
		printf("[%d] Kierownik Pociagu: Pociag wjechal na peron!\n");

		while (brak_odjazdu && pamiec_dzielona_pociagu[IndexWolnegoMiejsca] < P)
		{
			// czekaj na wsiadanie pasazerow
		}


	}

	detach_shared_memory(pamiec_dzielona_pociagu, shm_ID);
	free_shared_memory((void*)shm_ID);

	return;
}