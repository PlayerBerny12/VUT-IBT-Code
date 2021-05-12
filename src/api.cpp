// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 21.11.2020                                              ** //
// **   Module: API - code                                               ** //
// ************************************************************************ //

#include "api.h"

/**
 * Constructor for API.
 */
API::API(const string url, Database &database)
    : utils(Utils::get_instance()), database(database), base_url(url), request_header(nullptr)
{
    this->curl = curl_easy_init();
}

/**
 * Destructor for API.
 */
API::~API()
{
    curl_easy_cleanup(this->curl);
}

/**
 * Callback function for parsing response header.
 */
size_t API::header_parse_callback(char *buffer, size_t size, size_t nitems, void *userdata)
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

/**
 * Callback function for reading response body.
 */
size_t API::read_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    size_t real_size = size * nitems;
    response_content_t *response_content = (response_content_t *)userdata;

    // Prepare memory
    char *content = (char *)realloc(response_content->content, response_content->size + real_size + 1);

    if (content == nullptr)
    {
        return 0;
    }

    // Copy data from buffet
    response_content->content = content;
    memcpy(response_content->content + response_content->size, buffer, real_size);

    // Add size of buffer to actual size
    response_content->size += real_size;
    response_content->content[response_content->size] = '\0';

    return real_size;
}

/**
 * To test a connection (to do a ping).
 */
int API::ping()
{
    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "ping").c_str());

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);

    // Check returned response
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
    // Set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

#ifdef VDU_DEV
    // Set x-mock-response-code for return value selection
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 200");

    // Set x-api-key http request header value (postman using x-api-key for auth -> placeholder)
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());
#else
    // Set x-api-key http request header value
    this->request_header = curl_slist_append(this->request_header, ("x-api-key: " + this->x_api_key).c_str());
#endif

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);
    // get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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
    // Set http request header values
    this->request_header = curl_slist_append(this->request_header, ("from: " + from).c_str());
    this->request_header = curl_slist_append(this->request_header, "content-type: text/plain");

#ifdef VDU_DEV
    // Set x-mock-response-code for return value selection
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 201");
#endif

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Use password
    if (password.length() > 0)
    {
        curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, password.c_str());
    }
    // Use cert
    else
    {
        curl_easy_setopt(this->curl, CURLOPT_SSLCERT, utils.cert_file);
        curl_easy_setopt(this->curl, CURLOPT_KEYPASSWD, utils.pass_phrase);
        curl_easy_setopt(this->curl, CURLOPT_SSLKEY, utils.key_file);
        curl_easy_setopt(this->curl, CURLOPT_CAINFO, utils.ca_cert_file);
    }

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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
    // Set http request header values
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "auth/key").c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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

#ifdef VDU_DEV
    // Set x-mock-response-code for return value selection
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 200");

    // Set x-api-key http request header value (postman using x-api-key for auth -> placeholder)
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());
#else
    // Set x-api-key http request header value
    this->request_header = curl_slist_append(this->request_header, ("x-api-key: " + this->x_api_key).c_str());
#endif

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + access_token).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Set accept encoding for automatic decompression for all supporeted values by curl
    curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "");

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &response_content);
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, read_callback);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Assign content
    (*content) = response_content.content;
    (*size) = response_content.size;

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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
int API::file_post(string access_token, map<string, string> *header_values, const char *content)
{
    // Set http request header values
    this->request_header = curl_slist_append(this->request_header, "content-encoding: identity");
    this->request_header = curl_slist_append(this->request_header, "content-length: ");
    this->request_header = curl_slist_append(this->request_header, "content-location: ");
    this->request_header = curl_slist_append(this->request_header, "content-md5: ");
    // TODO
    this->request_header = curl_slist_append(this->request_header, "content-type: text/plain");

#ifdef VDU_DEV
    // Set x-mock-response-code for return value selection
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 201");

    // Set x-api-key http request header value (postman using x-api-key for auth -> placeholder)
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());
#else
    // Set x-api-key http request header value
    this->request_header = curl_slist_append(this->request_header, ("x-api-key: " + this->x_api_key).c_str());
#endif

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + access_token).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, content);

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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
#ifdef VDU_DEV
    // Set x-mock-response-code for return value selection
    this->request_header = curl_slist_append(this->request_header, "x-mock-response-code: 204");

    // Set x-api-key http request header value (postman using x-api-key for auth -> placeholder)
    this->request_header = curl_slist_append(this->request_header, ("x-api-key-test: " + this->x_api_key).c_str());
#else
    // Set x-api-key http request header value
    this->request_header = curl_slist_append(this->request_header, ("x-api-key: " + this->x_api_key).c_str());
#endif

    // Set request parameters
    curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(this->curl, CURLOPT_URL, (this->base_url + "file/" + this->x_api_key).c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->request_header);

    // Set callbacks with arguments
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, header_values);
    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, header_parse_callback);

#ifdef VDU_HTTP_3
    // Enabling HTTP 3
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_3);
#endif

    // Perform request
    CURLcode res = curl_easy_perform(this->curl);

    // Get response status code
    long res_code = 0;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &res_code);

    // Cleanup
    curl_easy_reset(this->curl);
    curl_slist_free_all(this->request_header);
    this->request_header = nullptr;

    // Check returned response
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
 * Authenticate user (load token from keyring or make HTTP POST call /api/key).
 */
int API::authenticate()
{
    Database::token_info_t token_info = {string(), 0};
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
            utils.load_secret(this->x_api_key, "token");
        }
        else
        {
            // Load token from keyring
            utils.load_secret(this->x_api_key, "token");
        }
    }

    // Remote authentification needed
    if (authentificateType == 1)
    {
        string username = utils.username;
        string password;

        if (username.length() == 0)
        {
            // Get system username
            username = getlogin();
        }

        if (utils.cert_file.length() == 0 && utils.ca_cert_file.length() == 0 && utils.key_file.length() == 0)
        {
            // Get password
            utils.load_secret(password, "password");

            if (password.length() == 0)
            {
                utils.run_zenity_password(password);
            }
        }

        // Authenticate
        map<string, string> header_values;
        if (this->auth_key_post(username, password, &header_values) != 201)
        {
            return 1;
        }

        // Update data
        this->expires = header_values["expires"];
        this->x_api_key = header_values["x-api-key"];
        this->usage = 5;

        // Save password and token to keyring
        if (password.length() > 0)
        {
            utils.save_secret(password, "password");
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
 * Renew token.
 */
int API::renew_token()
{
    // Reauthenticate
    map<string, string> header_values;
    if (this->auth_key_get(&header_values) != 200)
    {
        return 1;
    }

    // Update data
    this->expires = header_values["expires"];
    this->x_api_key = header_values["x-api-key"];
    this->usage = 5;

    // Save token info
    return this->save_token_info();
}

/**
 * Save token to keyring and also metadata to database.
 */
int API::save_token_info()
{
    // Save token to keyring
    if (utils.save_secret(this->x_api_key, "token"))
    {
        return 1;
    }

    // Save metadata to database
    Database::token_info_t token_info = {this->expires, this->usage};
    return this->database.save_token_info(token_info);
}