#include "ImageWindow.h"
#include "TextRenderer.h"

using namespace std;

ImageWindow::ImageWindow( WindowManager* owner, const string& title, const float xscale, const float yscale ):
    Window(owner, title, xscale, yscale),
    _url("")
{
    pthread_create(&_thread, 0, &ImageWindow::start_thread, this);
}

void ImageWindow::SetURL(const string &url)
{
    _url = url;
    UpdateImage();
}

void ImageWindow::UpdateImage()
{
    TextRenderer::I().LoadImage(_image, _url);
}

void ImageWindow::Draw()
{
    glPushMatrix();
    glScalef(.9,.9,.9);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    _image.Activate();
    
    Rectangle::unit.Draw( GL_TRIANGLE_FAN );
    
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}

void ImageWindow::deleteDataItem(DataItem* i) {
    delete i->image;            
    delete i;
}

void ImageWindow::fillList(DataItem *head, vector<DataItem*>& list)
{
    // scan the linked list
    list.reserve(10);
    DataItem* d = head; // start from the head
    while(d->prev != NULL) {
        list.push_back(d);
        d = d->prev;
    }
}

void ImageWindow::do_work()
{
    
}
