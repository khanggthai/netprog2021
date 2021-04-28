#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>


int main(int argc, char **argv){
	struct hostent *hostname;
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
}

