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
#include "utils.h"

using namespace std;

class File;

/**
 * Class for working with database.
 */
class Database
{
private:
    Notification& notification;
    Utils &utils;
    sqlite3 *database;

public:    
    typedef struct token_info {
        string expires;
        int usage;
    } token_info_t;

    Database();
    ~Database();
    int save_file_info(File& file);
    int load_file_info(File& file);
    int remove_file_info(File& file);
    int save_token_info(token_info& token_info);
    int load_token_info(token_info& token_info);
    int remove_token_info();
};

#endif