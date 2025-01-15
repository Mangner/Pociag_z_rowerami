#include "semafor_operations.h"


int create_semafor(const char *path, int identifier, int semafor_amount, int flags)
{
    key_t key = ftok(path, identifier);
    if ( key == - 1)
    {
        perror("Ftok failed");
        exit(1);
    }

    int sem_id = semget(key, semafor_amount, flags);
    if (sem_id == -1)
    {
        perror("Semget failed");
        exit(2);
    }
    return sem_id;
}


void initialize_semafor(int sem_id, int semafor_number, int value)
{
    if (semctl(sem_id, semafor_number, SETVAL, value) == -1 )
    {
        perror("Semtcl Setval");
        free_semafor(sem_id);
        exit(3);
    }
}


void wait_semafor(int sem_id, int number, int flags)
{
    struct sembuf semafor_operations;
    semafor_operations.sem_num = number;
    semafor_operations.sem_op = -1;
    semafor_operations.sem_flg = flags;

    if (semop(sem_id, &semafor_operations, 1) == -1 )
    {
        perror("Semop Wait Failed");
        free_semafor(sem_id);
        exit(4);
    }
}


int wait_semafor_no_wait(int sem_id, int number)
{
	struct sembuf semafor_operations;
    semafor_operations.sem_num = number;
    semafor_operations.sem_op = -1;
    semafor_operations.sem_flg = IPC_NOWAIT;

	int operation = semop(sem_id, &semafor_operations, 1);

	if (operation == -1)
		return 0;
	else
		return 1;
}


void signal_semafor(int sem_id, int number, int flags)
{
    struct sembuf semafor_operations;
    semafor_operations.sem_num = number;
    semafor_operations.sem_op = 1;
    semafor_operations.sem_flg = flags;

    if (semop(sem_id, &semafor_operations, 1) == -1 )
    {
        perror("Semop Signal Failed");
        free_semafor(sem_id);
        exit(4);
    }
}


void free_semafor(int sem_id)
{
	if (semctl(sem_id, 0, GETVAL) == -1 ) 
	{  
		return; 
	}

    if (semctl(sem_id, 69, IPC_RMID) == -1 )
    {
        perror("Semtcl IPC_RMID error");
        exit(6);
    }
    printf("The set of semafors has been deleted.\n");
}