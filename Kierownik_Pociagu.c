#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int nie_rozwiezieni = 1;
int brak_sygnalu_zawiadowcy;
int shared_memory_amount = Trains_Amount * ( Passengers + Bikes) + 3;
int train_number_index = Trains_Amount * (Passengers + Bikes);
int free_seet_index = Trains_Amount * (Passengers + Bikes) + 1;
int train_driver_index = Trains_Amount * (Passengers + Bikes) + 2;

int kolejka_komunikatow_zawiadowcy;
struct message Pociag_Przyjechal, Zawiadowca_Zapisal_PID, Odjechal;


void handler_sigint_rozwiezieni(int singal)
{
	nie_rozwiezieni = 0;
}


void handler_sygnal_zawiadowcy(int singal)
{
	brak_sygnalu_zawiadowcy = 0;
}

int main()
{
	int train_ID = create_shared_memory(".", 'A', sizeof(int) * shared_memory_amount, IPC_CREAT | 0600 );
	int* train_memory_adress = (int*)attach_shared_memory(train_ID, NULL, 0 );
	train_memory_adress[train_driver_index] = getpid();

	kolejka_komunikatow_zawiadowcy = create_message_queue(".", 'C', IPC_CREAT | 0600 );
	Pociag_Przyjechal.type = 1;
	Zawiadowca_Zapisal_PID.type = 2;
	Odjechal.type = 3;

	int peron = create_shared_memory(".", 'D', sizeof(int) * 2, IPC_CREAT | 0600 );
	int* peron_memory_adress = (int*)attach_shared_memory(peron, NULL, 0);


	recive_message(kolejka_komunikatow_zawiadowcy, &Zawiadowca_Zapisal_PID, 2, 0);

	signal(SIGINT, handler_sygnal_zawiadowcy);

	while (nie_rozwiezieni)
		for (int i = 0; i < Trains_Amount; i++)
		{	
			brak_sygnalu_zawiadowcy = 1;
			train_memory_adress[train_number_index] = i;
			train_memory_adress[free_seet_index] = 0;
			printf("Przyjechal pociag %d\n", i);
			send_message(kolejka_komunikatow_zawiadowcy, &Pociag_Przyjechal, 0);

			while (train_memory_adress[free_seet_index] < Passengers && brak_sygnalu_zawiadowcy)
			{
	
			}

			if (brak_sygnalu_zawiadowcy)
				kill(SIGINT, peron_memory_adress[Peron_Zawiadowca_Index]);

			recive_message(kolejka_komunikatow_zawiadowcy, &Odjechal, 3, 0);
		}


	delete_meesage_queue(kolejka_komunikatow_zawiadowcy);
	detach_shared_memory((void*)train_memory_adress, train_ID);
	free_shared_memory(train_ID);
	

	return 0;
}