#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <pthread.h>

class ConfigManager
{
public:
    ConfigManager();

     ~ConfigManager();

     // Create the thread and start work
     void go();
     void stop();


 private:
     volatile bool m_stoprequested; // Note 5
     volatile bool m_running;
     pthread_mutex_t m_mutex; // Variable declarations added 4/14/2010
     pthread_t m_thread;
     // This is the static class function that serves as a C style function pointer
     // for the pthread_create call
     static void* start_thread(void *obj)
     {
         //All we do here is call the do_work() function
         reinterpret_cast<ConfigManager*>(obj)->do_work();
     }

     // Compute and save fibonacci numbers as fast as possible
     void do_work();

};

#endif // CONFIGMANAGER_H
