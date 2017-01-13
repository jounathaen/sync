#ifndef SYNC_H
#define SYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>     // for command line options parsing

#include "fileList.h"

fileList ownFiles, filesToDelete, filesToTransfer, fileList2;

// Options to handle missing Items
enum missingHandling{
  deleteOnHost,
  deleteOnRemote,
  mergeEverything,
  ask
};

//command line parameters
enum missingHandling missingOptions = ask;
int recoursivelySyncing = 1; // TODO

/*
  prints the use of the program
*/
void printUsage();

#endif /* SYNC_H */
