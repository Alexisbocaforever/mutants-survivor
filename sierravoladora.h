// sierravoladora.h
#ifndef SIERRAVOLADORA_H
#define SIERRAVOLADORA_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <QList>
#include <cmath>

class SierraVoladora : public Dibujable_abstracto
{
public:
    SierraVoladora();

    void reiniciar(float x, float y, float angulo, float velocidad, float danio,
                   float distanciaMaxima, float radio);

    void dibujar(QPainter &painter) override;
    void actualizar(QPointF posJugador);

    float get_x() override { return pos_x - radio; }
    float get_y() override { return pos_y - radio; }
    float get_ancho() override { return radio * 2.0f; }
    float get_alto() override { return radio * 2.0f; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }
    float getDanio() const { return danio; }
    float getRadio() const { return radio; }

    // Perfora: pega una sola vez a cada enemigo por tirada, no se frena
    bool yaGolpeo(Dibujable_abstracto* enemigo) const { return golpeados.contains(enemigo); }
    void registrarGolpe(Dibujable_abstracto* enemigo) { golpeados.append(enemigo); }

private:
    float pos_x;
    float pos_y;
    float vel_x;
    float vel_y;
    float velocidad;
    float danio;
    float radio;
    float distanciaMaxima;
    float distanciaRecorrida;
    float anguloGiro;
    bool volviendo;
    bool activo;
    float tiempoVida;

    QList<Dibujable_abstracto*> golpeados;
};

#endif // SIERRAVOLADORA_H
