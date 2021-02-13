#include <stdio.h>
#include <stdlib.h>


void displayHoles(int blocks[], int size);
void display(int blocks[], int size);
void initilize(int arr[],int size);

FILE *processes;

int internalFrag = 0;
int externalFrag = 0;
int availableBlock = 0;
int insufficientMemory = 0;
int displayMode = 0;

int main(int argc, char** argv){

    do{
        printf("Do you want to activate display mode?\n");
        printf("0 - Normal mode\n1 - Show frames status.\n");
        scanf("%d*",&displayMode);
    }
    while(displayMode != 0 && displayMode != 1 );
    getchar();//clear buffer
    
    processes = fopen(argv[2],"r");

    int blockNum = atoi(argv[1]) /4+0.5;
    availableBlock = blockNum;

    int blocks[blockNum];
    initilize(blocks,blockNum);

    int strategy = atoi(argv[3]); 
    char letter;

    int id, kbNum; 

    fscanf(processes," %c",&letter);
    while(!feof(processes)){

        if(letter == 'B'){
            fscanf(processes, "%d %d",&id,&kbNum);
            printf("\nPID %d, size %d\n",id,kbNum);
            int blockConsume = kbNum/4.0+0.5;

            if(blockConsume < availableBlock){
                if(strategy == 1){
                    int i ;
                    for(i= 0; i < blockNum; i++){
                        if(blocks[i] == 0 && id != 0){ 

                            int proper = 1; //proper to place
                            for(int j = i; j < i+blockConsume;j++) // explore this space is proper or not 
                                if(blocks[j] != 0){ proper = 0; i = j; break; }
                            
                            if(proper == 1){
                                for (int j = i; j < i+blockConsume; j++)
                                {
                                    blocks[j] = id;
                                }

                                availableBlock -= blockConsume;
                                internalFrag += -1*(4-kbNum) % 4;

                                printf("PID: %d located starting point %d, consumed %d (remaining:%d) \n",id,i,blockConsume,availableBlock);
                                id = 0; kbNum = 0;blockConsume = 0;
                                break;
                            }
                            
                            
                        }
                    }
                    if(blocks[blockNum-1] != id && i >= blockNum){
                        externalFrag++;
                        printf("ERROR! External fragmentation: PID %d cannot be located(required :%d available: %d)\n",id,blockConsume,availableBlock);
                        id = 0; kbNum = 0;blockConsume = 0;
                    }
                }

                else if(strategy == 2){
                    int bestAdress = -1;
                    int bestSpace = blockNum+1; //make it max
                    int space = 0;
                    int adress = -1;

                    for(int i = 0 ; i < blockNum; i++){

                        if(blocks[i] == 0 && id != 0){
                            adress = i;
                            space ++;
                            while(++i < blockNum && blocks[i] == 0)
                               space++;
                        }
                        if(space >= blockConsume && space < bestSpace){
                            bestSpace = space;
                            bestAdress = adress;
                        }
                        else{ space = 0; adress = -1;}
                            
                    }

                    if(bestAdress == -1){
                        externalFrag++;
                        printf("ERROR! External fragmentation: PID %d cannot located(required :%d available: %d)\n",id,blockConsume,availableBlock);
                        id = 0; kbNum = 0;blockConsume = 0;
                    }
                    else
                    {
                        for(int i = bestAdress; i < bestAdress+blockConsume; i++ )
                            blocks[i] = id;

                        availableBlock -= blockConsume;
                        internalFrag += -1*(4-kbNum) % 4;

                        printf("PID: %d located starting point %d, consumed %d (remaining:%d) \n",id,bestAdress,blockConsume,availableBlock);
                        id = 0; kbNum = 0;blockConsume = 0;
                        
                    }
                    
                }

                else if(strategy == 3){
                    int worstAdress = -1;
                    int worstSpace = -1; //make it min
                    int space = 0;
                    int adress = -1;

                    for(int i = 0 ; i < blockNum; i++){

                        if(blocks[i] == 0 && id != 0){
                            adress = i;
                            space ++;
                            while(++i < blockNum && blocks[i] == 0)
                               space++;
                        }
                        if(space >= blockConsume && space > worstSpace){
                            worstSpace = space;
                            worstAdress = adress;
                        }
                        else{ space = 0; adress = -1;}
                            
                    }

                    if(worstAdress == -1){
                        externalFrag++;
                        printf("ERROR! External fragmentation: PID %d cannot located(required :%d available: %d)\n",id,blockConsume,availableBlock);
                        id = 0; kbNum = 0;blockConsume = 0;
                    }
                    else
                    {
                        for(int i = worstAdress; i < worstAdress+blockConsume; i++ )
                            blocks[i] = id;

                        availableBlock -= blockConsume;
                        internalFrag += -1*(4-kbNum) % 4;

                        printf("PID: %d located starting point %d, consumed %d (remaining:%d) \n",id,worstAdress,blockConsume,availableBlock);
                        id = 0; kbNum = 0;blockConsume = 0;
                        
                    }
                    
                    
                }
            }
            else{
                    insufficientMemory++;
                    printf("ERROR! Insufficient memory (required:%d, available %d )\n",blockConsume,availableBlock);
                    id = 0; kbNum = 0;blockConsume = 0;
                }

            
        }
        else if(letter == 'E'){
            fscanf(processes, "%d",&id);
            int free = 0;
            for(int i = 0 ; i < blockNum; i++){
                if(blocks[i] == id && id != 0){
                    blocks[i] = 0;
                    free ++;
                    if(i+1 < blockNum && blocks[i+1] != id )
                        break;

                }
            }
            availableBlock += free;
            printf("\nPID %d is DELETED, %d kB free, %d kB available\n", id,free,availableBlock);

        }
        fscanf(processes," %c",&letter);
        
        if(displayMode)
            display(blocks,blockNum);
            
    }

    printf("\nTotal free memory in holes: %d frames =  %d kB\n",availableBlock,availableBlock*4);
    printf("Total memory wasted as an internal fragmentation: %d\n",internalFrag);
    printf("Total number of rejected processes due to external fragmentation: %d\n",externalFrag);
    printf("Total number of rejected processes due to insufficient memory: %d\n",insufficientMemory);

    displayHoles(blocks,blockNum);

    return 0;
}



void displayHoles(int blocks[], int size){
    int space = 0;
    int adress = -1;

    if(externalFrag == 0){
        printf("There is no hole since external fragmantation is: %d\n",externalFrag); 
        return;
    }
    
    printf("\nHoles:\n------\n");
    for(int i = 0 ; i < size; i++){
        if(blocks[i] == 0){
            adress = i;
            space ++;
            while(++i < size && blocks[i] == 0)
                space++;
            printf("%d'th block, %d frame\n",adress,space);
            adress = -1;
            space = 0;
        }
                         
    }


}

void display(int blocks[], int size){
    for(int i = 0; i < size; i++){if(i%32 == 0 && i!= 0) printf("%d.'th frame\n",i); // to show tidy, not important 
        printf("%d  ",blocks[i]);}
    printf("%d.'th frame\n",size);
    printf("\nTotal internal fragmentation %d\n",internalFrag);
    printf("Press Enter\n");
    char wait;
    scanf("%c",&wait);
}

void initilize(int arr[],int size){
    for(int i = 0 ; i < size ; i++ )
        arr[i] = 0;
}
