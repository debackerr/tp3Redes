#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

//socket libraries:
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <pthread.h>

#define BUFSZ 500

int main(int argc, char **argv) {
	char string[14];
	strcpy(string, argv[1]);
	char * ip = strtok (string, ":");	//gets IP
	char * port = strtok (NULL, ":"); //gets port

	// socket parse:
	struct sockaddr_storage storage;
	
	if (0 != addrparse(ip, port, &storage)) {
		printf("error\n");
		exit(EXIT_FAILURE);
	}
	
	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr); //connection success

	//communication variables:
	size_t count;
	struct header issuerHeader;
	char buf[BUFSZ];
	
	issuerHeader.msgOrder = 0;
	issuerHeader.msgOrigin = 0; 
	issuerHeader.msgDestiny = 0; 

	if( issuerHeader.msgOrder == 0){
		//sends an "OI" message type (3)
		issuerHeader.msgType = 3; 
		count = send(s, &issuerHeader, sizeof(header), 0);
		printf("\n sent(oi):%u %u %u %u\n",issuerHeader.msgType, issuerHeader.msgOrigin, issuerHeader.msgDestiny, issuerHeader.msgOrder);
		if (count != sizeof(issuerHeader)) {
			logexit("send");  //in case of error
		}

		count = recv(s, &issuerHeader, sizeof(header),0); 
		printf("\n rcvd(oi) :%u %u %u %u\n",issuerHeader.msgType, issuerHeader.msgOrigin, issuerHeader.msgDestiny, issuerHeader.msgOrder);
		issuerHeader.msgOrder++;
	}
	
	if( issuerHeader.msgType == 1){
		//connection accepted
		while(1){
				
					
				issuerHeader.msgType = getsType();

				if(issuerHeader.msgType == 4){

					count = send(s,&issuerHeader, sizeof(header),0);
					close(s);
					printf("\n connection terminated");
					exit(EXIT_SUCCESS);

				} else{
					
					printf("\nchoose message's destiny: \n > [0] all exhibitors\n > [%u] connected exhibitor\n>> ", issuerHeader.msgDestiny);
						
					unsigned int d; fflush(stdin);
					scanf("%u", &d);
					if( d == 0){
						issuerHeader.msgDestiny = 0;
					}else if(d != issuerHeader.msgDestiny){
						printf("\n invalid destiny");
					}

					if( issuerHeader.msgType == 5){

						count = send(s, &issuerHeader, sizeof(header),0);
						printf("\n sent(5):%u %u %u %u\n",issuerHeader.msgType, issuerHeader.msgOrigin, issuerHeader.msgDestiny, issuerHeader.msgOrder);
						
						printf("\n> send message to %u: ", issuerHeader.msgDestiny);
						memset(buf, 0, BUFSZ); fflush(stdin);
						fgets(buf, BUFSZ-1, stdin); fgets(buf, BUFSIZ-1, stdin); //both fgets() are needed 

						unsigned short size = strlen(buf)-1;
						count = send(s, &size,sizeof(size),0);
						count = send(s, buf, sizeof(buf), 0);
						issuerHeader.msgOrder++;
							
						count = recv(s, &issuerHeader, sizeof(header),0);
						printf("\n rcv(5):%u %u %u %u\n",issuerHeader.msgType, issuerHeader.msgOrigin, issuerHeader.msgDestiny, issuerHeader.msgOrder);
						if( issuerHeader.msgType == 1){
							printf("\nmessage sent!");
						}
					}
				} 
		}

	}else if( issuerHeader.msgType == 2){
			printf("\n communication unnaccepted");
	}else{
		printf("\n unknown error");
	}
	
}
