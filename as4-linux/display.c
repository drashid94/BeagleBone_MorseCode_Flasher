#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "util.h"

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
static pthread_t displayThreadID;
static int stopping = 0;

static unsigned char dots[] = {0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};


// << 4 to get the one on the right
// the digit[0] and digit[1] will select the correct pattern from the table
/*static unsigned char numberTable[10][8] = {{0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x02, 0x00}, //0
                                    {0x02, 0x06, 0x02, 0x02, 0x02, 0x02, 0x07, 0x00}, //1
                                    {0x07, 0x01, 0x01, 0x07, 0x04, 0x04, 0x07, 0x00}, //2
                                    {0x07, 0x01, 0x01, 0x03, 0x01, 0x01, 0x07, 0x00}, //3 
                                    {0x05, 0x05, 0x05, 0x07, 0x01, 0x01, 0x01, 0x00}, //4 
                                    {0x07, 0x04, 0x04, 0x07, 0x01, 0x05, 0x07, 0x00}, //5 
                                    {0x07, 0x04, 0x04, 0x07, 0x05, 0x05, 0x07, 0x00}, //6 
                                    {0x07, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00}, //7
                                    {0x07, 0x05, 0x05, 0x02, 0x05, 0x05, 0x07, 0x00}, //8
                                    {0x07, 0x05, 0x05, 0x07, 0x01, 0x05, 0x07, 0x00}}; //9
*/

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


static void printCode(){
    printLine();
    printLine();
}
static void printLine(){
    int i2cFileDesc = initI2cBus(I2C_LINUX_BUS1, DISPLAY_I2C_ADDR);
    for (int i = 0; i < 8; i++){
        writeI2cReg(i2cFileDesc, ROW0, dots[i]);
        writeI2cReg(i2cFileDesc, ROW1, dots[i]);
        writeI2cReg(i2cFileDesc, ROW2, dots[i]);
        writeI2cReg(i2cFileDesc, ROW3, dots[i]);
        sleep_for_ms(500);
    }
}


/*thread for the display*/
static void *UpdateDisplay(void *_)
{
    while (!stopping){
        //display init
        // displayInit();

        printCode();
        sleep_for_ms(100);
    }
    return NULL;
}

/*init thread*/
void displayWriter_init(void)
{
    displayInternalInit();
    pthread_create(&displayThreadID, NULL, &UpdateDisplay, NULL);
}

/*clean up thread*/
void displayWriter_cleanup(void)
{
    stopping = 1;
    pthread_join(displayThreadID, NULL);
    resetDisplay();
}
