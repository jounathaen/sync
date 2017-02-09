#include "fileList.h"


int createMD5(const char * filename, unsigned char output[MD5_DIGEST_LENGTH]){
    /* unsigned char c[MD5_DIGEST_LENGTH]; */
    int bytes;
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return -1;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (output, &mdContext);
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
    createMD5(name, activeFileList->entry[activeFileList->index].filehash);
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
  /* TODO Bug, ./a and ./somedir/a */
  if(strcmp(entry1->filename, entry2->filename) == 0){
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


void createFileLists(fileList * sendList, fileList * recieveList, fileList * deletionList,
                     fileList * deleteRemoteList,  fileList * hostFiles,
                     fileList * remoteFiles, enum missingHandling option){
  /* setActiveList(sendList); */
  for (unsigned int i = 0; i < hostFiles->length; i++){
    for (unsigned int j = 0; j < remoteFiles->length; j++){
      /* printf("Index: %d \t", sendList->index); */
      /* printf("comparing %s and %s\t", &hostFiles->entry[i].filename, &remoteFiles->entry[j].filename); */

      switch (compareEntries(&hostFiles->entry[i], &remoteFiles->entry[j])){
      case FilesEqual:
        /* do notthing */
        goto nextfile;
      case File2Newer:
        /* remote is newer, delete and fetch from remote*/
        addFile(deletionList, sendList->index, hostFiles->entry[i].filename,
                hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
        deletionList->index++;
        addFile(recieveList, recieveList->index, remoteFiles->entry[j].filename,
                remoteFiles->entry[j].timestamp, remoteFiles->entry[j].filesize);
        recieveList->index++;
        goto nextfile;
      case File1Newer:
        /* printf("adding file %s to the list\n", &remoteFiles->entry[j].filename); */
        /* add file to resuling list */
        addFile(sendList, sendList->index, hostFiles->entry[i].filename,
                hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
        sendList->index++;
        addFile(deleteRemoteList, deleteRemoteList->index, remoteFiles->entry[j].filename,
                remoteFiles->entry[j].timestamp, remoteFiles->entry[j].filesize);
        deleteRemoteList->index++;
        goto nextfile;
      case FilesNotTheSame:
        /* printf("Files are different\n"); */
        /* try next file in remoteFiles list */
        break;
      case GeneralConfusion:
        /* TODO maybe ask */
        printf("Error: Skipping File %s\n", hostFiles->entry[i].filename);
        goto nextfile;
      }
    }
    /* file not found on remote */
    char answ = 0;
    switch(option){
    case deleteOnHost:
      printf("File %s not found on remote, deleting it on local\n", hostFiles->entry[i].filename);
      addFile(deletionList, deletionList->index, hostFiles->entry[i].filename,
              hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
      deletionList->index++;
      break;
    case deleteOnRemote:
      /* send it, this option does not affect local behaviour*/
      /* same action as mergeEverything */
    case mergeEverything:
      printf("File %s not found on Remote, sending it\n", hostFiles->entry[i].filename);
      addFile(sendList, sendList->index, hostFiles->entry[i].filename,
              hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
      sendList->index++;
      break;
    case ask:
    askbeginn:
      answ = 0;
      printf("File %s not found on remote. [T]ransfer or [D]elete local File?\n",
             hostFiles->entry[i].filename);
      scanf("%c%*c", &answ);
      if(answ == 'T' || answ == 't'){
        printf("Ok, transferring it!\n");
        addFile(sendList, sendList->index, hostFiles->entry[i].filename,
                hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
        sendList->index++;
      }
      else if(answ == 'D' || answ == 'd'){
        printf("Ok, deleting it!\n");
        addFile(deletionList, deletionList->index, hostFiles->entry[i].filename,
                hostFiles->entry[i].timestamp, hostFiles->entry[i].filesize);
        deletionList->index++;
      }
      else{
        printf("Error: Follow the Instructions! (stupid people...)\n");
        goto askbeginn;
      }
      break;
    }
  nextfile:;
  }

  /* search for files not on the host but on the remote  */
  for (unsigned int i = 0; i < remoteFiles->length; i++){
    for (unsigned int j = 0; j < hostFiles->length; j++){
      switch (compareEntries(&remoteFiles->entry[i], &hostFiles->entry[j])){
      case GeneralConfusion:
        printf("Error: Skipping File %s\n", hostFiles->entry[i].filename);
      case FilesEqual:
      case File2Newer:
      case File1Newer:
        goto remoteNextfile;
      case FilesNotTheSame:
        /* printf("Files are different\n"); */
        /* try next file in remoteFiles list */
        break;
      }
    }
    /* file not found on host*/
    char answ = 0;
    switch(option){
    case deleteOnRemote:
      printf("File %s not found on host, deleting it on remote\n", remoteFiles->entry[i].filename);
      addFile(deleteRemoteList, deleteRemoteList->index, remoteFiles->entry[i].filename,
              remoteFiles->entry[i].timestamp, remoteFiles->entry[i].filesize);
      deleteRemoteList->index++;
      break;
    case deleteOnHost:
      /* send it, this option does not affect local behaviour*/
      /* same action as mergeEverything */
    case mergeEverything:
      printf("File %s not found on host, sending it\n", remoteFiles->entry[i].filename);
      addFile(recieveList, recieveList->index, remoteFiles->entry[i].filename,
              remoteFiles->entry[i].timestamp, remoteFiles->entry[i].filesize);
      recieveList->index++;
      break;
    case ask:
    remoteaskbeginn:
      answ = 0;
      printf("File %s not found on host. [T]ransfer or [D]elete remote File?\n",
             remoteFiles->entry[i].filename);
      scanf("%c%*c", &answ);
      if(answ == 'T' || answ == 't'){
        printf("Ok, transferring it!\n");
        addFile(recieveList, recieveList->index, remoteFiles->entry[i].filename,
                remoteFiles->entry[i].timestamp, remoteFiles->entry[i].filesize);
        recieveList->index++;
      }
      else if(answ == 'D' || answ == 'd'){
        printf("Ok, deleting it!\n");
        addFile(deleteRemoteList, deleteRemoteList->index, remoteFiles->entry[i].filename,
                remoteFiles->entry[i].timestamp, remoteFiles->entry[i].filesize);
        deleteRemoteList->index++;
      }
      else{
        printf("Error: Follow the Instructions! (stupid people...)\n");
        goto remoteaskbeginn;
      }
      break;
    }
  remoteNextfile:;
  }


  /* clean up the lists */
  fileListEntry *newpointer;
  sendList->length = sendList->index;
  if (sendList->length > 0) {
    newpointer = (fileListEntry*) realloc((void*) sendList->entry,
                                          sendList->index * sizeof(fileListEntry));
    if (newpointer==NULL){
      printf("ERROR while shrinkening memory of sendList");
      return;
    }
    sendList->entry = newpointer;
    sendList->index = 0;
  }
  deletionList->length = deletionList->index;
  if (deletionList->length > 0) {
    newpointer = (fileListEntry*) realloc((void*) deletionList->entry,
                                          deletionList->index * sizeof(fileListEntry));
    if (newpointer==NULL){
      printf("ERROR whi shrinkening memory of deletionList");
      return;
    }
    deletionList->entry = newpointer;
    deletionList->index = 0;
  }

  recieveList->length = recieveList->index;
  if (recieveList->length > 0) {
    newpointer = (fileListEntry*) realloc((void*) recieveList->entry,
                                          recieveList->index * sizeof(fileListEntry));
    if (newpointer==NULL){
      printf("ERROR whi shrinkening memory of recieveList");
      return;
    }
    recieveList->entry = newpointer;
    recieveList->index = 0;
  }
  deleteRemoteList->length = deleteRemoteList->index;
  if (deleteRemoteList->length > 0) {
    newpointer = (fileListEntry*) realloc((void*) deleteRemoteList->entry,
                                          deleteRemoteList->index * sizeof(fileListEntry));
    if (newpointer==NULL){
      printf("ERROR while shrinkening memory of deleteRemoteList");
      return;
    }
    deleteRemoteList->entry = newpointer;
    deleteRemoteList->index = 0;
  }
}


void removeDirname(fileList *fL,  const char* dirname){
  for (unsigned int i = 0; i< fL->length && i < 50; i++){
    char substring [FILENAME_MAX_SIZE];
    int sublen = 0;
    sublen = strlen(dirname);
    strcpy(substring, fL->entry[i].filename + sublen);
    strcpy(fL->entry[i].filename, substring);
  }
}


void addDirname(fileList *fL,  const char* dirname){
  for (unsigned int i = 0; i< fL->length && i < 50; i++){
    char buff [FILENAME_MAX_SIZE];
    strcpy(buff, dirname);
    strcat(buff, fL->entry[i].filename);
    strcpy(fL->entry[i].filename, buff);
  }
}


void printFileList(fileList *fL){
  if (fL->length == 0){
    printf(">> File list is empty...\n");
    return;
  }
  char buff[20];
  for (unsigned int i = 0; i< fL->length && i < 50; i++){
    /* the time  */
    strftime(buff, sizeof(buff), "%b %d %H:%M", (struct tm*) localtime (&fL->entry[i].timestamp));
    printf("%-5s\t", buff);
    /* the size */
    printf("%ld\t",fL->entry[i].filesize);
    /* the name */
    printf("%-40s\t", fL->entry[i].filename);
    /* the hash */
    /* TODO only print filehash if it is not 0 */
    for(int j = 0; j < MD5_DIGEST_LENGTH; j++) printf("%02x", fL->entry[i].filehash[j]);
    printf("\n");
  }
  printf("sizeof of List on Memory: %d Byte\n", (int) malloc_usable_size((void*)fL->entry));
  printf("length of file list: %d\n", fL->length);
  /* printf("index of file list: %d\n", fL->index); */
}


int removeFileList(fileList* fL, const char* prependdir){
  for (unsigned int i = 0; i < fL->length; i++){
    char buff [FILENAME_MAX_SIZE];
    strcpy(buff, prependdir);
    strcat(buff,fL->entry[i].filename);
    printf("deleting file %s\n", buff);
    if (remove(buff) != 0){
      printf("Error when deleting file: %s\n", buff);
      return -1;
    }
  }

  /* free(fL->entry); */
  fL->length = 0;
  fL->index = 0;

  return 0;
}
