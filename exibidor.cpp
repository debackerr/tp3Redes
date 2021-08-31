#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
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
	
	struct header exhibitorHeader;	
	
	unsigned short exhibitorID = 0;

	exhibitorHeader.msgOrder = 0;
	exhibitorHeader.msgDestiny = 65535; //server's ID
	exhibitorHeader.msgOrigin = exhibitorID;

	

	if( exhibitorHeader.msgOrder == 0 ){

		exhibitorHeader.msgType = 3; // sends an "OI" message (3)
		send(s, &exhibitorHeader, sizeof(header),0);
		
		recv(s, &exhibitorHeader, sizeof(header),0);

		exhibitorID = exhibitorHeader.msgDestiny;
		
		std::cout << "exhibitor connected - ID: " << exhibitorID << std::endl;
	}
	
	if( exhibitorHeader.msgType == 1){
		
		while(1){

			recv(s, &exhibitorHeader, sizeof(header),0);

			std::cout << "rcvd: " << exhibitorHeader.msgType << " " << exhibitorHeader.msgDestiny << " " << exhibitorHeader.msgOrigin << " " << exhibitorHeader.msgOrder << std::endl;
			switch (exhibitorHeader.msgType)
			{
			case 4:
				close(s);
				std::cout << "\n connection terminated" << std::endl;
				exit(EXIT_SUCCESS);
				break;
			case 5:
				unsigned short size; memset(buf,0,BUFSZ);

				recv(s, &size, sizeof(size),0 );
				recv(s, buf, size, 0);

				std::cout <<"\n< message from "<< exhibitorHeader.msgOrigin << ": " <<  buf << std::endl;

				exhibitorHeader.msgType = 1; // "OK" message
				exhibitorHeader.msgDestiny = exhibitorHeader.msgOrigin;
				exhibitorHeader.msgOrigin = exhibitorID;

				send(s, &exhibitorHeader, sizeof(header),0);
				
				break;
			default:
				printf("\nERROR\n");
				exit(EXIT_FAILURE);
				break;
			}

			std::cout << "sent: " << exhibitorHeader.msgType << " " << exhibitorHeader.msgDestiny << " " << exhibitorHeader.msgOrigin << " " << exhibitorHeader.msgOrder << std::endl;
			
		}

	}else if(exhibitorHeader.msgType == 2){
		std::cout << "communication failed" << std::endl;
		close(s);
		exit(EXIT_FAILURE);
	}else{		
		std::cout <<  "unknown message type" << std::endl;
		close(s);
		exit(EXIT_FAILURE);
	}
	
}
