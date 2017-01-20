#include "fileList.h"


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

int compareMD5(const unsigned char * hash1, const unsigned char * hash2){
  for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
    if(hash1[i] != hash2[i])
      return -1;
  }
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
  /* add only files to the filelist */
  if(type == FTW_F) {
    if(addFile(activeFileList, activeFileList->index, name, status->st_mtime, status->st_size))
      return -1;
    activeFileList->index++;
  }
  return 0;
}


int addFile(fileList *fL, const unsigned int index, const char *name, time_t timestamp, int size){
  if(index == fL->length){
    fileListEntry *newpointer = (fileListEntry*) realloc((void*) fL->entry, (fL->length + INCR_STEP_SIZE)*sizeof(fileListEntry));
    if (!newpointer){
      printf("ERROR while reallocating memory");
      return -1;
    }
    fL->entry = newpointer;
    fL->length += INCR_STEP_SIZE;
  }else if (index > fL->length){
    printf("ERROR: writing to position outside of List\n");
  }

  if(strlen(name) >= FILENAME_MAX_SIZE){
    printf("ERROR: Filename %s too long\n", name);
    return -1;
  }
  strcpy(fL->entry[index].filename, name);
  fL->entry[index].timestamp = timestamp;
  fL->entry[index].filesize= size;
  createMD5(name, fL->entry[index].filehash);
  return 0;
}


int createFileList(const char* filepath, fileList *fL){
  setActiveList(fL);
  fL->index = 0;
  printf("filepath: %s\n", filepath);
  if (ftw(filepath, handleFile, 20) == -1) {
    printf("nftw error\n");
    fL->length = 0;
    return -1;
  }
  printf("created FileList\n");
  printf("length: %d\n", fL->length);

  fileListEntry *newpointer = (fileListEntry*) realloc((void*) fL->entry, fL->index * sizeof(fileListEntry));
  if (newpointer==NULL){
    printf("ERROR while shrinkening memory of fileList");
    return -1;
  }
  fL->entry = newpointer;
  fL->length = fL->index;
  fL->index = 0;
  return 0;
}


enum ComparisionReturnTypes compareEntries(fileListEntry *entry1, fileListEntry * entry2){
  /* extract filename from dirname */
  if(strcmp(strrchr(entry1->filename, '/'), strrchr(entry2->filename, '/')) == 0){
    /* Filenames are the same  */
    if (compareMD5(entry1->filehash, entry2->filehash) == 0){
      /* hashes are the same */
      return FilesEqual;
    }
    else{
      /* hashes differ */
      double timediff = difftime(entry1->timestamp, entry2->timestamp);
      if(timediff > 0){
        return File1Newer;
      }else if (timediff == 0){
        return GeneralConfusion;
      }
      else{
        return File2Newer;
      }
    }
  }
  return FilesNotTheSame;
}


void createFileListToSend(fileList * resultingList, fileList * hostFiles, fileList * remoteFiles){
  /* printf("Creatinig filesToSend List\n ======================\n"); */
  setActiveList(resultingList);
  for (unsigned int i = 0; i < hostFiles->length; i++){
    for (unsigned int j = 0; j < remoteFiles->length; j++){
      /* printf("Index: %d \t", resultingList->index); */
      /* printf("comparing %s and %s\t", &hostFiles->entry[i].filename, &remoteFiles->entry[j].filename); */

      switch (compareEntries(&hostFiles->entry[i], &remoteFiles->entry[j])){
      case FilesEqual:
      case File2Newer:
        /* printf("file %s is newer or equal\n", &remoteFiles->entry[j].filename); */
        /* doesn't has to be added to list */
        goto nextfile;
      case File1Newer:
        /* printf("adding file %s to the list\n", &remoteFiles->entry[j].filename); */
        /* add file to resuling list */
        addFile(resultingList, resultingList->index, hostFiles->entry[i].filename,
                hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
        resultingList->index++;
        goto nextfile;
      case FilesNotTheSame:
        /* printf("Files are different\n"); */
        /* try next file in remoteFiles list */
        break;
      case GeneralConfusion:
        /* printf("Error: You really have two files with same Name and Time, but different content???\n"); */
        goto nextfile;
        break;

      }
    }
    /* printf("File not found on Remote, has to send\n"); */
    addFile(resultingList, resultingList->index, hostFiles->entry[i].filename,
            hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
    resultingList->index++;

  nextfile:;

  }
  fileListEntry *newpointer = (fileListEntry*) realloc((void*) resultingList->entry, resultingList->index * sizeof(fileListEntry));
  if (newpointer==NULL){
    printf("ERROR while shrinkening memory of fileList");
    return;
  }
  resultingList->entry = newpointer;
  resultingList->length = resultingList->index;
  resultingList->index = 0;

}


void printFileList(fileList *fL){
  char buff[20];
  for (unsigned int i = 0; i< fL->length; i++){
    /* the time  */
    strftime(buff, sizeof(buff), "%b %d %H:%M", (struct tm*) localtime (&fL->entry[i].timestamp));
    printf("%-5s\t", buff);
    /* the size */
    printf("%ld\t",fL->entry[i].filesize);
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
