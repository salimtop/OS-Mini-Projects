#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define THREAD_NUM 3
#define ELEMENT_NUM 3
#define GAME_FINISHED 0
#define DRAW 3
#define WAIT 4

char * pick();
int won(char thread[ELEMENT_NUM][10]);

char elements[ELEMENT_NUM][10] = {"ROCK", "PAPER", "SCISSOR"};

pthread_t tid[3];
int threadID = 0;
pthread_mutex_t lock;
pthread_mutex_t element_lock[3];

int roundCounter = 1;

char playerChoices[THREAD_NUM][10];
int scores[3] = {0};
int elementCount[3] = {0};//These will be locked&unlocked in the pick() function. 

//compile with -lpthread
void* gamearea(void *arg){
    pthread_mutex_lock(&lock);
    int id = threadID++;
    pthread_mutex_unlock(&lock);
    int roundNum = 0;
    printf("thread %d joined the game\n",id+1);

    while(roundCounter){ //game loop
        if(roundNum < roundCounter){
            pthread_mutex_lock(&lock);
            strcpy(playerChoices[id],pick());
            pthread_mutex_unlock(&lock);
            roundNum++;
        }
        if(id == 2){//this thread dominates the game
            pthread_mutex_lock(&lock);//No one shouldn't change its choice
            int winner = won(playerChoices);
            if(winner != WAIT){
                if(winner == 0 || winner == 1 || winner == 2){
                    scores[winner]++;
                    printf("%d. Turn, 1.Thread: %s, 2.Thread: %s, 3.Thread: %s\n",roundCounter++,playerChoices[0],playerChoices[1],playerChoices[2]);
                    printf("%d. Thread win, Score: %d – %d - %d\n\n",winner+1,scores[0],scores[1],scores[2]);
                    if(scores[winner] >= 5){
                        printf("%d.Thread has won the game with score: Score: %d – %d – %d in %d Turns.\n\n",winner+1,scores[0],scores[1],scores[2],roundNum);
                        roundCounter = GAME_FINISHED;
                        printf("how many times items were selected: SCISSORS: %d, PAPER: %d, ROCK: %d\n\n",elementCount[2],elementCount[1],elementCount[0]);
                    }
                }
                else  if(winner == DRAW){
                    printf("%d. Turn, 1.Thread: %s, 2.Thread: %s, 3.Thread: %s\n",roundCounter++,playerChoices[0],playerChoices[1],playerChoices[2]);
                    printf("Draw, Score: %d – %d - %d\n\n",scores[0],scores[1],scores[2]);
                }
                for(int i = 0; i < THREAD_NUM; i++)
                    strcpy(playerChoices[i],"WAIT");
            }
            pthread_mutex_unlock(&lock);
        }

    }
    printf("%d.Thread terminated\n",id+1);
    return NULL;
}


int main(void){
    printf("The game has launched\n3 threads will be created\n");
    printf("Game will be started when all players joined the game\n\n");


    for(int i = 0; i < THREAD_NUM ; i++){
        strcpy(playerChoices[i],"WAIT");
        int error = pthread_create(&(tid[i]),NULL,&gamearea,NULL);
        if(error != 0)
            printf("\nThread can't be created : [%s]",strerror(error));
            
    }

    for(int i=0; i < THREAD_NUM;i++)
        pthread_join(tid[i], NULL);

    printf("Threads are joined by main process\n");
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(element_lock);
    printf("Game finished\n");
    return 0;
}

int won(char thread[ELEMENT_NUM][10]){
    
    if(strcmp(thread[0],"WAIT") == 0 || strcmp(thread[1],"WAIT") == 0 || strcmp(thread[2],"WAIT") == 0) // wait for all threads
        return WAIT;
    else if(strcmp(thread[0],"ROCK") == 0 && strcmp(thread[1],"SCISSOR") == 0 && strcmp(thread[1],thread[2]) == 0)
        return 0;
    else if(strcmp(thread[0],"PAPER") == 0 && strcmp(thread[1],"ROCK") == 0 && strcmp(thread[1],thread[2]) == 0)
        return 0;
    else if(strcmp(thread[0],"SCISSOR") == 0 && strcmp(thread[1],"PAPER") == 0 && strcmp(thread[1],thread[2]) == 0)
        return 0;
    else if(strcmp(thread[0],"SCISSOR") == 0 && strcmp(thread[1],"ROCK") == 0 && strcmp(thread[0],thread[2]) == 0)
        return 1;
    else if(strcmp(thread[0],"ROCK") == 0 && strcmp(thread[1],"PAPER") == 0 && strcmp(thread[0],thread[2]) == 0)
        return 1;
    else if(strcmp(thread[0],"PAPER") == 0 && strcmp(thread[1],"SCISSOR") == 0 && strcmp(thread[0],thread[2]) == 0)
        return 1;
    else if(strcmp(thread[0],"SCISSOR") == 0 && strcmp(thread[0],thread[1]) == 0 && strcmp(thread[2],"ROCK") == 0)
        return 2;
    else if(strcmp(thread[0],"ROCK") == 0 && strcmp(thread[0],thread[1]) == 0 && strcmp(thread[2],"PAPER") == 0)
        return 2;
    else if(strcmp(thread[0],"PAPER") == 0 && strcmp(thread[0],thread[1]) == 0 && strcmp(thread[2],"SCISSOR") == 0)
        return 2;
    else
        return DRAW;
    return -1;
}


char * pick(){
    int selected = (int)((drand48()*10000)+ random()/1000 + getpid())% ELEMENT_NUM;
    pthread_mutex_lock(&element_lock[selected]);
    elementCount[selected]++;
    pthread_mutex_unlock(&element_lock[selected]);

    return elements[selected%ELEMENT_NUM];
}
