#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "networking.h"

#if defined(PLATFORM_LINUX)
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netdb.h>

    void setup_addrinfo(struct addrinfo **servinfo, char *hostname, char *port, int flags){
        struct addrinfo hints;
        int rv;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = flags;

        if((rv = getaddrinfo(hostname, port, &hints, servinfo)) != 0){
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            freeaddrinfo(*servinfo);
            exit(1);
        }
    }

    int get_socket_file_descriptor(char *hostname, char *port){
        int sockfd;
        struct addrinfo *servinfo, *p;
        char s[INET_ADDRSTRLEN];

        setup_addrinfo(&servinfo, hostname, port, 0);

        // Connect to the first possible result
        for(p = servinfo; p != NULL; p = p->ai_next){
            if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
                perror("socket");
                continue;
            }

            if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
                close(sockfd);
                perror("connection");
                continue;
            }

            break;
        }

        if(p == NULL){
            fprintf(stderr, "failed to connect\n");
            freeaddrinfo(servinfo);
            exit(1);
        }

        inet_ntop(p->ai_family, &(((struct sockaddr_in *)p->ai_addr)->sin_addr), s, sizeof(s));
        printf("connecting to %s\n", s);

        freeaddrinfo(servinfo);

        return sockfd;
    }

    int get_listener_socket_file_descriptor(char *port){
        int sockfd;
        struct addrinfo *servinfo, *p;
        char s[INET_ADDRSTRLEN];
        int yes = 1;

        setup_addrinfo(&servinfo, NULL, port, AI_PASSIVE);

        // Bind to the first possible result
        for(p = servinfo; p != NULL; p = p->ai_next){
            if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
                perror("socket");
                continue;
            }

            // Allow this port to be reused later
            if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
                perror("setsockopt");
                exit(1);
            }

            if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
                close(sockfd);
                perror("connection");
                continue;
            }

            break;
        }

        if(p == NULL){
            fprintf(stderr, "%s\n", "server: failed to bind");
            exit(1);
        }

        inet_ntop(p->ai_family, &(((struct sockaddr_in *)p->ai_addr)->sin_addr), s, sizeof(s));
        printf("listening on %s\n", s);

        freeaddrinfo(servinfo);

        return sockfd;
    }
#endif

#if defined(PLATFORM_WINDOWS)
    #include <winsock2.h>
    int get_socket_file_descriptor(char *hostname, char *port) {
        WSADATA wsa;
        SOCKET s;
        struct sockaddr_in server;

        printf("\nInitialising Winsock...\n");
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
            printf("Failed. Error Code : %d",WSAGetLastError());
            return 1;
        }

        printf("Initialised.\n");

        if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
            printf("Could not create socket : %d" , WSAGetLastError());
        }

        printf("Socket created.\n");

        server.sin_addr.s_addr = inet_addr(hostname);
        server.sin_family = AF_INET;
        server.sin_port = htons( 3491 );

        //Connect to remote server
        if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0) {
            printf("Could not connect to server : %d" , WSAGetLastError());
            return 1;
        }

        puts("Connected\n");
        return s;
    }
#endif

