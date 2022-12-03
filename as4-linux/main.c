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
    displayWriter_init();
    meminit();
    char *buff = NULL;
    size_t sizeAllocated = 0;
    size_t numCh = getline(&buff, &sizeAllocated, stdin);

    pru_init();
    for (int i = 0; i < numCh - 1; i++){
        unsigned short decode = MorseCode_getFlashCode(buff[i]);
        //check code
        printf("code: %2x\n", decode);
        //write code to the array
        writeToDataArray(decode, i);
    }

    memMap_set_data_length(numCh - 1);
    memMap_set_data_ready_flag();

    while (true){

        while(!isSentenceCompleted()){
            while (!returnFlashingFlag()){
            //wait for the return flag
            }
        setLEDFlagOFF();
        updateDisplay(returnLEDstatus());
        }
    }

    displayWriter_cleanup();
    free(buff);
    buff = NULL;

}
