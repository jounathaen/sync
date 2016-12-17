
REMOTE FILE SYNC
======================
This is a University Project, no OpenSource Licenze. But here should not be any relevant new code for this world anyway.
In the End, this would be a kind of simple rsync clone


PROJECT DESCRIPTION:
------------------------
A Client for Syncing 2 Directories:

### Usage: ###
    sync /path/to/directory1 111.222.123.123:/path/to/directory2 [-OPTION: deleted file handeling]

### Procedure: ###
    Inform REMOTE of sync
    Generating hash list of files on HOST (recoursively, content only, md5 hash)
    Generating hash list of files on REMOTE (recoursively, content only, md5 hash)
    Transmitting list from REMOTE to HOST (name, path, modified date and hash)
    Comparing the lists on HOST (Sort the lists!)
        - For each name in List 1 search Name in List 2
        - Found name in Both: Compare Checksums
          - Checksums are the same: Discard entry (no sync needed)
          - Checksums differ: Sync newer one
        - Not found Name in List 1
          - Write on lists: Transmit from 1 to 2 OR delete on 2 (depending on Setting)
        - Same for List 2 / Delete files from List 2 and use remaining files
          - Write on List: Transmit from 2 to 1 OR delete on 1 (depending on Setting)
        Afterwards: 4 lists: (maybe interactive see command line param)
                  1. Files from HOST to REMOTE
                  2. Files from REMOTE to HOST
                  3. Files to delete on HOST
                  4. Files to delete on REMOTE
    transmit list 2 & 4 to REMOTE
    delete files & directories deletion list
    transmit files from HOST to REMOTE      ----\_____simultaneous
    transmit files from REMOTE to HOST      ----/
      - Manually correct time stamps
    Done :-)

### Features: ###
simultaneous send & recieve
lock file in dir to prevent two clients from syncing simultaneous (prevent race conditions)

### Command line Parameters: ###
    -dH   --deleteOnHOST
      don't ask, delete all files on HOST, when these files are not present on REMOTE
    -dR   --deleteOnREMOTE
      don't ask, delete all files on REMOTE, when these files are not present on HOST
    -mE   --mergeEverything
      don't delete anything, files that are not present on HOST will get copied from REMOTE and vice versa
    -A    --ask
      ask for every file, to remove on HOST/REMOTE or keep both (DEFAULT OPTION)
    -r    --recoursively
      sync directories inside directories

### Cases: ###
* File has same signature on both PC
  -> Do Nothing 
* File has different signature on PCs
  -> Copy the newer one to the other PC1 
* File is not existing on one PC
  -> depends on command line Parameter, default: ask for each file (yes, no, yes to all, yes to current directory)


### BONUS GOALS: ###
- Command Line Parser
- Logging Support (e.g. syslog)
- IPv4 and IPv6 Support


