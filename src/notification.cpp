#include "notification.h"

/**
 * Constructor for Notification.
 */
Notification::Notification()
    : notification(notify_notification_new("", "", 0))
{        
}

/**
 * Get instance of singleton.
 */
Notification& Notification::get_instance()
{
    static Notification instance;
    return instance;
}

/**
 * Dealocate all manualy created atributes.
 * Call before exiting application.
 */
void Notification::cleanup()
{
    notify_notification_close(this->notification, 0);
}

/**
 * Show notification for user based on message code.
 * Message code is index into messages vector.
 */
void Notification::notify(int message_code, bool error)
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

        // Show GUI notification with specific timeout
        notify_notification_set_timeout(this->notification, 5000);
        if (!notify_notification_show(this->notification, 0))
        {
            throw runtime_error("Showing notification failed.");
        }

        // Sleep before exit
        if(error) 
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