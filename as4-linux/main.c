#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"
#include "memMap.h"
#include "morsecode.h"
#include "display.h"
#include "util.h"
// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------


void pru_init(void)
{
    printf("Configuring pru pins\n");
    runCommand("config-pin p9_27 pruout");
    runCommand("config-pin p9_28 pruin");
}

int main (int argc, char *argv[]){
    printf("testing driver\n");
    printf("SIze of struct: %zu\n", sizeof(sharedMemStruct_t));
    displayWriter_init();
    pru_init();
    meminit();
    while(1)
    {
        char *buff = NULL;
        size_t sizeAllocated = 0;
        printf("Please write some text\n");
        size_t numCh = getline(&buff, &sizeAllocated, stdin);

        for (int i = 0; i < numCh - 1; i++){
            unsigned short decode = MorseCode_getFlashCode(buff[i]);
            //check code
            printf("code: %2x\n", decode);
            //write code to the array
            writeToDataArray(decode, i);
        }

        memMap_set_data_length(numCh - 1);
        memMap_set_data_ready_flag();

        printf("Flags: %d, %d, %d\n", isSentenceCompleted(), returnFlashingFlag(), returnLEDstatus());


        while(!isSentenceCompleted()){
            while (!returnFlashingFlag() && !isSentenceCompleted()){
            //wait for the return flag
            }
            if(!isSentenceCompleted())
            {
                setLedDataReadyFlagOFF();
                updateDisplay(returnLEDstatus());
                printf("Updated display\n");
            }
        }
        displayWriter_cleanup();
        free(buff);
        buff = NULL;
    }

}
