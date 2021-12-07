/**
 *
 *  Start the compiler
 *
**/


// Include the compiler flags
#include "./flags.h"

// Pre-define the `stopProcess` function so all the included files can use it
void stopProcess(int exitCode);

// Include all the console-related functions
#include "./console.c"

// Include the start message function
#include "./message.c"

// Include the command line arguments processing function
#include "./args.c"

// Define the main function
int main(int argc, char *argv[]){

    // Print the compiler start message
    printCmplrMsg();

    // Process the command line arguments
    processArgs(argc, argv);

    if(cmplrInputFilePth != NULL){

        // ...

        // Free the memory used by the `cmplrInputFilePth` and `cmplrOutputFilePth` variable
        free(cmplrInputFilePth);
        free(cmplrOutputFilePth);
        cmplrInputFilePth = NULL;
        cmplrOutputFilePth = NULL;

    }else{

        consoleError("No input file path was passed!");

    }

    // Print a warning about the state of the compiler
    consoleWarn("No output...");

    consoleError("Oops...");

    // Return a SUCCESS exit code
    return EXIT_SUCCESS;

}

// Define a function that stops the process
void stopProcess(int exitCode){

    // Free up any globally allocated variables
    // Note that there is no need to change the value of any of these variables to NUll (as we only
    // need to do that if we want to keep track of the allocation state of these variables)
    if(cmplrInputFilePth != NULL) {

        free(cmplrInputFilePth);

    }
    if(cmplrOutputFilePth != NULL){

        free(cmplrOutputFilePth);

    }

    // Use the built-in `exit` function to terminate the process
    exit(exitCode);

}