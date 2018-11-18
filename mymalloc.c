#include <stdio.h>

#define TOTAL_RAM 25000
#define RAM_HEADER_SIZE 100
#define BLOCK_HEADER_SIZE sizeof(TOTAL_RAM)
#define HEADER_START 1
#define HEADER_END HEADER_START + RAM_HEADER_SIZE
#define RAM_BODY_START HEADER_END

static char RAM[TOTAL_RAM];

// prototype
void *defVar(int);
void *mymalloc(int);
void printRAM();
void initRAM();
void *bestFit(int);
int delVar(int *);
void myfree(void *);
void mergeFreeBlocks(int *, int *);

/**
 * initialize ram if not initialized
*/
void initRAM() {
    
    if (RAM[0] == 0) {      // not initialized
        printf("Initilizing RAM\n");

        RAM[0] = 'i';       // i for initilized
        RAM[RAM_BODY_START] = 'f';                      // state very first block is free
        * (int *) &RAM[RAM_BODY_START + 1] = TOTAL_RAM - RAM_HEADER_SIZE - 1 ;      // write the size of free block
    }
}

/**
 * allocate a slot from RAM_HEADER for a variable
 * RAM_HEADER structure: [status bit][slot 1:[status bit][slot]][slot 2:[status bit][slot]]
 * i 00000 00000 000...00
 * i a2032 a2342 000...00
*/
void *defVar(int value) {
    void *pointer = &RAM[HEADER_START];

    while (pointer < (void *) &RAM[RAM_BODY_START]) {
        if (*(char *)pointer != 'a') {
            *(char *)pointer = 'a';
            *(int *)((char *)pointer + 1) = value;
            return (int *)((char *)pointer + 1);
        }

        pointer += BLOCK_HEADER_SIZE + 1;
    }
    return NULL;
}

/**
 * release a slot from RAM_HEADER 
 */
int delVar(int * pointer) {
    *((char *)pointer - 1) = 'f';
    return *pointer;
}

/**
 * bestFit algorithm for find the best location to allocate memory
*/
void *bestFit(int requiredSize) {
    int *currentPosition = defVar(RAM_BODY_START);
    int *bestPosition = defVar(*currentPosition);
    int *previousLeftSpace = defVar(TOTAL_RAM);
    int *found = defVar(0);
    int *currentBlockSize = defVar(*(int *) &RAM[*currentPosition + 1]);

    printf("Finding location for %d bytes\n", requiredSize);

    while (*currentPosition < TOTAL_RAM) {
        printf("searching...%d %p size: %d\n", *currentPosition, &RAM[*currentPosition], *currentBlockSize);
        
        *currentBlockSize = *(int *) &RAM[*currentPosition + 1];

        if (*currentBlockSize >= requiredSize) {       // found enough space
        // printf("found one");
            if (RAM[*currentPosition] == 'f') {
                int leftSpace = *currentBlockSize - requiredSize;
                *found = 1;

                if (leftSpace < *previousLeftSpace) {                // found a better location
                    *previousLeftSpace = *currentBlockSize;
                    *bestPosition = *currentPosition;
                }
            }
        }

        *currentPosition += *(int *) &RAM[*currentPosition + 1] + BLOCK_HEADER_SIZE + 1;
    }

    delVar(currentPosition);
    delVar(previousLeftSpace);
    delVar(currentBlockSize);

    if (delVar(found)) {
        printf("Found a location @ %p\n", &RAM[*bestPosition]);
        return &RAM[delVar(bestPosition)];
    }

    printf ("ERROR! not enough space!!\n");
    return NULL;
}

/**
 * actuall mymalloc
*/
void * mymalloc(int requiredSize) {
    initRAM();      // initialize RAM if not already

    char *allocPosition = bestFit(requiredSize);
    if (allocPosition == NULL) return NULL;         // EXITING :: not *found suitable location

    // allocating
    int *completeBlockSize = defVar(*(int *)(allocPosition + 1));
    *(char *)allocPosition = 'a';                   // change flag to 'allocated'

    if (*completeBlockSize - requiredSize >=  BLOCK_HEADER_SIZE + 1) {
        *(int *)(allocPosition + 1) = requiredSize;    // store the size of block
        *(char *)(allocPosition + 1 + BLOCK_HEADER_SIZE + requiredSize) = 'f';         // allocate left space as a free block
        *(int *)(allocPosition + 1 + BLOCK_HEADER_SIZE +  requiredSize + 1) = delVar(completeBlockSize) - requiredSize;        // store left space's (block) size

    }
    

    return allocPosition + BLOCK_HEADER_SIZE + 1;
}

/**
 * merge free blocks (previous, current & next) of memory 
 * given the currentLocation and previousLocation of the RAM
*/ 
void mergeFreeBlocks(int *currentLocation, int *previousLocation) {
    int *nextLocation = defVar(*currentLocation + *(int *) &RAM[*currentLocation + 1] + BLOCK_HEADER_SIZE + 1);

    *(char *)&RAM[*currentLocation] = 'f';        // define the block as free

    if (RAM[*nextLocation] == 'f') {                                                    // if next block is free
        *(int *) &(RAM[*currentLocation + 1]) += *(int *) &RAM[*nextLocation + 1];      // merge it with the current block
    }

    if (RAM[*previousLocation]  == 'f' && *currentLocation != *previousLocation) {                                                // if previous block is free
        *(int *) &(RAM[*previousLocation + 1]) += *(int *) &RAM[*currentLocation + 1];  // merge current block with the previous one
    }

    delVar(nextLocation);
}

/**
 * actual myfree
 * find the targetLocation & define it as free
 * if adjecent blocks are free merge them
 */
void myfree(void *targetLocation) {
    int *currentLocation = defVar(RAM_BODY_START);
    int *previousLocation = defVar(RAM_BODY_START);

    while (*currentLocation < TOTAL_RAM) {
        if (((char *) &RAM[*currentLocation]) + 1 + BLOCK_HEADER_SIZE == targetLocation) {
            mergeFreeBlocks(currentLocation, previousLocation);
            break;
        }

        *previousLocation = *currentLocation;
        *currentLocation += *(int *) &RAM[*currentLocation + 1] + BLOCK_HEADER_SIZE + 1;
    }
    delVar(currentLocation);
    delVar(previousLocation);
}


/**
 * just for print RAM array
 * 
*/
void printRAM() {
    for(int i = 100; i < 130; i++) {
        if (i%100 == 0) printf("\n");

        printf("%p : ram@%d: %d\tletter: %c\tsize: %d\n", &RAM[i], i, RAM[i], RAM[i], (int) *(int *) &RAM[i]);
    }

    printf("\n");
}

int main(int argc, char const *argv[])
{
    void *one = mymalloc(8);
    void *two = mymalloc(3);
    myfree(one);
    mymalloc(8);

    // for (int i=0; i<50; i++){
    //     mymalloc(3);
    // }
    // myfree(two);
    // mymalloc(2);
    // mymalloc(5);
    // mymalloc(5);

    printRAM();
    
    return 0;
}
