// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 21.11.2020                                              ** //
// **   Module: API - code                                               ** //
// ************************************************************************ //

#include "api.h"

API::API(const string url, Database &database) : database(database), base_url(url), request_header(nullptr)
{
    this->curl = curl_easy_init();
}

API::~API()
{
    curl_easy_cleanup(this->curl);
}

size_t API::header_parse(char *buffer, size_t size, size_t nitems, void *userdata)
{
    map<string, string> *header_values = (map<string, string> *)userdata;
    string header_line;
    string header_key;
    string header_value;
    istringstream stream(buffer);

    // Load one line from header
    getline(stream, header_line);

    size_t index = header_line.find(':', 0);

    if (index != std::string::npos)
    {
        // Create substrings
        header_key = header_line.substr(0, index);
        header_value = header_line.substr(index + 1);

        // Trim values
        trim(header_key);
        trim(header_value);

        // Insert trimed and lowered values to map
        header_values->insert({header_key, header_value});
    }

    return nitems * size;
}

size_t API::read_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    size_t real_size = size * nitems;
    response_content_t *response_content = (response_content_t *)userdata;

    char *content = (char *)realloc(response_content->content, response_content->size + real_size + 1);

    if (content == nullptr)
    {
        return 0;
    }
    response_content->content = content;
    memcpy(response_content->content + response_content->size, buffer, real_size);
    response_content->size += real_size;
    response_content->content[response_content->size] = '\0';

    return real_size;
}

/**
 * To test a connection (to do a ping).
 */
int API::ping()
{
    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "ping").c_str());

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        return res_code;
    }
}

/**
 * Authorization token (key) renewal.
 */
int API::auth_key_get(map<string, string> *header_values)
{
    // set http request header values
    cout << ("x-api-key-test: " + this->x_api_key).c_str() << endl;
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

    // !!! x-mock-response-code is set for development purposes !!! -> should be deleted in PROD
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 200");

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        return res_code;
    }
}

/**
 * Generate a new authorization token (key) for the first time.
 */
int API::auth_key_post(string &from, string &password, map<string, string> *header_values)
{
    // set http request header values
    this->request_header = curl_slist_append(this->request_header, ("from: " + from).c_str());
    this->request_header = curl_slist_append(this->request_header, "content-type: text/plain");

    // !!! x-mock-response-code is set for development purposes !!! -> should be deleted in PROD
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 201");

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Use password
    if (password.length() > 0)
    {
        curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, password.c_str());
    }
    // Use cert
    else
    {
        curl_easy_setopt(this->curl, CURLOPT_SSLCERT, Utils::cert_file);
        curl_easy_setopt(this->curl, CURLOPT_KEYPASSWD, Utils::pass_phrase);
        curl_easy_setopt(this->curl, CURLOPT_SSLKEY, Utils::key_name);
        curl_easy_setopt(this->curl, CURLOPT_CAINFO, Utils::ca_cert_file);
    }

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        return res_code;
    }
}

/**
 * Authorization token (key) invalidation.
 */
int API::auth_key_delete(map<string, string> *header_values)
{
    // set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        return res_code;
    }
}

/**
 * Get the content of a file available by the given access token.
 */
int API::file_get(string access_token, map<string, string> *header_values, size_t *size, char **content)
{
    response_content_t response_content = {nullptr, 0};

    // set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

    // !!! x-mock-response-code is set for development purposes !!! -> should be deleted in PROD
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 200");

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + access_token).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // set accept encoding for automatic decompression for all supporeted values by curl
    curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "");

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &response_content);
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, read_callback);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // assign content
    (*content) = response_content.content;
    (*size) = response_content.size;

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        this->usage--;
        return res_code;
    }
}

/**
 * Post/upload the content of a file available by the given access token.
 */
int API::file_post(map<string, string> *header_values, char *content)
{
    // set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());
    this->request_header = curl_slist_append(this->request_header, "content-encoding: identity");
    this->request_header = curl_slist_append(this->request_header, "content-length: ");
    this->request_header = curl_slist_append(this->request_header, "content-location: ");
    this->request_header = curl_slist_append(this->request_header, "content-md5: ");
    this->request_header = curl_slist_append(this->request_header, "content-type: ");

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + this->x_api_key).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        this->usage--;
        return res_code;
    }
}

/**
 * To invalid the given file access token.
 */
int API::file_delete(map<string, string> *header_values)
{
    // set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

    // set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + this->x_api_key).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse);

    // perform request
    CURLcode res = curl_easy_perform(this->curl);

    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // check returned response
    if (res != CURLE_OK)
    {
        return -1;
    }
    else
    {
        this->usage--;
        return res_code;
    }
}

/**
 * Authenticate user (load token from keyring or make HTTP call)
 */
int API::authenticate()
{    
    token_info_t token_info = {string(), 0};    
    int authentificateType = 0;

    // Load token metadata form database
    if (this->database.load_token_info(token_info))
    {
        return 1;
    }

    // Token is not found
    if (token_info.expires.length() == 0)
    {
        authentificateType = 1;
    }
    // Token is found
    else
    {
        this->expires = token_info.expires;
        this->usage = token_info.usage;         
        time_t expires = curl_getdate(token_info.expires.c_str(), nullptr);

        if (difftime(expires, chrono::system_clock::to_time_t(chrono::system_clock::now())) < 0)
        {
            // Expired
            authentificateType = 1;
        }
        else if (this->usage == 0)
        {
            // Renew token
            authentificateType = 2;

            // Load token from keyring
            Utils::load_secret(this->x_api_key, "token");
        }
        else
        {
            // Load token from keyring
            Utils::load_secret(this->x_api_key, "token");
        }
    }

    // Remote authentification needed
    if (authentificateType == 1)
    {
        string username;
        string password;
        
        if (Utils::cert_file.length() == 0 && Utils::ca_cert_file.length() == 0 && Utils::key_name.length() == 0)
        {
            // Get password
            Utils::load_secret(password, "password");

            if (password.length() == 0)
            {
                Utils::run_zenity_password(password);
            }
        }

        // Get username
        username = string(getenv("USERNAME"));

        // Authenticate
        map<string, string> header_values;
        if (this->auth_key_post(username, password, &header_values) != 201)
        {
            return 1;
        }

        this->expires = header_values["expires"];
        this->x_api_key = header_values["x-api-key"];
        this->usage = 5;

        // Save password and token to keyring
        if (password.length() > 0)
        {
            Utils::save_secret(password, "password");
        }

        return this->save_token_info();
    }
    else if (authentificateType == 2)
    {
        this->renew_token();
    }

    return 0;
}

/**
 * Renew token
 */
int API::renew_token()
{
    // Reauthenticate
    map<string, string> header_values;
    if (this->auth_key_get(&header_values) != 200)
    {
        return 1;
    }

    this->expires = header_values["expires"];
    this->x_api_key = header_values["x-api-key"];
    this->usage = 5;

    return this->save_token_info();
}

/**
 * Save token to keyring and also metadata to database.
 */
int API::save_token_info()
{
    if (Utils::save_secret(this->x_api_key, "token"))
    {
        return 1;
    }

    token_info_t token_info = {this->expires, this->usage};
    return this->database.save_token_info(token_info);
}