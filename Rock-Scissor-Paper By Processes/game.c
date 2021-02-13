#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 25
#define READ_END	0
#define WRITE_END	1

//implemented functions
int won(char *parentSide, char *childSide);
int pick();


#define ELEMENT_NUM 3
char elements[ELEMENT_NUM][10] = {"ROCK", "PAPER", "SCISSOR"};

int main(void)
{
	printf("Loading game settings...\n");
	int winner = -1, round = 0;
    char* choice;
	int parent = 0, child = 0;

    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];

    pid_t pid; 

    int fd[2];    // parent to child
	int fd1[2];  // child to parent

	if (pipe(fd) == -1) { fprintf(stderr,"Pipe failed"); return 1;}
	if (pipe(fd1) == -1) { fprintf(stderr,"Pipe failed"); return 1;}

	printf("The game has launched\n");

	int parentID = getpid();
	printf("Parent joined the game with PID:%d\n",parentID);

	pid = fork();
	if(pid == 0){
		printf("Child process is created\n");
		printf("Child joined the game with PID:%d\n",getpid());
		printf("\nGame starts..\n");
	}
		
	while(1){
	if (pid < 0) { fprintf(stderr, "Fork failed"); return 1; }

	if (pid > 0) { 
		strcpy(write_msg,elements[pick()]);

		write(fd[WRITE_END], write_msg, strlen(write_msg)+1); 

		//listen child
		close(fd1[WRITE_END]);
		read(fd1[READ_END], read_msg, BUFFER_SIZE);
		if(strcmp(read_msg,"Game Over") == 0){
			printf("\nParent waits child with wait()...\n");
			wait(NULL);
			close(fd[WRITE_END]);
			printf("Parent has termianted\n");
			return 0;
		}
	}
	else { /* child process */
	
	
		choice = elements[pick()];
		
		//listen parent
		read(fd[READ_END], read_msg, BUFFER_SIZE);
		char * parentsChoice = read_msg;
		
        winner = won(choice,parentsChoice);
		printf("--\n");

        	if(winner == 0){
            	printf("Round %2d parent:%-8s- child:%s\n",++round,choice,parentsChoice);
				printf("Parent - score: p:%d-c:%d \n",++parent,child);
            }
            else if(winner == 1){
                printf("Round %2d parent:%-8s- child:%s\n",++round,choice,parentsChoice);
				printf("Child - score: p:%d-c:%d \n",parent,++child);
            }
            else if(winner == 2){
                printf("Round %2d parent:%-8s- child:%s\n",++round,choice,parentsChoice);
				printf("Draw - score: p:%d-c:%d \n",parent,child);
            }

            if(child == 5){
                printf("\nGame Over!!\n\nChild is the winner with PID:%d\n\nparent:%d-child:%d\n",getpid(),parent,child);
				char *message = "Game Over";
				close(fd1[READ_END]);
				write(fd1[WRITE_END], message, strlen(message)+1);
				close(fd1[WRITE_END]);
				printf("Child terminates...\n");
                return 0;
            }
			else if(parent == 5){
				printf("\nGame Over!!\nScore: parent:%d-child:%d\nParent is the winner with PID:%d\n",parent,child,parentID);
				char *message = "Game Over";
				close(fd1[READ_END]);
				write(fd1[WRITE_END], message, strlen(message)+1);
				close(fd1[WRITE_END]);
				printf("Child terminates...\n");
                return 0;

			}
			else{
				char * message = "Go On";
				close(fd1[READ_END]);
				write(fd1[WRITE_END], message, strlen(message)+1);
				
		}
		
	}

	}
	return 0;
}


int won(char *parentSide, char *childSide){
    
    if(strcmp(parentSide,"ROCK") == 0 && strcmp(childSide,"PAPER") == 0)
        return 1;
    else if(strcmp(parentSide,"ROCK") == 0 && strcmp(childSide,"SCISSOR") == 0)
        return 0;
    else if(strcmp(parentSide,"PAPER") == 0 && strcmp(childSide,"ROCK") == 0)
        return 0;
    else if(strcmp(parentSide,"PAPER") == 0 && strcmp(childSide,"SCISSOR") == 0)
        return 1;
    else if(strcmp(parentSide,"SCISSOR") == 0 && strcmp(childSide,"ROCK") == 0)
        return 1;
    else if(strcmp(parentSide,"SCISSOR") == 0 && strcmp(childSide,"PAPER") == 0)
        return 0;
    else if(strcmp(parentSide,childSide) == 0 )
        return 2;
    return -1;
}


int pick(){
    return random()%getpid()%ELEMENT_NUM;
}
