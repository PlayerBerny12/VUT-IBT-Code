#include "notification.h"

Notification::Notification()
    : notification(notify_notification_new("", "", 0))
{        
}

Notification::~Notification()
{
    notify_notification_close(this->notification, 0);
}

void Notification::notify(int message_code, bool error, bool sleep)
{
    try
    {
        // Select message
        auto message = this->messages.at(message_code);
        if (!notify_notification_update(
                this->notification, "VDU-App",
                message.first.c_str(), message.second.c_str()))
        {
            throw runtime_error("Updating notification values failed.");
        }

        // Print to out or err console
        if (error)
        {
            cerr << message.first << endl;
        }
        else
        {
            cout << this->messages[0].first << endl;
        }

        // Show GUI notification with specific timeout
        notify_notification_set_timeout(this->notification, 5000);
        if (!notify_notification_show(this->notification, 0))
        {
            throw runtime_error("Showing notification failed.");
        }

        if(sleep) 
        {
            usleep(5000000);
        }
    }
    catch (const out_of_range &e)
    {
        cerr << "Unknown message selected for notifying user." << endl;
    }
    catch (const exception &e)
    {
        cerr << "Unknown error occurred." << endl
             << e.what() << endl;
    }
}