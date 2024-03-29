
#include "header.h"
#include "constants.h"


void readArgumentsFile(char* filename);

int main(int argc , char** argv){

  char* file_name = (char*)malloc(50*sizeof(char));
  int i, status,n;
  int pid, team1[NUMBER_OF_PLAYERS_In_TEAM], team2[NUMBER_OF_PLAYERS_In_TEAM];

  int num;
  char* player_team_number="1";
  pid_t next_pid = 0;
  int f_des[2];
  static char message[BUFSIZ];
  printf("My process ID is %d\n", getpid());

  if ( argc != 2 ){//check if the user passed the arguments
      // Use the default file names
    	printf("Usage: Invalid arguments.\n");
      printf("Using default file names: arguments.txt\n");
      strcpy(file_name,"arguments.txt");
    }
  else {
      // Use the file names provided by the user
      strcpy(file_name,argv[1]);
  }
  printf("Reading the arguments from the file: %s\n", file_name);
  readArgumentsFile(file_name);//read the arguments from the file
  
  printf("\n*****************************************************\n");
  printf("NUMBER_OF_LOST_ROUNDS = %d\n", NUMBER_OF_LOST_ROUNDS);
  printf("SIMULATION_THRISHOLD = %d\n", SIMULATION_THRISHOLD);
  printf("ROUND_TIME = %d\n", ROUND_TIME);
  printf("RANGE_ENERGY = %d %d\n", RANGE_ENERGY[0], RANGE_ENERGY[1]);
  printf("*****************************************************\n\n");

  //create the teams
  i = NUMBER_OF_PLAYERS-1;//create the players

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
      num = i%6 + 1;
      char player_number_in_team[3];
      char next_player_pid[10];

      sprintf(player_number_in_team, "%d", num);//convert the (number of player in team) to string
      sprintf(next_player_pid, "%d", next_pid);//convert the (the pid of the next player) to string
      
      execlp("./player", "player",player_number_in_team,player_team_number,next_player_pid, NULL);
      perror("Error:Execute player Failed.\n");
      exit(1);
      break;

    default://I'm parent
      if(i>5){
        printf("Team1:Player #%d With PID = %d  was created\n",i%6+1,pid);
        team1[i%6] = pid;
        if (i==6){
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
        if(i==0)
          printf("\nTeam 2 was created susccessfully\n=================================================================================\n\n");
      }

      break;
    }

    }



   /* close(f_des[0]);
    char message1[10] ;
    sprintf(message1, "%d", team2[0]);//convert the (number of player in team) to string
    if ( write(f_des[1], message1, 10) != -1 ) {
      printf("Message sent by parent: [%s]\n", message1);
      fflush(stdout);
      close(f_des[1]);
    }
    else {
      perror("Write");
      exit(5);
    }*/
    //sleep(5);
    //kill(team1[0],SIGUSR1);


    return 0;
}       


void readArgumentsFile(char* filename){

    char line[200];
    char label[50];

    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL){
        perror("The file not exist\n");
        exit(-2);
    }
    char separator[] = " ";

    while(fgets(line, sizeof(line), file) != NULL){

        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "NUMBER_OF_LOST_ROUNDS") == 0){
             NUMBER_OF_LOST_ROUNDS = atoi(str);
        }

        else if (strcmp(label, "SIMULATION_THRISHOLD") == 0){
             SIMULATION_THRISHOLD = atoi(str);
        }
        else if (strcmp(label, "ROUND_TIME") == 0){
             ROUND_TIME = atoi(str);
        }
        else if (strcmp(label, "RANGE_ENERGY") == 0){
             RANGE_ENERGY[0] = atoi(str);
            str = strtok(NULL, separator);
             RANGE_ENERGY[1] = atoi(str);
        }
         /*else {
            printf("Invalid variable name: %s\n", label);
            fflush(stdout);
        }*/


    }

    fclose(file);
}
