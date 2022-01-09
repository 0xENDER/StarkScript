/**
 *
 *  Manage the preprocessor
 *
**/


// The first thing the preprocessor needs to do is combine all the files into a "mega input file"
// This is the combining format that the preprocessor is going to follow:
//
//
// (?) "<content>" represents random code, and "<import>" represents an importing narrative
//
// A file with this content:
//    ______________________________________________________
//   |                                                      |
//   |   <content>                                          |
//   |   <import>                                           |
//   |   <import>                                           |
//   |   <content>                                          |
//   |______________________________________________________|
//
// Will be like this when converted to a "mega input file":
//    ______________________________________________________
//   |                                                      |
//   |   <content>                                          |
//   |   #file ./example1.stark ? /path/to/example1.stark   |
//   |   <content>                                          |
//   |   #endfile                                           |
//   |   #file ./example2.stark ? /path/to/example2.stark   |
//   |   <content>                                          |
//   |   #endfile                                           |
//   |   <content>                                          |
//   |______________________________________________________|
//
//
// In short words, the preprocessor is going to replace each the import narrative with two guiding
// lines with the content of the imported file placed between those two lines.
// Note the start and end of the imported file will be also marked with a "form feed new page"
// character and an "escape" character (\f & \e)! This is done so the user would be able to include
// more than one import narrative within one file, without losing track of the column value.
//
// The opening guiding line follows this format:
// "#file <library name/ID>/<relative file path> ? <absolute file path>"
// And the closing guiding line follows this format:
// "#endfile"
//
// Note that Import narratives is nest-able. (An import narrative is allowed to include other
// import narratives inside of it)

// Import the `ctype.h` library
// This library is useful for detecting data types! (e.g. detecting the type of a char)
#include <ctype.h>

// Import the `opnImportFile` function and its related functions
#include "./files.c"

// Import all the comments-related functions
#include "./comments.c"

// Import all the quotes-related functions
#include "./quotes.c"

// Define a function that can be used to generate a code path
char* createCodePath(const char *codePath, const char *path, int line, int column){

    // The code path (codePath) should follow the format of the `report` function's reporting path:
    //  ___________________________________________________________________
    // |                                                                   |
    // |  ...*<path_3>:L;C*<path_2>:L;C*<path_1>:L;C                       |
    // |___________________________________________________________________|

    // Allocate memory for the new code path
    char *newCodePath = calloc(strlen(codePath) + strlen(path) + 3 + 10 + 10 + 1, sizeof(char));
    sprintf(newCodePath, "%s:%d;%d*%s", path, line, column, codePath);

    // Return the new code path
    return newCodePath;

}

// Define a function to handle preprocessor errors
void preprocError(FILE *inputFile, FILE *outputFile, const char *msg, const char *codePath, const char *filePath, int line, int column, int length, int height){

    // Get the current code path
    char *currentCodePath = createCodePath(codePath, filePath, line, column);
    consoleDebug("Generated a code path: %s", currentCodePath);

    // Close the opened file streams
    fclose(inputFile);
    fclose(outputFile);

    // Report this error to the user
    report(currentCodePath, length, height, msg, REPORT_ERROR);

}

// Define a function to handle preprocessor warnings
void preprocWarn(const char *msg, const char *codePath, const char *filePath, int line, int column, int length, int height){

    // Get the current code path
    char *currentCodePath = createCodePath(codePath, filePath, line, column);
    consoleDebug("Generated a code path: %s", currentCodePath);

    // Report this error to the user
    report(currentCodePath, length, height, msg, REPORT_WARN);

    // Free the memory used by the `currentCodePath` variable
    free(currentCodePath);

}

// Pre-define the `preprocR` function so the injection function could use it!
void preprocR(FILE *inputFile, FILE *outputFile, int *processedFiles, const char *codePath,
                const char *filePath, const char *libsDir);

// Import all the functions that are related to the "import" keyword
#include "./import.c"

// Import all the functions that are related to the "use" keyword
#include "./use.c"

// Define a function that can be used recursively to process the file
void preprocR(FILE *inputFile, FILE *outputFile, int *processedFiles, const char *codePath,
                const char *filePath, const char *libsDir){

    // Update the files count
    (*processedFiles)++;

    // Get the first character in the file
    char currChar = fgetc(inputFile);

    // Keep track of the previous character
    char prvsChar = '\0';

    // You should watch out for quoted text, as quoted text may include valid statements, and we
    // should make sure to not process quoted text as normal statements!
    int inDoubleQuote = 0,
        inQuote = 0;

    // Keep track of comments, you don't want to accidentally process commented code!
    int inLinearComm = 0,
        inMultilinearComm = 0;

    // Watch out for the end of multi-linear comments! (It's important to never neglect whitespace
    // within multi-linear comments so the compiler won't lose track of the column count for any
    // code that may follow the comment)
    int multilinearCommEnd = 0,
        multilinearCommStart = 0;

    // Keep track of importing contexts
    int importContextStart = 0,
        useContextStart = 0,
        useContextStartColumn = 0;

    // Keep track of the lastest library path
    int latestLibPathSize = 1;
    char *latestLibPath = calloc(latestLibPathSize, sizeof(char));

    // Keep track of the line and the column numbers so you could inform the user of the exact
    // position of the source of errors/warnings
    int line = 1, column = 1;

    // Print a white space character for each character you deal with to keep the column value
    // intact
    int injectionWhitespace = 0;

    // Start a loop to look through all the characters within the input file
    // Only stop the loop when you reach the "end of file" character (EOF)
    while (currChar != EOF){

        // Keep track of the text quoting status and the commenting status!

        // Check for quoting contexts
        checkForQuotes(&inQuote, &inDoubleQuote, currChar, prvsChar, inLinearComm,
                        inMultilinearComm);

        // Check for and keep track of comment context
        checkForComments(&inputFile, &currChar, &inLinearComm, &inMultilinearComm,
                            &multilinearCommStart, &multilinearCommEnd, inQuote, inDoubleQuote,
                            &column);

        // If you reach a new line character, make sure to reset the text quoting status
        // Even though this might be wrong syntax, you should
        if(currChar == '\n') {

            // New lines can end a quoting context
            inDoubleQuote = 0;
            inQuote = 0;

            // New lines are the end of a linear comment
            inLinearComm = 0;

            // Update the line number and the column
            line++;
            column = 0;


        }

        // Notes:
        // inQuote & inDoubleQuote              A quoting context starts with the first quote and
        //                                      ends before the second quote.
        // inLinearComm                         A linear comment context starts with the first slash
        //                                      and ends before a new line character.
        // inMultilinearComm                    A multi-linear comment context starts with the first
        //                                      slash and ends with the last slash.

        // Check if you're in a context that does not include any comments...
        if(!inLinearComm && !inMultilinearComm){

            // Check if the current character is not whitespace
            if(currChar != ' ' && currChar != '\t'){

                // Note: move this chunk of code to the "use.c" file when you're done with it!
                // Warning: this chunk of code is not read yet!

                // Check if you're in a quoting context
                if(inQuote || inDoubleQuote) {

                    // Check if you're in a "use" context
                    if(useContextStart){

                        // There should be no strings within a "use" context!
                        preprocError(inputFile, outputFile, STR_ERROR_000001, codePath, filePath,
                                        line, column, 1, 0);

                    }

                }

                // Note: move this chunk of code to the "import.c" file when you're done with it!
                // Warning: this chunk of code is not read yet!

                // Check if you're in an "import" context
                if(importContextStart){

                    outputImport(&inputFile, &outputFile, &currChar, &injectionWhitespace, line,
                                    &column, codePath, filePath, inQuote, &inDoubleQuote,
                                    &importContextStart, processedFiles, libsDir);

                }

            }

        }

        // Check if you're in a context that can only include code that's intended to be ran
        if(!inQuote && !inDoubleQuote && !inLinearComm && !inMultilinearComm) {

            // The preprocessor is only responsible for filtering comments and importing files
            // The preprocessor is already filtering comments, so you need to take of importing
            // all the files used within the main file!

            // Note that you should not process any keyword contexts (like "import" and "use")
            // outside within a comments context. You may branch off to another function when you
            // check for the keywords themselves, or for any expected data that you know is there!
            // (You should not deal with any whitespace or redundant characters in such contexts)
            // This is done to make sure that comments (that may be inside a keyword's context) will
            // not trigger false errors or warnings, or cause any unwated special cases.
            // An example of this would be this:
            //  __________________________________________________________________
            // |                                                                  |
            // |  import /* my comment! */ "file.stark";                          |
            // |__________________________________________________________________|
            //
            // The while loop is written specifically to prevent such contexts from triggering false
            // errors. So you should always make sure to stay within the while loop when you are not
            // sure of the nature of the input/"input context" you're dealing with!

            // Note: move the `useContextStart` condition to the "use.c" file when you're done with
            // it!
            // Warning: this chunk of code is not read yet!

            // Check if the current character is not whitespace
            if(currChar != ' ' && currChar != '\t'){

                // Check if you're in a "use" context
                if(useContextStart){

                    outputUse(&inputFile, &outputFile, &currChar, prvsChar, &injectionWhitespace,
                                line, &column, codePath, filePath, processedFiles, libsDir,
                                &latestLibPath, &latestLibPathSize, &useContextStartColumn,
                                &useContextStart);

                }

            }

            // Check if you're in not in a "use" or "import" context
            if(!useContextStart && !importContextStart){

                // Search for an "import" context
                importContextStart = importContext(&inputFile, &prvsChar, &currChar, &column);

                // Check if you didn't reach an "import" context yet
                if(!importContextStart){

                    // Search for a "use" context
                    useContextStart = useContext(&inputFile, &prvsChar, &currChar, &column);

                    if(useContextStart){

                        useContextStartColumn = column;

                    }

                }

                // Check if a file will be added now
                if(importContextStart || useContextStart){

                    fprintf(outputFile, CHAR_SPECIAL_FILE_SEPARATOR);

                }

            }

        }

        // Check if you're not in a linear comment context
        if(!inLinearComm){

            // Check if you're in a multi-linear comment context
            if(inMultilinearComm){

                // Check if this is not an import context
                if(!importContextStart){

                    // Check if this is the end/start of a mutli-linear comment!
                    if(multilinearCommEnd || multilinearCommStart){

                        // Add two whitespace characters to the injection whitespace suffix
                        // variable
                        injectionWhitespace += 2; // Smthing is going on here...
                                                  // This does not make any sense, why is
                                                  // this working?

                    }

                }

                // Check if this is the end of a multi-linear comment
                if(multilinearCommEnd){

                    if(!importContextStart && !useContextStart){

                        // Print two whitespaces
                        fprintf(outputFile, "  ");

                    }

                    // Update the status of the multi-linear comment context
                    inMultilinearComm = 0;
                    multilinearCommEnd = 0;

                }else if(multilinearCommStart){ // Check if this is the start of a multi-linear
                                                // comment

                    if(!importContextStart && !useContextStart){

                        // Print two whitespaces
                        fprintf(outputFile, "  ");

                    }

                    // Update the multi-linear comment start variable
                    multilinearCommStart = 0;

                }else if(!importContextStart && !useContextStart){

                    // Print whitespace so you won't lose track of the column number
                    // And allow line line characters to be printed normally so you won't lose track
                    // of line numbers
                    // You need to watch for the "\r" special character when processing CRLF files!
                    // Also, this compiler will ignore the "\036" and "\025" special characters and
                    // treat them as whitespace...
                    // (For more about new lines: https://en.wikipedia.org/wiki/Newline)
                    fprintf(outputFile, "%c", ((currChar == '\n' || currChar == '\r') ?
                            currChar : ' '));

                }else{

                    // Add one whitespace character to the injection whitespace suffix variable
                    injectionWhitespace++;

                }

            }else if(!importContextStart && !useContextStart){

                // Print the output (temp)
                // Replace this with a function that will write the output content into the output
                // file pointer
                fprintf(outputFile, "%c", currChar);

                if(currChar == CHAR_SPECIAL_ESCAPE[0]){

                    // Check if there was any ignored characters in a use/import context
                    if(injectionWhitespace != 0){

                        // Print all the whitespace characters
                        for(int i = 0; i < injectionWhitespace; i++){

                            fprintf(outputFile, " ");

                        }

                        // Reset the injection whitespace characters count
                        injectionWhitespace = 0;

                    }

                }

            }else{

                // Add one whitespace character to the injection whitespace suffix variable
                injectionWhitespace++;

            }

        }

        // Update the column value
        column++;

        // Store the current character in the "prvsChar" variable
        prvsChar = currChar;

        // Get the next character
        currChar = fgetc(inputFile);

    }

    // Free up the memory used by the `latestLibPath` variable
    free(latestLibPath);

}

// Define a function that triggers the preprocessor
int preproc(char *inputPth, char *tempDir, char *inputDir, char *outputFileName, char *libsDir){

    // You are not allowed to modify any of this function's arguments

    // Keep track of the number of processed files
    int processedFiles = 0;

    // Open a file stream in read-only mode to read the content of the input file
    FILE *inputFile = fopen(inputPth, "r");

    // Check if the file stream wasn't opened successfully
    if(inputFile == NULL){

        // Tell the user that the input file is inaccessible
        consoleError("Couldn't access the input file!", 1);

    }

    // Get the path of the temporary "mega file"
    char *outputFileDir = joinDirFileExt(tempDir, outputFileName,
                                            STRING_IO_PREPROCESSOR_OUTPUT_EXT);

    // Create the temporary "mega file"
    FILE* outputFile = fopen(outputFileDir, "w");

    // Free the memory used by the "outputFileDir" variable
    free(outputFileDir);

    // Call the recursive function
    // Note that you have to use the `preprocError` function to report errors inside the recursive
    // function. This is done to prevent memory leaks on sudden termination!
    preprocR(inputFile, outputFile, &processedFiles, "<root>:0;0", inputPth, libsDir);

    // Close the output file stream
    fclose(outputFile);

    // Close the file stream
    fclose(inputFile);

    // Return the number of processed files
    return processedFiles;

}
