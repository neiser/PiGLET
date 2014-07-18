#include "ConfigManager.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include <cstdlib>
#include <stdio.h>
#include <signal.h>

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
        cerr << "Cmd " << cmd << " was already added. This should never happen. Exit!" << endl;
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




bool ConfigManager::sgetline(int client, string& line)
{
    int buf_size = 128;
    int bytesloaded = 0;
    int ret;
    char buf;
    char *buffer = (char*)malloc(buf_size);
    char *newbuf;

    if(buffer == NULL)
        return false;

    while(1)
    {
        // read a single byte
        ret = read(client, &buf, 1);
        if (ret < 1)
        {
            // error or disconnect
            free(buffer);
            return false;
        }

        // has end of line been reached?
        if (buf == '\n')
            break; // yes

        buffer[bytesloaded] = buf;
        bytesloaded++;



        // is more memory needed?
        if (bytesloaded >= buf_size)
        {
            buf_size += 128;
            newbuf = (char*)realloc(buffer, buf_size);

            if (NULL == newbuf)
            {
                free(buffer);
                return false;
            }

            buffer = newbuf;
        }
    }

    // if the line was terminated by "\r\n", ignore the
    // "\r". the "\n" is not in the buffer
    if(bytesloaded > 0 && buffer[bytesloaded-1] == '\r')
        bytesloaded--;

    line.clear();
    line.append(buffer, bytesloaded);

    return true;
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
            string line;
            if(!sgetline(client, line)) {
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

            /*
             *
             * Please note that the strings returned to the client are parsed by PiGLETManager
             * So change them with care!
             *
             **/


            // be careful about unlocking the mutex properly
            pthread_mutex_lock(&_mutex);

            if(_callbacks.count(cmd)==0) {
                client_connected = SendToClient(client, "Error: Command not found. Try 'List'.");
                pthread_mutex_unlock(&_mutex);
                continue;
            }

            _callback_arg = arg;
            _callback_cmd = cmd;

            // wait until ExecutePendingCallback signals it has executed it
            pthread_cond_wait(&_callback_done, &_mutex);

            if(!_callback_return.empty()) {
                client_connected = SendToClient(client,  "Error: "+_callback_return);
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
        stringstream ss;
        ss << "Failed bind() on " << _address << ":" << _port;
        perror(ss.str().c_str());
        close(_socket);
        exit(EXIT_FAILURE);
    }

    // start listening
    listen(_socket,0);

    // ignore SIGPIPES during communication with client
    // the writes are handled properly if something fails there...
    signal(SIGPIPE, SIG_IGN);
}

string ConfigManager::Kill(const string& arg)
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
