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
#include "database.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include "notification.h"
#include <openssl/md5.h>
#include "string.h"
#include <unistd.h>
#include "utils.h"
#include <vector>
#include <iterator>


using namespace std;

class API;
class Database;

class File
{
private:
    API &api;
    Database &database;
    Notification &notification;
    Utils &utils;

    string get_full_path();
    void map_header_vales(map<string, string> header_values);
    int validate_file(unsigned char *content, size_t size);
    int save_file(char *content);
    void open_file(); 
    int test_and_authenticate();

public:
    string allow;
    string content_location;
    string content_checksum;
    string content_type;
    string expires;
    string etag;

    string access_token;

    File(API &api, Database &database);
    ~File();
    int download(string url);
    int upload(string path);
    int check(string path);    
    int rename(string from, string to);
    int remove(string path);
};

#endif