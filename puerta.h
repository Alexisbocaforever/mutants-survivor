#ifndef PUERTA_H
#define PUERTA_H

#include "dibujable_abstracto.h"
#include <QPainter>

class Puerta : public Dibujable_abstracto {
public:
    // Se define con posición y dimensiones
    Puerta(float x, float y, float ancho, float alto);
    void dibujar(QPainter &p) override;


    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

private:
    float x, y, ancho, alto;
};

#endif // PUERTA_H
