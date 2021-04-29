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

int main(int argc, char *argv[])
{
    int err_code = 0;    
    auto notification = Notification();
    auto api = API("https://e526bc93-da2e-4001-94a3-d9fa02033458.mock.pstmn.io/");
    auto database = Database(notification);
    auto file = File(api, database, notification);
    
    init();
    notification.notify(0, false, false);

    if (argc == 3)
    {
        // Process fake file
        if (string(argv[1]) == "--fake-file")
        {
            err_code = file.process_fake_file(argv[2]);            
        }
        // Process real file
        else if (string(argv[1]) == "--real-file")
        {
            // TODO: fuse real-file processing
        }
        else
        {
            err_code = 1;
            notification.notify(2, true, true);        
        }
    }
    else
    {
        err_code = 1;
        notification.notify(1, true, true);
    }        

    // Cleanup and exit
    cleanup();    
    return err_code;
}
