#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char const *argv[]) {
    char sms[50];
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    unsigned short port = 8784;
    char buffer[256];

    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket");
        return -1;
    }

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
            printf("Error accepting connection\n");
            return -1;
        } 
        while (1) {
            send(clientfd, "Hello, world!%s\n", 13,0);
            printf("Server got connected.");
            int  n = 0;
            while (fgets(buffer, 255, stdin) != 0){ 
                if (scanf("%255[^\n]", buffer) == 1) {
                    printf("Found <<%s>>\n", buffer);
                }
            }
            n = read(clientfd,buffer,255);
            if (n < 0){
                printf("ERROR reading from socket");
            }
            printf("Client: %s\n",buffer);
            close(clientfd);
            close(sockfd);
            return 0;
        }
    }
}
