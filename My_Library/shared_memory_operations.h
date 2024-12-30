#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>


int create_shared_memory(const char *path, int identifier, size_t memory_amount, int flags);
void* attach_shared_memory(int shm_id, const void* shm_adress, int flags);
void detach_shared_memory(const void* shm_adress, int shm_id);
void free_shared_memory(int shm_id);
