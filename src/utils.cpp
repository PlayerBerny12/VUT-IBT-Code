// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Config - code                                            ** //
// ************************************************************************ //

#include "utils.h"

/**
 * Constructor for Utils
 */
Utils::Utils() 
    : notification(Notification::get_instance())
{
    // Get user home dir
    char *home_dir;
    if ((home_dir = getenv("HOME")) == NULL) {
        home_dir = getpwuid(getuid())->pw_dir;
    }

    // Set path to database   
    this->db_path = home_dir;
    this->db_path += "/.local/share/vdu/vdu.db";        
    
    // Set config path
    this->default_config_path = home_dir;
    this->default_config_path += "/.vdu.conf";
        
    this->creat_config_if_not_exists();    
    this->load_config();    
}

/**
 * Creating config file in user home folder if not exists
 */
void Utils::creat_config_if_not_exists()
{    
    // Open file in append mode
    fstream config_file;
    config_file.open(default_config_path, ios_base::app);
    
    // Check if it is empty
    if (!config_file.tellg())
    {
        string config_content = "# ---------------------------------------------------------------\n\
# System username is default username used in authentication.\n\
#\n\
# If username is set in this configuration it is used insed of\n\
# default one.\n\
# ---------------------------------------------------------------\n\
\n\
Username = username\n\
\n\
# ---------------------------------------------------------------\n\
# Client and CA certificat has to be in PEM format.\n\
#\n\
# If ClientCertFile, CACert and KeyName is set\n\
# application starts using these certificate.\n\
#\n\
# Please set only names of certificate because certificates are\n\
# loading from system cerfiticate store.\n\
# --------------------------------------------------------------\n\
\n\
#ClientCertFile = client.pem\n\
#CACertFile = ca.pem\n\
#KeyFile = client.key\n\
#Passphrase = SuperSecretPassPhraseXX12@\n";

        // Write to file
        config_file << config_content;
    }
}

/**
 * Loading configuration atributes
 */
void Utils::load_config()
{
    // Prepare stream and variables
    string line;
    ifstream config_file(default_config_path);
    getline(config_file, line);

    // Read
    while (getline(config_file, line))
    {
        trim(line);

        // Skip commented and empty line
        if (line.length() == 0 || line.at(0) == '#')
        {
            continue;
        }
        else
        {
            istringstream stream(line);
            string key;

            // Split by equal sign
            if (getline(stream, key, '='))
            {
                std::string value;
                getline(stream, value);

                trim(key);
                trim(value);

                if (key == "ClientCertFile")
                {
                    cert_file = value;
                }
                else if (key == "CACertFile")
                {
                    ca_cert_file = value;
                }
                else if (key == "KeyFile")
                {
                    key_file = value;
                }
                else if (key == "Passphrase")
                {
                    pass_phrase = value;
                }
                else if (key == "Username")
                {
                    username = value;
                }
            }
        }
    }

    // Close stream
    config_file.close();
}

/**
 * Get instance of singleton
 */
Utils &Utils::get_instance()
{
    static Utils instance;
    return instance;
}

/**
 * Open file in assigned application via xdg-open
 */
void Utils::run_xdg_open(string &filename)
{
    // Run xdg-open command
    auto f = popen(("xdg-open '" + filename + "'").c_str(), "r");
    if (f == nullptr)
    {
        return;
    }

    pclose(f);
}

/**
 * Open Zenity password dialog to get password form user
 */
void Utils::run_zenity_password(string &password)
{
    // Run zenity command
    char input_raw[256];
    auto f = popen("zenity --password --title 'VDU login'", "r");

    if (f == nullptr)
    {
        return;
    }

    // Read output of command
    fgets(input_raw, 256, f);
    if (pclose(f))
    {
        return;
    }

    // Set password
    password = string(input_raw);
    trim(password);
}

/**
 * Save secret to keyring
 */
int Utils::save_secret(string &secret, const char *name)
{
    GError *error = nullptr;

    // Reamove secret befor saving new one
    clear_secret(name);

    // Save secret
    secret_password_store_sync(
        &secret_schema, SECRET_COLLECTION_DEFAULT, name,
        secret.c_str(), nullptr, &error, "name", name,
        "secret_val", 42, nullptr);

    // Error occurred
    if (error != nullptr)
    {        
        g_error_free(error);
        this->notification.notify(13, true);
        return 1;
    }

    return 0;
}

/**
 * Load secret from keyring
 */
int Utils::load_secret(string &secret, const char *name)
{
    GError *error = nullptr;

    // Search for secret 
    gchar *secret_raw = secret_password_lookup_sync(
        &secret_schema, nullptr, &error, "name", name,
        "secret_val", 42, nullptr);

    // Error occurred
    if (error != nullptr)
    {        
        g_error_free(error);
        this->notification.notify(13, true);
        return 1;
    }

    // Secret found
    if (secret_raw != nullptr)
    {
        secret = secret_raw;
        secret_password_free(secret_raw);
    }
    return 0;
}

/**
 * Remove secret form keyring
 */
int Utils::clear_secret(const char *name)
{
    GError *error = NULL;

    // Remove secret
    gboolean removed = secret_password_clear_sync(
        &secret_schema, nullptr, &error, "name", name,
        "secret_val", 42, nullptr);

    // Error occurred
    if (error != NULL)
    {        
        g_error_free(error);
        this->notification.notify(13, true);
        return 1;
    }

    return removed;
}
