#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';
        command_count = 0;

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */
        int pipes [command_count-1][2];
	    for (int i = 0; i < command_count; i++){
		pipe(pipes[i]);
		if (pipes[i] == -1){
			perror("pipe failed");
			exit(1);
		    }
	    }
        int PID[command_count];
        for (int i = 0; i < command_count; i++) 
        {
            char *args[256];
            int arg_count = 0;
            char *start_ptr = commands[i];
            char *end_ptr = commands[i];
            int in_quotes = 0;

            while (*end_ptr != '\0') {
                if (*end_ptr == '\"') {
                    in_quotes = !in_quotes;
                } else if (*end_ptr == ' ' && !in_quotes) {
                    *end_ptr = '\0';
                    if (start_ptr[0] == '\"' && start_ptr[strlen(start_ptr)-1] == '\"') {
                        start_ptr[strlen(start_ptr)-1] = '\0';
                        start_ptr++;
                    }
                    args[arg_count++] = start_ptr;
                    start_ptr = end_ptr + 1;
                }
                end_ptr++;
            }
            if (start_ptr[0] == '\"' && start_ptr[strlen(start_ptr)-1] == '\"') {
                start_ptr[strlen(start_ptr)-1] = '\0';
                start_ptr++;
            }
            args[arg_count++] = start_ptr;
            args[arg_count] = NULL;

            switch(PID[i] = fork())
            {
                case -1:
                    perror("fork failed");
                    exit(1);
                case 0:
                    if (i != 0){
                        close(pipes[i-1][1]);
                        dup2(pipes[i-1][0], STDIN_FILENO); 
                        close(pipes[i-1][0]);
                    } 
                    if (i != command_count-1){
                        close(pipes[i][0]);
                        dup2(pipes[i][1], STDOUT_FILENO);
                        close(pipes[i][1]);
                    }
                    execvp(args[0], args);
                    perror("execvp failed");
                    exit(1);
                
                default:
                    wait(NULL);
                    if (i != 0){
                        close(pipes[i-1][0]);
                    }
                    if (i != command_count-1){
                        close(pipes[i][1]);
                    }
            }    
        }  
    }
    return 0;
}




// char *args[256];
//     char *token = strtok(command, " ");
//     int arg_count = 0;
//     while (token != NULL) 
//     {
//         if (token[0] == '"' && token[strlen(token)-1] == '"') {
//         token[strlen(token)-1] = '\0';
//         token++;
//         }
//         args[arg_count++] = token;
//         token = strtok(NULL, " ");
//     }
//     args[arg_count] = NULL;


// char *args[256];
    // int arg_count = 0;
    // char *start_ptr = command;
    // char *end_ptr = command;
    // int in_quotes = 0;

    // while (*end_ptr != '\0') {
    //     if (*end_ptr == '\"') {
    //         in_quotes = !in_quotes;
    //     } else if (*end_ptr == ' ' && !in_quotes) {
    //         *end_ptr = '\0';
    //         if (start_ptr[0] == '\"' && start_ptr[strlen(start_ptr)-1] == '\"') {
    //             start_ptr[strlen(start_ptr)-1] = '\0';
    //             start_ptr++;
    //         }
    //         args[arg_count++] = start_ptr;
    //         start_ptr = end_ptr + 1;
    //     }
    //     end_ptr++;
    // }
    // if (start_ptr[0] == '\"' && start_ptr[strlen(start_ptr)-1] == '\"') {
    //     start_ptr[strlen(start_ptr)-1] = '\0';
    //     start_ptr++;
    // }
    // args[arg_count++] = start_ptr;
    // args[arg_count] = NULL;