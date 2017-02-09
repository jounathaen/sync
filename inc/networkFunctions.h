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
#include <libgen.h>     // For dirname
#include <utime.h>

/* cldoc:begin-category(NetworkFunctions) */

struct addrinfo hints;
struct addrinfo *addresses, *p;


/* create and connect to a TCP socket
   @portnum Port to create the socket on

   Uses SO_REUSEPORT and SO_REUSEADDR as sockoptions, so the server can be restarted immideatly
   Works with ipv4 and ipv6

   @return the created socket
 */
int createSocketListen(const char* portnum);


/* create a socet to send
   @ipaddr the ipaddr of the recieving socket. Names like "localhost" work as well
   @portnum port of the recieving socket
   Works with ipv4 and ipv6

   @return the created socket
 */
int createSocketSending(const char* ipaddr, const char* portnum);


/* Helper Function: recieves chars on the socket and prints them
   @sock socket to recieve on
 */
void recieveBuf(int sock);

/* check if a correct SYNC request was sent
   @sock socket to read from

   @return 0 if "START_SYNC" was recieved, -1 if not
 */
int recieveSync(int sock);


/* recieves a number of files and saves it
   @sock the socket to recieve from
   @prependdir parent directory name to save the relative directories inet

   recieves the length of the list, then recieves length times a file with <recieveFile>
 */
void recieveListFiles(int sock, const char * prependdir);


/* recieves files and writes them to filesystem
   @sock the socket to recieve from
   @prependdir parent directory name to save the relative directories in

   revieves first the filesize and mallocs space for the file in memory, then revieves the
   filename and the timestamp, and afterwards the file content. Then it stores the file to the filesystem
 */
void recieveFile(int sock, const char * prependdir);


/* recieve a <fileList>
   @sock the socket to recieve from
   @fl pointer to the <fileList> where to store the recieved list
 */
void recieveList(int sock, fileList * fl);


/* sends a <fileList>
   @sock the socket to send to
   @fl the <fileList> to send
 */
void sendList(int sock, fileList * fl);


/* sends buffer, make sure all parts are sent
   @sock the socket to semd to
   @buffer the buffer to send
   @length the length of the buffer

   @return -1 on error, otherwise the number of send bytes
 */
int sendBuf(int sock, char *buffer, int length);


/* sends all files from a given file list
   @sock the socket to send todo
   @fl the <fileList> to send
   @prependdir parent directory name to change relative directories in <fl> to absolutes

 */
void sendListFiles(int sock, fileList *fl, const char* prependdir);


/* sends file with metadata
   @sock the socket to recieve from
   @fle the 'fileListEntry' to send
   @prependdir parent directory name to change relative directories in fl to absolutes

   uses <sendFileContent> but also sends the meta information and changes relative filename to absolute
 */
void sendFile(int sock, fileListEntry * fle, const char* prependdir);


/* sends the content of a single file, used by <sendFile>
   @sock the socket to recieve from
   @filename the absolute filename of the file to send

   @return -1 on error, 0 else
 */
int sendFileContent(int sock, const char* filename);

/* cldoc:end-category() */
#endif //NETWORKFUNCTIONS_H
