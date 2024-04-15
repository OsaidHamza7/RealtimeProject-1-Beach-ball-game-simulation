
#include "header.h"
/*
  Osaid Hamza - Team Leader
  Razan Abdelrahman
  Maisam Alaa
  Ansam Rihan

*/

//***********************************************************************************
void checkArguments(int argc, char **argv, char *file_name);
void createTeams();
void sendPidesToTeamlead(int first_player_pid, int other_team_lead_pid);
void init_signals_handlers();
void signal_handler_throw_newball(int sig);
void startGame();
void startRound();
void calculateRoundScores();
void killAllPlayers();
void create_fifos_players();
void signal_handler_SIGALRM(int sig);
//***********************************************************************************
int num_player;
int i, status, n;
int pid, next_pid = 0;
int team1[NUMBER_OF_PLAYERS_In_TEAM], team2[NUMBER_OF_PLAYERS_In_TEAM];
int current_round_number = 1;
int pause_time_round;
int f_des[2];
char message[BUFSIZ], message2[BUFSIZ], ranges[10];
char *player_team_number = "1";
char player_fifo_name[20], player_fifo_name2[20];
int team1_score = 0, team2_score = 0; // teams score
int team1_round_balls = 0, team2_round_balls = 0;
int gui_pid;
int current_ball_number = 0;
char ball_number[5];
int is_alarmed = 0;
int main(int argc, char **argv)
{
  char *file_name = (char *)malloc(50 * sizeof(char));

  // Start the program
  printf("*******************************************\nStart the program, My process ID is %d\n\n", getpid());

  // check a number of arguments,and read a file name
  checkArguments(argc, argv, file_name);

  // read the arguments from the file
  readArgumentsFromFile(file_name);

  gui_pid = fork();
  if (gui_pid == 0)
  {
    execlp("./gui", "gui", NULL);
    perror("Error:Execute gui Failed.\n");
    exit(1);
  }

  // create a fifos for the teams and for GUI
  createFifo(GUIFIFO);
  createFifo(TEAM1FIFO);
  createFifo(TEAM2FIFO);
  create_fifos_players();
  printf("\n\nFifos was created successfully\n\n");

  // create the teams
  createTeams();

  send_message_fifo(GUIFIFO, "t"); // send message to the gui to start the game

  // Send Pids of ('first player' and 'other team lead') to each team lead ,after creating the teams
  sendPidesToTeamlead(team1[0], team2[5]); // send pid of player1 in team1 and pid of team2 leader to the team1 lead
  sleep(2);
  sendPidesToTeamlead(team2[0], team1[5]); // send pid of player1 in team2 and pid of team1 leader to the team2 lead
  sleep(2);

  // init signals handlers
  init_signals_handlers();

  // Start the game
  send_message_fifo(GUIFIFO, "0");
  alarm(SIMULATION_THRISHOLD);
  startGame();

  // kill all players
  killAllPlayers();
  send_message_fifo(GUIFIFO, "e"); // send message to the gui to end the game
  sleep(1);
  send_message_fifo(GUIFIFO, "w"); // send message to the gui to display the winner game
  printf("\n\n**The Game Is Finished**\n**Good Bye**\n\n");
  return 0;
}

void checkArguments(int argc, char **argv, char *file_name)
{
  if (argc != 2) // check if the user passed the correct arguments
  {
    printf("Usage: Invalid arguments.\n"); // Use the default file names
    printf("Using default file names: arguments.txt\n");
    strcpy(file_name, FILE_NAME);
  }
  else
  {
    strcpy(file_name, argv[1]); // Use the file names provided by the user
  }
}

void createTeams()
{

  i = NUMBER_OF_PLAYERS - 1;

  if (pipe(f_des) == -1)
  {
    perror("Pipe");
    exit(2);
  }

  for (; i > -1; --i)
  {
    switch (pid = fork())
    {

    case -1: // Fork Failed
      perror("Error:Fork Failed.\n");
      exit(1);
      break;

    case 0: // I'm Player,firstly the value of next_pid is 0,then it will be updated by the pid of the next player
      num_player = i % 6 + 1;
      char player_number_in_team[3];
      char next_player_pid[10];

      sprintf(player_number_in_team, "%d", num_player);           // convert the (number of player in team) to string
      sprintf(next_player_pid, "%d", next_pid);                   // convert the (the pid of the next player) to string
      sprintf(ranges, "%d %d", RANGE_ENERGY[0], RANGE_ENERGY[1]); // convert the (energy range) to string

      if (i == 11 || i == 5)
      { // I'm team lead,wait for the parent to send the Pids of ('first player in team', 'other team lead')
        close(f_des[1]);
        if (read(f_des[0], message, BUFSIZ) != -1)
        {
          printf("Message received by child: [%s]\n", message);
          fflush(stdout);
        }
        else
        {
          perror("Read");
          exit(4);
        }
        if (message != NULL)
        {
          strcpy(next_player_pid, message); // the pids was sent to the team lead
        }
      }

      execlp("./player", "player", player_number_in_team, player_team_number, next_player_pid, ranges, NULL);
      perror("Error:Execute player Failed.\n");
      exit(1);
      break;

    default: // I'm parent
      next_pid = pid;
      if (i > 5)
      {
        printf("Team1:Player #%d With PID = %d  was created\n", i % 6 + 1, pid);
        team1[i % 6] = pid;
        if (i == 6)
        {
          player_team_number = "2";
          next_pid = 0; // reset the next_pid to 0 to start the next team , to let the team lead of team2 take it
          printf("\nTeam 1 was created susccessfully\n=================================================================================\n\n");
        }
      }

      else
      {
        printf("Team2:Player #%d With PID = %d  was created\n", i % 6 + 1, pid);
        team2[i % 6] = pid;
        if (i == 0)
          printf("\nTeam 2 was created susccessfully\n=================================================================================\n\n");
      }

      break;
    }
  }
}

void sendPidesToTeamlead(int first_player_pid, int other_team_lead_pid)
{
  close(f_des[0]);
  char message1[20];
  sprintf(message1, "%d %d", first_player_pid, other_team_lead_pid); // convert the (pid of first player in team1) to string
  if (write(f_des[1], message1, strlen(message1)) != -1)
  {
    printf("Message sent by parent: [%s] to the team lead\n", message1);
    fflush(stdout);
  }
  else
  {
    perror("Write");
    exit(5);
  }
}

void startGame()
{
  while (team1_score != NUMBER_OF_LOST_ROUNDS && team2_score != NUMBER_OF_LOST_ROUNDS && is_alarmed == 0)
  {
    startRound();
    calculateRoundScores();
    printf("\n\nTeam results after the Round #%d finished :\n\tteam1  %d    -    %d  team2\n\n", current_round_number, team1_score, team2_score);
    sprintf(message, "s%d%d", team1_score, team2_score);
    send_message_fifo(GUIFIFO, message);
    sleep(1);
    team1_round_balls = 0;
    team2_round_balls = 0;
    current_round_number++;
    current_ball_number = 0;
    send_message_fifo(GUIFIFO, "0");
  }
  if (is_alarmed == 1)
  {
    printf("\nThe *SIMULATION-THRISHOLD* of the game is reached \n");
  }
  else
  {
    printf("\nThe *NUMBER-OF-LOST-ROUNDS* of the game is reached \n");
  }
  fflush(stdout);
}

void startRound()
{
  printf("\n\n> Round #%d started after 3 seconds.\n\n", current_round_number);
  fflush(stdout);
  sleep(3);

  // initially throw two balls to the team leads
  send_message_fifo(GUIFIFO, "1"); // send signal to the gui to throw new ball to team1
  sleep(1);
  kill(team1[5], SIGUSR1); // throw the ball to the team1 lead

  sprintf(ball_number, "%d", current_ball_number); // send signal to the gui to throw new ball to team2
  send_message_fifo(TEAM1FIFO, ball_number);       // send number of ball to the team lead
  current_ball_number++;

  kill(team2[5], SIGUSR2); // throw the ball to the team2 lead

  send_message_fifo(GUIFIFO, "2"); // send signal to the gui to throw new ball to team1

  sprintf(ball_number, "%d", current_ball_number); // send signal to the gui to throw new ball to team2
  send_message_fifo(TEAM2FIFO, ball_number);       // send number of ball to the team lead
  current_ball_number++;

  //  wait for current round to finish
  pause_time_round = sleep(ROUND_TIME);
  while (pause_time_round != 0 && is_alarmed == 0)
  {
    pause_time_round = sleep(pause_time_round);
  }

  printf("\n\n> Round #%d is finished.\n\n", current_round_number);
  fflush(stdout);

  // send signal to all players to stop send signals (ball)
  // the team lead will pass the number of balls that the team have to the parent
  for (i = 0; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
  {
    kill(team1[i], SIGHUP);
    kill(team2[i], SIGHUP);
  }

  read_message_fifo(TEAM1FIFO, message);  // Open the public FIFO2 for reading for the team1
  read_message_fifo(TEAM2FIFO, message2); // Open the public FIFO2 for reading for the team2

  team1_round_balls = atoi(message);
  team2_round_balls = atoi(message2);

  printf("\n\nRound #%d finished and current balls are:\n\tteam 1   \tteam 2     \n\t  %d\t\t  %d\n", current_round_number, team1_round_balls, team2_round_balls);
  fflush(stdout);
}

void init_signals_handlers()
{
  if (sigset(SIGUSR1, signal_handler_throw_newball) == -1)
  { // throw the ball from parent to team lead, or from team lead to other team lead
    perror("Signal Error\n");
    exit(-1);
  }
  if (sigset(SIGUSR2, signal_handler_throw_newball) == -1)
  { // throw the ball from parent to team lead, or from team lead to other team lead
    perror("Signal Error\n");
    exit(-1);
  }
  if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
  { // throw the ball from parent to team lead, or from team lead to other team lead
    perror("Signal Error\n");
    exit(-1);
  }
}
// function signal_handler_SIGALRM
void signal_handler_SIGALRM(int sig)
{
  is_alarmed = 1;
  printf("The signal %d reached the parent, the game is finished.\n\n", sig);
  fflush(stdout);
}
void signal_handler_throw_newball(int sig)
{
  if (sig == SIGUSR1)
  {
    printf("The signal %d reached the parent,send ball to team #1 lead .\n\n", sig);
    sleep(1);
    kill(team1[5], sig);
    send_message_fifo(GUIFIFO, "1"); // send signal to the gui to throw new ball to team1

    sprintf(ball_number, "%d", current_ball_number); // send signal to the gui to throw new ball to team2
    send_message_fifo(TEAM1FIFO, ball_number);       // send number of ball to the team lead
    current_ball_number++;
  }
  else if (sig == SIGUSR2)
  {
    printf("The signal %d reached the parent,send ball to team #2 lead .\n\n", sig);
    sleep(1);
    kill(team2[5], sig);
    send_message_fifo(GUIFIFO, "2"); // send signal to the gui to throw new ball to team2

    sprintf(ball_number, "%d", current_ball_number); // send signal to the gui to throw new ball to team2
    send_message_fifo(TEAM2FIFO, ball_number);       // send number of ball to the team lead
    current_ball_number++;
  }
}

void calculateRoundScores()
{
  if (team1_round_balls < team2_round_balls)
  {
    team1_score++;
    printf("Team 1 won the round\n");
    fflush(stdout);
  }
  else if (team1_round_balls > team2_round_balls)
  {
    team2_score++;
    printf("Team 2 won the round\n");
    fflush(stdout);
  }
  else
  {
    printf("The round is draw,so the scores is unchanged\n");
    fflush(stdout);
  }
}

void create_fifos_players()
{
  for (i = 1; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
  {
    sprintf(player_fifo_name, "/tmp/TEAM1FIFO%d", i);
    sprintf(player_fifo_name2, "/tmp/TEAM2FIFO%d", i);
    createFifo(player_fifo_name);
    createFifo(player_fifo_name2);
  }
}

void killAllPlayers()
{
  printf("\nStart kill all players\n");
  for (i = 0; i < NUMBER_OF_PLAYERS_In_TEAM; i++)
  {
    kill(team1[i], SIGQUIT);
    kill(team2[i], SIGQUIT);
  }
}