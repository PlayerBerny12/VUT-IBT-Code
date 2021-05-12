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

/**
 * Global initialiation.
 */
void init()
{
    notify_init("VDU-App");
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

/**
 * Global cleanup.
 */
void cleanup()
{
    notify_uninit();
    curl_global_cleanup();
}

/**
 * Parse and process arguments.
 */
int process_arguments(int argc, char *argv[])
{
    auto &notification = Notification::get_instance();
    auto database = Database();
    auto api = API("https://e526bc93-da2e-4001-94a3-d9fa02033458.mock.pstmn.io/", database);
    auto file = File(api, database);

    if (argc == 3)
    {
        // Process fake file
        if (string(argv[1]) == "--url-handler")
        {
            notification.notify(0, false);
            return file.download(argv[2]);
        }
        else
        {
            notification.notify(4, true);
        }
    }
    else if (argc == 4)
    {
        // Process real file
        if (string(argv[1]) == "--real-file")
        {
            // Reading file
            if (string(argv[2]) == "check")
            {
                return file.check(argv[3]);
            }
            // Writing file
            else if (string(argv[2]) == "save")
            {
                return file.upload(argv[3]);
            }
            else if (string(argv[2]) == "delete")
            {
                return file.remove(argv[3]);
            }
        }
        else
        {
            notification.notify(4, true);
        }
    }
    else if (argc == 5)
    {
         // Process real file
        if (string(argv[1]) == "--real-file")
        {
            // Renaming file
            if (string(argv[2]) == "rename")
            {

                return file.rename(argv[3], argv[4]);
            }
        }
        else
        {
            notification.notify(4, true);
        }
    }
    else
    {
        notification.notify(3, true);
    }

    return 1;
}

/**
 * Entry point of vdu-app.
 */
int main(int argc, char *argv[])
{
    int err_code = 0;

    // Global initializon
    init();

    err_code = process_arguments(argc, argv);

    // Cleanup
    cleanup();

    // Exit
    cout << err_code;
    return err_code;
}
