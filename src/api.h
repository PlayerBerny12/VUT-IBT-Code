// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 21.11.2020                                              ** //
// **   Module: API - header                                             ** //
// ************************************************************************ //

#ifndef VDU_API_H
#define VDU_API_H

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include "database.h"
#include <iostream>
#include <map>
#include <sstream>
#include "string.h"

using namespace std;

typedef struct response_content
{
    char *content;
    size_t size;
} response_content_t;

class Database;

class API
{
private:
    Database &database;
    CURL *curl;

    const string base_url;
    string x_api_key;
    string expires;
    int usage;

    static size_t header_parse(char *buffer, size_t size, size_t nitems, void *userdata);
    static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata);

    int auth_key_get(map<string, string> *header_values);
    int auth_key_post(string &from, string &password, map<string, string> *header_values);
    int auth_key_delete(map<string, string> *header_values);
    int renew_token();

public:
    struct curl_slist *request_header;

    API(const string url, Database &database);
    ~API();
    int ping();
    int file_get(string access_token, map<string, string> *header_values, size_t *size, char **content);
    int file_post(map<string, string> *header_values, char *content);
    int file_delete(map<string, string> *header_values);
    int authenticate();
    int save_token_info();
};

#endif