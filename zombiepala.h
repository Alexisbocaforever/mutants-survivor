#ifndef ZOMBIEPALA_H
#define ZOMBIEPALA_H

#include "enemigos.h"
#include <QPainter>

class ZombiePala : public Enemigo
{
public:
    ZombiePala(float x, float y);
    ~ZombiePala() = default;

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
    void actualizarConJugador(QPointF jugador) override;
private:
    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;
    float anguloMovimiento;

    int tipoSkin;

    void obtenerColoresSkin(QColor &colorPiel, QColor &colorRopa, QColor &colorDetalle, QColor &colorOjos); // NUEVO
    void dibujarZombiePalaAbajo(QPainter &p);     // NUEVO
    void dibujarZombiePalaArriba(QPainter &p);    // NUEVO
    void dibujarZombiePalaIzquierda(QPainter &p); // NUEVO
    void dibujarZombiePalaDerecha(QPainter &p);   // NUEVO
};

#endif // ZOMBIEPALA_H
