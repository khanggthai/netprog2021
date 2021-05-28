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
#include <sys/select.h>

int main(int argc, char const *argv[]) {
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buffer[256];
    int clientfds[100];

    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);

    memset(clientfds, 0, sizeof(clientfds));

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

        while (1) {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(sockfd, &set);
            int maxfd = sockfd;
            for (int i = 0; i < 100; i++) {
                if (clientfds[i] > 0) FD_SET(clientfds[i], &set);
                if (clientfds[i] > maxfd) maxfd = clientfds[i];
            }
            select(maxfd+1, &set, NULL, NULL, NULL);
            if (FD_ISSET(sockfd, &set)) {
                clientfd = accept(sockfd, (struct sockaddr *) &saddr, &clen);
                
                fl = fcntl(clientfd, F_GETFL, 0);
                fl |= O_NONBLOCK;
                fcntl(clientfd, F_SETFL, fl);

                for (int i = 0; i < MAX_CLIENT; i++) { 
                    if (clientfds[i] == 0) {
                        clientfds[i] = clientfd;
                    break; 
                    }
                }
            }
            for (i = 0; i < MAX_CLIENT; i++) {
                if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set)) {
                    if (read(clientfds[i], s, sizeof(s)) > 0) {
                        printf("client %d says: %s\nserver>", clientfds[i], s);
                    }
                    else {
                        printf("client %d has disconnected.\n", clientfds[i]);
                        clientfds[i] = 0;
                    }                   
                }   
            }
        }
    }