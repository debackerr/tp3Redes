#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

struct header{
    //8 bytes header struct declaration
    unsigned short msgType;
    unsigned short msgOrigin;
    unsigned short msgDestiny;
    unsigned short msgOrder;
};

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

unsigned short getsType();
unsigned short getsDestiny();
