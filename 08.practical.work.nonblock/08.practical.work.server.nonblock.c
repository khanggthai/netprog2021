#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>

int main(int argc, char const *argv[]) {
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buffer[256];

    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Error binding");
        return -1;
    }

    if (listen(sockfd, 5) < 0) {
        printf("Error listening");
        return -1;
    }
    while (1) {
        clen=sizeof(caddr);
        if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
            printf("Error accepting connection.\n");
            return -1;
        } 
        int fl = fcntl(clientfd, F_GETFL, 0);
        fl |= O_NONBLOCK;
        fcntl(clientfd, F_SETFL, fl);

        while (1) {
            send(clientfd, "Hello!%s\n", 13,0);
            printf("Server connected.");
            
            while (fgets(buffer, 255, stdin) != 0){ 
                if (scanf("%255[^\n]", buffer) == 1) {
                    printf("Found <<%s>>\n", buffer);
                }
            }
            memset(buffer, 0, 255);
            if (read(clientfd, buffer, 255) <= 0)
            {
                printf("Client disconnected.\n");
                break;
            }
            printf("Client: %s\n", buffer);

            struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
            if (poll(input, 1, 100) > 0)
            {
                printf("Server: ");
                fgets(buffer, sizeof buffer, stdin);
                buffer[255] = 0;

                write(clientfd, buffer, 255);
            }
        }
    }