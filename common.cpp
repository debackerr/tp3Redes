#include "common.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

void logexit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) {
        //returns error case port is not found
    }
    //parse:
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    //atoi = string to integer
    if (port == 0) {
        return -1;
        //wrong information transmisted (there is no port 0)
    }
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP address
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage; 
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port; 
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1; 
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            logexit("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    } else {
        logexit("unknown protocol family.");
    }
    if (str) {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
}

unsigned short getsType(){
    printf("\nselect message type:");
    printf("\n  [4] disconnect from server");
    printf("\n  [5] send text message");
    printf("\n  [6] CREQ\n>>");

    unsigned int type; scanf("%u", &type);
    if(((type != 4) && (type !=5)) && ((type != 8) && (type !=6) ) ){
        return -1;
    }else{
        return type;
    }
}



int checksExib( vector<client> &clients, unsigned short id){
    int aux = -1;
    for(long unsigned int s; s< clients.size(); s++){
        if( clients[s].id == id){
            aux++;
        }
    }
    return aux; 
    //return 0 case id param is unused
}

unsigned short getsDestiny(unsigned short exhibitorID){

    std::cout << "\nchoose message's destiny:" << std::endl;
    unsigned int d;

    while(1){            
        std::cout << " [0] all exhibitors" << std::endl;
        std::cout << " [" << exhibitorID <<"] connected exhibitor" << std::endl;
        std::cin.ignore();
        scanf("%u", &d);

        if( (d == 0) || (d == exhibitorID)){
           break;
        }else{
            std::cout << "invalid destination" << std::endl;
        }
    }
    
	return( (d == 0) ? d : exhibitorID);			
}

unsigned short returnsID( const vector<client> client, char c ){
   if ( client.size() == 1){
       switch (c)
       {
       case 'e':
           return 4096; //first exhibitor's id
           break;
       case 'i':
            return 1; //first issuer's id
            break;
       default:
           return -1; //unrecognized type
           break;
       }
   }else{
       long unsigned int s = 1;
       int aux = 0;
       for( s = 1; s < client.size(); s++){
           if( client[s].id - client[s-1].id > 1 ){
            aux++; break;
           }
       }

      if( aux == 0){
        return ( client.back().id + 1);
      }else{
        return ( client[s-1].id + 1);
      }
       
       
   }
}
