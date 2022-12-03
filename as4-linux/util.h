//util.h
/*
file has functions that are generally useful to potentially multiple modules
*/

void sleep_for_ms(long long delayInMs); //kernal sleep for given ms

//int file_read(char* filename); //read file given by filename - returns value

//long long get_current_time_in_us(void); //returns timestamp in microseconds

void runCommand (char* command); // run command in linux
