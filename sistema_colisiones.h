// sistema_colisiones.h
#ifndef SISTEMA_COLISIONES_H
#define SISTEMA_COLISIONES_H

#include "dibujable_abstracto.h"
#include <QHash>
#include <QList>

// Particiona el mundo en celdas para que cada proyectil solo compare contra
// los enemigos cercanos, en vez de contra las 11 listas completas.
class SistemaColisiones
{
public:
    explicit SistemaColisiones(float tamanioCelda = 180.0f);

    void limpiar();
    void insertar(Dibujable_abstracto* entidad);
    QList<Dibujable_abstracto*> obtenerVecinos(Dibujable_abstracto* entidad) const;
    QList<Dibujable_abstracto*> obtenerEnRadio(float x, float y, float radio) const;

private:
    float tamanioCelda;
    QHash<qint64, QList<Dibujable_abstracto*>> celdas;

    qint64 claveCelda(int celX, int celY) const;
    void celdaDe(Dibujable_abstracto* entidad, int &celX, int &celY) const;
};

#endif // SISTEMA_COLISIONES_H
