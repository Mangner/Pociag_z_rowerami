#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"

int spawning_passengers = 1;
int kolejka_komunikatow_zawiadowcy;
struct message Pociag_Przyjechal, Zawiadowca_Zapisal_PID, Odjechal, Semafory_Ustawione, Pasazer_Usiadl;

void handler_koniec_pasazerow(int signal)
{
	spawning_passengers = 0;
}

int main()
{
	int pid;

	int peron = create_shared_memory(".", 'D', sizeof(int) * 2, IPC_CREAT | 0600 );
	int* peron_memory_adress = (int*)attach_shared_memory(peron, NULL, 0);
	peron_memory_adress[Peron_Created_Passengers_Amount_Index] = 0;

	kolejka_komunikatow_zawiadowcy = create_message_queue(".", 'C', IPC_CREAT | 0600 );
	Pociag_Przyjechal.type = 1;
	Zawiadowca_Zapisal_PID.type = 2;
	Odjechal.type = 3;
	Semafory_Ustawione.type = 4;
	Pasazer_Usiadl.type = 5;


	signal(SIGINT, handler_koniec_pasazerow);

	
	recive_message(kolejka_komunikatow_zawiadowcy, &Semafory_Ustawione, 4, 0);
	
	while (spawning_passengers)
	{
		peron_memory_adress[Peron_Created_Passengers_Amount_Index] += 10;
		for (int i = 0; i < Passengers_Spawn_Amount; i++)
		{
			switch (fork())
			{
				case -1:
					perror("Fork creating passengers failed");
					exit(2137);

				case 0:
					if ( execl("./Pasazer", "Pasazer", NULL) == -1 )
					{
						perror("Execl creating passengers failed");
						exit(2138);
					}
			}
		}
		peron_memory_adress[Peron_Created_Passengers_Amount_Index] = peron_memory_adress[Peron_Created_Passengers_Amount_Index] + 10;
		printf("Obecna liczba pasazerow czekajacych na pociag: %d\n", peron_memory_adress[Peron_Created_Passengers_Amount_Index]);
		sleep(10);
	}


}