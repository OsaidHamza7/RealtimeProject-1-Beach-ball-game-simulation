
#include "header.h"
#include "constants.h"


int player_number_in_team;
int player_team_number;
int is_team_lead;
int number_balls = 0;

int next_player_pid,next_player[2];

void signal_handler(int sig);
void signal_handler1(int sig);
void signal_handler2(int sig);

int main(int argc, char** argv){


    if ( argc < 5){
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid,is_team_lead\n");
        exit(-1);
    }

    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    is_team_lead = atoi(argv[4]);
    //next_player_pid = atoi(argv[3]);
    


    if(is_team_lead == 1){//team lead
        //split the next player pid to 2 players

        char *token = strtok(argv[3], " ");

        if (token != NULL) {
            next_player[0] = atoi(token);
            next_player_pid = next_player[0];
            token = strtok(NULL, " ");
            if (token != NULL) {
                next_player[1] = atoi(token);
                printf("Team leader Player #%d in team #%d with PID = %d And next player1=%d, next team lead=%d\n",
                        player_number_in_team, player_team_number, getpid(), next_player[0], next_player[1]);
            } else {
                printf("Expected another PID but got NULL\n");
            }
        } else {
            printf("Expected a PID but got NULL\n");
        }
        fflush(stdout);
    }
    else{//normal player
        next_player_pid = atoi(argv[3]);
        printf("Player #%d from team #%d with PID = %d And next player PID = %d\n",player_number_in_team,player_team_number,getpid(),next_player_pid);
        fflush(stdout);
    }

    if(sigset(SIGUSR1, signal_handler) == -1){//throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }
    if(sigset(SIGUSR2, signal_handler) == -1){//throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }
    if(sigset(SIGCLD, signal_handler1) == -1){//throw the ball between players
        perror("Signal Error\n");
        exit(-1);
    }


    while(1){
        pause();
    }

    exit(0);
    return 0;
}

void signal_handler(int sig){
    number_balls++;
    printf("Signal %d,team lead player #%d , team #%d, PID = %d ,next player=%d\n", sig,player_number_in_team,player_team_number,getpid(),next_player_pid);
    sleep(3);
    kill(next_player_pid ,SIGCLD);//next player is first player in the team
    next_player_pid=next_player[1];//next player is the other team lead
}


void signal_handler1(int sig){
    printf("The signal %d, reached to player #%d ,team #%d ,next player is %d\n", sig,player_number_in_team,player_team_number,next_player_pid);
    sleep(3);
    if (is_team_lead == 1){
        number_balls--;
        kill(next_player_pid,SIGCLD);//the ball gets back to the team lead,so throw it to the other team lead
       next_player_pid=next_player[0];
       if (number_balls == 0){
            //send signal to the parent to throw a new ball
            kill(getppid(),SIGUSR1);
        }
       // exit(0);
    }
    else
        kill(next_player_pid ,SIGCLD);
}

