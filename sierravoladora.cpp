// sierravoladora.cpp
#include "sierravoladora.h"

SierraVoladora::SierraVoladora()
    : pos_x(0), pos_y(0), vel_x(0), vel_y(0), velocidad(0), danio(0), radio(14.0f),
    distanciaMaxima(0), distanciaRecorrida(0), anguloGiro(0), volviendo(false),
    activo(false), tiempoVida(0)
{
}

void SierraVoladora::reiniciar(float x, float y, float angulo, float velocidadNueva, float danioNuevo,
                               float distanciaMaximaNueva, float radioNuevo)
{
    pos_x = x;
    pos_y = y;
    velocidad = velocidadNueva;
    danio = danioNuevo;
    radio = radioNuevo;
    distanciaMaxima = distanciaMaximaNueva;
    distanciaRecorrida = 0.0f;
    anguloGiro = 0.0f;
    volviendo = false;
    activo = true;
    tiempoVida = 4.0f; // red de seguridad, se apaga sola si algo sale mal

    float anguloRad = angulo * M_PI / 180.0f;
    vel_x = std::cos(anguloRad) * velocidad;
    vel_y = std::sin(anguloRad) * velocidad;

    golpeados.clear();
}

void SierraVoladora::actualizar(QPointF posJugador)
{
    if (!activo) return;

    anguloGiro += 25.0f;
    if (anguloGiro > 360.0f) anguloGiro -= 360.0f;

    if (!volviendo) {
        pos_x += vel_x;
        pos_y += vel_y;
        distanciaRecorrida += velocidad;

        if (distanciaRecorrida >= distanciaMaxima) {
            volviendo = true;
        }
    } else {
        float dx = posJugador.x() - pos_x;
        float dy = posJugador.y() - pos_y;
        float distancia = std::sqrt(dx * dx + dy * dy);

        if (distancia < 22.0f) {
            activo = false;
            return;
        }

        vel_x = (dx / distancia) * velocidad;
        vel_y = (dy / distancia) * velocidad;
        pos_x += vel_x;
        pos_y += vel_y;
    }

    tiempoVida -= 0.016f;
    if (tiempoVida <= 0.0f) activo = false;
}

void SierraVoladora::dibujar(QPainter &painter)
{
    if (!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(pos_x, pos_y);
    painter.rotate(anguloGiro);

    QRadialGradient cuerpo(0, 0, radio);
    cuerpo.setColorAt(0, QColor(230, 230, 235));
    cuerpo.setColorAt(0.7, QColor(150, 155, 160));
    cuerpo.setColorAt(1, QColor(80, 85, 90));
    painter.setBrush(cuerpo);
    painter.setPen(QPen(QColor(40, 40, 45), 2));
    painter.drawEllipse(QPointF(0, 0), radio, radio);

    painter.setBrush(QColor(200, 200, 205));
    int dientes = 8;
    for (int i = 0; i < dientes; i++) {
        float a = (360.0f / dientes) * i * M_PI / 180.0f;
        QPolygonF diente;
        diente << QPointF(std::cos(a) * radio, std::sin(a) * radio)
               << QPointF(std::cos(a + 0.15f) * (radio + 6), std::sin(a + 0.15f) * (radio + 6))
               << QPointF(std::cos(a - 0.15f) * (radio + 6), std::sin(a - 0.15f) * (radio + 6));
        painter.drawPolygon(diente);
    }

    painter.setBrush(QColor(60, 60, 65));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), radio * 0.3f, radio * 0.3f);

    painter.restore();
}
