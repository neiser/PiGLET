#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iostream>
#include <map>
#include <functional>

extern "C" {
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
}

#include <Callback.h>

#define BUFFER_SIZE 1024

using util::Callback; // Callback lives in the util namespace

class ConfigManager
{
public:

    // Create the thread and start work
    void go();
    typedef Callback<int (std::string)> ConfigCallback;
    void setCmd(std::string cmd, ConfigCallback cb);

    pthread_mutex_t* Mutex();
    
    // access to the singleton instance
    static ConfigManager& I() {
        static ConfigManager instance;
        return instance;
    }

private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(ConfigManager const& copy);            // Not Implemented
    ConfigManager& operator=(ConfigManager const& copy); // Not Implemented

    pthread_mutex_t m_mutex;
    pthread_t m_thread;

    int _socket;
    const unsigned int _port;
    const std::string _address;

    std::map<std::string, ConfigCallback> _callbacks;

    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<ConfigManager*>(obj)->do_work();
    }

    // Compute and save fibonacci numbers as fast as possible
    void do_work();

    void InitSocket();
    
    int Kill(std::string arg);
};

#endif // CONFIGMANAGER_H
