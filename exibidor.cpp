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
	char *ip;
	char *port;
	
	ip = strtok (string, ":");	
	port = strtok (NULL, ":");

	unsigned short exhibitorID = 0;
	char buf[BUFSZ];

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

	printf("connected to %s\n", addrstr);
	
	size_t count;
	struct header exhibitorHeader;
	exhibitorHeader.msgOrder = 0;
	exhibitorHeader.msgDestiny = 0;
	exhibitorHeader.msgOrigin = exhibitorID;

	if( exhibitorHeader.msgOrder == 0 ){
		exhibitorHeader.msgType = 3; // sends an "OI" message (3)
		count = send(s, &exhibitorHeader, sizeof(header),0);
		printf("\n sent(oi): %u %u %u %u", exhibitorHeader.msgType, exhibitorHeader.msgOrigin, exhibitorHeader.msgDestiny, exhibitorHeader.msgOrder);
		
		count = recv(s, &exhibitorHeader, sizeof(header),0);
		exhibitorID = exhibitorHeader.msgDestiny;
		printf("\n rcvd(oi): %u %u %u %u", exhibitorHeader.msgType, exhibitorHeader.msgOrigin, exhibitorHeader.msgDestiny, exhibitorHeader.msgOrder);
		printf("oo");exhibitorHeader.msgOrder++;
	}
	
	if( exhibitorHeader.msgType == 1){
		
		while(1){

			count = recv(s, &exhibitorHeader, sizeof(header),0);

			switch (exhibitorHeader.msgType)
			{
			case 4:
				close(s);
				printf("\n connection terminated");
				exit(EXIT_SUCCESS);
				break;
			case 5:
				count = recv(s,buf,BUFSIZ,0);
				printf("< message from %u: ", exhibitorHeader.msgOrigin);
				puts(buf);
			default:
				break;
			}
			

			printf("\n < message from %u:", exhibitorHeader.msgOrigin);

		}

	}else if(exhibitorHeader.msgType == 2){
		printf("\n falhou");
	}else{
		
		printf("\n unknown msg type: %u %u %u %u",exhibitorHeader.msgType, exhibitorHeader.msgOrigin, exhibitorHeader.msgDestiny, exhibitorHeader.msgOrder);
	}
	
}
