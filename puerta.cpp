#include "puerta.h"

Puerta::Puerta(float x, float y, float ancho, float alto)
    : x(x), y(y), ancho(ancho), alto(alto)
{
}

void Puerta::dibujar(QPainter &p)
{
    p.save();
    p.setBrush(QColor(139, 69, 19));
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(QRectF(x, y, ancho, alto));
    p.restore();
}

// CORREGIDO: Retornos correspondientes agregados a los getters
float Puerta::get_x()     { return x; }
float Puerta::get_y()     { return y; }
float Puerta::get_ancho() { return ancho; }
float Puerta::get_alto()  { return alto; }
