#ifndef PLAYER_H
#define PLAYER_H

#include "header.h"

extern char message[BUFSIZ];
extern int delayToThrowSecondBall;
extern int team_signal_number;
extern int other_team_signal_number;
extern int player_number_in_team;
extern int player_team_number;
extern int is_team_lead;
extern int number_balls_player;
extern int number_balls_team;
extern int next_player_pid, next_players_pids[2];
extern char player_fifo_name[20], next_player_fifo_name[20];
extern int team_fifo;
extern int other_team_number;
extern char ball_number[5];

extern int is_round_finished;
extern int is_alarmed;
extern int energy;
extern int energy_change_per_second;
extern int sleep_status;
extern int pause_time;
extern int energy_range[2];
extern int reached_signal;
extern int current_ball_number;
extern int previous_ball_number;
extern int previous_player_pid;

// Function prototypes
void get_information_player(char **argv);
void init_signals_handlers();
void signal_handler_throw_leader(int sig);
void signal_handler_throw_players(int sig);
void signal_handler_stop_throwing(int sig);
void signal_handler_SIGALRM(int sig);
int get_random_energy(int min, int max);
int calculate_pause_time();
int apply_pause_time();
void process_interrupt();

#endif