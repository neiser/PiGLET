#include "WindowManager.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

std::ostream& operator<<( std::ostream& stream, const WindowManager& wman ) {
    stream << "Main Window Size: " << wman.SizeX() << " x " << wman.SizeY() << " ]";
    return stream;
}

void WindowManager::align_windws(){
    _rows.clear();
    int row = -1;
    int i = 0;
    while ( i < NumWindows() ){
        if ( i < 3 ){
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
    cout << endl << "New tiling:" << endl;
    for ( int r = 0 ; r < _rows.size() ; ++r ){
        cout << "  " <<_rows.at(r) << endl;
    }
    cout << endl;
}

void WindowManager::Draw(){

    float dy = 2. / _rows.size();
    float dx = 0;

    int i_window = 0;
    for ( int row = 0; row < _rows.size() ; ++row){
        dx = 2. / _rows.at(row);
        for ( int in_row = 0 ; in_row < _rows.at(row) ; ++in_row ){
            glPushMatrix();
            glTranslatef(-1 + (dx / 2) + (in_row * dx ),1 - (dy / 2. ) - (row * dy ),0.);
            glScalef(1. / _rows.at(row), 1.  / _rows.size() ,1);
            _window_list.at(i_window)->Draw();
            i_window++;
            glPopMatrix();
        }
    }


}
