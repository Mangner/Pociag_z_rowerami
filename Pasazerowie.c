#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue.operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"




int main()
{
	int pid;
	int spawning_passengers = 1;

	int peron = create_shared_memory(".", 'D', sizeof(int) * 2, IPC_CREAT | 0600 );
	int* peron_memory_adress = (int*)attach_shared_memory(peron, NULL, 0);
	peron_memory_adress[Peron_Created_Passengers_Amount_Index] = 0;

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
						eixt(2138);
					}
			}
		}
	}




}