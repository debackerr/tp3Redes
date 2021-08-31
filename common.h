#pragma once

#include <stdlib.h>
#include <iostream>
#include <vector>

#include <arpa/inet.h>

using namespace std;

struct header{
    //8 bytes header struct declaration
    unsigned short msgType;
    unsigned short msgOrigin;
    unsigned short msgDestiny;
    unsigned short msgOrder;
};

struct client{
    int socket;
    unsigned short id;
};


void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

unsigned short getsType();

unsigned short getsDestiny( unsigned short destiny );

int checksExib( vector<client> &clients, unsigned short id);

unsigned short returnsID( const vector<client> client, char c );
