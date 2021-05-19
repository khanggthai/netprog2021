#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    while (1) {
        printf("Enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        int n = 0;            
        n = write(sockfd, buffer, strlen(buffer));
        while (fgets(buffer, 255, stdin) != 0){ 
            if (scanf("%255[^\n]", buffer) == 1) {
                printf("Found <<%s>>\n", buffer);
            }
        }
        if (n < 0) {
            printf("Cannot write to socket");
        }
        bzero(buffer,256);
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            printf("Cannot read from socket");
        }
        printf("%s\n", buffer);
        close(sockfd);
        return 0;
    } 
}
