#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"


int nie_rozwiezieni = 1;
int nie_odjechal_wczesniej = 1;
int shared_memory_amount = Trains_Amount * ( Passengers + Bikes) + 3;
int train_driver_index = Trains_Amount * (Passengers + Bikes) + 2;
int pid_kierowcy_pociagu = 0;


int kolejka_komunikatow_zawiadowcy;
struct message Pociag_Przyjechal, Zawiadowca_Zapisal_PID, Odjechal;
int odliczacz_czasu;

void odjazd_pociagu()
{
	if (pid_kierowcy_pociagu == 0)
		return;
	else
		if (kill(pid_kierowcy_pociagu, SIGINT) == -1) 
		{
        	perror("Nie udało się wysłać sygnału");
        	exit(2005);
		}
		else 
		{
			printf("Odjazd!\n");
			send_message(kolejka_komunikatow_zawiadowcy, &Odjechal, 0);
		}
}	


void handler_przedwczesny_odjazd(int signal)
{
	nie_odjechal_wczesniej = 0;
	odjazd_pociagu();
}


int main()
{
	int train_ID = create_shared_memory(".", 'A', sizeof(int) * shared_memory_amount, IPC_CREAT | 0600 );
	int* train_memory_adress = (int*)attach_shared_memory(train_ID, NULL, 0 );


	kolejka_komunikatow_zawiadowcy = create_message_queue(".", 'C', IPC_CREAT | 0600 );
	Pociag_Przyjechal.type = 1;
	Zawiadowca_Zapisal_PID.type = 2;
	Odjechal.type = 3;

	int peron = create_shared_memory(".", 'D', sizeof(int) * 2, IPC_CREAT | 0600 );
	int* peron_memory_adress = (int*)attach_shared_memory(peron, NULL, 0);
	peron_memory_adress[Peron_Zawiadowca_Index] = getpid();	

	send_message(kolejka_komunikatow_zawiadowcy, &Zawiadowca_Zapisal_PID, 0);

	signal(SIGINT, handler_przedwczesny_odjazd);


	while (nie_rozwiezieni)
	{
		odliczacz_czasu = 0;
		nie_odjechal_wczesniej = 1;
		recive_message(kolejka_komunikatow_zawiadowcy, &Pociag_Przyjechal, 1, 0);
		pid_kierowcy_pociagu = train_memory_adress[train_driver_index];
		
		while ( odliczacz_czasu < Train_Departure_Time_Interval && nie_odjechal_wczesniej )
		{
			sleep(1);
			odliczacz_czasu++;
		}

		if (nie_odjechal_wczesniej)
			odjazd_pociagu();
	}


	return 0;
}