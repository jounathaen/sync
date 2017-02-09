#<cldoc:index>

Project Documentation

REMOTE FILE SYNC
====================


PROJECT DESCRIPTION:
------------------------
A Client for Syncing 2 Directories:

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

### Cases: ###
* File has same signature on both PC
  -> Do Nothing 
* File has different signature on PCs
  -> Copy the newer one to the other PC1 
* File is not existing on one PC
  -> depends on command line Parameter, default: ask for each file (yes, no, yes to all, yes to current directory)


### BONUS GOALS: ###
- Command Line Parser
- IPv4 and IPv6 Support


