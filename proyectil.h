#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <cmath>

class Proyectil : public Dibujable_abstracto
{
public:
    // proyectil.h, agregar en la parte publica
    Proyectil(); // constructor por defecto, lo usa el pool
    // proyectil.h
    void reiniciar(float x, float y, float angulo, float velocidad, float danio,
                   bool esExplosivo = false, float radioExplosion = 0.0f,
                   float multiplicadorAlcance = 1.0f); // NUEVO
    ~Proyectil() = default;

    void dibujar(QPainter &painter) override;
    void actualizar();

    float get_x() override { return pos_x; }
    float get_y() override { return pos_y; }
    float get_ancho() override { return ancho; }
    float get_alto() override { return alto; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }

    float getDanio() const { return danio; }
    bool esExplosivo() const { return explosivo; }
    float getRadioExplosion() const { return radioExplosion; }


    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
    float angulo;
    float ancho;
    float alto;
    float danio;
    bool activo;
    bool explosivo;
    float radioExplosion;
    int tiempoVida;
};

#endif // PROYECTIL_H
