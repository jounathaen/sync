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


void fileListInit(fileList *newFileList){
  newFileList->entry = (fileListEntry*)malloc(INCR_STEP_SIZE*sizeof(fileListEntry));
  newFileList->length = INCR_STEP_SIZE;
  newFileList->index = 0;
}


void setActiveList (fileList *newFileList){
  activeFileList = newFileList;
}


int handleFile(const char *name, const struct stat *status, int type) {
  /* expand the file list, when it is full */
  if(activeFileList->index +1 >= activeFileList->length){
    fileListEntry *newpointer = (fileListEntry*) realloc((void*) activeFileList->entry, (activeFileList->length + INCR_STEP_SIZE)*sizeof(fileListEntry));
    if (!newpointer){
      printf("ERROR while reallocating memory");
      return -1;
    }
    activeFileList->entry = newpointer;
    activeFileList->length += INCR_STEP_SIZE;
  }
  /* Folders are not of interest */
  if(type == FTW_NS)
    return 0;

  /* add files to the filelist */
  if(type == FTW_F) {
    strcpy(activeFileList->entry[activeFileList->index].filename, name);
    activeFileList->entry[activeFileList->index].timestamp = status->st_mtime;
    createMD5(name, activeFileList->entry[activeFileList->index].filehash);

    activeFileList->index++;
  }
  return 0;
}


int createFileList(const char* filepath, fileList *fL){
  setActiveList(fL);
  printf("filepath: %s\n", filepath);
  if (ftw(filepath, handleFile, 20) == -1) {
    printf("nftw error\n");
    return -1;
  }
  fL->length = fL->index;
  printf("created FileList\n");

  /* fileListEntry *newpointer = (fileListEntry*) realloc(&fL->entry, fL->length * sizeof(fileListEntry)); */
  /* if (newpointer==NULL){ */
  /*   printf("ERROR while shrinkening memory of fileList"); */
  /*   return -1; */
  /* } */
  /* fL->entry = newpointer; */
  /* printf("mem reallocated\n"); */
  return 0;
}



void printFileList(fileList *fL){
  char buff[20];
  for (unsigned int i = 0; i< fL->length; i++){
    /* the time  */
    strftime(buff, sizeof(buff), "%b %d %H:%M", (struct tm*) localtime (&fL->entry[i].timestamp));
    printf("%-5s\t", buff);
    /* the name */
    printf("%-40s\t", fL->entry[i].filename);
    /* the hash */
    for(int j = 0; j < MD5_DIGEST_LENGTH; j++) printf("%02x", fL->entry[i].filehash[j]);
    printf("\n");
  }
  printf("sizeof of List on Memory: %d\n", (int) malloc_usable_size((void*)fL->entry));
  printf("length of file list: %d\n", fL->length);
  printf("index of file list: %d\n", fL->index);
}


int main(int argc, char** argv){
  /* printf("Sizeof fileListEntry: %d \n", sizeof(fileListEntry) ); */

  printf ("===============================\n");
  fileListInit(&fileList1);

  createFileList(".", &fileList1);
  printFileList(&fileList1);


  printf ("===============================\n");

  fileListInit(&fileList2);
  createFileList("./src", &fileList2);
  printFileList(&fileList2);



  free(fileList1.entry);
  free(fileList2.entry);
  /* use ftw to walk through file list  */
  /* use realloc to create dynamical growing lists  */
  /* use openssh to md5 the files  */


  /*parse Arguments*/

  /*get file list of directory*/

  /*send remote directory name to remote and initialize sync*/

  /*... */
}
