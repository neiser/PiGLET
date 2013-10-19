#include <iostream>
#include <stdlib.h>

#include "ConfigManager.h"
#include "WindowManager.h"
#include "PlotWindow.h"


using namespace std;

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman ) {
    stream << "Main Window Size: " << wman.SizeX() << " x " << wman.SizeY() << " ]";
    return stream;
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
    return AddWindow(new PlotWindow(this, arg));
}

void WindowManager::align_windows(){
    _rows.clear();
    int row = -1;
    size_t i = 0;
    while ( i < NumWindows() ){
        if ( i < 2 ){
            _rows.push_back(1);
            i++;
        } else{
            row++;
            if ( row >= (int)_rows.size() && i < NumWindows() ){
                _rows.push_back(1);
                i++;
                while( _rows.at(row) < (int)(_rows.size() - 1) && ( i < NumWindows() ) ){
                    _rows.at(row)++;
                    i++;
                    row = -1;
                    break;
                }
            } else{
                while ( (_rows.at(row) < (int)_rows.size()) && (i < NumWindows()) ){
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
    
    for ( size_t row = 0; row < _rows.size() ; ++row){
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
    ConfigManager::I().addCmd("RemoveAllWindows",BIND_MEM_CB(&WindowManager::callback_remove_all_windows,this));
    ConfigManager::I().addCmd("AddPlotWindow",BIND_MEM_CB(&WindowManager::callback_add_plotwindow,this));
}

int WindowManager::AddWindow(Window *win)
{
    int ret = win->Init();
    if(ret==0) {
        _window_list.push_back(win);
        align_windows();
    }
    else {
        // delete the window again if 
        // init was unsuccessful
        delete win;
    }
    return ret;
}

int WindowManager::RemoveWindow(const size_t n){
    if ( n >= NumWindows() ) return 1;
    delete _window_list.at(n);
    _window_list.erase(_window_list.begin() + n);
    align_windows();
    return 0;
}

int WindowManager::RemoveWindow(const string &name)
{
    for(size_t i=0; i<NumWindows(); i++) {
        if(_window_list[i]->Name() == name) {
            RemoveWindow(i);
            return 0;
        }        
    }
    return 1;
}



void WindowManager::Draw(){
    
    float dy = 2. / _rows.size();
    float dx = 0;
    
    float wscaley = 1. / _rows.size();
    float wscalex = 1.;
    
    int i_window = 0;
    for ( size_t row = 0; row < _rows.size() ; ++row){
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
