#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


struct message PociagiGotowe = { .mtype = 1 };				// Proces Pociagi skonczyl dzialanie

int main()
{
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);

	for (int i = 0; i < N; i++)
	{
		switch(fork())
		{
			case -1:
				perror("Fork failed");
				exit(1);
			
			case 0:
				execl("./Kierownik_Pociagu", "Kierownik_Pociagu", NULL);
				perror("Execl failed");
				exit(2);
		}
	}

	send_message(kolejowa_kolejka_komunikatow, &PociagiGotowe, 0);
	return 0;
}