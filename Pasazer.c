#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int kolejka_komunikatow_zawiadowcy;
struct message Pociag_Przyjechal, Zawiadowca_Zapisal_PID, Odjechal, Semafory_Ustawione, Pasazer_Usiadl;

int free_seat_index = Trains_Amount * (Passengers + Bikes) + 1;
int train_driver_index = Trains_Amount * (Passengers + Bikes) + 2;
int shared_memory_amount = Trains_Amount * ( Passengers + Bikes) + 3;
int train_number_index = Trains_Amount * (Passengers + Bikes);


int main()
{
	int train_ID = create_shared_memory(".", 'A', sizeof(int) * shared_memory_amount, IPC_CREAT | 0600 );
	int* train_memory_adress = (int*)attach_shared_memory(train_ID, NULL, 0 );

	printf("Żyje %d.\n",getpid());

	kolejka_komunikatow_zawiadowcy = create_message_queue(".", 'C', IPC_CREAT | 0600 );
	Pociag_Przyjechal.type = 1;
	Zawiadowca_Zapisal_PID.type = 2;
	Odjechal.type = 3;
	Semafory_Ustawione.type = 4;
	Pasazer_Usiadl.type = 5;

	int train_semafors = create_semafor(".", 'B', 4, IPC_CREAT | 0600);


	signal_semafor(train_semafors, Passenger_Wants_To_Sit_Semafor, 0);
	
	wait_semafor(train_semafors, Passengers_Entry, 0);
	printf("Pasazer %d wszedl do pociagu.\n",getpid());
	wait_semafor(train_semafors, Train_Memory_Semafor, 0);

	int my_seat_index = train_memory_adress[free_seat_index];
	train_memory_adress[my_seat_index] = getpid();
	train_memory_adress[free_seat_index] = train_memory_adress[free_seat_index] + 1;

	printf("Pasazer %d usiadl.\n", getpid());
	send_message(kolejka_komunikatow_zawiadowcy, &Pasazer_Usiadl, 0);

	signal_semafor(train_semafors, Train_Memory_Semafor, 0);


}