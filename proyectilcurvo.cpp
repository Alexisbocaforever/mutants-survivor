// proyectilcurvo.cpp
#include "proyectilcurvo.h"

ProyectilCurvo::ProyectilCurvo()
    : pos_x(0), pos_y(0), vel_x(0), vel_y(0), angulo(0), velocidad(0),
    curvatura(0), ancho(6.0f), alto(6.0f), danio(0), activo(false), tiempoVida(0)
{
}

void ProyectilCurvo::reiniciar(float x, float y, float anguloInicial, float velocidadNueva, float danioNuevo,
                               float curvaturaPorFrame, float radioAncho, int duracionFrames)
{
    pos_x = x;
    pos_y = y;
    angulo = anguloInicial;
    velocidad = velocidadNueva;
    curvatura = curvaturaPorFrame;
    danio = danioNuevo;
    ancho = radioAncho;
    alto = radioAncho;
    activo = true;
    tiempoVida = duracionFrames;
    estela.clear();

    vel_x = velocidad * cos(angulo * M_PI / 180.0f);
    vel_y = velocidad * sin(angulo * M_PI / 180.0f);
}

void ProyectilCurvo::actualizar()
{
    if (!activo) return;

    estela.append(QPointF(pos_x, pos_y));
    if (estela.size() > 10) estela.removeFirst();

    angulo += curvatura;
    vel_x = velocidad * cos(angulo * M_PI / 180.0f);
    vel_y = velocidad * sin(angulo * M_PI / 180.0f);

    pos_x += vel_x;
    pos_y += vel_y;

    tiempoVida--;
    if (tiempoVida <= 0) activo = false;
}

void ProyectilCurvo::dibujar(QPainter &painter)
{
    if (!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < estela.size(); i++) {
        float progreso = (float)i / estela.size();
        painter.setBrush(QColor(80, 255, 150, (int)(180 * progreso)));
        painter.drawEllipse(estela[i], ancho * 0.5f * progreso, ancho * 0.5f * progreso);
    }

    QRadialGradient nucleo(pos_x, pos_y, ancho);
    nucleo.setColorAt(0, QColor(230, 255, 240));
    nucleo.setColorAt(0.5, QColor(100, 255, 160));
    nucleo.setColorAt(1, QColor(20, 150, 80, 0));
    painter.setBrush(nucleo);
    painter.drawEllipse(QPointF(pos_x, pos_y), ancho, ancho);

    painter.restore();
}
