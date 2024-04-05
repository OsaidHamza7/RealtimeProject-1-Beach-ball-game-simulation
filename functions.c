#include "header.h"


void send_message_fifo(char* team_fifo_name,char* message){

    int fifo;
    /* Open the public FIFO for reading and writing */
    if ((fifo = open(team_fifo_name, O_WRONLY)) == -1)
    {
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


void createFifo(char* fifo_name)
{
    // remove fifo if it's exist
    remove(fifo_name);

    if ((mkfifo(fifo_name, S_IFIFO | 0777)) == -1)
    {
        perror("Error Creating Fifo");
        exit(-1);
    }
}


void read_message_fifo(char* team_fifo_name,char* message){

    int fifo;
    // Open the public FIFO for reading
    if ((fifo = open(team_fifo_name, O_RDONLY)) == -1)
    {
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



void split_string(char* argv,int arr[]){
    char *token = strtok(argv, " ");

    if (token != NULL) {
        arr[0] = atoi(token);
        token = strtok(NULL, " ");
        if (token != NULL) {
            arr[1] = atoi(token);
        } else {
            printf("Expected another argument integer but got NULL\n");
        }
    } else {
        printf("Expected a argument integer but got NULL\n");
    }
    fflush(stdout);
    return ;
}