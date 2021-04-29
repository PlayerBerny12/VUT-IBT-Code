// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Database - header                                        ** //
// ************************************************************************ //

#ifndef VDU_DATABASE_H
#define VDU_DATABASE_H

#include "file.h"
#include "notification.h"
#include <sqlite3.h>

using namespace std;

class File;

class Database
{
private:
    Notification& notification;

public:    
    Database(Notification& notification);
    ~Database();
    int save_file_info(File& file);
    int load_file_info(File& file);
};

#endif