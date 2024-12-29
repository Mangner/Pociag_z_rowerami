#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "My_Library/message_queue.operations.h"
#include "My_Library/shared_memory_operations.h"
#define Trains_Amount 4
#define Passengers 10
#define Bikes 5
#define Time_Interval 100

int main()
{
	int shared_memory_amount = Trains_Amount * ( Passengers + Bikes) + 2;
	int train_ID = create_shared_memory(".", 'A', sizeof(int) * shared_memory_amount, IPC_CREAT | 0600 );
	int train_memory_adress = (int*)attach_shared_memory(train_ID, NULL, 0 );




	detach_shared_memory((void*)train_memory_adress, train_ID);
	free_shared_memory(train_ID);
	

	return 0;
}