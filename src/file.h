// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 22.03.2021                                              ** //
// **   Module: File - header                                            ** //
// ************************************************************************ //

#ifndef VDU_FILE_H
#define VDU_FILE_H

#include "api.h"
#include "config.h"
#include "database.h"
#include <fstream>
#include <iostream>
#include "notification.h"
#include "string.h"

using namespace std;

class Database;

class File
{
private:
    API& api;    
    Database& database;
    Notification& notification;

    string get_full_path();
    void map_header_vales(map<string, string> header_values);
    int save_file(char *content);
    void open_file();

public:    
    string allow;
    string content_encoding;
    string content_length;
    string content_location;
    string content_checksum;
    string content_type;
    string expires;
    string date;
    string last_modified;
    string etag;

    string access_token;

    File(API& api, Database& database, Notification& notification);
    ~File();  
    int process_fake_file(const string path);
    int download();
};

#endif