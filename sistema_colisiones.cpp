// sistema_colisiones.cpp
#include "sistema_colisiones.h"
#include <cmath>

SistemaColisiones::SistemaColisiones(float tamanioCelda) : tamanioCelda(tamanioCelda)
{
}

void SistemaColisiones::limpiar()
{
    celdas.clear();
}

void SistemaColisiones::celdaDe(Dibujable_abstracto* entidad, int &celX, int &celY) const
{
    float centroX = entidad->get_x() + entidad->get_ancho() / 2.0f;
    float centroY = entidad->get_y() + entidad->get_alto() / 2.0f;
    celX = (int)std::floor(centroX / tamanioCelda);
    celY = (int)std::floor(centroY / tamanioCelda);
}

qint64 SistemaColisiones::claveCelda(int celX, int celY) const
{
    return (static_cast<qint64>(celX) << 32) ^ static_cast<quint32>(celY);
}

void SistemaColisiones::insertar(Dibujable_abstracto* entidad)
{
    int celX, celY;
    celdaDe(entidad, celX, celY);
    celdas[claveCelda(celX, celY)].append(entidad);
}

QList<Dibujable_abstracto*> SistemaColisiones::obtenerVecinos(Dibujable_abstracto* entidad) const
{
    QList<Dibujable_abstracto*> vecinos;
    int celX, celY;
    celdaDe(entidad, celX, celY);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            auto it = celdas.constFind(claveCelda(celX + dx, celY + dy));
            if (it != celdas.constEnd()) vecinos.append(it.value());
        }
    }
    return vecinos;
}

QList<Dibujable_abstracto*> SistemaColisiones::obtenerEnRadio(float x, float y, float radio) const
{
    QList<Dibujable_abstracto*> resultado;
    int celMinX = (int)std::floor((x - radio) / tamanioCelda);
    int celMaxX = (int)std::floor((x + radio) / tamanioCelda);
    int celMinY = (int)std::floor((y - radio) / tamanioCelda);
    int celMaxY = (int)std::floor((y + radio) / tamanioCelda);

    for (int cx = celMinX; cx <= celMaxX; cx++) {
        for (int cy = celMinY; cy <= celMaxY; cy++) {
            auto it = celdas.constFind(claveCelda(cx, cy));
            if (it != celdas.constEnd()) resultado.append(it.value());
        }
    }
    return resultado;
}
