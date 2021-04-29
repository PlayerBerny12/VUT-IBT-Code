// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.1.2021                                               ** //
// **   Module: Database - code                                          ** //
// ************************************************************************ //

#include "database.h"

Database::Database(Notification &notification)
    : notification(notification)
{
}

Database::~Database()
{
}

int Database::save_file_info(File &file)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "INSERT OR REPLACE INTO Files (Location, Encoding, Type, Allow, Expires, ETag, AccessToken) VALUES (?, ?, ?, ?, ?, ?, ?);";

    try
    {
        // Open databse
        if (sqlite3_open(Config::db_path.c_str(), &database) != SQLITE_OK)
        {            
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            // Bind parameters
            sqlite3_bind_text(statement, 1, file.content_location.c_str(), file.content_location.length(), nullptr);
            sqlite3_bind_text(statement, 2, file.content_encoding.c_str(), file.content_encoding.length(), nullptr);
            sqlite3_bind_text(statement, 3, file.content_type.c_str(), file.content_type.length(), nullptr);
            sqlite3_bind_text(statement, 4, file.allow.c_str(), file.allow.length(), nullptr);
            sqlite3_bind_text(statement, 5, file.expires.c_str(), file.expires.length(), nullptr);
            sqlite3_bind_text(statement, 6, file.etag.c_str(), file.etag.length(), nullptr);
            sqlite3_bind_text(statement, 7, file.access_token.c_str(), file.access_token.length(), nullptr);
        }
        else
        {            
            throw runtime_error(nullptr);
        }

        // Execute prepared statement
        if (sqlite3_step(statement) != SQLITE_DONE)
        {
            throw runtime_error(nullptr);
        }

        // Cleanup
        sqlite3_finalize(statement);
        sqlite3_close(database);
        return 0;
    }
    catch (const exception &e)
    {
        // Cleanup
        if (statement != nullptr)
        {
            sqlite3_finalize(statement);
        }
        sqlite3_close(database);

        // Show notification
        this->notification.notify(4, true, true);

        return 1;
    }
}

int Database::load_file_info(File &file)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "SELECT Location, Encoding, Type, Allow, Expires, ETag, AccessToken FROM Files WHERE Location = ?;";

    try
    {
        // Open databse
        if (sqlite3_open(Config::db_path.c_str(), &database) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            // Bind parameters
            sqlite3_bind_text(statement, 1, file.content_location.c_str(), file.content_location.length(), nullptr);
        }
        else
        {
            throw runtime_error(nullptr);
        }

        // Execute prepared statement
        if (sqlite3_step(statement) == SQLITE_ROW)
        {
            file.content_location = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)));
            file.content_encoding = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)));
            file.content_type = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 2)));
            file.allow = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)));
            file.expires = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)));
            file.etag = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 5)));
            file.access_token = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 6)));
        }
        else
        {
            throw runtime_error(nullptr);
        }

        if (sqlite3_step(statement) != SQLITE_DONE)
        {
            throw runtime_error(nullptr);
        }

        // Cleanup
        sqlite3_finalize(statement);
        sqlite3_close(database);
        return 0;
    }
    catch (const exception &e)
    {
        // Cleanup
        if (statement != nullptr)
        {
            sqlite3_finalize(statement);
        }
        sqlite3_close(database);

        // Show notification
        this->notification.notify(5, true, true);

        return 1;
    }
}