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

    bool newline = true;
    updateDisplay(newline);
    sleep_for_ms(1000);

    newline = false;
    updateDisplay(newline);

    sleep_for_ms(500);
    newline = true;
    updateDisplay(newline);

    sleep_for_ms(500);
    newline = true;
    updateDisplay(newline);

    sleep_for_ms(500);
    newline = true;
    updateDisplay(newline);

    sleep_for_ms(500);
    newline = false;
    updateDisplay(newline);

    sleep_for_ms(500);
    newline = true;
    updateDisplay(newline);




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
    sleep_for_ms(5000);
    displayWriter_cleanup();

    memMap_set_data_length(numCh - 1);
    memMap_set_data_ready_flag();

    free(buff);
    buff = NULL;

}
