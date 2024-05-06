#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

int child (int * index, int read_fd, int write_fd, char * command, int last_command){
    // printf("Soy el hijo %i y ejecuto %s\n", *index, command);
    dup2(read_fd, 0);
    close(read_fd);
    char *args[256];
    char *token = strtok(command, " ");
    int arg_count = 0;
    while (token != NULL) 
    {
        if (token[0] == '"' && token[strlen(token)-1] == '"') {
        token[strlen(token)-1] = '\0';
        token++;
        }
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;
    // for (int i = 0; i < arg_count; i++)
    //     printf("args: %s\n", args[i]);

    if (last_command){
        // printf("Last command\n");
        // aca iria el execvp
        execvp(args[0], args);
        perror("Error en execvp\n");
        return 0;
    }

    dup2(write_fd, 1);
    close(write_fd);
    // aca iria el execvp
    execvp(args[0], args);
    perror("Error en execvp\n");
    return 0;
}

int parent (){
    printf("Soy el padre\n");
    return 0;
}

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
        int * PID = (int *) malloc(command_count * sizeof(int));
        for (int i = 0; i < command_count; i++) 
        {
            if (strcmp(commands[i], "q") == 0){
                printf("Exiting shell\n");
                exit(0);
            };
            printf("Command %d: %s\n", i, commands[i]);
            switch(PID[i] = fork())
            {
                case -1:
                    perror("fork failed");
                    exit(1);
                case 0:
                    if (i == command_count-1){
                        return child(&i, pipes[i][0], 1, commands[i], 2);
                    } else if (i != 0){
                        return child(&i, pipes[i][0], pipes[(i+1)%command_count][1], commands[i], 1);
                    } else {
                        return child(&i, 0, pipes[(i+1)%command_count][1], commands[i], 0);
                    }
            }
        }    
        int Status;
        for (int i = 0; i < command_count; i++){
            waitpid(PID[i], &Status, 0);
        }
    }
    return 0;
}
