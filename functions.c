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
