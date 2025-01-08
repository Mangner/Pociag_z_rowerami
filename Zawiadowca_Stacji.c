#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int PracaTrwa = 1;					// Zmienna warunkowa podczas ktorej zawiadowca pracuje, konczy sie gdzy zostana rozwiezieni wszyscy pasazerowie

struct message PociagiGotowe, WjazdPociagu;
PociagiGotowe.type = 1;				// Proces Pociagi skonczyl dzialanie
WjazdPociagu.type = 2;				// Zawiadowca wysyla sygnal pociagowi ze moze wjechac




int main()
{
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'A', IPC_CREAT | 0600);
	recive_message(kolejowa_kolejka_komunikatow, &PociagiGotowe, 1, 0);


	printf("[%d] Zawiadowca Stacji rozpoczal prace.\n", getpid());


	while (PracaTrwa)
	{
		printf("Zawiadowca Stacji: Pociag moze wjechac!\n");
		send_message(kolejowa_kolejka_komunikatow, &WjazdPociagu, 0);


		




	}
}