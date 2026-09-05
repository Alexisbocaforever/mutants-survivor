// proyectilcurvo.h
#ifndef PROYECTILCURVO_H
#define PROYECTILCURVO_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <QList>
#include <cmath>

class ProyectilCurvo : public Dibujable_abstracto
{
public:
    ProyectilCurvo(); // constructor por defecto, para poder poolearlo igual que Proyectil

    // curvaturaPorFrame: grados que gira el angulo cada frame.
    // positivo curva para un lado, negativo para el otro, 0 lo deja recto.
    void reiniciar(float x, float y, float anguloInicial, float velocidad, float danio,
                   float curvaturaPorFrame, float radioAncho = 6.0f, int duracionFrames = 200);

    void dibujar(QPainter &painter) override;
    void actualizar();

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
    float velocidad;
    float curvatura;
    float ancho;
    float alto;
    float danio;
    bool activo;
    int tiempoVida;

    QList<QPointF> estela;
};

#endif // PROYECTILCURVO_H
