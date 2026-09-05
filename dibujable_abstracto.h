#ifndef DIBUJABLE_ABSTRACTO_H
#define DIBUJABLE_ABSTRACTO_H

#include <QPainter>

class Dibujable_abstracto
{
public:
    Dibujable_abstracto();
    virtual ~Dibujable_abstracto() = default;

    virtual float get_x() = 0;
    virtual float get_y() = 0;
    virtual float get_ancho() = 0;
    virtual float get_alto() = 0;

    virtual void dibujar(QPainter &p) = 0;
};

#endif // DIBUJABLE_ABSTRACTO_H
