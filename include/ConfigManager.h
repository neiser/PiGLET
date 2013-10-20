#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iostream>
#include <map>

#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "Callback.h"

#define BUFFER_SIZE 1024

using util::Callback; // Callback lives in the util namespace

class ConfigManager {
public:

    typedef Callback<std::string (const std::string&)> ConfigCallback;
    void addCmd(std::string cmd, ConfigCallback cb);
    void removeCmd(std::string cmd);
    
    
    void MutexLock();
    void MutexUnlock();
    
    void ExecutePendingCallback();
    
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

    pthread_mutex_t _mutex;
    pthread_t _thread;
    pthread_cond_t _callback_done;

    int _socket;
    const std::string _address;
    const unsigned int _port;
    

    std::map<std::string, ConfigCallback> _callbacks;

    std::string _callback_cmd;
    std::string _callback_return;
    std::string _callback_arg;
    
    // This is the static class function that serves as a C style function pointer
    // for the pthread_create call
    static void* start_thread(void *obj)
    {
        //All we do here is call the do_work() function
        reinterpret_cast<ConfigManager*>(obj)->do_work();
        return NULL;
    }

    void do_work();
    bool SendToClient(int client, std::string msg);
    
    void InitSocket();
    
    std::string Kill(const std::string& arg);
    static void trim(std::string& str, const std::string& whitespace = " \t\r\n");
};

#endif // CONFIGMANAGER_H
