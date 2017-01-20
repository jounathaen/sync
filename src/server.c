#include "server.h"


void printUsage(){
  // TODO Adapt
  printf("Usage: sync [OPTIONS] path/to/local/directory IPADDRESS:/path/to/remote/directory\n");
}

int main (int argc, char** argv){
  /* int mysocket = createSocketSending(); */
  int mysocket = createSocketListen();

  listen(mysocket, 5);
  socklen_t slen = sizeof(struct sockaddr);
  struct sockaddr_in clientaddr;
  int currentsocket = accept(mysocket, (struct sockaddr*) &clientaddr, &slen);

  /* recieveBuf(currentsocket); */

  recieveList(currentsocket);

  return 0;
}
