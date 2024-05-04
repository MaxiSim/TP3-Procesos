#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int child(int * index, int read_fd, int write_fd){
	int readed;
	read(read_fd, &readed, sizeof(int));
	printf("Soy el hijo %i y recibí %i\n", *index, readed);
	readed++;
	write(write_fd, &readed, sizeof(int));
	return 0;}

int parent(int read_fd){
	int recieved = 0;
	read(read_fd, &recieved, sizeof(int));
	printf("Soy el padre y recibi %i\n", recieved);
	return 0;}
	
int main(int argc, char **argv){	
	int start, status, pid, n;
	int buffer[1];
	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    /* Parsing of arguments */
	n = atoi(argv[1]);
	buffer[0] = atoi(argv[3]);
	start = atoi(argv[2]);
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
	int pipes [n][2];
	for (int i = 0; i < n; i++){
		pipe(pipes[i]);
		if (pipes[i] == -1){
			perror("pipe failed");
			exit(1);
		}
	}
	int * PID = (int *) malloc(n * sizeof(int));
	for (int i = 0; i < n; i++){
		switch (PID[i] = fork()){
		case -1:
			perror("fork failed");
			exit(1);
		case 0: // pipe[0] = read, pipe[1] = write
			return child(&i, pipes[i][0], pipes[(i+1)%n][1]);
		}
	}
	printf("Se crearon todos los hijos\n");
	write(pipes[start][1], &buffer[0], sizeof(int));
	int Status;
	for (int i = 0; i < n; i++){
		waitpid(PID[i], &Status, 0);
	}
	return parent(pipes[start][0]);
}


