#include "header.h"
#include "constants.h"
#include <stdbool.h>

bool isPassedCorrectArguments(int argc, char** argv, char* message_error,int number_of_arguments){
    if ( argc != number_of_arguments ){//check if the user passed the correct arguments
      // Use the default file names
      printf("Usage: Invalid arguments.\n");
      printf("%s\n", message_error);
      return false;
    }
  else {
      //the user passed the correct arguments
       printf("The user passed the correct arguments.\n");
       return true;
  }
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