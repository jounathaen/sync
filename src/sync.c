#include "sync.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void printUsage() {
  printf("Usage: sync [OPTIONS] path/to/local/directory IPADDRESS:/path/to/remote/directory\n");
}

int main(int argc, char** argv){


  // COMMAND LINE PARSING
  /* TODO error if argc < 3 */
  /* TODO Portnummber */
  int c;
  struct option longopts[] = {
    { "delete",    required_argument, NULL, 'd' },
    { "help",      no_argument,       NULL, 'h' },
    { 0, 0, 0, 0 }
  };

  while ((c = getopt_long(argc, argv, "d:h", longopts, NULL)) != -1) {
    switch (c) {
    case 'd':
      switch (optarg[0]){
      case 'a':
      case 'A':
        missingOptions = ask;
        break;
      case 'h':
      case 'H':
        missingOptions = deleteOnHost;
        break;
      case 'r':
      case 'R':
        missingOptions = deleteOnRemote;
        break;
      case 'm':
      case 'M':
        missingOptions = mergeEverything;
        break;
      default:
        printf("Error: unknown option for --delete\n");
        return -1;
      }
      break;
    case 'h':
      printUsage();
      return 0;
    default:
      printf("character was %c\n", c);
    }
  }

  /* printf("\nargv: argc %d\n", argc); */
  /* for (int i = 0; i < argc; i++){ */
  /*   printf("%i: %s\n", i, argv[i]); */
  /* } */

  //extract directories and IP addresses
  hostdirectory = argv[argc - 2];

  int seperatorpos = 0;
  for (int i = 0; i < 42; i++){
    if (argv[argc-1][i] == '/'){
      if (i<7){
        printf("Error: invalid IP Address of remote\n");
        return -1;
      }
      seperatorpos = i;
      break;
    }
  }
  if (seperatorpos == 0){
    printf("Error: invalid IP Address of remote\n");
    return -1;
  }
  /* printf("Seperator: %d\n", seperatorpos); */

  remoteip = malloc(seperatorpos*sizeof(char));
  strncpy(remoteip, argv[argc-1], seperatorpos);
  remotedirectory = argv[argc-1] + seperatorpos * sizeof(char);

  printf("Starting Sync with %s!\n", remoteip);
  fileListInit(&ownFiles);
  fileListInit(&remoteFiles);
  fileListInit(&filesToTransfer);
  fileListInit(&remoteFilesToTransfer);
  fileListInit(&filesToDelete);
  fileListInit(&remoteFilesToDelete);

  int mysocket = createSocketSending(remoteip, "1234");

  char *init="START_SYNC";
	send(mysocket, init, strlen(init), 0);

  /* TODO send directory name*/
  /* TODO timeout for connection */

  createFileList(hostdirectory, &ownFiles);
  removeDirname(&ownFiles, hostdirectory);
  printf ("\nOwn File List:\n");
  printf ("===============================\n");
  printFileList(&ownFiles);

  recieveList(mysocket, &remoteFiles);
  printf ("\nRecieved File List:\n");
  printf ("===============================\n");
  printFileList(&remoteFiles);

  printf("\nCreating File Lists:\n");
  createFileLists(&filesToTransfer, &remoteFilesToTransfer, &filesToDelete,
                  &remoteFilesToDelete, &ownFiles, &remoteFiles, missingOptions);


  /* Printing... */
  printf ("\nFiles to send:\n");
  printf ("===============================\n");
  printFileList(&filesToTransfer);

  printf ("\nFiles to delete:\n");
  printf ("===============================\n");
  printFileList(&filesToDelete);

  printf ("\nFiles to recieve:\n");
  printf ("===============================\n");
  printFileList(&remoteFilesToTransfer);

  printf ("\nFiles to delete on remote:\n");
  printf ("===============================\n");
  printFileList(&remoteFilesToDelete);


  printf("\nSending Lists...\n");
  sendList(mysocket, &remoteFilesToTransfer);
  sendList(mysocket, &remoteFilesToDelete);

  printf("Sending Files...\n");
  sendListFiles(mysocket, &filesToTransfer, hostdirectory);

  printf("Deleting...\n");
  removeFileList(&filesToDelete, hostdirectory);

  printf ("\nRecieving Files:\n");
  recieveListFiles(mysocket, hostdirectory);

  printf("Success\n\n");

  shutdown(mysocket, 2);

  free(filesToTransfer.entry);
  free(filesToDelete.entry);
  free(remoteFilesToTransfer.entry);
  free(remoteFilesToDelete.entry);
  free(ownFiles.entry);
  free(remoteFiles.entry);

  free(remoteip);
  return 0;
}
