
#include "header.h"

int  player_number_in_team;
int  player_team_number;
int  is_team_lead;
int  number_balls_player = 0;
int  number_balls_team = 0;
int  next_player_pid,next_player[2];
char team_fifo_name[20];
int  team_fifo;
char message[BUFSIZ] ;

int isRoundFinished = 0;
int energy = 100;
int energyChangePerSecond = 10;
//***********************************************************************************
void signal_handler(int sig);
void signal_handler1(int sig);
void signal_handler3(int sig);
//***********************************************************************************


int main(int argc, char** argv){


    if ( argc < 4){
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid\n");
        exit(-1);
    }

    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    is_team_lead = (player_number_in_team == 6) ? 1 : 0 ;//team lead or not
    

    if(is_team_lead == 1){//team lead
        //split the next player pid to 2 pids, the first one is the first player in the team, and the second one is the other team lead
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

    } else {    //normal player
        next_player_pid = atoi(argv[3]);
        printf("Player #%d from team #%d with PID = %d And next player PID = %d\n",player_number_in_team,player_team_number,getpid(),next_player_pid);
        fflush(stdout);
    }

    strcpy(team_fifo_name, (player_team_number == 1) ? TEAM1FIFO : TEAM2FIFO);

    // set the alarm to change the energy of the player
    alarm(energyChangePerSecond);
    
    if(signal(SIGUSR1, signal_handler) == SIG_ERR){//throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }
    
    if(signal(SIGUSR2, signal_handler) == SIG_ERR){//throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    if(signal(SIGCLD, signal_handler1) == SIG_ERR){//throw the ball between players
        perror("Signal Error\n");
        exit(-1);
    }

    //SIGALARM is used for changing the energy of the player every specific time
   if(sigset(SIGALRM, signal_handler_SIGALRM) == -1){
        perror("Signal Error\n");
        exit(-1);
    }

   if(signal(SIGHUP, signal_handler3) == SIG_ERR){//catch the signal from parent to stop send signals to players
        perror("Signal Error\n");
        exit(-1);
    }

    while(1){
        pause();
    }

    exit(0);
    return 0;
}

void signal_handler(int sig){//team lead only
    number_balls_team++;
    next_player_pid=next_player[0];//next player is the first player in it's team
    printf("Signal %d,team lead player #%d , team #%d, PID = %d ,next player=%d\n", sig,player_number_in_team,player_team_number,getpid(),next_player_pid);
    int a = sleep(4);
    if (a != 0){
        printf("Sleep is intrupted player #%d team #%d.\n",player_number_in_team,player_team_number);
        fflush(stdout);
        return;
    }
    //throw the ball(signal) to the first player in the team
    kill(next_player_pid ,SIGCLD);//next player is first player in the team
}

void signal_handler1(int sig){
    printf("The signal %d, reached to player #%d ,team #%d ,next player is %d\n", sig,player_number_in_team,player_team_number,next_player_pid);
    if (is_team_lead == 1){//reached the ball from player number 5 to the team lead,so send it to the other team lead (bu signal SIGTRAP)
        number_balls_team--;
        next_player_pid=next_player[1];//next player is the other team lead
        kill(next_player_pid,SIGUSR1);//the ball gets back to the team lead,so throw it to the other team lead
        if (number_balls_team == 0){//if the team has no balls, then send signal to the parent to throw a new ball
            sleep(1);
            printf("The team #%d has no balls, so send signal to the parent to throw a new ball\n",player_team_number);
            kill(getppid(),SIGUSR1);//send signal SIGUSR1 to the parent to throw a new ball
        }
        return ;
    }
    int a = sleep(5);
    
    while (a != 0){
        // if the round is finished, then stop the ball
        if (isRoundFinished == 1){
            printf("Sleep is intrupted player #%d team #%d due to Round finished.\n",player_number_in_team,player_team_number);
            isRoundFinished = 0;
            fflush(stdout);
            return;
        }
        
        // sleep for the remaining time
        printf("Sleep is intrupted player #%d team #%d.\n",player_number_in_team,player_team_number);
        fflush(stdout);
        a = sleep(a);
    }
    //throw the ball to the next player
    kill(next_player_pid ,SIGCLD);
    //printf("The ball is thrown from player #%d ,team #%d to player #%d\n",player_number_in_team,player_team_number,next_player_pid);
    //fflush(stdout);
    
}


//function signal handler3 is used to catch the signal from parent to stop send signals to players
void signal_handler3(int sig){
    printf("The signal %d, reached to player #%d ,team #%d ,stop sending signals\n", sig,player_number_in_team,player_team_number);
    fflush(stdout);
    if (is_team_lead == 1)
    {
        //call function to send the number of balls to the parent
        sprintf(message, "%d", number_balls_team);
        send_message_fifo(team_fifo_name, message);    
        number_balls_team = 0;

    }
}

// function signal_handler_SIGALRM is used to change the energy of the player every specific time
void signal_handler_SIGALRM(int sig){
    energy -= 1;
    if (energy <= 0){
        printf("Player #%d from team #%d with PID = %d has no energy\n",player_number_in_team,player_team_number,getpid());
        fflush(stdout);
        energy=0;
        return;
    }
    alarm(energyChangePerSecond);
}

