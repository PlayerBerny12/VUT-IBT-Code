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

#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
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

class API
{

private:
    const string base_url;        
    
    CURL *curl;
    string x_api_key;

    static size_t header_parse(char *buffer, size_t size, size_t nitems, void *userdata); 
    static size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata); 

    //status: check test
    int ping();
    //status: in progress
    int auth_key_get(map<string, string> *header_values);
    //status: tbd
    int auth_key_post(string from, map<string, string> *header_values);
    //status: tbd
    int auth_key_delete(map<string, string> *header_values); 

public:    
    struct curl_slist *request_header;
    
    API(const string url);
    ~API();
    //status: tbd
    int file_get(string access_token, map<string, string> *header_values, char **content);
    //status: tbd
    int file_post(map<string, string> *header_values, char *content);
    //status: tbd
    int file_delete(map<string, string> *header_values);
};

#endif