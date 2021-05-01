// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 22.03.2021                                              ** //
// **   Module: File - code                                              ** //
// ************************************************************************ //

#include "file.h"

File::File(API &api, Database &database, Notification &notification)
    : api(api), database(database), notification(notification)
{
}

File::~File()
{
}

string File::get_full_path()
{
    return Utils::files_path + this->content_location;
}

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

void File::map_header_vales(map<string, string> header_values)
{
    this->allow = header_values["allow"];
    this->content_location = header_values["content-location"];
    this->content_checksum = header_values["content-md5"];
    this->content_type = header_values["content-type"];
    this->expires = header_values["expires"];
    this->etag = header_values["etag"];    
}

int File::save_file(char *content)
{
    if (this->database.save_file_info(*this))
    {
        return 1;
    }

    ofstream file;
    file.open(this->get_full_path());
    file << content;
    file.close();

    return 0;
}

void File::open_file()
{
    auto path = get_full_path();
    Utils::run_xdg_open(path);
}

int File::process_fake_file(const string path)
{
    // Prepare stream and variables
    string header, access_token;
    ifstream fake_file(path);

    // Read
    getline(fake_file, header);
    getline(fake_file, access_token);

    // Close stream
    fake_file.close();

    // Fake file validation
    if (header != "#VUD-App access token#" || !fake_file.eof())
    {
        this->notification.notify(5, true);
        return 1;
    }

    this->access_token = access_token;
    return this->download();
}

int File::download()
{
    map<string, string> header_values;
    char *content = nullptr;
    size_t size = 0;
    
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

    this->open_file();
    return 0;
}