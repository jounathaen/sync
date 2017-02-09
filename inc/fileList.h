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

/* cldoc:begin-category(fileList) */

#define INCR_STEP_SIZE 32
#define FILENAME_MAX_SIZE 256-sizeof(time_t)-MD5_DIGEST_LENGTH*sizeof(unsigned char)-sizeof(unsigned long int)

// A fileListEntry is a element of <fileList> storing the Metadata of a file
typedef struct fileListEntry{
  char filename[FILENAME_MAX_SIZE];
  unsigned char filehash[MD5_DIGEST_LENGTH];
  time_t timestamp;
  unsigned long int filesize;
}fileListEntry;

// A fileList is a data structure which stores metadata of a list of files, so they can be compared
typedef struct fileList{
  // Array to the Elements of the fileList
  fileListEntry *entry;
  // Number of Elements in fileList
  unsigned int length;
  // Index used when iterating through the fileList
  unsigned int index;
}fileList;


// needed, as ftw does not allow further arguments but we still want to add the file to different fileLists
fileList *activeFileList;

// return types for comparing two files with <compareEntries>
enum ComparisionReturnTypes{
  FilesEqual,
  FilesNotTheSame,
  File1Newer,
  File2Newer,
  GeneralConfusion
};

// Options to handle missing Items when creating <fileList>s with <createFileLists>
enum missingHandling{
  deleteOnHost,
  deleteOnRemote,
  mergeEverything,
  ask
};


/*  Funtion calculates the MD5 Hash of a given File and prints it.
   @filename filename of the file to create the hash from.
   @output the buffer, where the hash is stored in.

   uses the MD5 functions from libcrypto.

   @return 0 on success -1 when file not readable
 */
int createMD5(const char * filename, unsigned char output[MD5_DIGEST_LENGTH]);


/* compares two MD5 Hashes, returns 0 if equal, else -1
   @hash1 The first hash
   @hash2 The second hash

   @return 0 when hashes equal, -1 when not
 */
int compareMD5(const unsigned char *hash1, const unsigned char *hash2);


/* initializes File list. Allocates new Memory of size 'INCR_STEP_SIZE' and initializes length and index to 0
  @newFileList Pointer to the file list that should be initialized
*/
void fileListInit(fileList *newFileList);


/* Sets the fileList for <createFileList>, as function cant pass any more arguments
   @newFileList Pointer to the fileList that should be marked active
*/
void setActiveList(fileList *newFileList);


/* Function that is executed for every file, the filewalker walks through.
  @name filename
  @status file status e.g. modification time, size, ...
  @type filetype from FTW

  When 'type' is a file, it gets added to activeFileList.
  setActiveList must be used before

  @return 0 on success, -1 on error when adding file to filelist
*/
int handleFile(const char *name, const struct stat *status, int type);


/* Adds a file to a fileList
   @fL the file list where to add the filewalker
   @index location in the <fileList> where to store the element
   @name the name of the file to be added
   @timestamp the timestamp of the file to be added
   @size the filesize of the file to be added

   @return -1 on error 0 otherwise
 */
int addFile(fileList *fL, const unsigned int index, const char *name, time_t timestamp, int size);


/* creates a file list from the given filepath and stores it in fL
   @filepath parent directory to start searching for filesize
   @fL <fileList> to store the file information information

   @return -1 on Error, 0 else
*/
int createFileList(const char* filepath, fileList *fL);


/* Compares two fileListEntries.
   @entry1 first entry to compares
   @entry2 second entry to compare

   Use for comparing two lists. Finds same file by filename. Decision is based on timestamp and hash

   @return information, which entry is newer, or are they the same or totally different
 */
enum ComparisionReturnTypes compareEntries(fileListEntry *entry1, fileListEntry * entry2);


/* Main logic of the programm.
   @sendList list of files to transfer from client to server
   @recieveList list of files to transfer from server to client
   @deletionList list of files to delete on client side
   @deleteRemoteList list of files to delete on server side
   @hostFiles fileList of the directory to sync from
   @remoteFiles fileList of the directory to sync to
   @option handling of the files that are not found on one of the devices

   creates sendList, recieveList, deleteRemoteList and deletionList from hostFiles and remoteFiles.
 */
void createFileLists(fileList * sendList, fileList * recieveList, fileList * deletionList,
                     fileList * deleteRemoteList,  fileList * hostFiles,
                     fileList * remoteFiles, enum missingHandling option);


/* Removes the given dirname from all filenames in the filelist
   @fL fileList to remove the dirname from
   @dirname string which is removed from all elements of <fL>
 */
void removeDirname(fileList *fL,  const char* dirname);


/* Prepends the given Dirname to all filenames in the filelist
   @fL fileList to add the dirname
   @dirname string which is prepended to all elements of <fL>
 */
void addDirname(fileList *fL,  const char* dirname);


/* Printing out the File List
   @fL fileList to print

   prints all elements of <fL> with timestamp, size, name and MD5Hash
*/
void printFileList(fileList *fL);


/* Delete all Files in a fileList
   @fL fileList to delete
   @prependdir string to prepend to all filenames to change relative filenames to absolutes

   @return -1 on error, 0 else
 */
int removeFileList(fileList* fL, const char* prependdir);


/* cldoc:end-category() */
#endif //FILELIST_H
