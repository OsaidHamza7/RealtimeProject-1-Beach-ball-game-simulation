#include "header.h"


int  team_signal_number;
int  other_team_signal_number;
int  player_number_in_team;
int  player_team_number;
int  is_team_lead;
int  number_balls_player = 0;
int  number_balls_team = 0;
int  next_player_pid,next_players_pids[2];
char team_fifo_name[20];
int  team_fifo;
char message[BUFSIZ] ;

int isRoundFinished = 0;
int energy;
int energyChangePerSecond = 10;
int sleep_status;
int pause_time;

//***********************************************************************************
void init_signals_handlers();
void signal_handler(int sig);
void signal_handler1(int sig);
void signal_handler3(int sig);
void signal_handler_SIGALRM(int sig);
int  calculate_and_apply_pause();
int  getRandomNumber(int min, int max);
//***********************************************************************************

int main(int argc, char** argv){

    //check the number of arguments
    if ( argc < 4){
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid\n");
        exit(-1);
    }

    srand((unsigned) getpid()); // seed for the random function with the ID of the current process
    //get the player number in the team, the team number, and the next player pid
    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    is_team_lead = (player_number_in_team == 6) ? 1 : 0 ;//team lead or not

    if(is_team_lead == 1){//team lead
        //split the next player pid to 2 pids, the first one is the first player in the team, and the second one is the other team lead
        split_string(argv[3],next_players_pids);//?
        printf("Team leader Player #%d in team #%d with PID = %d And next player1=%d, next team lead=%d\n",player_number_in_team, player_team_number, getpid(), next_players_pids[0], next_players_pids[1]);

    } else {//normal player
        next_player_pid = atoi(argv[3]);
        printf("Player #%d from team #%d with PID = %d And next player PID = %d\n",player_number_in_team,player_team_number,getpid(),next_player_pid);
    }

    fflush(stdout);
    strcpy(team_fifo_name, (player_team_number == 1) ? TEAM1FIFO : TEAM2FIFO);
    team_signal_number = (player_team_number == 1) ? SIGUSR1 : SIGUSR2;
    other_team_signal_number = (player_team_number == 1) ? SIGUSR2 : SIGUSR1;

    // set the alarm to change the energy of the player
    alarm(energyChangePerSecond);

    energy = getRandomNumber(30,100);
    pause_time = calculate_and_apply_pause();
    
    //initialize the signal handlers
    init_signals_handlers();
    
    //sleep(pause_time);
    while(1){
        pause();
    }

    exit(0);
    return 0;
}



//function init_signals_handlers is used to initialize the signal handlers
void init_signals_handlers(){
     //only team lead can catch the signal SIGUSR1 from parent or from other team lead
    if(signal(SIGUSR1, signal_handler) == SIG_ERR){//throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    //only team lead can catch the signal SIGUSR2,from parent or from other team lead
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
}




void signal_handler(int sig){//team lead only
    number_balls_team++;
    next_player_pid=next_players_pids[0];//catch the ball from parent or other team lead ,so next player is the first player in it's team
    printf("Signal %d,team lead player #%d , team #%d, PID = %d ,next player=%d\n", sig,player_number_in_team,player_team_number,getpid(),next_player_pid);
    fflush(stdout);
    pause_time = calculate_and_apply_pause();
    //team lead
    while (pause_time != 0){
        // if the round is finished, then stop the ball
        if (isRoundFinished == 1){
            printf("Sleep is intrupted player #%d team #%d due to Round finished.\n",player_number_in_team,player_team_number);
            fflush(stdout);
            isRoundFinished = 0;
            return;
        }
        //intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
        pause_time = sleep(pause_time);
    }
    //throw the ball(signal) to the first player in the team
    kill(next_player_pid ,SIGCLD);//next player is first player in the team
}

void signal_handler1(int sig){
    if (is_team_lead == 1){//reached the ball from player number 5 to the team lead,so send it to the other team lead (bu signal SIGTRAP)
        number_balls_team--;
        next_player_pid=next_players_pids[1];//next player is the other team lead
        printf("The signal %d, reached to player #%d ,team #%d ,next player is %d\n", sig,player_number_in_team,player_team_number,next_player_pid);
        //sleep(1);
        fflush(stdout);
        kill(next_player_pid,other_team_signal_number);//team lead catch the ball back from player 5,so throw it to the other team lead
        if (number_balls_team == 0){//if the team has no balls, then team lead will send signal to the parent to throw a new ball
            //sleep(1);//check if round is finished?or keep it like that?
            printf("The team #%d has no balls, so send signal to the parent to throw a new ball\n",player_team_number);
            fflush(stdout);
            kill(getppid(),team_signal_number);//send signal SIGUSR1 to the parent to throw a new ball
        }
        return;
    }
    printf("The signal %d, reached to player #%d ,team #%d ,next player is %d\n", sig,player_number_in_team,player_team_number,next_player_pid);
    fflush(stdout);
    pause_time = sleep(calculate_and_apply_pause());
    //normal player
    while (pause_time != 0){
        // if the round is finished, then stop the ball
        if (isRoundFinished == 1){
            printf("Sleep is intrupted player #%d team #%d due to Round finished.\n",player_number_in_team,player_team_number);
            fflush(stdout);
            isRoundFinished = 0;
            return;
        }
        //intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
        pause_time = sleep(pause_time);
    }
    //throw the ball to the next player
    kill(next_player_pid ,SIGCLD);
    //printf("The ball is thrown from player #%d ,team #%d to player #%d\n",player_number_in_team,player_team_number,next_player_pid);
    //fflush(stdout);
}

//function used to catch the signal from parent to stop send signals among players
void signal_handler3(int sig){
    isRoundFinished = 1;
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
    if (energy != 1){
        energy -= 1;
    }
    alarm(energyChangePerSecond);
}

// function calculate_and_apply_pause is used to calculate the pause time for the player
int calculate_and_apply_pause() {
    
    // Define the maximum pause time when energy is zero
    int max_pause_time = 10;

    // Calculate base pause time inversely proportional to the energy
    int time = max_pause_time - (energy * max_pause_time) / 100;

    // Determine if the ball falls
    float probability_fall = (100 - energy) / 100.0;
    int ball_falls = ((float)rand() / (float)RAND_MAX) < probability_fall;


    // If the ball falls, add a random time between 1 to 5 to re-collect the ball
    if (ball_falls) {
        int a = rand() % 5 + 1;
        time += a;

        printf("Ball Failled for %d,Player #%d Team #%d,Energy: %d%%, Pausing for %d seconds...\n",a,player_number_in_team,player_team_number, energy, time);
        fflush(stdout);
        return time;
    }

    // Ensure pause time is at least 2 second
    time = (time < 2) ? 2 : time;

    printf("Player #%d Team #%d,Energy: %d%%, Pausing for %d seconds...\n",player_number_in_team,player_team_number, energy, time);
    fflush(stdout);
    return time;
}

int getRandomNumber(int min, int max) {

    if (min > max) {
        //printf("Error: min should be less than or equal to max.\n");
        return -1; // Return an error code
    }

    // Calculate the range and generate a random number within that range
    int range = max - min + 1;
    int randomNumber = rand() % range + min;

    return randomNumber;
}

