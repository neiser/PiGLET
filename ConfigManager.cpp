#include "ConfigManager.h"
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <stdio.h>

using namespace std;

ConfigManager::ConfigManager() :
    _address("0.0.0.0"), _port(1337), _callbacks()
{
    pthread_mutex_init(&m_mutex, NULL);
    InitSocket();
    
    setCmd("Kill", BIND_MEM_CB(&ConfigManager::Kill, this));
}

ConfigManager::~ConfigManager()
{
    cout << "dtor" << endl;
    pthread_mutex_destroy(&m_mutex);
    close(_socket);
}

void ConfigManager::go() {
    pthread_create(&m_thread, 0, &ConfigManager::start_thread, this);
}

void ConfigManager::setCmd(string cmd, ConfigManager::ConfigCallback cb)
{
    pthread_mutex_lock(&m_mutex);
    _callbacks[cmd] = cb;
    pthread_mutex_unlock(&m_mutex);
}

pthread_mutex_t *ConfigManager::Mutex()
{
    return &m_mutex;
}


void ConfigManager::do_work() {
    while(1) {
        int client;
        struct sockaddr_in client_addr; /* Client address */
        socklen_t client_addr_size = sizeof(client_addr);
        if ((client = accept(_socket, (struct sockaddr*)&client_addr, &client_addr_size)) < 0) {
            perror("accept()");
            close(_socket);
            exit(EXIT_FAILURE);
        }
        cout << "client connected" << endl;
        // set keepalive
        int optval = 1;
        socklen_t optlen = sizeof(optval);
        if(setsockopt(client, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
            perror("setsockopt()");
            close(client);
            continue;
        }
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
                cout << "Client disconnected" << endl;
                client_connected = false;
                continue;
            }
            // remove newline at the end
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

            if(line == "Exit:") {
                client_connected = false;
                continue;
            }
            
            size_t pos = line.find_first_of(':');
            if(pos == string::npos) {
                bzero(buf, BUFFER_SIZE);
                strcpy(&buf[0], "Invalid command. No colon ':' found in string.\n");
                n = write(client, buf, strlen(buf)+1);
                if(n<=0) {
                    client_connected = false;
                }
                continue;
            }                
            
            // split the line at the colon
            string cmd = line.substr(0,pos);
            string arg = line.substr(pos+1,line.length()-pos);
            
            // be careful about unlocking the mutex properly
            pthread_mutex_lock(&m_mutex);

            if(_callbacks.count(cmd)==0) {
                bzero(buf, BUFFER_SIZE);
                strcpy(&buf[0], "Command not found in list.\n");
                n = write(client, buf, strlen(buf)+1);
                if(n<=0) {
                    client_connected = false;
                }
                pthread_mutex_unlock(&m_mutex);
                continue;
            }            
            
            int ret = _callbacks[cmd](arg);
            pthread_mutex_unlock(&m_mutex);
            
            if(ret>0) {
                bzero(buf, BUFFER_SIZE);
                strcpy(&buf[0], "Command returned non-zero value.\n");
                n = write(client, buf, strlen(buf)+1);
                if(n<=0) {
                    client_connected = false;
                }                
            }
            
            
        }
        
        // properly close the socket after exiting the client's while loop
        close(client);        
    }
}

void ConfigManager::InitSocket()
{
    cout << "Init Socket binding to " << _address << ":" << _port << endl;

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

    // allow maximum one connection at a time
    listen(_socket, 1);

    
}

int ConfigManager::Kill(string arg)
{
    exit(EXIT_SUCCESS);
}
