#ifndef PROYECTILBUSCADOR_H
#define PROYECTILBUSCADOR_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <QPointF>
#include <cmath>

class ProyectilBuscador : public Dibujable_abstracto
{
public:
    ProyectilBuscador(float x, float y, float angulo, float velocidad, float danio);
    ~ProyectilBuscador() = default;

    void dibujar(QPainter &painter) override;
    void actualizar();
    void actualizarHacia(QPointF objetivo);

    float get_x() override { return pos_x; }
    float get_y() override { return pos_y; }
    float get_ancho() override { return ancho; }
    float get_alto() override { return alto; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }
    float getDanio() const { return danio; }

private:
    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
    float angulo;
    float ancho;
    float alto;
    float danio;
    bool activo;

    float tiempoVida;
    float tiempoVidaMax;
    float velocidadGiro;
};

#endif // PROYECTILBUSCADOR_H
