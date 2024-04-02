
#include "header.h"
#include "constants.h"


int player_number_in_team;
int player_team_number;
int is_team_lead;
int number_balls = 0;

int next_player_pid,next_player[2];
char team_fifo_name[20];
int team_fifo;
//***********************************************************************************
void signal_handler(int sig);
void signal_handler1(int sig);
void signal_handler3(int sig);
void send_number_balls_to_parent();
//***********************************************************************************


int main(int argc, char** argv){


    if ( argc < 5){
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid,is_team_lead\n");
        exit(-1);
    }

    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    is_team_lead = atoi(argv[4]);

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
    } else {//normal player
        next_player_pid = atoi(argv[3]);
        printf("Player #%d from team #%d with PID = %d And next player PID = %d\n",player_number_in_team,player_team_number,getpid(),next_player_pid);
        fflush(stdout);
    }

    strcpy(team_fifo_name, (player_team_number == 1) ? TEAM1FIFO : TEAM2FIFO);


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

    //SIGPOLL is used for the parent will tell the team leaders that round has finished
   /*if(sigset(SIGPOLL, signal_handler2) == -1){
        perror("Signal Error\n");
        exit(-1);
    }*/

   if(sigset(SIGHUP, signal_handler3) == -1){//catch the signal from parent to stop send signals to players
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
    number_balls++;
    next_player_pid=next_player[0];//next player is the first player in it's team
    printf("Signal %d,team lead player #%d , team #%d, PID = %d ,next player=%d\n", sig,player_number_in_team,player_team_number,getpid(),next_player_pid);
    sleep(3);
    //throw the ball(signal) to the first player in the team
    kill(next_player_pid ,SIGCLD);//next player is first player in the team
}


void signal_handler1(int sig){
    printf("The signal %d, reached to player #%d ,team #%d ,next player is %d\n", sig,player_number_in_team,player_team_number,next_player_pid);
    int a = sleep(5);
    if (a != 0){
        printf("Sleep is intrupted player #%d team #%d.\n",player_number_in_team,player_team_number);
        fflush(stdout);
        return;
    }

    if (is_team_lead == 1){//reached the ball from player number 5 to the team lead,so send it to the other team lead (bu signal SIGTRAP)
        number_balls--;
        next_player_pid=next_player[1];//next player is the other team lead
        kill(next_player_pid,SIGUSR1);//the ball gets back to the team lead,so throw it to the other team lead
       /*if (number_balls == 0){
            //send signal to the parent to throw a new ball
            kill(getppid(),SIGUSR1);
        }*/
       // exit(0);
    }
    else{
        //throw the ball to the next player
        kill(next_player_pid ,SIGCLD);
        printf("The ball is thrown from player #%d ,team #%d to player #%d\n",player_number_in_team,player_team_number,next_player_pid);
        fflush(stdout);
    }
}


//function signal handler3 is used to catch the signal from parent to stop send signals to players
void signal_handler3(int sig){
    printf("The signal %d, reached to player #%d ,team #%d ,stop sending signals\n", sig,player_number_in_team,player_team_number);
    fflush(stdout);
    if (is_team_lead == 1)
    {
        //call function to send the number of balls to the parent
        send_number_balls_to_parent();           
    }
}

void send_number_balls_to_parent(){

    char message[BUFSIZ];

    // Assuming this is part of the main or a function in player.c
    /* Open the public FIFO for reading and writing */
    if ((team_fifo = open(team_fifo_name, O_WRONLY | O_NONBLOCK)) == -1)
    {
        perror("hi error open fifo");
        printf("\n\n\nA Player Died !!!!!! \n\n\n");
        fflush(stdout);
        exit(1);
    }
    sprintf(message,"%d",number_balls);

    if (write(team_fifo, message, sizeof(message)) == -1)
    {
        perror("write error");
        printf("\n\n\nAb Player Died !!!!!! \n\n\n");
        fflush(stdout);
        exit(1);
    }
    sleep(5);
    close(team_fifo);

}