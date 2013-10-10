#include "TextRenderer.h"
#include <iostream>

using namespace std;

TextRenderer::TextRenderer()
{
    MagickWandGenesis();    // just once somewhere
    mw = NewMagickWand();

    MagickSetSize(mw,256,64);
    MagickSetPointsize(mw,48);
    MagickSetFont(mw,"Sans");
    MagickSetOption(mw,"fill","white");
    MagickSetOption(mw,"background","red");
    MagickSetGravity(mw,CenterGravity);
    cout << "ctor" <<endl;
}

TextRenderer::~TextRenderer()
{
    if(mw)DestroyMagickWand(mw);
}

void TextRenderer::test(const string &text)
{
    MagickReadImage(mw, text.c_str());
    MagickWriteImage(mw,"caption.gif");
    MagickReadImage(mw, "label:A");
    MagickWriteImage(mw,"caption2.gif");
}

void TextRenderer::Text2Texture(const GLuint texhandle, const string &text)
{
    _watch.Start();

    MagickReadImage(mw, text.c_str());
    MagickWriteImage(mw,"text.gif");

    unsigned char *Buffer = NULL;
    size_t width = MagickGetImageWidth(mw);
    size_t height = MagickGetImageHeight(mw);
    Buffer = new unsigned char[ 4 * width * height];

    cout << "w="<<width << " h="<<height << endl;

    // Export the whole image
    MagickExportImagePixels(mw, 0, 0, width, height, "RGBA", CharPixel, Buffer);

    glBindTexture(GL_TEXTURE_2D, texhandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    delete [] Buffer;

    _watch.Stop();

    cout << "Text generation took: " << _watch.TimeElapsed() << " s" << endl;

}

