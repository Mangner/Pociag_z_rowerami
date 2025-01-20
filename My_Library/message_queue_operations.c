#include "message_queue_operations.h"


int create_message_queue(const char *path, int identifier, int flags)
{
	key_t key = ftok(path, identifier);
	if ( key == - 1)
    {
        perror("Ftok failed");
        exit(5);
    }

	int mesg_queue_ID =  msgget(key, flags);
	if ( mesg_queue_ID == -1 )
	{
		perror("Msgget failed");
		exit(6);
	}

	return mesg_queue_ID;
}


void send_message(int mesg_queue_ID, struct message *msg_ptr, int msg_flag)
{
	if (msgsnd(mesg_queue_ID, (void*)msg_ptr, sizeof(msg_ptr->content) + sizeof(msg_ptr->pid_grupy), msg_flag) == -1 ) 
	{
        perror("Msgsnd failed");
        exit(7);
	}
}


int recive_message(int mesg_queue_ID, struct message *msg_ptr,int message_type, int msg_flag)
{
	if (msgrcv(mesg_queue_ID, (void*)msg_ptr, sizeof(msg_ptr->content) + sizeof(msg_ptr->pid_grupy), message_type, msg_flag) == -1 )
	{ 
		if (errno == EINTR)
			return 1;
		else 
		{
			perror("Msg Recive failed");
			exit(2435464);
		}
	}
	return 0;
}


void delete_meesage_queue(int mesg_queue_ID)
{
	if ( msgctl(mesg_queue_ID, IPC_RMID, NULL) == -1 )
	{
		perror("Msgctl IPC_RMID failed");
		exit(9);
	}
}