#include "WindowManager.h"


std::ostream& operator<<( std::ostream& stream, const WindowManager& wman ) {
    stream << "Main Window Size: " << wman.SizeX() << " x " << wman.SizeY() << " ]";
    return stream;
}


void WindowManager::Draw(){

  const float dy = 2. / NumWindows();

  for ( int i = 0; i < NumWindows() ; ++i){
      glPushMatrix();
      glTranslatef(0.,1 - (dy / 2. ) - (i * dy ),0.);
      glScalef(1, 1.  / NumWindows() ,1);
      _window_list.at(i)->Draw();
      glPopMatrix();
  }

}
