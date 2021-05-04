#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char const *argv[]) {
    int sockfd, clen, clientfd;
    struct sockaddr_in saddr, caddr;
    struct hostent *hostname;
    unsigned short port = 8784;

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
		printf("%s\n", " Please enter hostname: ");
		char str[50];
		scanf("%s", str);
		hostname = gethostbyname(str);
	}

	printf("IP address is: \n");

    if (!hostname) {
        printf("Can not find ip.");
    }
    else {
	    for (unsigned int i=0; hostname->h_addr_list[i] != NULL; i++){
		    printf("%s\n", inet_ntoa( *(struct in_addr*)(hostname->h_addr_list[i])));
        }
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    memcpy((char *) &saddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length)
    saddr.sin_port = htons(port);


    //int connect(int sockfd, const struct sockaddr *saddr, socklen_t addrlen)
    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        printf(“Cannot connect\n”);
        return -1;
    }
    return 0;
}
