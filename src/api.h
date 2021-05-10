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

#include "gtest/gtest.h"
#include <chrono>
#include <curl/curl.h>
#include "database.h"
#include <map>
#include "utils.h"

using namespace std;

class Database;

/**
 * Class for comunicating with VDU via HTTP (curl)
 */
class API
{
private:
    typedef struct response_content
    {
        char *content;
        size_t size;
    } response_content_t;

    Utils &utils;
    Database &database;
    CURL *curl;

    const string base_url;
    string x_api_key;
    string expires;
    int usage;

    static size_t header_parse_callback(char *buffer, size_t size, size_t nitems, void *userdata);
    static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata);

    int auth_key_get(map<string, string> *header_values);
    int auth_key_post(string &from, string &password, map<string, string> *header_values);
    int auth_key_delete(map<string, string> *header_values);
    int renew_token();

    FRIEND_TEST(APITest, auth_key_get_200);
    FRIEND_TEST(APITest, auth_key_get_401);
    FRIEND_TEST(APITest, auth_key_post_201);
    FRIEND_TEST(APITest, auth_key_post_401);
    FRIEND_TEST(APITest, auth_key_delete_204);
    FRIEND_TEST(APITest, auth_key_delete_401);    
public:
    struct curl_slist *request_header;

    API(const string url, Database &database);
    ~API();
    int ping();
    int file_get(string access_token, map<string, string> *header_values, size_t *size, char **content);
    int file_post(string access_token, map<string, string> *header_values, const char *content);
    int file_delete(map<string, string> *header_values);
    int authenticate();
    int save_token_info();
};

#endif