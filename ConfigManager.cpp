#include "ConfigManager.h"
#include <iostream>

using namespace std;

ConfigManager::ConfigManager()
    : m_stoprequested(false), m_running(false)
{
    pthread_mutex_init(&m_mutex, NULL);
}

ConfigManager::~ConfigManager()
{

    pthread_mutex_destroy(&m_mutex);

}

void ConfigManager::go() {
    m_running = true;
    pthread_create(&m_thread, 0, &ConfigManager::start_thread, this);
}

void ConfigManager::stop()
{
    m_running = false;
    m_stoprequested = true;
    pthread_join(m_thread, 0);
}

void ConfigManager::do_work() {
    while (!m_stoprequested) {
        pthread_mutex_lock(&m_mutex);
        cout << "Bla" << endl;
        pthread_mutex_unlock(&m_mutex);
    }
}
