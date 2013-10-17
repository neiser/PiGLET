#include "ConfigManager.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <stdio.h>

using namespace std;

ConfigManager::ConfigManager() :
    _address("0.0.0.0"), _port(1337), _callbacks()
{
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init (&_callback_done, NULL);
    InitSocket();
    
    setCmd("Kill", BIND_MEM_CB(&ConfigManager::Kill, this));
    
    pthread_create(&_thread, 0, &ConfigManager::start_thread, this);
}

ConfigManager::~ConfigManager()
{
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_callback_done);
    close(_socket);
}

void ConfigManager::setCmd(string cmd, ConfigManager::ConfigCallback cb)
{
    // check if there is already a callback for that command
    if(_callbacks.count(cmd)!=0) {
        cerr << "Cmd " << cmd << " was already added. This should never happen." << endl;
        exit(EXIT_FAILURE);        
    }
    _callbacks[cmd] = cb;
}

void ConfigManager::MutexLock()
{
    pthread_mutex_lock(&_mutex);
}

void ConfigManager::MutexUnlock()
{
    pthread_mutex_unlock(&_mutex);
}




void ConfigManager::do_work() {
    while(1) {
        int client;
        struct sockaddr_in client_addr; /* Client address */
        socklen_t client_addr_size = sizeof(client_addr);
        if ((client = accept(_socket, (struct sockaddr*)&client_addr, &client_addr_size)) < 0) {
            perror("client accept()");
            close(_socket);
            exit(EXIT_FAILURE);
        }
        // set keepalive
        int optval = 1;
        socklen_t optlen = sizeof(optval);
        if(setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
            perror("setsockopt()");
            close(client);
            continue;
        }
        
        // close the listening socket 
        close(_socket);
        
        // start 
        bool client_connected = true;
        while(client_connected) {
            char buf[BUFFER_SIZE];
            string line;
            int n = 0;
            bzero(buf, BUFFER_SIZE);
            while(n = read(client, buf, BUFFER_SIZE-1))
            {
                line += buf;
                buf[n+1] = '\0';
                const char* pos = strchr(&buf[0], '\n');
                if(pos != NULL) {
                    break;
                }
            }
            if(n<=0) {
                // close client socket
                client_connected = false;
                continue;
            }
            // remove newline characters
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            
            
            size_t pos = line.find_first_of(':');
            if(pos == string::npos) {
                client_connected = SendToClient(client, "Invalid command. No colon ':' found in string.");
                continue;
            }                
            
            // split the line at the colon
            string cmd = line.substr(0,pos);
            string arg = line.substr(pos+1,line.length()-pos);
            
            // some meta-commands
            if(cmd=="Exit") {
                client_connected = false;
                continue;
            }
            else if(cmd=="List") {
                stringstream ss;
                ss << "Available commands: Exit List ";
                for (map<string, ConfigCallback>::iterator it = _callbacks.begin(); it != _callbacks.end(); ++it) {
                    ss << it->first << " ";
                }
                client_connected = SendToClient(client,  ss.str());
                continue;
            }
            
            
            
            // be careful about unlocking the mutex properly
            pthread_mutex_lock(&_mutex);

            if(_callbacks.count(cmd)==0) {
                client_connected = SendToClient(client, "Command not found. Try 'List:'.");
                pthread_mutex_unlock(&_mutex);
                continue;
            }            
            
            _callback_arg = arg;
            _callback_cmd = cmd;           
            
            // wait until ExecutePendingCallback signals it has executed it
            pthread_cond_wait(&_callback_done, &_mutex);
   
            if(_callback_return>0) {         
                stringstream ss;
                ss << "Command returned non-zero value: " << _callback_return;
                client_connected = SendToClient(client,  ss.str());        
            }
            _callback_cmd.clear();
            
            pthread_mutex_unlock(&_mutex);
        }
        
        // properly close the socket after exiting the client's while loop
        close(client);        
        // but open the server socket again
        InitSocket();
    }
}

void ConfigManager::ExecutePendingCallback()
{
    if(_callback_cmd.empty())
        return;
    _callback_return = _callbacks[_callback_cmd](_callback_arg);
    pthread_cond_signal(&_callback_done);
}

bool ConfigManager::SendToClient(int client, string msg)
{
    msg += "\n";
    int n = write(client, msg.c_str(), msg.length()+1);
    return n == msg.length()+1;
}

void ConfigManager::InitSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket==-1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(_port);
    inet_pton(my_addr.sin_family, _address.c_str(), &(my_addr.sin_addr));

    // set keepalive
    int optval = 1;
    socklen_t optlen = sizeof(optval);
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) < 0) {
        perror("setsockopt()");
        close(_socket);
    }

    if (bind(_socket, (struct sockaddr*)&my_addr, sizeof(my_addr))==-1) {
        perror("bind()");
        close(_socket);
        exit(EXIT_FAILURE);
    }    
    
    // start listening
    listen(_socket,0);
}

int ConfigManager::Kill(const string& arg)
{
    // exit is a bit special, since it never returns...
    pthread_cond_signal(&_callback_done);
    exit(EXIT_SUCCESS);
}
