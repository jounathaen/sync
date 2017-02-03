#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>     // for command line options parsing

#include "fileList.h"
#include "networkFunctions.h"

fileList ownFiles, filesToDelete, filesToTransfer, remoteFiles, remoteFilesToDelete, remoteFilesToTransfer;

//command line parameters
enum missingHandling missingOptions = mergeEverything;

char *hostdirectory;
char *remotedirectory;
char *remoteip;

/*
  prints the use of the program
*/
void printUsage();

#endif /* SYNC_H */
