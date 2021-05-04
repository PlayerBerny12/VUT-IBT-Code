// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Config - header                                          ** //
// ************************************************************************ //

#include <fstream>
#include <iostream>
#include <libsecret/secret.h>
#include "string.h"
#include <sstream>
#include <unistd.h>

using namespace std;

class Utils
{
private:
    inline static const string app_path = "/var/lib/vdu/";
    inline static const string configs_path = "/etc/vdu/";

    inline static const SecretSchema secret_schema = {
		"org.freedesktop.secret", SECRET_SCHEMA_NONE,
		{
			{"name", SECRET_SCHEMA_ATTRIBUTE_STRING},
            {"secret_val", SECRET_SCHEMA_ATTRIBUTE_INTEGER},
            {"NULL", (SecretSchemaAttributeType)0 }, 
		},
        
        //Privates values initialization
        0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr           
	};

public:
    inline static const string files_path = "/mnt/vdu/";

    inline static const string db_path = app_path + "vdu.db";
    inline static const string default_config_path = configs_path + "vdu.conf";

    inline static string username;
    inline static string cert_file;
    inline static string ca_cert_file;
    inline static string key_name;
    inline static string pass_phrase;

    static void run_xdg_open(string &filename);
    static void run_zenity_password(string &password);    
    static void load_config();
    static int save_secret(string& secret, const char *name);
    static int load_secret(string& secret, const char *name);
    static int clear_secret(const char *name);
};