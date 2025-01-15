#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"




struct message RodzajPasazera = { .mtype = 5 };					// Komunikat dla Kierownika od Pasażera który mówi czy pasażer ma rower czy nie
struct message LosPasazera = { .mtype = 6 };						// Komunikat dla pasażera który mówi czy wchodzi do pociągu czy wraca do kolejki na peronie 
struct message KoniecPasazera = { .mtype = 7 };						// Komunikat dla Kierowcy Pociągu że ten proces pasażerski się wpisał albo wrócił do kolejki 

int main()
{
	/*
	int kolejowa_kolejka_komunikatow = create_message_queue(".", 'H', IPC_CREAT | 0600);
	snprintf(RodzajPasazera.content, sizeof(RodzajPasazera.content), "%s", "Z rowerem");		// Zapisuje że pasażer nie ma roweru

	size_t rozmiar_pamieci_pociagu = P + R + 2;
	int IndexWolnegoMiejsca = P + R;
	int IndexWolnegoMiejscaRowerowego = P + R + 1;
	int shm_ID = create_shared_memory(".", 'B', sizeof(int) * rozmiar_pamieci_pociagu, IPC_CREAT | 0600);
	int* pamiec_dzielona_pociagu = (int*)attach_shared_memory(shm_ID, NULL, 0);
	
	int semafory_pociagu = create_semafor(".", 'C', 3, IPC_CREAT | 0600);

	wait_semafor(semafory_pociagu, 0, 0);							// Czeka czy pasażerowie mogą wejść
	signal_semafor(semafory_pociagu, 1, 0);							// Pasażer wyraża chęć że chce wejść
	wait_semafor(semafory_pociagu, 2, 0);							// Pasażer czeka na kontrole 
	
	send_message(kolejowa_kolejka_komunikatow, &RodzajPasazera, 0);			// Informuje ze jest pasazerem bez roweru
	recive_message(kolejowa_kolejka_komunikatow, &LosPasazera, 6, 0);		// Dostaje wiadomosc czy wchodzi czy wraca do kolejki

	if (strcmp(LosPasazera.content, "Wchodz do Pociagu") == 0)
	{
		int moje_miejsce = pamiec_dzielona_pociagu[IndexWolnegoMiejsca];
		int miejsce_mojego_rowera = pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego];
		pamiec_dzielona_pociagu[moje_miejsce] = getpid();
		pamiec_dzielona_pociagu[miejsce_mojego_rowera] = getpid();
		pamiec_dzielona_pociagu[IndexWolnegoMiejsca] = pamiec_dzielona_pociagu[IndexWolnegoMiejsca] + 1;
		pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego] = pamiec_dzielona_pociagu[IndexWolnegoMiejscaRowerowego] + 1;
		printf("[%d] Pasazer z Rowerem: Usiadłem i mój rower też.\n", getpid()); 
		send_message(kolejowa_kolejka_komunikatow, &KoniecPasazera, 0);
	}
	else
	{
		printf("[%d] Pasazer z Rowerem: Nie ma miejsca dla mnie i mojego rowerku :( ,wracam do kolejki.\n", getpid());
		send_message(kolejowa_kolejka_komunikatow, &KoniecPasazera, 0);
		execl("./Pasazer_z_rowerem", "Pasazer_z_rowerem", NULL);
	}
	*/
	return 0;
}