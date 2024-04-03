#ifndef LIBRARIES
#define LIBRARIES

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <pthread.h>
#include<sys/shm.h>
#include <errno.h>
#include <stdbool.h>


#define NUMBER_OF_PLAYERS 12
#define NUMBER_OF_PLAYERS_In_TEAM 6
#define NUMBER_INITIAL_BALLS 2
#define FILE_NAME "arguments.txt"


#define TEAM1FIFO "/tmp/TEAM1FIFO"
#define TEAM2FIFO "/tmp/TEAM2FIFO"


//default values, if the file that opend is empty or somthing get error
static int NUMBER_OF_LOST_ROUNDS = 5;
static int SIMULATION_THRISHOLD = 500;
static int ROUND_TIME = 15;
static int RANGE_ENERGY[2] = {1,10};



void send_message_fifo(char* team_fifo_name,char* message);
void createFifo(char* fifo_name);
void read_message_fifo(char* team_fifo_name,char* message);


#endif