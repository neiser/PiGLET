#include "GLTools.h"

const Rectangle Rectangle::unit(-1,-1,1,1);

void Rectangle::_update_vertices()
{
    _vertices[0].x = _center.X() - _width/2.0f;
    _vertices[0].y = _center.Y() - _height/2.0f;

    _vertices[1].x = _center.X() - _width/2.0f;
    _vertices[1].y = _center.Y() + _height/2.0f;

    _vertices[2].x = _center.X() + _width/2.0f;
    _vertices[2].y = _center.Y() + _height/2.0f;

    _vertices[3].x = _center.X() + _width/2.0f;
    _vertices[3].y = _center.Y() - _height/2.0f;
}

Rectangle::Rectangle(const float x1, const float y1, const float x2, const float y2)
{
    _center.X() = (x1 + x2) / 2.0f;
    _center.Y() = (y1 + y2) / 2.0f;
    _width    = x2 - x1;
    _height   = y2 - y1;

    _update_vertices();
}

Rectangle::Rectangle( const Vector2& center, const float width, const float height): _center(center), _width(width), _height(height)
{
    _update_vertices();
}

void Rectangle::SetCenter(const Vector2& center)
{
    _center = center;
    _update_vertices();
}

void Rectangle::SetWidth(const float width)
{
    _width = width;
    _update_vertices();
}

void Rectangle::SetHeight(const float height)
{
    _height = height;
    _update_vertices();
}

void Rectangle::Draw(GLenum mode) const
{
    glVertexPointer( 2, GL_FLOAT, 0, _vertices);
    glDrawArrays( mode, 0, 4);
}
