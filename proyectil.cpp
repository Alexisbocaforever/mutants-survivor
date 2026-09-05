#include "proyectil.h"

// proyectil.cpp, agregar

Proyectil::Proyectil()
    : pos_x(0), pos_y(0), vel_x(0), vel_y(0), angulo(0), ancho(5.0f), alto(5.0f),
    danio(0), activo(false), explosivo(false), radioExplosion(0.0f), tiempoVida(0)
{
}

// proyectil.cpp
void Proyectil::reiniciar(float x, float y, float anguloNuevo, float velocidad, float danioNuevo,
                          bool esExplosivo, float radioExplosionNueva, float multiplicadorAlcance)
{
    pos_x = x;
    pos_y = y;
    angulo = anguloNuevo;
    danio = danioNuevo;
    activo = true;
    explosivo = esExplosivo;
    radioExplosion = radioExplosionNueva;
    tiempoVida = (int)(300 * multiplicadorAlcance); // antes: fijo en 300

    ancho = explosivo ? 8.0f : 5.0f;
    alto = explosivo ? 8.0f : 5.0f;

    vel_x = velocidad * cos(angulo * M_PI / 180.0f);
    vel_y = velocidad * sin(angulo * M_PI / 180.0f);
}

void Proyectil::dibujar(QPainter &painter)
{
    if (!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // DETECTAR tipos de proyectiles por velocidad y daño
    float velocidadActual = sqrt(vel_x*vel_x + vel_y*vel_y);
    bool esOrbeAlien = (abs(velocidadActual - 8.0f) < 0.1f && abs(danio - 30.0f) < 0.1f);      // Actualizado
    bool esOrbePredictivo = (abs(velocidadActual - 10.0f) < 0.1f && abs(danio - 35.0f) < 0.1f); // Actualizado
    bool esOrbeCruz = (abs(velocidadActual - 15.0f) < 0.1f && abs(danio - 25.0f) < 0.1f);

    if(esOrbeCruz) {
        // === ORBE AMARILLO (ATAQUE EN CRUZ) ===
        painter.translate(pos_x, pos_y);

        // Aura exterior AMARILLA brillante
        QRadialGradient aura(0, 0, 12);
        aura.setColorAt(0, QColor(255, 255, 100, 250));
        aura.setColorAt(0.4, QColor(255, 200, 0, 220));
        aura.setColorAt(0.7, QColor(255, 150, 0, 120));
        aura.setColorAt(1, QColor(200, 100, 0, 0));
        painter.setBrush(aura);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(0, 0), 12, 12);

        // Núcleo brillante central
        QRadialGradient nucleo(0, 0, 6);
        nucleo.setColorAt(0, QColor(255, 255, 255));
        nucleo.setColorAt(0.5, QColor(255, 255, 150));
        nucleo.setColorAt(1, QColor(255, 200, 0));
        painter.setBrush(nucleo);
        painter.drawEllipse(QPointF(0, 0), 6, 6);

        // Punto central super brillante
        painter.setBrush(QColor(255, 255, 255));
        painter.drawEllipse(QPointF(0, 0), 2, 2);

        // Destellos intensos (rayos)
        painter.setPen(QPen(QColor(255, 255, 255, 230), 2.5));
        painter.drawLine(-8, 0, 8, 0);
        painter.drawLine(0, -8, 0, 8);

        painter.setPen(QPen(QColor(255, 255, 150, 180), 1.5));
        painter.drawLine(-6, -6, 6, 6);
        painter.drawLine(-6, 6, 6, -6);

        // Estela larga (va muy rápido)
        painter.setPen(QPen(QColor(255, 255, 0, 200), 4));
        float estelaX = -vel_x * 3.0f;
        float estelaY = -vel_y * 3.0f;
        painter.drawLine(0, 0, estelaX, estelaY);

    } else if(esOrbeAlien || esOrbePredictivo) {
        // === ORBE DE ENERGÍA AZUL (ALIEN NORMAL O PREDICTIVO) ===
        painter.translate(pos_x, pos_y);

        // Aura exterior brillante (más grande)
        QRadialGradient aura(0, 0, 15);
        aura.setColorAt(0, QColor(150, 200, 255, 250));
        aura.setColorAt(0.4, QColor(100, 150, 255, 200));
        aura.setColorAt(0.7, QColor(50, 100, 255, 100));
        aura.setColorAt(1, QColor(0, 50, 200, 0));
        painter.setBrush(aura);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(0, 0), 15, 15);

        // Núcleo brillante central
        QRadialGradient nucleo(0, 0, 8);
        nucleo.setColorAt(0, QColor(220, 240, 255));
        nucleo.setColorAt(0.5, QColor(150, 200, 255));
        nucleo.setColorAt(1, QColor(100, 150, 255));
        painter.setBrush(nucleo);
        painter.drawEllipse(QPointF(0, 0), 8, 8);

        // Punto central super brillante
        painter.setBrush(QColor(255, 255, 255));
        painter.drawEllipse(QPointF(0, 0), 3, 3);

        // Destellos en cruz
        painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
        painter.drawLine(-10, 0, 10, 0);
        painter.drawLine(0, -10, 0, 10);

        // Destellos diagonales
        painter.setPen(QPen(QColor(200, 230, 255, 150), 1.5));
        painter.drawLine(-7, -7, 7, 7);
        painter.drawLine(-7, 7, 7, -7);

    } else if (explosivo) {
        // Proyectil explosivo (granada/cohete)
        painter.translate(pos_x, pos_y);
        painter.rotate(angulo);

        // Cuerpo del proyectil
        QColor colorCuerpo = (radioExplosion > 80.0f) ? QColor(80, 80, 90) : QColor(60, 80, 60);
        painter.setBrush(colorCuerpo);
        painter.setPen(Qt::NoPen);
        painter.drawRect(-4, -3, 8, 6);

        // Cabeza puntiaguda
        painter.setBrush(QColor(200, 50, 0));
        QPolygonF cabeza;
        cabeza << QPointF(4, 0) << QPointF(7, -2) << QPointF(7, 2);
        painter.drawPolygon(cabeza);

        // Llama/estela si es cohete
        if (radioExplosion > 80.0f) {
            painter.setBrush(QColor(255, 200, 0, 180));
            painter.drawEllipse(QPointF(-5, 0), 3, 2);
            painter.setBrush(QColor(255, 100, 0, 120));
            painter.drawEllipse(QPointF(-7, 0), 2, 1);
        }

    } else {
        // Proyectil normal (bala)
        painter.setBrush(QColor(255, 220, 0));
        painter.setPen(QPen(QColor(200, 180, 0), 1));
        painter.drawEllipse(QPointF(pos_x, pos_y), ancho/2, alto/2);

        // Estela
        painter.setPen(QPen(QColor(255, 200, 0, 100), 2));
        float estelaX = pos_x - vel_x * 0.5f;
        float estelaY = pos_y - vel_y * 0.5f;
        painter.drawLine(QPointF(pos_x, pos_y), QPointF(estelaX, estelaY));
    }

    painter.restore();
}

void Proyectil::actualizar()
{
    if (!activo) return;

    pos_x += vel_x;
    pos_y += vel_y;

    tiempoVida--;
    if (tiempoVida <= 0) {
        activo = false;
    }
}
