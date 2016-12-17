#include "sync.h"
#include <malloc.h>

int createMD5(const char * filename, unsigned char c[MD5_DIGEST_LENGTH]){
    /* unsigned char c[MD5_DIGEST_LENGTH]; */
    int bytes;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    /* print it */
    /* for(int i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", c[i]); */
    /* printf (" \n"); */

    fclose (inFile);
    return 0;
}


int createFileList1(const char *name, const struct stat *status, int type) {
  if(fileList1.index>=fileList1.length){
    fileListEntry *newpointer;
    newpointer = (fileListEntry*) realloc((void*) fileList1.entry, (fileList1.length + INCR_STEP_SIZE)*sizeof(fileListEntry));
    if (!newpointer){
      printf("ERROR while reallocating memory");
      return -1;
    }
    fileList1.entry = newpointer;
    fileList1.length += INCR_STEP_SIZE;
  }
  if(type == FTW_NS)
    return 0;

  if(type == FTW_F) {
    /* add files to the filelist */
    strcpy(fileList1.entry[fileList1.index].filename, name);
    fileList1.entry[fileList1.index].timestamp = status->st_mtime;
    createMD5(name, fileList1.entry[fileList1.index].filehash);

    fileList1.index++;
  }
  return 0;
}


void printFileList(fileList fL){
  char buff[20];
  for (unsigned int i = 0; i< fL.length; i++){
    /* the time  */
    strftime(buff, sizeof(buff), "%b %d %H:%M", (struct tm*) localtime (&fL.entry[i].timestamp));
    printf("%-20s\t", buff);
    /* the name */
    printf("%-40s\t", fL.entry[i].filename);
    /* the hash */
    for(int j = 0; j < MD5_DIGEST_LENGTH; j++) printf("%02x", fL.entry[i].filehash[j]);
    printf("\n");
  }
}


int main(int argc, char** argv){
  /* printf("Sizeof fileListEntry: %d \n", sizeof(fileListEntry) ); */


  fileList1.entry = (fileListEntry*)malloc(INCR_STEP_SIZE*sizeof(fileListEntry));
  fileList1.length = INCR_STEP_SIZE;
  fileList1.index = 0;


  if (ftw("." , createFileList1, 20) == -1) {
      printf("nftw error\n");
  }
  fileList1.length = fileList1.index;
  fileList1.entry = (fileListEntry*) realloc(fileList1.entry, fileList1.length * sizeof(fileListEntry));

  printFileList(fileList1);

  printf("sizeof of List on Memory: %d\n", (int) malloc_usable_size((void*)fileList1.entry));
  printf("length of file list: %d", fileList1.length);
  free(fileList1.entry);
  /* use ftw to walk through file list  */
  /* use realloc to create dynamical growing lists  */
  /* use openssh to md5 the files  */


  /*parse Arguments*/

  /*get file list of directory*/

  /*send remote directory name to remote and initialize sync*/

  /*... */
}
