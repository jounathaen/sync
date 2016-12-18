#include "sync.h"



int main(int argc, char** argv){
  /* printf("Sizeof fileListEntry: %d \n", sizeof(fileListEntry) ); */

  printf ("\nFilelist3:\n");
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
