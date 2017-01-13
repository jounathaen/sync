#include "sync.h"


void printUsage() {
  printf("Usage: sync [OPTIONS] path/to/local/directory IPADDRESS:/path/to/remote/directory\n");
}

int main(int argc, char** argv){

  // COMMAND LINE PARSING
  int c;
  struct option longopts[] = {
    { "delete",    required_argument, NULL, 'd' },
    { "recursive", no_argument,       NULL, 'r' },
    { "help",      no_argument,       NULL, 'h' },
    { 0, 0, 0, 0 }
  };

  while ((c = getopt_long(argc, argv, "d:rh", longopts, NULL)) != -1) {
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
    case 'r':
      recoursivelySyncing = 1;
      break;
    case 'h':
      print_usage();
      return 0;
    default:
      printf("character was %c\n", c);
    }
  }

  /* printf("\nargv:\n"); */
  /* for (int i = 0; i < argc; i++){ */
  /*   printf("%s\n", argv[i]); */
  /* } */
  /* return 0; */

  /* printf("Sizeof fileListEntry: %d \n", sizeof(fileListEntry) ); */

  printf ("\nFilelist1:\n");
  printf ("===============================\n");
  fileListInit(&ownFiles);

  createFileList("./test1", &ownFiles);
  printFileList(&ownFiles);


  printf ("\nFilelist2:\n");
  printf ("===============================\n");

  fileListInit(&fileList2);
  createFileList("./test2", &fileList2);
  printFileList(&fileList2);


  fileListInit(&filesToTransfer);
  createFileListToSend(&filesToTransfer, &fileList2, &ownFiles);
  /* printf("md5hash return is %d\n", compareMD5(ownFiles.entry[0].filehash, ownFiles.entry[0].filehash)); */
  /* if () */
  printf ("\nFiles to send:\n");
  printf ("===============================\n");
  printFileList(&filesToTransfer);

  free(filesToTransfer.entry);
  free(ownFiles.entry);
  free(fileList2.entry);
  /* use ftw to walk through file list  */
  /* use realloc to create dynamical growing lists  */
  /* use openssh to md5 the files  */


  /*parse Arguments*/

  /*get file list of directory*/

  /*send remote directory name to remote and initialize sync*/

  /*... */
}
