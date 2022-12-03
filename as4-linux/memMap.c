#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"

#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000 // Length of PRU memory
#define PRU0_DRAM 0x00000 // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap
// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)
volatile void *pPruBase;
volatile sharedMemStruct_t *pSharedPru0;

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
// Points to start of PRU memory.
    pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}

void freePruMmapAddr(){
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void meminit(){
    pPruBase = getPruMmapAddr();
    pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);
}


void writeToDataArray(unsigned short code, int index) {
    //printf("Sharing memory with PRU\n");
    //printf("putting the code on the array");

    pSharedPru0->morseCodeData[index] = code;

    //printf("input to array: %2x\n", pSharedPru0->morseCodeData[index]);
    //printf("index: %d\n", index);

}


void memMap_set_data_ready_flag()
{
    //printf("Setting morse data ready to true\n");
    pSharedPru0->isMorseDataReady = true;
}

void memMap_set_data_length(int len)
{
    //printf("setting data length to %d\n", len);
    
    pSharedPru0->morseCodeDataLength = len;

}

void memMap_cleanup(void)
{
    freePruMmapAddr(getPruMmapAddr);
}

bool returnFlashingFlag(){
    return pSharedPru0->isFlagDataReady;
}

bool returnLEDstatus(){
    return pSharedPru0->isLedOn;
}

void setLedDataReadyFlagOFF(){
    pSharedPru0->isFlagDataReady = false;
}

bool isSentenceCompleted(){
    //done with the sentence 
    return pSharedPru0->isFlashingCompleted;
}