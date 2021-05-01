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
    string sql = "INSERT OR REPLACE INTO Files (Location, Allow, Type, Expires, ETag, AccessToken, Usage) VALUES (?, ?, ?, ?, ?, ?, ?)";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            // Bind parameters
            sqlite3_bind_text(statement, 1, file.content_location.c_str(), file.content_location.length(), nullptr);
            sqlite3_bind_text(statement, 2, file.allow.c_str(), file.allow.length(), nullptr);
            sqlite3_bind_text(statement, 3, file.content_type.c_str(), file.content_type.length(), nullptr);
            sqlite3_bind_text(statement, 4, file.expires.c_str(), file.expires.length(), nullptr);
            sqlite3_bind_text(statement, 5, file.etag.c_str(), file.etag.length(), nullptr);
            sqlite3_bind_text(statement, 6, file.access_token.c_str(), file.access_token.length(), nullptr);
            sqlite3_bind_int(statement, 7, 0);
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
        this->notification.notify(6, true);

        return 1;
    }
}

int Database::load_file_info(File &file)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "SELECT Location, Allow, Type, Expires, ETag, AccessToken, Usage FROM Files WHERE Location = ?";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
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
            file.allow = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)));
            file.content_type = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 2)));
            file.expires = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)));
            file.etag = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)));
            file.access_token = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 5)));
        }
        else
        {
            throw runtime_error(nullptr);
        }

        if (sqlite3_step(statement) != SQLITE_DONE)
        {            throw runtime_error(nullptr);
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
        this->notification.notify(7, true);

        return 1;
    }
}

int Database::remove_file_info(File &file)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "DELETE FROM Files WHERE Location = ?";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
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
        this->notification.notify(8, true);

        return 1;
    }
}

int Database::save_token_info(token_info &token_info)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "INSERT OR REPLACE INTO Metadata (Key, Value) VALUES ('token_expires', ?), ('token_usage', ?)";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            auto usage = to_string(token_info.usage);

            // Bind parameters
            sqlite3_bind_text(statement, 1, token_info.expires.c_str(), token_info.expires.length(), nullptr);
            sqlite3_bind_text(statement, 2, usage.c_str(), usage.length(), nullptr);
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
        this->notification.notify(9, true);

        return 1;
    }
}

int Database::load_token_info(token_info &token_info)
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "SELECT Key, Value FROM Metadata WHERE Key = 'token_expires' OR Key = 'token_usage'";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Execute prepared statement
        while(sqlite3_step(statement) != SQLITE_DONE)
        {
            auto key = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)));
            auto value = string(reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)));
            
            if(key == "token_expires")
            {
                token_info.expires = value;
            }
            else if(key == "token_usage")
            {
                token_info.usage = stoi(value);
            }
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
        this->notification.notify(10, true);

        return 1;
    }
}

int Database::remove_token_info()
{
    sqlite3 *database;
    sqlite3_stmt *statement = nullptr;
    string sql = "DELETE FROM Metadata WHERE Key = 'token_expires' OR Key = 'token_usage'";

    try
    {
        // Open databse
        if (sqlite3_open(Utils::db_path.c_str(), &database) != SQLITE_OK)
        {
            throw runtime_error(nullptr);
        }

        // Prepare statement
        if (sqlite3_prepare_v2(database, sql.c_str(), -1, &statement, 0) != SQLITE_OK)
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
        this->notification.notify(11, true);

        return 1;
    }
}
