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
#include <fcntl.h>

int main(int argc, char const *argv[]) {
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buffer[256];
    int clientfds[100];
    int MAX_CLIENT = 100;

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
                clen=sizeof(caddr);
                if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
                    printf("Error accepting connection.\n");
                    return -1;
                }
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

            for (int i = 0; i < MAX_CLIENT; i++) {
                send(clientfd, "Hello!%s\n", 13,0);
                printf("Server connected.");
            
                while (fgets(buffer, 255, stdin) != 0){ 
                    if (scanf("%255[^\n]", buffer) == 1) {
                        printf("Found <<%s>>\n", buffer);
                    }
                }
                if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set)) {
                    if (read(clientfds[i], buffer, sizeof(buffer)) <= 0) {
                        printf("Client %d disconnected", clientfds[i]);
                    }
                    printf("Client %d say: \n%s", clientfds[i], buffer);

                    struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
                    if (poll(input, 1, 100) > 0) {
                        printf("Server: ");
                        fgets(buffer, sizeof buffer, stdin);
                        buffer[255] = 0;

                        write(clientfds[i], buffer, 255);
                        clientfds[i] = 0;
                    }           
                }   
            }
        }
    }
}