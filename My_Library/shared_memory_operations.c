#include "shared_memory_operations.h"


int create_shared_memory(const char *path, int identifier, size_t memory_amount, int flags)
{
	key_t key = ftok(path, identifier);
	if ( key == - 1)
    {
        perror("Ftok failed");
        exit(1);
    }

	int shm_id = shmget(key, memory_amount, flags);
	if (shm_id == -1)
    {
        perror("Shmget failed");
        exit(2);
    }

	return shm_id;
}


void* attach_shared_memory(int shm_id, const void* shm_adress, int flags)
{	
	void* shared_memory_adress = shmat(shm_id, shm_adress, flags);
	if (*(int*)shared_memory_adress == -1 )
	{
		perror("Shmat failed");
		free_shared_memory(shm_id);
		exit(2);
	}
	return shared_memory_adress;
}


void detach_shared_memory(const void* shm_adress, int shm_id)
{
	int result = shmdt(shm_adress);
	if ( result == -1 )
	{
		perror("Shmdt failed");
		free_shared_memory(shm_id);
		exit(3);
	}
}


void free_shared_memory(int shm_id)
{
	if (shmctl(shm_id, IPC_RMID, NULL) == -1 )
	{
		perror("Shmctl IPC_RMID error");
		exit(4);
	}
}
