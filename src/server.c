#include "server.h"
#include "fileList.h"
#include <signal.h>


int mysocket;
int currentsocket;


void intterruphandler(int sig){
  printf("\nInterrupt! shutting down!\n");
  shutdown(currentsocket, 2);
  shutdown(mysocket, 2);
  close(currentsocket);
  close(mysocket);
  exit (0);
}


void printUsage(){
  // TODO More Information here
  printf("Usage: server [OPTIONS]");
}


int main (int argc, char** argv){
  // TODO command line Parameters PORTNUM!!!
  signal(SIGINT, intterruphandler);
  int c;
  struct option longopts[] = {
    { "help",      no_argument,       NULL, 'h' },
    { "port",      required_argument, NULL, 'p' },
    { 0, 0, 0, 0 }
  };

  while ((c = getopt_long(argc, argv, "p:h", longopts, NULL)) != -1) {
    switch (c) {
      case 'p':
      port=optarg;
      break;
    case 'h':
      printUsage();
      return 0;
    default:
      printf("character was %c\n", c);
    }
  }

  if(port == NULL)
    port = "1234";
  int portint = atoi(port);
  if (portint > 65535 || portint < 0){
    printf("ERROR: port %s is not a valid portnumber\n", port);
    return -1;
  }

  mysocket = createSocketListen(port);

  fileList remoteFiles, remoteFilesToDelete, remoteFilesToTransfer;
	if (listen(mysocket, 5) == -1) {
		printf("Error listen Socket\n");
		exit(1);
	}

  socklen_t slen = sizeof(struct sockaddr);
  struct sockaddr_in clientaddr;
  while(1){
    printf("Waiting for connection\n");
    currentsocket = accept(mysocket, (struct sockaddr*) &clientaddr, &slen);
    if (recieveSync(currentsocket) == -1){
      shutdown(currentsocket, 2);
      break;
    }

    /* TODO recieve directory */
    char * directory = "test2";
    printf ("Directory to sync: %s", directory);

    fileListInit(&remoteFiles);
    fileListInit(&remoteFilesToTransfer);
    fileListInit(&remoteFilesToDelete);


    createFileList(directory, &remoteFiles);
    removeDirname(&remoteFiles, directory);
    printf ("\nOwn File List of Dir: %s\n", directory);
    printf ("===============================\n");
    printFileList(&remoteFiles);

    printf("\nsending File List...\n");
    sendList(currentsocket, &remoteFiles);
    printf("Done!\n");

    printf("recieving File Lists...\n");
    recieveList(currentsocket, &remoteFilesToTransfer);
    recieveList(currentsocket, &remoteFilesToDelete);
    printf("Done!\n");
    printf ("\nRecieved File List to Transfer:\n");
    printf ("===============================\n");
    printFileList(&remoteFilesToTransfer);
    printf ("\nRecieved File List to Delete:\n");
    printf ("===============================\n");
    printFileList(&remoteFilesToDelete);

    printf("\nDeleting local Files...\n");
    removeFileList(&remoteFilesToDelete, directory);

    printf("\nRecieving Files...\n");
    recieveListFiles(currentsocket, directory);

    printf("Sending Files...\n");
    sendListFiles(currentsocket, &remoteFilesToTransfer, directory);

    shutdown(currentsocket, 2);
    close(currentsocket);
    free(remoteFiles.entry);
    free(remoteFilesToTransfer.entry);
    free(remoteFilesToDelete.entry);
    printf("Success\n");
  }

  return 0;
}
