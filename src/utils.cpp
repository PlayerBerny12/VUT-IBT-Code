// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Config - code                                            ** //
// ************************************************************************ //

#include "utils.h"

void Utils::run_xdg_open(string &filename)
{
    auto f = popen(("xdg-open '" + filename + "'").c_str(), "r");
    pclose(f);
}

void Utils::run_zenity_password(string &password)
{
    char input_raw[256];
    auto f = popen("zenity --password --title 'VDU login'", "r");

    if (f == nullptr)
    {
        return;
    }

    fgets(input_raw, 256, f);
    if (pclose(f))
    {
        return;
    }

    password = string(input_raw);    
}

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
                else if (key == "CACert")
                {
                    ca_cert_file = value;
                }
                else if (key == "KeyName")
                {
                    key_name = value;
                }
                else if (key == "Passphrase")
                {
                    pass_phrase = value;
                }
            }
        }
    }

    // Close stream
    config_file.close();
}

int Utils::save_secret(string &secret, const char *name)
{
    GError *error = nullptr;

    if(clear_secret(name)) {
        return 1;
    }

    secret_password_store_sync(&secret_schema, SECRET_COLLECTION_DEFAULT,
                               name, secret.c_str(), nullptr, &error,
                               "name", name,
                               "secret_val", 42,
                               nullptr);

    if (error != nullptr)
    {
        // TODO: Error
        g_error_free(error);
        return 1;
    }

    return 0;
}

int Utils::load_secret(string &secret, const char *name)
{
    GError *error = nullptr;

    gchar *secret_raw = secret_password_lookup_sync(&secret_schema, nullptr, &error,
                                                    "name", name,
                                                    "secret_val", 42,
                                                    nullptr);

    if (error != nullptr)
    {
        // TODO: Error
        g_error_free(error);
        return 1;
    }

    if (secret_raw != nullptr)
    {
        secret = secret_raw;
        secret_password_free(secret_raw);
    }
    return 0;
}

int Utils::clear_secret(const char *name)
{
    GError *error = NULL;

    gboolean removed = secret_password_clear_sync(&secret_schema, nullptr, &error,
                                                  "name", name,
                                                  "secret_val", 42,
                                                  nullptr);

    if (error != NULL)
    {
        // TODO: Error
        g_error_free(error);
        return 1;
    }

    if (!removed)
    {
        return 1;
    }

    return 0;
}

