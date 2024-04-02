
#include "header.h"
#include "constants.h"
#include <stdbool.h>

//***********************************************************************************
void checkArguments(int argc, char** argv, char* file_name);
void readArgumentsFile(char* filename);
void startRound(int round_number);
void createTeams();
void signal_handler(int sig);
void send_pides_to_team_lead(int first_player_pid,int other_team_lead_pid);
void createFifos();
//***********************************************************************************


  int num;
  int next_pid = 0;

  int f_des[2];
  static char message[BUFSIZ];
  int i, status,n;
  int pid;
  int team1[NUMBER_OF_PLAYERS_In_TEAM], team2[NUMBER_OF_PLAYERS_In_TEAM];
  int current_round_number = 0;

  char* player_team_number="1";


int main(int argc , char** argv){
  char* file_name = (char*)malloc(50*sizeof(char));

  //Start the program
  printf("*******************************************\nStart the program, My process ID is %d\n\n", getpid());

  //check the arguments
  checkArguments(argc, argv, file_name);
  printf("Reading the arguments from the file: %s\n\n", file_name);

  //read the arguments from the file
  readArgumentsFile(file_name);
  
  //print the arguments 
  printf("\n*****************************************************\n");
  printf("NUMBER_OF_LOST_ROUNDS = %d\n", NUMBER_OF_LOST_ROUNDS);
  printf("SIMULATION_THRISHOLD = %d\n", SIMULATION_THRISHOLD);
  printf("ROUND_TIME = %d\n", ROUND_TIME);
  printf("RANGE_ENERGY = %d %d\n", RANGE_ENERGY[0], RANGE_ENERGY[1]);
  printf("*****************************************************\n\n");

  //create the teams
  createTeams();

  //create a two fifos for the teams
  createFifos();

  //Send Pids of ( 'first player' and 'other team lead') to each team lead ,after creating the teams
  send_pides_to_team_lead(team1[0],team2[5]); //send pid of player1 in team1 and pid of team2 leader to the team1 lead
  sleep(3);
  send_pides_to_team_lead(team2[0],team1[5]);//send pid of player1 in team2 and pid of team1 leader to the team2 lead
  sleep(3);
  
  //Start the game
  //start the round ,the parent initially will throw one ball to each team lead (by send signal SIGUSR1 to them)
  startRound(current_round_number);


 /* if(sigset(SIGUSR1, signal_handler) == -1){//throw the ball from parent to team lead, or from team lead to other team lead
    perror("Signal Error\n");
    exit(-1);
  }*/

  /*while(1){
    pause();
  }*/

  sleep(1);
  // kill all children
  printf("\nStart kill all players\n");

  for (i = 0; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
  {
      kill(team1[i], SIGQUIT);
      sleep(0.5);
      kill(team2[i], SIGQUIT);
      sleep(0.5);
  }

  printf("\n\n**Good Bay**\n\n");

    return 0;
}       


void checkArguments(int argc, char** argv, char* file_name){
    if ( argc != 2 ){//check if the user passed the correct arguments
      // Use the default file names
    	printf("Usage: Invalid arguments.\n");
      printf("Using default file names: arguments.txt\n");
      strcpy(file_name,"arguments.txt");
    }
  else {
      // Use the file names provided by the user
      strcpy(file_name,argv[1]);
  }
}



void createTeams(){

  i = NUMBER_OF_PLAYERS-1;

  if ( pipe(f_des) == -1 ) {
    perror("Pipe");
    exit(2);
  }

  for ( ; i > -1; --i ) {
    switch (pid=fork())
    {

    case -1://Fork Failed
      perror("Error:Fork Failed.\n");
      exit(1);
      break;

    case 0://I'm child
      num = i % 6 + 1;
      char player_number_in_team[3];
      char next_player_pid[10];

      sprintf(player_number_in_team, "%d", num);//convert the (number of player in team) to string
      sprintf(next_player_pid, "%d", next_pid);//convert the (the pid of the next player) to string

     if( i == 11 || i == 5 ){//I'm team lead,wait for the parent to send the Pids of ('first player in team', 'other team lead')
        close(f_des[1]);
        if (read(f_des[0], message, BUFSIZ) != -1 ) {
          printf("Message received by child: [%s]\n", message);
          fflush(stdout);
        }
        else {
          perror("Read");
          exit(4);
        }
        if(message!=NULL){
          strcpy(next_player_pid,message);//the pids was sent to the team lead
        }

      }

      execlp("./player", "player",player_number_in_team,player_team_number,next_player_pid,NULL);
      perror("Error:Execute player Failed.\n");
      exit(1);
      break;

    default://I'm parent
      if( i > 5 ){
        printf("Team1:Player #%d With PID = %d  was created\n",i%6+1,pid);
        team1[i%6] = pid;
        if ( i == 6 ){
          player_team_number="2";
          next_pid=0;
          printf("\nTeam 1 was created susccessfully\n=================================================================================\n\n");
        }
        else
          next_pid = pid;
      }

      else{
        next_pid = pid;
        printf("Team2:Player #%d With PID = %d  was created\n",i%6+1,pid);
        team2[i%6] = pid;
        if( i == 0 )
          printf("\nTeam 2 was created susccessfully\n=================================================================================\n\n");
      }

      break;
    }

    }


}

void startRound(int round_number){
    round_number++;
    printf("\n\n> Round #%d started after 2 seconds.\n\n", round_number);
    fflush(stdout);
    sleep(2);
    kill(team1[5],SIGUSR1);//throw the ball to the team lead
    sleep(20);
    kill(team2[5],SIGUSR2);

    // wait for current round to finish ( finishes after 7 seconds)
    sleep(30);

    printf("\n\n> Round #%d is finished.\n\n", round_number);
    fflush(stdout);
    
    //send signal to all players to stop send signals (ball)
    //the team lead will pass the number of balls that the team have to the parent
    for (i = 0; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
    {
        kill(team1[i], SIGHUP);
        sleep(0.5);
    }

    for (i = 0; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
    {
        kill(team2[i], SIGHUP);
        sleep(0.5);
    }
    

  int fifo1,fifo2;
  char message[BUFSIZ];
  char message2[BUFSIZ];


  if ((fifo1 = open(TEAM1FIFO, O_RDONLY)) == -1)
  {
    perror("error open fifo1");
    printf("\n\n\nparent dead !!!!!! \n\n\n");
    fflush(stdout);
    exit(1);
  }

  else{
    if(read(fifo1, message, sizeof(message)) == -1){
      perror("read error");
      printf("\n\n\nread parent dead : !!!!!! \n\n\n");
      fflush(stdout);
      exit(1);
    }



  }
    if ((fifo2 = open(TEAM2FIFO, O_RDONLY)) == -1)
  {
    perror("error open fifo2");
    printf("\n\n\nparent dead !!!!!! \n\n\n");
    fflush(stdout);
    exit(1);
  }


  else{
    if(read(fifo2 , message2, sizeof(message2)) == -1){
      perror("read error");
      printf("\n\n\nread parent dead : !!!!!! \n\n\n");
      fflush(stdout);
      exit(1);
    }
  }
    //print the message
    printf("The message is reached to the parent is : %s\n",message);
    printf("The message is reached to the parent is : %s\n",message2);
    fflush(stdout);
    close(fifo1);
    close(fifo2);


  int balls1 = atoi(message);
  int balls2 = atoi(message2);

  printf("\n\nRound #%d finished and current balls are\n\tteam 1 - %d\n\tteam 2 - %d\n\n", round_number, balls1, balls2);
}

void signal_handler(int sig){

  //kill(team1[5],SIGUSR1);
  printf("the signal reached the parent\n");
}

void send_pides_to_team_lead(int first_player_pid,int other_team_lead_pid){
  close(f_des[0]);
  char message1[20] ;
  sprintf(message1, "%d %d", first_player_pid,other_team_lead_pid);//convert the (pid of first player in team1) to string

  if (write(f_des[1], message1, strlen(message1)) != -1 ) {
    printf("Message sent by parent: [%s] to the team lead\n", message1);
    fflush(stdout);
  }
  else {
    perror("Write");
    exit(5);
  }

  //kill(team_lead_pid, signal);
}

void createFifos()
{
    // remove pipes if they exist
    remove(TEAM1FIFO);
    remove(TEAM2FIFO);

    if ((mkfifo(TEAM1FIFO, S_IFIFO | 0777)) == -1)
    {
        perror("Error Creating Fifo1");
        exit(-1);
    }


    // Generate the  TEAM2FIFO
    if ((mkfifo(TEAM2FIFO, S_IFIFO | 0777)) == -1)
    {
        perror("Error Creating Fifo2");
        exit(-1);
    }

}











