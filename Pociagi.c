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
	
	int pid_pociagu;
	pid_t parent_pid = getpid();
	if (setpgid(parent_pid, parent_pid) == -1) 
	{
    	perror("setpgid (parent) failed");
    	exit(1);
	}

	for (int i = 0; i < N; i++)
	{
		pid_pociagu = fork();
		switch(pid_pociagu)
		{
			case -1:
				perror("Fork failed");
				exit(2);
			
			case 0:
				if (setpgid(0, parent_pid) == -1) 
				{
                    perror("setpgid (child) failed");
                    exit(3);
                }
				execl("./Kierownik_Pociagu", "Kierownik_Pociagu", NULL);
				perror("Execl failed");
				exit(4);
		}
	}

	PociagiGotowe.pid_grupy = (int)getpgrp();
	send_message(kolejowa_kolejka_komunikatow, &PociagiGotowe, 0);
	
	return 0; 
}