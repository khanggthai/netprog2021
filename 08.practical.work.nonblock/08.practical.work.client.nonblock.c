#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char const *argv[]) {
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    struct hostent *hostname;
    char *ipaddr;
    unsigned short port = 8784;
    char sms[50];
    char buffer[256];

    // int socket(int domain, int type, int protocol)
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error creating socket");
        return -1;
    }

    //from labwork2
	if(argc == 2) {
		hostname = gethostbyname(argv[1]);
	}
    else {
        printf("Please enter hostname: \n");
        char str[50];
        scanf("%s", str);
        hostname = gethostbyname(str);
	}

	printf("IP address is: \n");

    if (!hostname) {
        printf("Host unknown.");
        ipaddr = inet_ntoa(*(struct in_addr *)hostname->h_name);
        printf(hostname->h_name, ipaddr);
    }
    else {
        for (unsigned int i=0; hostname->h_addr_list[i] != NULL; i++){
            printf("%s\n", inet_ntoa( *(struct in_addr*)(hostname->h_addr_list[i])));
        }
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
    saddr.sin_port = htons(port);

    //int connect(int sockfd, const struct sockaddr *saddr, socklen_t addrlen)
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf("Cannot connect");
        close(sockfd);
        return -1;
    }

    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);

    while (1) {
        printf("Enter the bufferage: ");
        while (fgets(buffer, 255, stdin) != 0){ 
            if (scanf("%255[^\n]", buffer) == 1) {
                printf("Found <<%s>>\n", buffer);
            }
        struct pollfd input[1] = {{.fd = 0, .events = POLLIN}};
        if (poll(input, 1, 100) > 0) {
            printf("Client: ");
            fgets(buffer, sizeof buffer, stdin);
            buffer[strlen(buffer) - 1] = 0;
            write(sockfd, buffer, strlen(buffer));
        }

        fgets(buffer, sizeof buffer, stdin);
        buffer[strlen(buffer) - 1] = 0;
        
        if (strcmp(buffer, "/quit") == 0) {
            printf("Client disconnected\n");
            shutdown(sockfd, SHUT_RDWR);
            close(sockfd);
            return 0;
        }
        write(sockfd, buffer, strlen(buffer));
        memset(buffer, 0, sizeof buffer);
        if (read(sockfd, buffer, sizeof(buffer)) > 0) {
            printf("Server: %s\n", buffer);
        }
        else {
            printf("Server disconnected.\n");
            break;
        }
    } 
}
