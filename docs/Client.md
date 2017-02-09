#<cldoc:SyncingClient>

Client Documentation

Client Program
==================

The Client connects to the Remote to sync files. 
The main program logic is implemented here.

Usage:
-------
`./sync [-p PORTNUM] [-h] [-d DELETE_OPTION] /path/to/localdir REMOTEIP/path/to/remotedir`

Procedure:
-----------

1. Sends Sync request to <Server>
1. Sends path of remote directoy to <Server>
1. Recieve file List from <Server> with the content of remote directoy
1. Create the fileLists filesToDelete, filesToTransfer, remoteFilesToDelete and remoteFilesToTransfer
1. Transfer remoteFilesToDelete and remoteFilesToTransfer to <Server>
1. Send files of filesToTransfer to <Server>
1. Delete files of filesToDelete
1. Recieve files of remoteFilesToTransfer from <Server>

Done!

Command Line Parameters:
----------------------------

  `-d   --delete [OPTION]`

  OPTION is:  
  `h, host`:   
    don't ask, delete all files on HOST, when these files are not present on REMOTE  
  `r, remote`:  
    don't ask, delete all files on REMOTE, when these files are not present on HOST  
  `m, merge`:  
    don't delete anything, files that are not present on HOST will get copied from REMOTE and vice versa  
  `a, ask`:  
    ask for every file, to remove on HOST/REMOTE or keep both (DEFAULT OPTION)  
    
  `-h   --help`  
  prints a small usage text  
  
  `-p --port [PORTNUM]`  
    specifies the portnumber, if not set 1234 is used
