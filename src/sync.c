#include "sync.h"



int main(int argc, char** argv){
  /* printf("Sizeof fileListEntry: %d \n", sizeof(fileListEntry) ); */

  printf ("===============================\n");
  fileListInit(&ownFiles);

  createFileList(".", &ownFiles);
  printFileList(&ownFiles);


  printf ("===============================\n");

  fileListInit(&fileList2);
  createFileList("./src", &fileList2);
  printFileList(&fileList2);


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
