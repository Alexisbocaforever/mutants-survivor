#ifndef GOLIAT_H
#define GOLIAT_H

#include "enemigos.h"
#include <QPainter>
#include <QPointF>

class Goliat : public Enemigo
{
public:
    Goliat(float x, float y);
    ~Goliat() = default;

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
    void actualizarConJugador(QPointF jugador) override;
private:
    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;
    float anguloMovimiento;

    void dibujarArriba(QPainter &painter, float offset);
    void dibujarAbajo(QPainter &painter, float offset);
    void dibujarIzquierda(QPainter &painter, float offset);
    void dibujarDerecha(QPainter &painter, float offset);
    void dibujarDiagonal(QPainter &painter, float offset, int direccion);
};

#endif // GOLIAT_H
