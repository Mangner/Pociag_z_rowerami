#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/errno.h>


int create_semafor(const char *path, int identifier, int semafor_amount, int flags);
void initialize_semafor(int sem_id, int semafor_number, int value);
void wait_semafor(int sem_id, int number, int flags);
int wait_semafor_no_wait(int sem_id, int number);
void signal_semafor(int sem_id, int number, int flags);
void free_semafor(int sem_id);
int how_many_waiting_processes_on_semafor(int sem_id, int number);
int isSemaphoreLowered(int semid, int semnum);