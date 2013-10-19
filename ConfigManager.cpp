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
    
    addCmd("Kill", BIND_MEM_CB(&ConfigManager::Kill, this));
    
    pthread_create(&_thread, 0, &ConfigManager::start_thread, this);
}

ConfigManager::~ConfigManager()
{
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_callback_done);
    close(_socket);
}

void ConfigManager::addCmd(string cmd, ConfigManager::ConfigCallback cb)
{
    // check if there is already a callback for that command
    if(_callbacks.count(cmd)!=0) {
        cerr << "Cmd " << cmd << " was already added. This should never happen." << endl;
        exit(EXIT_FAILURE);        
    }
    _callbacks[cmd] = cb;
}

void ConfigManager::removeCmd(string cmd)
{
    _callbacks.erase(cmd);;
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
        
        // close the listening socket, 
        // to prevent another connection
        close(_socket);
        
        // print a welcome (also used by PiGLETManager to check connection)
        bool client_connected = SendToClient(client,  "Welcome to PiGLET!");
        
        // start client loop
        while(client_connected) {
            
            // wait and read input until newline
            char buf[BUFFER_SIZE];
            string line;
            int n = 0;
            bzero(buf, BUFFER_SIZE);
            while((n = read(client, buf, BUFFER_SIZE-1)))
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
                        
            // trim whitespace (including newlines)
            trim(line);                        
            
            size_t pos = line.find_first_of(' ');
            // split the line at the first space
            string cmd = line.substr(0,pos); 
            string arg;
            if(pos < string::npos) {
                // there is an argument, 
                arg = line.substr(pos+1,line.length()-pos);
                // also trim leading/trailing whitespace again
                trim(arg);
            }            
            
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
                client_connected = SendToClient(client, "Command not found. Try 'List'.");
                pthread_mutex_unlock(&_mutex);
                continue;
            }            
            
            _callback_arg = arg;
            _callback_cmd = cmd;           
            
            // wait until ExecutePendingCallback signals it has executed it
            pthread_cond_wait(&_callback_done, &_mutex);
   
            if(_callback_return != 0) {         
                stringstream ss;
                ss << "Command returned non-zero value: " << _callback_return;
                client_connected = SendToClient(client,  ss.str());        
            }
            else {
                client_connected = SendToClient(client,  "Ok."); 
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
    size_t n = write(client, msg.c_str(), msg.length()+1);
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

    // reuseaddress might be helpful if 
    // the socket was not properly closed
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

void ConfigManager::trim(string& str, const string& whitespace)
{
    const size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        str = "";
        return; // no content
    }

    const size_t strEnd = str.find_last_not_of(whitespace);
    const size_t strRange = strEnd - strBegin + 1;

    str = str.substr(strBegin, strRange);
}
