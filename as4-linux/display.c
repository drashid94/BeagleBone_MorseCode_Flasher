#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "util.h"
#include "stdbool.h"

/*hardware specs defines*/
#define DISPLAY_I2C_ADDR 0x70

#define I2C_LINUX_BUS1 "/dev/i2c-1"

/*row define*/
#define ROW0 0X00
#define ROW1 0X02
#define ROW2 0X04
#define ROW3 0X06
#define ROW4 0X08
#define ROW5 0X0A
#define ROW6 0X0C
#define ROW7 0X0E

static unsigned char rows[] = {ROW7, ROW6, ROW5, ROW4, ROW3, ROW2, ROW1, ROW0};
//static unsigned char masked[] = {0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};
static unsigned char resultBits = 0x00;
//bool newLine = false;
static unsigned char dots[] = {0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};
//static unsigned char colTrack[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static __uint8_t bitMap = 0b00000000;

//init for the i2c bus
static int initI2cBus (char* bus, int address){
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
    perror("I2C: Unable to set I2C device to slave address.");
    exit(1);
}
    return i2cFileDesc;
}

static void updateBitmap(bool newLine){
    // printf("before shift: %x\n", bitMap);
    bitMap <<= 1;  
    // printf("after shift: %x\n", bitMap);
    if (newLine == true){
        bitMap |= 0x01; //the most right bit
    }
    else {
        bitMap &= ~0x01;
    }
    // printf("after condition shift: %x\n", bitMap);
}

static void updateBitArray(){
    resultBits = 0x00;
    for (int i = 0; i< 8; i++){
        if (bitMap & (1 << i)){
            resultBits |= dots[7-i];
            // printf("dots to OR: %x\n", dots[7-i]);
        }
    }
    // printf("result bit: %x\n", resultBits);
}

//taken from the tutorials
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,unsigned char value){
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    res++;
}

//reset the display to empty
static void resetDisplay(){
    int i2cFileDesc = initI2cBus(I2C_LINUX_BUS1, DISPLAY_I2C_ADDR);
    for (int i = 0; i < 8; i++){
        writeI2cReg(i2cFileDesc, rows[i], 0x00);
    }
}

//init the i2c pins and turn on the display
static void displayInternalInit(){
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    int i2cFileDesc = initI2cBus(I2C_LINUX_BUS1, DISPLAY_I2C_ADDR);
    writeI2cReg(i2cFileDesc, 0x21, 0x00);
    writeI2cReg(i2cFileDesc, 0x81, 0x00);
    resetDisplay();
}


static void printLine(){
    int i2cFileDesc = initI2cBus(I2C_LINUX_BUS1, DISPLAY_I2C_ADDR);
        writeI2cReg(i2cFileDesc, ROW0, resultBits);
        writeI2cReg(i2cFileDesc, ROW1, resultBits);
        writeI2cReg(i2cFileDesc, ROW2, resultBits);
        writeI2cReg(i2cFileDesc, ROW3, resultBits);
}

void updateDisplay(bool newLine)
{
        updateBitmap(newLine);
        updateBitArray();
        printLine();
}

/*init thread*/
void displayWriter_init(void)
{
    displayInternalInit();
}

/*clean up thread*/
void displayWriter_cleanup(void)
{
    resetDisplay();
}