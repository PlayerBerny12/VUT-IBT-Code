// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 01.04.2021                                              ** //
// **   Module: Notification - header                                    ** //
// ************************************************************************ //

#ifndef VDU_NOTIFICATION_H
#define VDU_NOTIFICATION_H

#include <iostream>
#include <libnotify/notify.h>
#include <unistd.h>
#include <vector>

using namespace std;

class Notification
{
private:
    NotifyNotification *notification;
    vector<pair<string, string>> messages{{"Opening file...", "dialog-information"},
                                          {"Error: VDU server not responding.", "dialog-error"},
                                          {"Error: Authentication failed.", "dialog-error"},
                                          {"Error: Invalid number of arguments.", "dialog-error"},
                                          {"Error: Invalid argument identifying process type.", "dialog-error"},
                                          {"Error: Selected file is not in the correct format or is corrupt.", "dialog-error"},
                                          {"Error: Cannot save metadata of file to database.", "dialog-error"},
                                          {"Error: Cannot retrieve metadata of file from database.", "dialog-error"},
                                          {"Error: Cannot remove metadata of file from database.", "dialog-error"},
                                          {"Error: Cannot save metadata to database.", "dialog-error"},
                                          {"Error: Cannot retrieve metadata from database.", "dialog-error"},
                                          {"Error: Cannot remove metadata from database.", "dialog-error"},
                                          {"Error: Received file is corrupted.", "dialog-error"},
                                          {"Error: Working with keyring failed.", "dialog-error"}};

    Notification();
public:
    static Notification& get_instance();
    
    Notification(Notification const &) = delete;
    void operator=(Notification const &) = delete;
    
    void cleanup();
    void notify(int message_code, bool error);
};
#endif