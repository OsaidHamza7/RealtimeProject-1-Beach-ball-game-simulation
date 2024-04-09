#include "header.h"

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

//***********************************************************************************
void get_information_player(char **argv);
void init_signals_handlers();
void signal_handler(int sig);
void signal_handler1(int sig);
void signal_handler3(int sig);
void signal_handler_SIGALRM(int sig);
int get_random_energy(int min, int max);
int calculate_pause_time();
int apply_pause_time();
void process_interrupt();
//***********************************************************************************

int main(int argc, char **argv)
{

    // check the number of arguments
    if (argc < 5)
    {
        perror("The user should pass the arguments like: player_number_in_team,player_team_number,next_player_pid,energy_range\n");
        exit(-1);
    }

    srand((unsigned)getpid()); // seed for the random function with the ID of the current process

    // get the pinformation of the player
    get_information_player(argv);

    // set the alarm to change the energy of the player every specific time
    // alarm(energy_change_per_second);

    energy = get_random_energy(energy_range[0], energy_range[1]);
    pause_time = calculate_pause_time();

    // initialize the signal handlers
    init_signals_handlers();

    // sleep(pause_time);
    while (1)
    {
        pause();

        // check the source of the interrupt signal and process it
        process_interrupt();
    }

    exit(0);
    return 0;
}

// function init_signals_handlers is used to initialize the signal handlers
void init_signals_handlers()
{
    // only team lead can catch the signal SIGUSR1 from parent or from other team lead
    if (signal(SIGUSR1, signal_handler) == SIG_ERR)
    { // throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    // only team lead can catch the signal SIGUSR2,from parent or from other team lead
    if (signal(SIGUSR2, signal_handler) == SIG_ERR)
    { // throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    if (signal(SIGTERM, signal_handler1) == SIG_ERR)
    { // throw the ball between players
        perror("Signal Error\n");
        exit(-1);
    }

    // SIGALARM is used for changing the energy of the player every specific time
    /*if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    {
        perror("Signal Error\n");
        exit(-1);
    }*/

    if (signal(SIGHUP, signal_handler3) == SIG_ERR)
    { // catch the signal from parent to stop send signals to players
        perror("Signal Error\n");
        exit(-1);
    }
}

void signal_handler(int sig)
{
    // team lead only,catch the signal from parent or from other team lead
    is_round_finished = 0;
    number_balls_team++;
    number_balls_player++;
    next_player_pid = next_players_pids[0]; // next player will be the first player in it's team
    reached_signal = sig;

    printf("Signal %d,player #%d,team #%d,PID=%d,next player=%d\n", sig, player_number_in_team, player_team_number, getpid(), next_players_pids[0]);
    fflush(stdout);

    read_message_fifo(player_fifo_name, message); // read it from the parent

    if (number_balls_player == 2)
    {

        if (is_team_lead == 1)
        {
            previous_player_pid = next_players_pids[0];
        }
        previous_ball_number = atoi(message);

        printf("Player #%d in Team %d has balls = %d\n", player_number_in_team, player_team_number, number_balls_player);
        fflush(stdout);
        // hold the signal SIGTERM
        if (sighold(SIGTERM) == -1)
        {
            perror("Signal SIGTERM Error\n");
            exit(-1);
        }

        // hold the signal SIGUSR
        if (sighold(team_signal_number) == -1)
        {
            perror("Signal SIGUSR Error\n");
            exit(-1);
        }
    }
    else
    {
        current_ball_number = atoi(message);
    }
    printf("*********************%s\n", message);
    fflush(stdout);
}

void signal_handler1(int sig)
{
    printf("The signal %d, reached to player #%d ,team #%d\n", sig, player_number_in_team, player_team_number);
    is_round_finished = 0;
    number_balls_player++;
    reached_signal = sig;

    read_message_fifo(player_fifo_name, message); // read it from the parent

    if (number_balls_player == 2)
    {
        if (is_team_lead == 1)
        {
            previous_player_pid = next_players_pids[1];
        }
        previous_ball_number = atoi(message);
        printf("Player #%d in Team %d has balls = %d\n", player_number_in_team, player_team_number, number_balls_player);
        fflush(stdout);
        // hold the signal SIGTERM
        if (sighold(SIGTERM) == -1)
        {
            perror("Signal SIGTERM Error\n");
            exit(-1);
        }
        // hold the signal SIGUSR
        if (sighold(team_signal_number) == -1)
        {
            perror("Signal SIGUSR Error\n");
            exit(-1);
        }
    }

    else
    {
        current_ball_number = atoi(message);
    }
    printf("*********************%s\n", message);
    fflush(stdout);
}

// function used to catch the signal from parent to stop send signals among players
void signal_handler3(int sig)
{
    is_round_finished = 1;
    printf("The signal %d, reached to player #%d ,team #%d ,stop sending signals\n", sig, player_number_in_team, player_team_number);
    fflush(stdout);
    if (is_team_lead == 1)
    {
        // call function to send the current number of balls for teams to the parent
        sprintf(message, "%d", number_balls_team);
        send_message_fifo(player_fifo_name, message);
        number_balls_team = 0;
    }
}

// function signal_handler_SIGALRM is used to change the energy of the player every specific time
void signal_handler_SIGALRM(int sig)
{
    is_alarmed = 1;
    if (energy != 1)
    {
        energy -= 1;
    }
    alarm(energy_change_per_second);
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
    int max_pause_time = 10;

    // Calculate base pause time inversely proportional to the energy
    int time = max_pause_time - (energy * max_pause_time) / 100;

    // Determine if the ball falls
    float probability_fall = (100 - energy) / 100.0;
    int ball_falls = ((float)rand() / (float)RAND_MAX) < probability_fall;

    // If the ball falls, calculate additional time inversely proportional to energy
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
    time = (time < 2) ? 2 : time;

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

void process_interrupt()
{
    // check if the signal is SIGALRM

    /*if (is_alarmed == 1)
    {
        is_alarmed = 0;
        return; // continue
    }*/

    // if the signal is SIGHUP, then the round is finished
    if (is_round_finished == 1)
    {
        is_round_finished = 0;
        return;
    }

    if (is_team_lead == 1)
    {
        if (reached_signal == SIGTERM)
        {

            printf("&&&&&&&Reached signal SIGTERM\n");
            next_player_pid = next_players_pids[1]; // next player is the other team lead

            sleep(1);                                             // pause time
            kill(next_players_pids[1], other_team_signal_number); // team lead catch the ball back from player 5,so throw it to the other team lead

            sprintf(message, "%d %d %d", other_team_number, 6, current_ball_number);
            send_message_fifo(GUIFIFO, message);
            sleep(1);

            sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO", other_team_number);
            sprintf(ball_number, "%d", current_ball_number);       // send signal to the gui to throw new ball to team2
            send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

            // send a message to fifo to inform the player that the ball is thrown to the next player

            /*if (number_balls_player == 2)
            {
                printf("Team Leader SIGTERM 2 balls\n");
                sleep(delayToThrowSecondBall);
                // throw the second ball to the next player
                if (is_team_lead == 1)
                {
                    next_player_pid = previous_player_pid;
                }
                kill(next_player_pid, SIGTERM);

                sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, previous_ball_number);
                send_message_fifo(GUIFIFO, message);

                sprintf(ball_number, "%d", previous_ball_number);      // send signal to the gui to throw new ball to team2
                send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

                // release the signal SIGTERM
                if (sigrelse(SIGTERM) == -1)
                {
                    perror("Signal SIGTERM Error\n");
                    exit(-1);
                }

                // release the signal SIGUSR
                if (sigrelse(team_signal_number) == -1)
                {
                    perror("Signal SIGUSR Error\n");
                    exit(-1);
                }
            }*/

            number_balls_player = 0;
            number_balls_team--;
            /*if (number_balls_team == 0)
            { // if the team has no balls, then team lead will send signal to the parent to throw a new ball
                // sleep(1);
                printf("The team #%d has no balls, so send signal to the parent to throw a new ball\n", player_team_number);
                fflush(stdout);
                kill(getppid(), team_signal_number); // send signal SIGUSR1 to the parent to throw a new ball
            }*/
            return;
        }

        else
        {
            next_player_pid = next_players_pids[0];

            /*kill(next_players_pids[0], SIGTERM); // next player is first player in the team
            printf("throw the ball to the next player %d\n", next_player_pid);
            fflush(stdout);

            sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, current_ball_number);
            send_message_fifo(GUIFIFO, message);

            sprintf(ball_number, "%d", current_ball_number); // send signal to the gui to throw new ball to team2
            send_message_fifo(team_fifo_name, ball_number);  // send number of ball to the team lead

            // printf("throw ball #1 from leader to player 1\n");
            pause_time = sleep(5);
            // team lead
            while (pause_time != 0)
            {
                // if the round is finished, then stop the ball
                if (is_round_finished == 1)
                {
                    printf("Pause is intrupted player #%d team #%d due to Round finished.\n", player_number_in_team, player_team_number);
                    fflush(stdout);
                    is_round_finished = 0;
                    return;
                }
                if (is_alarmed == 1)
                {
                    is_alarmed = 0;
                }

                // intrupted by signal SIGALARM,so continue the sleeping (while the round not finshed)
                pause_time = sleep(pause_time);
            }
            // printf("throw ball #2 from leader to player 1 next = %d\n", next_players_pids[0]);
            // fflush(stdout);

            kill(next_players_pids[0], SIGTERM); // next player is first player in the team

            sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, current_ball_number + 1);
            send_message_fifo(GUIFIFO, message);

            sprintf(ball_number, "%d", current_ball_number + 1); // send signal to the gui to throw new ball to team2
            send_message_fifo(team_fifo_name, ball_number);      // send number of ball to the team lead

            return;*/
        }
    }

    // normal player or team lead with signal SIGUSR first
    printf("Start The Pause Time\n");
    fflush(stdout);
    if (apply_pause_time() == 0)
        return;

    if (is_team_lead == 1)
    {
        next_player_pid = next_players_pids[0];
    }
    printf("finish SIGUSR or SIGTERM,throw ball next player %d\n", next_player_pid);
    fflush(stdout);
    kill(next_player_pid, SIGTERM);
    sleep(1);

    sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, current_ball_number);
    send_message_fifo(GUIFIFO, message);

    sprintf(ball_number, "%d", current_ball_number);       // send signal to the gui to throw new ball to team2
    send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

    if (number_balls_player == 2)
    {
        printf("SIGUSR 2 balls\n");
        sleep(delayToThrowSecondBall);
        // throw the second ball to the next player
        sprintf(ball_number, "%d", previous_ball_number); // send signal to the gui to throw new ball to team2

        if (is_team_lead == 1 && reached_signal == SIGTERM)
        {
            next_player_pid = next_players_pids[1];

            sprintf(message, "%d %d %d", other_team_number, 6, previous_ball_number);
            send_message_fifo(GUIFIFO, message);
            sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO", other_team_number);

            kill(next_player_pid, other_team_signal_number);       // team lead catch the ball back from player 5,so throw it to the other team lead
            send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead
        }

        else
        { // normal player or team lead with signal SIGUSR
            sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, previous_ball_number);
            send_message_fifo(GUIFIFO, message);

            kill(next_player_pid, SIGTERM);
            send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead
        }

        // release the signal SIGTERM
        if (sigrelse(SIGTERM) == -1)
        {
            perror("Signal SIGTERM Error\n");
            exit(-1);
        }

        // release the signal SIGUSR
        if (sigrelse(team_signal_number) == -1)
        {
            perror("Signal SIGUSR Error\n");
            exit(-1);
        }
    }

    number_balls_player = 0;
}