#include "player.h"

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
    alarm(energy_change_per_second);

    energy = get_random_energy(energy_range[0], energy_range[1]);

    // initialize the signal handlers
    init_signals_handlers();
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
    if (signal(SIGUSR1, signal_handler_throw_leader) == SIG_ERR)
    { // throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    // only team lead can catch the signal SIGUSR2,from parent or from other team lead
    if (signal(SIGUSR2, signal_handler_throw_leader) == SIG_ERR)
    { // throw the ball from parent to team lead, or from team lead to other team lead
        perror("Signal Error\n");
        exit(-1);
    }

    // signal SIGTERM is used to throw the ball between players
    if (signal(SIGTERM, signal_handler_throw_players) == SIG_ERR)
    { // throw the ball between players
        perror("Signal Error\n");
        exit(-1);
    }

    // SIGALARM is used for changing the energy of the player every specific time
    if (sigset(SIGALRM, signal_handler_SIGALRM) == -1)
    {
        perror("Signal Error\n");
        exit(-1);
    }
    // catch the signal from parent to stop send signals to players
    if (signal(SIGHUP, signal_handler_stop_throwing) == SIG_ERR)
    { // catch the signal from parent to stop send signals to players
        perror("Signal Error\n");
        exit(-1);
    }
}

void signal_handler_throw_leader(int sig)
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
        previous_ball_number = atoi(message);

        if (is_team_lead == 1)
        {
            previous_player_pid = next_players_pids[0];
        }

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

void signal_handler_throw_players(int sig)
{ // SIGTERM is used to throw the ball between players
    printf("The signal %d, reached to player #%d ,team #%d\n", sig, player_number_in_team, player_team_number);
    is_round_finished = 0;
    number_balls_player++;
    reached_signal = sig;

    read_message_fifo(player_fifo_name, message); // read it from the parent

    if (number_balls_player == 2)
    {
        previous_ball_number = atoi(message);

        if (is_team_lead == 1)
        {
            previous_player_pid = next_players_pids[1];
        }

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
void signal_handler_stop_throwing(int sig)
{
    is_round_finished = 1;
    number_balls_player = 0;

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

void process_interrupt()
{
    // check if the signal is SIGALRM

    if (is_alarmed == 1)
    {
        is_alarmed = 0;
        return; // continue
    }

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
            number_balls_team--;

            sleep(1);                                             // pause time
            kill(next_players_pids[1], other_team_signal_number); // team lead catch the ball back from player 5,so throw it to the other team lead

            sprintf(message, "%d %d %d", other_team_number, 6, current_ball_number);
            send_message_fifo(GUIFIFO, message);
            sleep(1);

            sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO", other_team_number);
            printf("@@@@@@@@@%s\n", next_player_fifo_name);
            sprintf(ball_number, "%d", current_ball_number);       // send signal to the gui to throw new ball to team2
            send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

            // send a message to fifo to inform the player that the ball is thrown to the next player
            // number_balls_player = 0;
            if (number_balls_team == 0)
            { // if the team has no balls, then team lead will send signal to the parent to throw a new ball
                // sleep(1);
                printf("The team #%d has no balls, so send signal to the parent to throw a new ball\n", player_team_number);
                fflush(stdout);
                kill(getppid(), team_signal_number); // send signal SIGUSR1 to the parent to throw a new ball
            }
            goto skip;
        }

        else
        {
            next_player_pid = next_players_pids[0];
            sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO1", player_team_number);
        }
    }

    // normal player or team lead with signal SIGUSR

    if (apply_pause_time() == 0)
        return;

    printf("finish SIGUSR or SIGTERM,throw ball next player %d\n", next_player_pid);
    fflush(stdout);
    kill(next_player_pid, SIGTERM);
    sleep(1);

    sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, current_ball_number);
    send_message_fifo(GUIFIFO, message);

    printf("@@@@@@@@%s\n", next_player_fifo_name);

    sprintf(ball_number, "%d", current_ball_number);       // send signal to the gui to throw new ball to team2
    send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

skip:
    if (number_balls_player == 2)
    {
        printf("SIGUSR 2 balls\n");
        sleep(delayToThrowSecondBall);
        // throw the second ball to the next player
        sprintf(ball_number, "%d", previous_ball_number); // send signal to the gui to throw new ball to team2

        if (is_team_lead == 1)
        {
            //  Reached the signal SIGUSR then signal SIGTERM
            if (reached_signal == SIGTERM)
            {
                number_balls_team--;

                next_player_pid = next_players_pids[1];

                sprintf(message, "%d %d %d", other_team_number, 6, previous_ball_number);
                send_message_fifo(GUIFIFO, message);
                sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO", other_team_number);

                kill(next_player_pid, other_team_signal_number);       // team lead catch the ball back from player 5,so throw it to the other team lead
                send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead
                goto skip2;
            }

            else
            { // Reached signal is SIGUSR then signal SIGUSR
                sprintf(next_player_fifo_name, "/tmp/TEAM%dFIFO1", player_team_number);
                next_player_pid = previous_player_pid;
            }
        }

        sprintf(message, "%d %d %d", player_team_number, player_number_in_team % 6 + 1, previous_ball_number);
        send_message_fifo(GUIFIFO, message);

        printf("@@@@@@@@%s\n", next_player_fifo_name);

        kill(next_player_pid, SIGTERM);
        send_message_fifo(next_player_fifo_name, ball_number); // send number of ball to the team lead

    skip2:
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