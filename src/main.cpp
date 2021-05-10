// ************************************************************************ //
// **   Project: Application for controlled access                       ** //
// **            to remote documents for GNU\Linux                       ** //
// **   University: VUT FIT Brno                                         ** //
// **   Authors: Jan Bernard                                             ** //
// **   Created: 17.11.2020                                              ** //
// **   Module: Main - code                                              ** //
// ************************************************************************ //

#include "file.h"
#include "notification.h"

void init()
{
    notify_init("VDU-App");
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void cleanup()
{
    notify_uninit();
    curl_global_cleanup();
}

int process_arguments(int argc, char *argv[], API &api, Database &database)
{
    auto &notification = Notification::get_instance();
    auto file = File(api, database, notification);

    if (argc == 3)
    {
        // Process fake file
        if (string(argv[1]) == "--url-handler")
        {
            notification.notify(0, false);
            return file.download(argv[2]);
            return 0;
        }
        else
        {
            notification.notify(4, true);
            return 1;
        }
    }
    else if (argc == 4)
    {
        // Process real file
        if (string(argv[1]) == "--real-file")
        {
            if (string(argv[2]) == "read")
            {                
                return file.check(argv[3]);
            }
            else if (string(argv[2]) == "save")
            {
                return file.upload(argv[3]);
            }
            else if (string(argv[2]) == "rename")
            {
                return file.rename(argv[3]);
            }
            else
            {
                return 1;
            }

        }
        else
        {
            notification.notify(4, true);
            return 1;
        }
    }
    else
    {
        notification.notify(3, true);
        return 1;
    }
}

int main(int argc, char *argv[])
{
    int err_code = 0;
    auto &notification = Notification::get_instance();
    auto database = Database();
    auto api = API("https://e526bc93-da2e-4001-94a3-d9fa02033458.mock.pstmn.io/", database);
    init();

    if (api.ping() != 204)
    {
        err_code = 1;
        notification.notify(1, true);
    }
    else
    {
        if (api.authenticate())
        {
            err_code = 1;
            notification.notify(2, true);
        }
        else
        {
            err_code = process_arguments(argc, argv, api, database);

            // Save API state
            api.save_token_info();
        }
    }

    // Cleanup and exit
    cleanup();
    return err_code;
}
