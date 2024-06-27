# Beach Ball Game Simulation

A multi-processing application that simulates the behavior of 2 teams of players playing beach ball at the Gaza sea shore using signals and pipes facilities.

## Demo 


https://github.com/OsaidHamza7/RealtimeProject-1-Beach-ball-game-simulation/assets/108472343/ede7caa8-a0c1-42f4-9da5-a34547a89b0b



## Project Description

This project simulates a beach ball game between two teams, Team A and Team B, at the Gaza sea shore. Each team consists of 5 players and a team lead. The simulation involves passing balls among team members, with the goal of keeping the game active for a user-defined amount of time. The team with the most balls at the end of each round loses.

## Features

- Two teams (Team A and Team B) with 5 players each and a team lead.
- Randomized ball passing among team members based on energy levels.
- Handling of dropped balls and re-collection time.
- Parent application manages the game and introduces new balls.
- User-defined game duration and number of rounds.
- Determination of the losing team based on the number of balls they possess.

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/OsaidHamza7/RealtimeProject-1-Beach-ball-game-simulation.git
    ```
2. Navigate to the project directory:
    ```bash
    cd RealtimeProject-1-Beach-ball-game-simulation
    ```
3. Build the project:
    ```bash
    make all
    ``` 

## Usage

1. Run the simulation:
    ```bash
    make run
    ```
2. Set the user-defined game duration and number of rounds as prompted.
3. The simulation will run for the specified time, displaying the ball passing and team activities.
4. At the end of each round, the results will be displayed, and the game will either continue or end based on the specified conditions.


## Authors
* **Osaid Hamza - Team Leader**
* Razan Abdelrahman
* Misam Alaa
* Ansam Rihan

