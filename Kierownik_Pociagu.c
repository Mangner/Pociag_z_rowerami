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
int train_number_index = Trains_Amount * (Passengers + Bikes);
int free_seat_index = Trains_Amount * (Passengers + Bikes) + 1;
int train_driver_index = Trains_Amount * (Passengers + Bikes) + 2;
int shared_memory_amount = Trains_Amount * ( Passengers + Bikes) + 3;

int kolejka_komunikatow_zawiadowcy;
struct message Pociag_Przyjechal, Zawiadowca_Zapisal_PID, Odjechal, Semafory_Ustawione, Pasazer_Usiadl;


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
	Semafory_Ustawione.type = 4;
	Pasazer_Usiadl.type = 5;

	int peron = create_shared_memory(".", 'D', sizeof(int) * 2, IPC_CREAT | 0600 );
	int* peron_memory_adress = (int*)attach_shared_memory(peron, NULL, 0);

	int train_semafors = create_semafor(".", 'B', 4, IPC_CREAT | 0600);
	initialize_semafor(train_semafors, Passengers_Entry, 0);
	initialize_semafor(train_semafors, Bikes_Entry, 0);
	initialize_semafor(train_semafors, Train_Memory_Semafor, 1);
	initialize_semafor(train_semafors, Passenger_Wants_To_Sit_Semafor, 0);

	recive_message(kolejka_komunikatow_zawiadowcy, &Zawiadowca_Zapisal_PID, 2, 0);
	send_message(kolejka_komunikatow_zawiadowcy, &Semafory_Ustawione, 0);

	printf("Kierownik Pociagu rozpoczal prace :)\n");

	signal(SIGINT, handler_sygnal_zawiadowcy);

	while (nie_rozwiezieni)
		for (int i = 0; i < Trains_Amount; i++)
		{	
			brak_sygnalu_zawiadowcy = 1;
			train_memory_adress[train_number_index] = i;
			train_memory_adress[free_seat_index] = 0;
			printf("Kierownik Pociągu: Przyjechal pociag %d\n", i);
			send_message(kolejka_komunikatow_zawiadowcy, &Pociag_Przyjechal, 0);

			while ( brak_sygnalu_zawiadowcy )
			{
				if ( wait_semafor_no_wait(train_semafors, Passenger_Wants_To_Sit_Semafor) == 0)
					continue;
				
				wait_semafor(train_semafors, Train_Memory_Semafor, 0);
				if ( train_memory_adress[free_seat_index] > Passengers )
				{
					signal_semafor(train_semafors, Train_Memory_Semafor, 0);
					break;
				}

				signal_semafor(train_semafors, Passengers_Entry, 0);
				signal_semafor(train_semafors, Train_Memory_Semafor, 0);
				recive_message(kolejka_komunikatow_zawiadowcy, &Pasazer_Usiadl, 5, 0);

			}

			if (brak_sygnalu_zawiadowcy)
				kill(peron_memory_adress[Peron_Zawiadowca_Index], SIGINT);

			recive_message(kolejka_komunikatow_zawiadowcy, &Odjechal, 3, 0);
		}


	delete_meesage_queue(kolejka_komunikatow_zawiadowcy);
	detach_shared_memory((void*)train_memory_adress, train_ID);
	free_shared_memory(train_ID);
	

	return 0;
}