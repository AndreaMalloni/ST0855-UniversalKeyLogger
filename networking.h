#ifndef NETWORKING_H
#define NETWORKING_H

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

#define PORT "3491"

/**
 * Returns the file descriptor of a socket for writing to.
 *
 * If a connection cannot be established, the process is terminated.
 *
 * \param hostname IP address/name of host
 * \param port Port number to connect to on host
 *
 * \returns The file descriptor of a stream socket
 */
int get_socket_file_descriptor(char *hostname, char *port);

/**
 * Returns the file descriptor of a listening socket.
 *
 * If a connection cannot be established, the process is terminated.
 *
 * \param port Port number to listen on
 *
 * \returns The file descriptor of a stream socket
 */
int get_listener_socket_file_descriptor(char *port);

#endif
