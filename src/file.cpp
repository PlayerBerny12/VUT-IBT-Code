// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 22.03.2021                                              ** //
// **   Module: File - code                                              ** //
// ************************************************************************ //

#include "file.h"

File::File(API& api, Database& database, Notification& notification)
    : api(api), database(database), notification(notification)
{
}

File::~File()
{
}

string File::get_full_path() {
	return Config::files_path + this->content_location;    
}

void File::map_header_vales(map<string, string> header_values)
{
    for (auto &&[key, value] : header_values)
    {
        if (key == "allow")
        {
            this->allow = value;
        }
        else if (key == "content-encoding")
        {
            this->content_encoding = value;
        }
        else if (key == "content-length")
        {
            this->content_length = value;
        }
        else if (key == "content-location")
        {
            this->content_location = value;
        }
        else if (key == "content-md5")
        {
            this->content_checksum = value;
        }
        else if (key == "content-type")
        {
            this->content_type = value;
        }
        else if (key == "date")
        {
            this->date = value;
        }
        else if (key == "expires")
        {
            this->expires = value;
        }
        else if (key == "last-modified")
        {
            this->last_modified = value;
        }
        else if (key == "etag")
        {
            this->etag = value;
        }
    }
}

int File::save_file(char *content)
{   
    if(this->database.save_file_info(*this)) {
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
    // TODO: open file implementation
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

    // File validation
    if (header != "#VUD-App access token#" || !fake_file.eof())
    {
        this->notification.notify(3, true, true);
        return 1;
    }

    this->access_token = access_token;
    return this->download();    
}

int File::download()
{
    map<string, string> header_values;
    char *content = nullptr;

    api.request_header = curl_slist_append(api.request_header, "x-mock-response-code: 200");    
    this->api.file_get(this->access_token, &header_values, &content);
    
    this->map_header_vales(header_values);  
    this->save_file(content);
    this->open_file();  

    return 0;  
}