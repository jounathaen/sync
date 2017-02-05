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
  // TODO Adapt
  printf("Usage: sync [OPTIONS] path/to/local/directory IPADDRESS:/path/to/remote/directory\n");
}


int main (int argc, char** argv){
  // TODO command line Parameters PORTNUM!!!
  signal(SIGINT, intterruphandler);

  mysocket = createSocketListen("1234");

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

    printf("Deleting...\n");
    removeFileList(&remoteFilesToDelete, directory);

    printf("\nRecieving...\n");
    recieveListFiles(currentsocket, directory);

    printf("Sending...\n");
    sendListFiles(currentsocket, &remoteFilesToTransfer, directory);

    shutdown(currentsocket, 2);
    close(currentsocket);
    free(remoteFiles.entry);
    free(remoteFilesToTransfer.entry);
    free(remoteFilesToDelete.entry);
    printf("Sucess\n");
  }

  return 0;
}
