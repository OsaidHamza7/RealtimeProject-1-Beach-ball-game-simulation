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
#include <sys/shm.h>
#include <errno.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>

#define NUMBER_OF_PLAYERS 12
#define NUMBER_OF_PLAYERS_In_TEAM 6
#define NUMBER_INITIAL_BALLS 2
#define FILE_NAME "arguments.txt"

#define TEAM1FIFO "/tmp/TEAM1FIFO"
#define TEAM2FIFO "/tmp/TEAM2FIFO"
#define GUIFIFO "/tmp/PUBLICFIFO"

// default values, if the file that opend is empty or somthing get error
extern int NUMBER_OF_LOST_ROUNDS;
extern int SIMULATION_THRISHOLD;
extern int ROUND_TIME;
extern int RANGE_ENERGY[2];

void send_message_fifo(char *team_fifo_name, char *message);
void createFifo(char *fifo_name);
void read_message_fifo(char *team_fifo_name, char *message);
void split_string(char *argv, int arr[]);
void readArgumentsFromFile(char *filename);

#endif