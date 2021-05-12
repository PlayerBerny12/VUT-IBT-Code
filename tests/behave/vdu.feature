Feature: Testing VDU app system 
    
    Testing basic user interaction with vdu-app and vdu-app-fuse.
    
    Scenario: Download file form VDU
        Given open URL "vdu://abc123456789"
        Then file is downloaded to FUSE file system
        And metadata are stored in database
    
    Scenario: Edit downloaded file in FUSE
        Given append text to downloaded file
        Then content is change in filesystem
    
    Scenario: Rename downloaded file in FUSE
        Given rename downloaded file
        Then name of file is change in filesystem
        And metadata are updated in database

    Scenario: Delete downloaded file in FUSE
        Given delete downloaded file
        Then file is no longer in filesystem
        And metadata are deleted in database