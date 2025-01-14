#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "My_Library/message_queue_operations.h"
#include "My_Library/shared_memory_operations.h"
#include "My_Library/semafor_operations.h"
#include "My_Library/enviromental_variables.h"




int main()
{
	int semafory_pociagu = create_semafor(".", 'C', 2, IPC_CREAT | 0600);
	
	signal_semafor(semafory_pociagu, 0, 0);
	wait_semafor(semafory_pociagu, 1, 0);

	// zapisywanie w pociagu


	return 0;
}