#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>     // for command line options parsing

#include "fileList.h"
#include "networkFunctions.h"

/* cldoc:begin-category(Server) */

fileList ownFiles, filesToDelete, filesToTransfer, remoteFiles;
char *port = NULL;

/* The server main loop */
int main (int argc, char** argv);

/* prints the a small message about the use of the program
*/
void printUsage();

/* cldoc:end-category() */
#endif //SERVER_H
