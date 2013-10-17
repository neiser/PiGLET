#include "WindowManager.h"

#include <iostream>
#include <stdlib.h>

#include "ConfigManager.h"

using namespace std;

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman ) {
    stream << "Main Window Size: " << wman.SizeX() << " x " << wman.SizeY() << " ]";
    return stream;
}

int WindowManager::callback_remove_window(const string& arg){
    return RemoveWindow(atoi(arg.c_str()));
}

int WindowManager::callback_remove_all_windows(const string &arg){
    int i;
    for ( i = NumWindows() ; i > 0 ; --i){
        RemoveWindow(0);
    }
    return i;
}

int WindowManager::callback_add_plotwindow(const string &arg)
{
    return AddWindow(new PlotWindow(arg));
}

void WindowManager::align_windws(){
    _rows.clear();
    int row = -1;
    int i = 0;
    while ( i < NumWindows() ){
        if ( i < 2 ){
            _rows.push_back(1);
            i++;
        } else{
            row++;
            if ( row >= _rows.size() && i < NumWindows() ){
                _rows.push_back(1);
                i++;
                while( _rows.at(row) < _rows.size() - 1 && ( i < NumWindows() ) ){
                    _rows.at(row)++;
                    i++;
                    row = -1;
                    break;
                }
            } else{
                while ( (_rows.at(row) < _rows.size()) && (i < NumWindows()) ){
                    _rows.at(row)++;
                    i++;
                }
            }
        }
    }
    //cout << endl << "New tiling:" << endl;
    //for ( int r = 0 ; r < _rows.size() ; ++r ){
    //   cout << "  " <<_rows.at(r) << endl;
    //}
    //cout << endl;
    
    float wscaley = 1. / _rows.size();
    float wscalex = 1.;
    int i_window = 0;
    
    for ( int row = 0; row < _rows.size() ; ++row){
        for ( int in_row = 0 ; in_row < _rows.at(row) ; ++in_row ){
            wscalex = 1. / _rows.at(row);
            _window_list.at(i_window)->XPixels() = wscalex * GetWindowWidth();
            _window_list.at(i_window)->YPixels() = wscaley * GetWindowHeight();
            _window_list.at(i_window)->Update();
            i_window++;
        }
    }
}

WindowManager::WindowManager(const int dx, const int dy): _size_x(dx), _size_y(dy)
{
    // register the callbacks in the ConfigManager
    ConfigManager::I().setCmd("RemoveWindow",BIND_MEM_CB(&WindowManager::callback_remove_window,this));
    ConfigManager::I().setCmd("RemoveWindows",BIND_MEM_CB(&WindowManager::callback_remove_all_windows,this));
    ConfigManager::I().setCmd("AddPlotWindow",BIND_MEM_CB(&WindowManager::callback_add_plotwindow,this));
}

int WindowManager::AddWindow(Window *win)
{
    int ret = win->Init();
    if(ret==0) {
        _window_list.push_back(win);
        align_windws();
    }
    else {
        delete win;
    }
}

int WindowManager::RemoveWindow(const int n){
    if ( n >= NumWindows() ) return 1;
    delete _window_list.at(n);
    _window_list.erase(_window_list.begin() + n);
    align_windws();
    return 0;
}

void WindowManager::Draw(){
    
    float dy = 2. / _rows.size();
    float dx = 0;
    
    float wscaley = 1. / _rows.size();
    float wscalex = 1.;
    
    int i_window = 0;
    for ( int row = 0; row < _rows.size() ; ++row){
        dx = 2. / _rows.at(row);
        for ( int in_row = 0 ; in_row < _rows.at(row) ; ++in_row ){
            wscalex = 1. / _rows.at(row);
            glPushMatrix();
            glTranslatef(-1 + (dx / 2) + (in_row * dx ),1 - (dy / 2. ) - (row * dy ),0.);
            glScalef( wscalex , wscaley ,1);
            _window_list.at(i_window)->Draw();
            i_window++;
            glPopMatrix();
        }
    }
    
    
}
