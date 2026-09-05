// ondaexpansiva.h
#ifndef ONDAEXPANSIVA_H
#define ONDAEXPANSIVA_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <cmath>

class OndaExpansiva : public Dibujable_abstracto
{
public:
    OndaExpansiva();

    void reiniciar(float origenX, float origenY, float danioPorTick, float radioMaximo,
                   float velocidadExpansion, float grosorBanda = 25.0f);

    void dibujar(QPainter &painter) override;
    void actualizar();

    float get_x() override { return origenX - radioMaximo; }
    float get_y() override { return origenY - radioMaximo; }
    float get_ancho() override { return radioMaximo * 2.0f; }
    float get_alto() override { return radioMaximo * 2.0f; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }
    float getDanio() const { return danioPorTick; }

    // Verdadero mientras el punto esta dentro de la banda que se esta expandiendo
    bool colisionaCon(float px, float py, float radioObjetivo) const;

private:
    float origenX;
    float origenY;
    float radioActual;
    float radioMaximo;
    float velocidadExpansion;
    float grosorBanda;
    float danioPorTick;
    bool activo;
};

#endif // ONDAEXPANSIVA_H
