#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>     // for command line options parsing

#include "fileList.h"
#include "networkFunctions.h"

fileList ownFiles, filesToDelete, filesToTransfer, remoteFiles;

void printUsage();

#endif //SERVER_H
