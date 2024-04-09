#include "header.h"

int NUMBER_OF_LOST_ROUNDS = 5;
int SIMULATION_THRISHOLD = 500;
int ROUND_TIME = 15;
int RANGE_ENERGY[2] = {1, 10};

void send_message_fifo(char *team_fifo_name, char *message)
{

    int fifo;
back2:
    /* Open the public FIFO for reading and writing */
    if ((fifo = open(team_fifo_name, O_WRONLY)) == -1)
    {
        if (errno == EINTR)
        {
            printf("Write error inruptted %d\n", getpid());
            fflush(stdout);
            goto back2;
        }
        perror("open fifo error");

        fflush(stdout);
        exit(1);
    }
    if (write(fifo, message, sizeof(message)) == -1)
    {
        perror("write in fifo error");
        fflush(stdout);
        exit(1);
    }
    close(fifo);
}

void createFifo(char *fifo_name)
{
    // remove fifo if it's exist
    remove(fifo_name);

    if ((mkfifo(fifo_name, S_IFIFO | 0777)) == -1)
    {
        perror("Error Creating Fifo");
        exit(-1);
    }
}

void read_message_fifo(char *team_fifo_name, char *message)
{

    int fifo;
    // Open the public FIFO for reading
back1:
    if ((fifo = open(team_fifo_name, O_RDONLY)) == -1)
    {
        if (errno == EINTR)
        {
            printf("Read error inruptted %d \n", getpid());
            fflush(stdout);
            goto back1;
        }
        perror("open fifo error");
        fflush(stdout);
        exit(1);
    }
    if (read(fifo, message, sizeof(message)) == -1)
    {
        perror("read in fifo error");
        fflush(stdout);
        exit(1);
    }
    close(fifo);
}

void split_string(char *argv, int arr[])
{
    char *token = strtok(argv, " ");
    int i = 0;
    while (token != NULL)
    {
        arr[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
    }
}

void readArgumentsFromFile(char *filename)
{
    char line[200];
    char label[50];

    FILE *file;
    file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("The file not exist\n");
        exit(-2);
    }
    char separator[] = " ";

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);

        if (strcmp(label, "NUMBER_OF_LOST_ROUNDS") == 0)
        {
            NUMBER_OF_LOST_ROUNDS = atoi(str);
        }

        else if (strcmp(label, "SIMULATION_THRISHOLD") == 0)
        {
            SIMULATION_THRISHOLD = atoi(str);
        }
        else if (strcmp(label, "ROUND_TIME") == 0)
        {
            ROUND_TIME = atoi(str);
        }
        else if (strcmp(label, "RANGE_ENERGY") == 0)
        {
            RANGE_ENERGY[0] = atoi(str);
            str = strtok(NULL, separator);
            RANGE_ENERGY[1] = atoi(str);
        }
    }
    fclose(file);
}
