#<cldoc:Server>

Server Documentation

SERVER PROGRAM
===================

The Server Programm runs on the REMOTE and answers the requests of a client.

Usage:
-------
`./server [-p PORTNUM] [-h]`

Procedure:
-----------

1. Waits for Sync request (recieve string "START_SYNC")
1. Recieves Directory name from <SyncingClient>
1. Creates <fileList> from the recieved directory name
1. Sends this <fileList>
1. Recieves <filesToDelete> and <filesToTransfer> from <SyncingClient>
1. Deletes Files which are in <filesToDelete>
1. Recieves files from <SyncingClient> 
1. Sends Files which are in <filesToTransfer>
1. Shutdown connection and start again from 1.)


Command Line Parameters:
------------------------------
  `-h   --help`  
  prints a small usage text  
  
  `-p --port [PORTNUM]`  
    specifies the portnumber, if not set 1234 is used
