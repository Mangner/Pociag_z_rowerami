#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>


struct message
{
	int type;
	char content[30];
};



int create_message_queue(const char *path, int identifier, int flag);
void send_message(int mesg_queue_ID, struct message *msg_ptr, int msg_flag);
int recive_message(int mesg_queue_ID, struct message *msg_ptr,int message_type, int msg_flag);
void delete_meesage_queue(int mesg_queue_ID);