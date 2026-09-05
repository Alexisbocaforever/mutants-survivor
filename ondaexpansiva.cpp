// ondaexpansiva.cpp
#include "ondaexpansiva.h"

OndaExpansiva::OndaExpansiva()
    : origenX(0), origenY(0), radioActual(0), radioMaximo(0),
    velocidadExpansion(0), grosorBanda(25.0f), danioPorTick(0), activo(false)
{
}

void OndaExpansiva::reiniciar(float ox, float oy, float danioNuevo, float radioMax,
                              float velocidadNueva, float grosor)
{
    origenX = ox;
    origenY = oy;
    radioActual = 0.0f;
    radioMaximo = radioMax;
    velocidadExpansion = velocidadNueva;
    grosorBanda = grosor;
    danioPorTick = danioNuevo;
    activo = true;
}

void OndaExpansiva::actualizar()
{
    if (!activo) return;

    radioActual += velocidadExpansion;
    if (radioActual >= radioMaximo) {
        activo = false;
    }
}

bool OndaExpansiva::colisionaCon(float px, float py, float radioObjetivo) const
{
    if (!activo) return false;

    float dx = px - origenX;
    float dy = py - origenY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    return distancia > (radioActual - grosorBanda / 2.0f - radioObjetivo) &&
           distancia < (radioActual + grosorBanda / 2.0f + radioObjetivo);
}

void OndaExpansiva::dibujar(QPainter &painter)
{
    if (!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    float progreso = radioActual / radioMaximo;
    int alpha = (int)(220 * (1.0f - progreso * 0.6f));

    painter.setPen(QPen(QColor(200, 60, 230, alpha), grosorBanda));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(origenX, origenY), radioActual, radioActual);

    painter.setPen(QPen(QColor(255, 220, 255, (int)(alpha * 0.8f)), grosorBanda * 0.35f));
    painter.drawEllipse(QPointF(origenX, origenY), radioActual, radioActual);

    painter.restore();
}
