
REMOTE FILE SYNC
======================
This is a University Project, no OpenSource License as I don't know which sites I've taken code snippets from. But here should not be any relevant new code for this world anyway.
In the End, this would be a kind of simple rsync clone


PROJECT DESCRIPTION:
------------------------
A Client for Syncing 2 Directories:

### Usage: ###
`sync /path/to/directory1 111.222.123.123/path/to/directory2 [-OPTION: deleted file handeling]`

### Detailed Procedure: ###
- Inform REMOTE of sync
- Generating hash list of files on HOST (recursively, content only, md5 hash)
- Generating hash list of files on REMOTE (recursively, content only, md5 hash)
- Transmitting list from REMOTE to HOST (name, path, modified date and hash)
- Comparing the lists on HOST 
  - For each name in List 1 search Name in List 2
  - Found name in both lists: Compare checksums
    - Checksums are the same: Go to next entry (no sync needed)
    - Checksums differ: Sync newer one
  - Not found Name in List 1
    - Write on lists: Transmit from HOST to REMOTE OR delete on HOST (depending on Setting)
  - Same for List 2 
    - Write on List: Transmit from REMOTE to HOST OR delete on REMOTE (depending on Setting)
  - Afterwards: 4 lists: (maybe interactive see command line param)
            1. Files to transfer from HOST to REMOTE
            2. Files to transfer from REMOTE to HOST
            3. Files to delete on HOST
            4. Files to delete on REMOTE
- transmit list 2 & 4 to REMOTE
- delete files & directories deletion list
- transmit files from HOST to REMOTE
- transmit files from REMOTE to HOST
- Manually correct time stamps
Done :-)

### Command line Parameters: ###
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

### Possible File Collisions: ###
* File has same signature on both PC
  -> Do Nothing 
* File has different signature on PCs
  -> Copy the newer one to the other PC1 
* File is not existing on one PC
  -> depends on command line Parameter, default: ask for each file (yes, no, yes to all, yes to current directory)


### BONUS GOALS: ###
- Command Line Parser
- IPv4 and IPv6 Support
- html documentation with cldoc (https://github.com/jessevdk/cldoc)
- version contoll with git on github (https://github.com/jounathaen/sync)
