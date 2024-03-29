
#include "header.h"
#include "constants.h"


int player_number_in_team;
int player_team_number;
pid_t next_player_pid;
static char message[BUFSIZ];
int f_des[2];
void signal_handler(int sig);
void signal_handler1(int sig);

int main(int argc, char** argv){


    if ( argc < 4){
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid,f_des\n");
        exit(-1);
    }

    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    next_player_pid = atoi(argv[3]);
    f_des[0] = atoi(argv[4]);
    f_des[1] = atoi(argv[5]);
    
    printf("Player #%d from team #%d with PID = %d And next player PID = %d\n",player_number_in_team,player_team_number,getpid(),next_player_pid);


   if(sigset(SIGUSR1, signal_handler) == -1){
        perror("Signal Error\n");
        exit(-1);
    }
    if(sigset(SIGCLD, signal_handler1) == -1){
        perror("Signal Error\n");
        exit(-1);
    }
    pause();

    return 0;
}

void signal_handler(int sig){

    printf("The signal %d, reached to player #%d from team #%d with PID = %d \n", sig,player_number_in_team,player_team_number,getpid());
    sleep(3);
    kill(next_player_pid ,SIGCLD);
}


void signal_handler1(int sig){

    printf("The signal %d, reached to player #%d\n", sig,player_number_in_team);
     sleep(3);
    if(0 == next_player_pid)
        exit(0);
    kill(next_player_pid ,SIGCLD);
    exit(0);
}