#include "player.h"

int team_signal_number;
int other_team_signal_number;
int player_number_in_team;
int player_team_number;
int is_team_lead;
int number_balls_player = 0;
int number_balls_team = 0;
int next_player_pid, next_players_pids[2];
char player_fifo_name[20], next_player_fifo_name[20];
int team_fifo;
int other_team_number;
char message[BUFSIZ];
int delayToThrowSecondBall = 2;
char ball_number[5];

int is_round_finished = 0;
int is_alarmed = 0;
int energy;
int energy_change_per_second = 10;
int sleep_status;
int pause_time;
int energy_range[2];
int reached_signal;
int current_ball_number;
int previous_ball_number;
int previous_player_pid;

//==============================================================================
void get_information_player(char **argv)
{

    player_number_in_team = atoi(argv[1]);
    player_team_number = atoi(argv[2]);
    other_team_number = (player_team_number == 1) ? 2 : 1;
    split_string(argv[4], energy_range);
    is_team_lead = (player_number_in_team == 6) ? 1 : 0; // team lead or not

    if (is_team_lead == 1)
    { // team lead
        // split the next player pid to 2 pids, the first one is the first player in the team, and the second one is the other team lead
        split_string(argv[3], next_players_pids);
        printf("Team leader Player #%d in team #%d with PID = %d And next player1=%d, next team lead=%d\n", player_number_in_team, player_team_number, getpid(), next_players_pids[0], next_players_pids[1]);
    }
    else
    { // normal player
        next_player_pid = atoi(argv[3]);
        printf("Player #%d from team #%d with PID = %d And next player PID = %d\n", player_number_in_team, player_team_number, getpid(), next_player_pid);
    }
    fflush(stdout);
    team_signal_number = (player_team_number == 1) ? SIGUSR1 : SIGUSR2;
    other_team_signal_number = (player_team_number == 1) ? SIGUSR2 : SIGUSR1;

    if (is_team_lead == 1)
    {
        sprintf(player_fifo_name, "/tmp/TEAM%dFIFO", player_team_number);
        sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO1", player_team_number);
    }
    else if (player_number_in_team == 5)
    {
        sprintf(player_fifo_name, "/tmp/TEAM%dFIFO%d", player_team_number, player_number_in_team);
        // next player fifo name
        sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO", player_team_number);
    }
    else
    {
        sprintf(player_fifo_name, "/tmp/TEAM%dFIFO%d", player_team_number, player_number_in_team);
        // next player fifo name
        sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO%d", player_team_number, player_number_in_team + 1);
    }

    // print player fifo name and next player fifo name
    printf("Player #%d in team #%d, Player FIFO Name: %s, Next Player FIFO Name: %s\n", player_number_in_team, player_team_number, player_fifo_name, next_player_fifo_name);
}

int get_random_energy(int min, int max)
{
    if (min > max)
    {
        // printf("Error: min should be less than or equal to max.\n");
        return -1; // Return an error code
    }
    // Calculate the range and generate a random number within that range
    int range = max - min + 1;
    int randomNumber = rand() % range + min;

    return randomNumber;
}

// function calculate_and_apply_pause is used to calculate the pause time for the player
int calculate_pause_time()
{

    // Define the maximum pause time when energy is zero
    int max_pause_time = 15;

    // Calculate base pause time inversely proportional to the energy
    int time = max_pause_time - (energy * max_pause_time) / 100;

    // Determine if the ball falls
    float probability_fall = (100 - energy) / 100.0;
    int ball_falls = ((float)rand() / (float)RAND_MAX) < probability_fall;

    // If the ball falls, calculate additional time to re-collicting the ball (inversely proportional to energy)
    if (ball_falls)
    {
        int max = 5; // Maximum additional time
        int min = 1; // Minimum additional time
        // Scale additional time based on energy: lower energy gets more time
        int additional_time = min + (int)((max - min) * (1 - ((float)energy / 100)));
        additional_time = additional_time + rand() % (max - additional_time + 1);
        time += additional_time;
        printf("Ball Failled for %d,Player #%d Team #%d,Energy: %d%%, Pausing for %d seconds...\n", additional_time, player_number_in_team, player_team_number, energy, time);
        fflush(stdout);
        return time;
    }
    // Ensure pause time is at least 4 second
    time = (time < 4) ? 4 : time;

    printf("Player #%d Team #%d,Energy: %d%%, Pausing for %d seconds...\n", player_number_in_team, player_team_number, energy, time);
    fflush(stdout);
    return time;
}

int apply_pause_time()
{

    pause_time = sleep(calculate_pause_time());
    // team lead
    while (pause_time != 0)
    {
        // if the round is finished, then stop the ball
        if (is_round_finished == 1)
        {
            printf("Pause is intrupted player #%d team #%d due to Round finished.\n", player_number_in_team, player_team_number);
            fflush(stdout);
            is_round_finished = 0;
            return 0;
        }
        if (is_alarmed == 1)
        {
            is_alarmed = 0;
        }
        // intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
        pause_time = sleep(pause_time);
    }
    return 1;
}
