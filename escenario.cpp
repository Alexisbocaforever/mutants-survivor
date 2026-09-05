#include "escenario.h"

Escenario::Escenario()
    : x_escenario(0.0f),
    y_escenario(0.0f),
    ancho_escenario(0.0f),
    alto_escenario(0.0f)
{
}

void Escenario::agregarPared(Pared* pared)
{
    paredes.append(pared);
}

QList<Pared*>& Escenario::obtenerParedes()
{
    return paredes;
}

void Escenario::dibujarParedes(QPainter &p)
{
    for (Pared* pared : paredes) {
        if (pared) {
            pared->dibujar(p);
        }
    }
}
