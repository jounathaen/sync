#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>        // for file tree walker
#include <time.h>       // for timestamps
#include <sys/stat.h>   // for file stats
#include <openssl/md5.h>// for the md5 algorithm

#define INCR_STEP_SIZE 32

/* //adjust the Sizes */
typedef struct fileListEntry{
  char filename[256-sizeof(time_t)-MD5_DIGEST_LENGTH*sizeof(unsigned char)];
  unsigned char filehash[MD5_DIGEST_LENGTH];
  time_t timestamp;
}fileListEntry;

typedef struct fileList{
  fileListEntry *entry;
  unsigned int length;
  unsigned int index;
}fileList;

fileList *activeFileList;
fileList fileList1, fileList2;

/*
  Funtion calculates the MD5 Hash of a given File and prints it
*/
int createMD5(const char * filename, unsigned char c[MD5_DIGEST_LENGTH]);

/*
  initializes File list. Allocates new Memory of size INCR_STEP_SIZE and initializes length and index to 0
*/
void fileListInit(fileList *newFileList);

/*
  Sets the fileList for createFileList, as function cant pass any more arguments
*/
void setActiveList (fileList *newFileList);

/*
  Function that is executed for every file, the filewalker walks through
*/
int handleFile(const char *name, const struct stat *status, int type); 

/*
  creates a file list from the given filepath and stores it in fL
*/
int createFileList(const char* filepath, fileList *fL);

/*
  Printing out the File List (for Debugging)
*/
void printFileList(fileList fL);

#endif /* SYNC_H */
