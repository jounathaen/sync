#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

#include "fileList.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>



struct addrinfo hints;
struct addrinfo *addresses, *p;


// create and connect to TCP socket
int createSocketListen(const char* portnum);

// create a socet to send
int createSocketSending(const char* ipaddr, const char* portnum);


int recieveBuf(int sock);

//check if SYNC request is correct
int recieveSync(int sock);


//recieves a list of files and saves it
void recieveList(int sock);


//recieves files and writes them to filesystem
void recieveFile(int sock);


// sends buffer, make sure all parts are sent
int sendBuf(int sock, char *buffer, int length);


// sends a single file
int sendFileContent(int sock, const char* filename);


//sends all files from a given file list
void sendList(int sock, fileList *fl);

#endif //NETWORKFUNCTIONS_H
