#ifndef FILELIST_H
#define FILELIST_H

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>        // for file tree walker
#include <time.h>       // for timestamps
#include <sys/stat.h>   // for file stats
#include <openssl/md5.h>// for the md5 algorithm
#include <libgen.h>     // for basename

#define INCR_STEP_SIZE 32
#define FILENAME_MAX_SIZE 256-sizeof(time_t)-MD5_DIGEST_LENGTH*sizeof(unsigned char)-sizeof(unsigned long int)


typedef struct fileListEntry{
  char filename[FILENAME_MAX_SIZE];
  unsigned char filehash[MD5_DIGEST_LENGTH];
  time_t timestamp;
  unsigned long int filesize;
}fileListEntry;

typedef struct fileList{
  fileListEntry *entry;
  unsigned int length;
  unsigned int index;
}fileList;

// needed, as ftw does not allow further arguments
fileList *activeFileList;
int curdirnamelen;

// return types for comparing two files
enum ComparisionReturnTypes{
  FilesEqual,
  FilesNotTheSame,
  File1Newer,
  File2Newer,
  GeneralConfusion
};

// Options to handle missing Items
enum missingHandling{
  deleteOnHost,
  deleteOnRemote,
  mergeEverything,
  ask
};


/*
  Funtion calculates the MD5 Hash of a given File and prints it
*/
int createMD5(const char * filename, unsigned char c[MD5_DIGEST_LENGTH]);

/*
  compares two MD5 Hashes, returns 0 if equal, else -1
 */
int compareMD5(const unsigned char *hash1, const unsigned char *hash2);

/*
  initializes File list. Allocates new Memory of size INCR_STEP_SIZE and initializes length and index to 0
*/
void fileListInit(fileList *newFileList);

/*
  Sets the fileList for createFileList, as function cant pass any more arguments
*/
void setActiveList(fileList *newFileList);

/*
  Function that is executed for every file, the filewalker walks through.
  When 'type' is a file, it gets added to activeFileList.
  setActiveList must be used before
*/
int handleFile(const char *name, const struct stat *status, int type);

/*
  adds the file 'name' with 'timestamp' to the fileList fL at index.
 */
int addFile(fileList *fL, const unsigned int index, const char *name, time_t timestamp, int size);

/*
  creates a file list from the given filepath and stores it in fL
*/
int createFileList(const char* filepath, fileList *fL);

/*
  Compares two fileListEntries. returns ComparisionReturnTypes. Use for comparing two lists
 */
enum ComparisionReturnTypes compareEntries(fileListEntry *entry1, fileListEntry * entry2);

/*
  Creates a list of Files which are notexistent on remotefiles and stores it into resultingList
 */
void createFileLists(fileList * sendList, fileList * recieveList, fileList * deletionList,
                     fileList * deleteRemoteList,  fileList * hostFiles,
                     fileList * remoteFiles, enum missingHandling option);

/*
  Removes the given dirname from all filenames in the filelist
 */
void removeDirname(fileList *fL,  const char* dirname);

/*
  Prepends the given Dirname to all filenames in the filelist
 */
void addDirname(fileList *fL,  const char* dirname);

/*
  Printing out the File List (for Debugging)
*/
void printFileList(fileList *fL);

/*
  Delete all Files in a fileList
 */
int removeFileList(fileList* fL, const char* prependdir);


#endif //FILELIST_H
