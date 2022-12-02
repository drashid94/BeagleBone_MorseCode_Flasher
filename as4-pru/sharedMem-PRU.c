#include <stdint.h>
#include <stdbool.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "../as4-linux/sharedDataStruct.h"

volatile register uint32_t __R30; // output GPIO register
volatile register uint32_t __R31; // input GPIO register
// Use pru0_rpu_r30_5 as an output (bit 5 = 0b0010 0000)
#define LED_MASK (1 << 5)
// Use pru0_pru_r31_3 as a button (bit 3 = 0b0000 1000)
#define BUTTON_MASK (1 << 3)
//Delays
#define DELAY_250_MS 50000000
#define DELAY_500_MS (DELAY_250_MS * 2)
#define DELAY_1_MS (50000000 / 250)

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200 // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)
// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void*)THIS_PRU_DRAM_USABLE;

void delayInMs(int msDelay)
{
    for (int i = 0; i < msDelay; i++) {
        __delay_cycles(DELAY_1_MS);
    }
}

static void flash_empty_dot_times(int n)
{

}

void main(void)
{
    // Initialize:
    pSharedMemStruct->isLedOn = false;
    pSharedMemStruct->isButtonPressed = false;

    bool isMorseDataReady = pSharedMemStruct->isMorseDataReady;
    while(!isMorseDataReady)
    {
        //wait for data to be ready
    }

    //Clear flag
    pSharedMemStruct->isMorseDataReady = false;
    //start flashing the led based on morse data
    int i = 0;
    int morseDataLen = pSharedMemStruct->morseCodeDataLength;
    while(i < morseDataLen)
    {
        uint16_t morse_letter = pSharedMemStruct->morseCodeData[i]; // One charactor in morse code
        //Check if whitespace letter
        if(!morse_letter)
        {
            flash_empty_dot_times(7);
        }
        while(morse_letter)
        {
            if(morse_letter & 0x8000)
            {
                __R30 |= LED_MASK;
                pSharedMemStruct->isLedOn = true;
            }
            else
            {
                __R30 &= ~LED_MASK;
                pSharedMemStruct->isLedOn = false;
            }
            pSharedMemStruct->isFlagDataReady = true;
            delayInMs(300);
            morse_letter <<= morse_letter;
            pSharedMemStruct->isButtonPressed = (__R31 & BUTTON_MASK) != 0;
        }
        //End of letter
        flash_empty_dot_times(3);
        i++;
    }
    //When done with morse data
    pSharedMemStruct->isLedOn = false;
    pSharedMemStruct->isflashingCompleted = true;
 }

 //Linux::
