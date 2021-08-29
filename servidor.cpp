#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common.h"

#define BUFSZ 500

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv ){

    const char * PORT = argv[1]; //gets  PORT
    
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    //char buf[256];    // buffer for client data
    
    char buf[BUFSZ];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    printf("server ready for connections\n");
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);

            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            struct header servHeader;
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, &servHeader, sizeof( header), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        
                    } else {
                        // we got some data from a client
                        printf("\n new message from %d: ",i);
                        printf("recvd: %u %u %u %u", servHeader.msgType, servHeader.msgDestiny, servHeader.msgOrigin, servHeader.msgOrder);
                        
                        switch ( servHeader.msgType )
                        {
                        case 3:
                            servHeader.msgType = 1; //message accepted;
                            servHeader.msgOrigin++;
                            servHeader.msgDestiny++;
                            
                            nbytes = send(i, &servHeader, sizeof(header),0);
                            //nbytes = send(i, &servHeader, sizeof(header),0);
                            break;
                        case 4:
                            servHeader.msgType = 1;
                            break;
                        case 5:
                            servHeader.msgType = 1;

                            memset(buf, 0, BUFSZ);
                            unsigned short size;
                            
                            nbytes = recv(i, &size,sizeof(size),0); //receives the message size first
                            printf("\nincomming message of %u characteres\n", size);
                            //nbytes = send(i, &servHeader, sizeof(header),0);  //sends to exhibitor                            
                            
                            nbytes = recv(i, buf, size,0); //receives message 
                            puts(buf);
                            // nbytes = send(i, &servHeader, sizeof(header),0); 
                            if( servHeader.msgDestiny == 0){

                                for(j = 0; j <= fdmax; j++) {
                                    // send to everyone!
                                    if (FD_ISSET(j, &master)) {
                                    // except the listener and ourselves
                                        if (j != listener && j != i) {
                                            if (send(j, &servHeader, nbytes, 0) == -1) {
                                                perror("send");
                                            }
                                        }   
                                    }
                                }

                            }else{
                                printf("\n mandar pro exibidor");
                            }
                            break;

                        case 6:
                            servHeader.msgType = 7; //sends CLIST
                            break;
                        case 8:
                            servHeader.msgType = 9;
                            break;
                        default:
                            printf("\nERROR\n");
                            exit(EXIT_FAILURE);
                        }
                        
                        printf("\nsent: %u %u %u %u", servHeader.msgType, servHeader.msgDestiny, servHeader.msgOrigin, servHeader.msgOrder);
                        
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)
    return 0;
}
