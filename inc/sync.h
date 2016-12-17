#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>        // for file tree walker
#include <time.h>       // for timestamps
#include <sys/stat.h>   // for file stats
#include <openssl/md5.h>// for the md5 algorithm

#define INCR_STEP_SIZE 64

/* //adjust the Sizes */
typedef struct fileListEntry{
  char filename[512-sizeof(time_t)-MD5_DIGEST_LENGTH*sizeof(unsigned char)];
  unsigned char filehash[MD5_DIGEST_LENGTH];
  time_t timestamp;
}fileListEntry;

typedef struct fileList{
  fileListEntry *entry;
  unsigned int length;
  unsigned int index;
}fileList;

fileList fileList1; 

/*
  Funtion calculates the MD5 Hash of a given File and prints it
*/
int createMD5(const char * filename, unsigned char c[MD5_DIGEST_LENGTH]);

/*
  Printing out the File List (for Debugging)
 */
void printFileList(fileList fL);

/*
  Function that is executed for every file, the filewalker walks through
*/
int createFileList1(const char *name, const struct stat *status, int type); 

#endif /* SYNC_H */
