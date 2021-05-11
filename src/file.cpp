// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 22.03.2021                                              ** //
// **   Module: File - code                                              ** //
// ************************************************************************ //

#include "file.h"

/**
 * Constructor for Notification.
 */
File::File(API &api, Database &database)
    : api(api), database(database), notification(Notification::get_instance()), utils(Utils::get_instance())
{
}

/**
 * Destructor for Notification.
 */
File::~File()
{
}

/**
 * Get absolute path to mount point.
 */
string File::get_full_path()
{
    return this->utils.files_path + this->content_location;
}

/**
 * Calculate MD5 checksum of given content and compre it with
 * recieved MD5 checksum.
 */
int File::validate_file(unsigned char *content, size_t size)
{
    ostringstream md5_sum_stream;
    unsigned char md5_sum[MD5_DIGEST_LENGTH];

    // Calculate MD5 sum
    MD5(content, size, md5_sum);

    // Convert to string stream
    md5_sum_stream << hex << setfill('0');
    for (int64_t c : md5_sum)
    {
        md5_sum_stream << setw(2) << c;
    }

    // Compare checksums
    if (md5_sum_stream.str() != this->content_checksum)
    {
        this->notification.notify(12, true);
        return 1;
    }

    return 0;
}

/**
 * Map values from HTTP header (std::map<string, string>) to
 * object atributes.
 */
void File::map_header_vales(map<string, string> header_values)
{
    this->allow = header_values["allow"];
    this->content_location = header_values["content-location"];
    this->content_checksum = header_values["content-md5"];
    this->content_type = header_values["content-type"];
    this->expires = header_values["expires"];
    this->etag = header_values["etag"];
}

/**
 * Save file to file system and his metadata to database.
 */
int File::save_file(char *content)
{
    if (this->database.save_file_info(*this))
    {
        return 1;
    }

    // Save content to file system
    ofstream file;
    file.open(this->get_full_path());
    file << content;
    file.close();

    return 0;
}

/**
 * Open given file in assigned application.
 */
void File::open_file()
{
    auto path = get_full_path();
    this->utils.run_xdg_open(path);
}

/**
 * Test VDU connection and authentificat user.
 */
int File::test_and_authenticate()
{
    // Test if VDU is alive
    if (this->api.ping() != 204)
    {
        this->notification.notify(1, true);
        return 1;
    }

    // Authentication
    if (this->api.authenticate())
    {
        this->notification.notify(2, true);
        return 1;
    }

    return 0;
}
 
/**
 * Download file from VDU to file system.
 */
int File::download(const string &url)
{
    map<string, string> header_values;
    char *content = nullptr;
    size_t size = 0;

    // Retrieve access token from url
    this->access_token = url.substr(6);

    // Test and authentificate user
    if (this->test_and_authenticate())
    {
        return 1;
    }

    // Get data form server
    if (this->api.file_get(this->access_token, &header_values, &size, &content) != 200)
    {
        return 1;
    }

    // Map values from response to object variables
    this->map_header_vales(header_values);

    // Validate content of file by comparing checksums
    if (this->validate_file((unsigned char *)content, size))
    {
        return 1;
    }

    // Save file to db and filesystem
    if (this->save_file(content))
    {
        return 1;
    }

    // Cleanup
    free(content);

    // Save token info to db
    this->api.save_token_info();

    // Open file in assigned app
    this->open_file();

    return 0;
}

/**
 * Upload file to VDU.
 */
int File::upload(string path)
{
    map<string, string> header_values;

    // Set Content Location withoout first /
    this->content_location = path.substr(1);

    // Load data from db to this object
    this->database.load_file_info(*this);

    if (this->allow == "GET")
    {
        return 1;
    }

    // Read file
    ifstream infile(get_full_path());
    ostringstream ss;
    ss << infile.rdbuf();
    const string &s = ss.str();

    // Test and authentificate user
    if (this->test_and_authenticate())
    {
        return 1;
    }

    return this->api.file_post(this->access_token, &header_values, s.c_str());
}

/**
 * Check if file is not expired and if its writable
 */
int File::check(string path)
{
    // Set Content Location withoout first /
    this->content_location = path.substr(1);

    // Load datab from db
    this->database.load_file_info(*this);

    time_t expires = curl_getdate(this->expires.c_str(), nullptr);
    if (difftime(expires, chrono::system_clock::to_time_t(chrono::system_clock::now())) < 0)
    {
        this->download("vdu://" + this->access_token);
    }

    if (this->allow == "GET")
    {
        return -1;
    }

    return 0;
}

/**
 * Rename file in database (local changes)
 */
int File::rename(string from, string to)
{
    if(from[1] == '.')
    {
        // Set old Content Location withoout first /
        this->content_location = to.substr(1);

        // Load datab from db
        if (this->database.load_file_info(*this))
        {
            return 1;
        }
    }
    else
    {
        // Set old Content Location withoout first /
        this->content_location = from.substr(1);

        // Load datab from db
        if (this->database.load_file_info(*this))
        {
            return 1;
        }

        if(this->database.remove_file_info(*this)) 
        {
            return 1;
        }

        // Set new Content Location withoout first /
        this->content_location = to.substr(1);

        // Load datab from db
        if (this->database.save_file_info(*this))
        {
            return 1;
        }
    }

    this->upload("/" + this->content_location);

    return 0;
}