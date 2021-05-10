// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Config - header                                          ** //
// ************************************************************************ //

#ifndef VDU_UTILS_H
#define VDU_UTILS_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <libsecret/secret.h>
#include "notification.h"
#include <pwd.h>
#include "string.h"
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

/**
 * Class containing support functions and atributes load from configuration
 */
class Utils
{
private: 
    Notification &notification;

    const SecretSchema secret_schema = {
		"org.freedesktop.secret", SECRET_SCHEMA_NONE,
		{
			{"name", SECRET_SCHEMA_ATTRIBUTE_STRING},
            {"secret_val", SECRET_SCHEMA_ATTRIBUTE_INTEGER},
            {"NULL", (SecretSchemaAttributeType)0 }, 
		},
        
        //Privates values initialization
        0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr           
	};

    Utils();
    void creat_config_if_not_exists();
    void load_config();
public:
    static Utils& get_instance();
    
    Utils(Utils const &) = delete;
    void operator=(Utils const &) = delete;

    string files_path = "/mnt/vdu/";

    string db_path;
    string default_config_path;

    string username;
    string cert_file;
    string ca_cert_file;
    string key_file;
    string pass_phrase;

    void run_xdg_open(string &filename);
    void run_zenity_password(string &password);    
    int save_secret(string& secret, const char *name);
    int load_secret(string& secret, const char *name);
    int clear_secret(const char *name);
};

#endif