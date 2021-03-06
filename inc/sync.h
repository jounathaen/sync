#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>     // for command line options parsing

#include "fileList.h"
#include "networkFunctions.h"

/* cldoc:begin-category(SyncingClient) */
fileList ownFiles, filesToDelete, filesToTransfer, remoteFiles, remoteFilesToDelete, remoteFilesToTransfer;

//command line parameters
enum missingHandling missingOptions = mergeEverything;

char *hostdirectory;
char *remotedirectory;
char *remoteip;
char *port = NULL;

/* prints a small message about the use of the program
*/
void printUsage();

/* cldoc:end-category() */
#endif /* SYNC_H */
