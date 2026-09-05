#ifndef MUTANTE_H
#define MUTANTE_H

#include "enemigos.h"
#include <QPainter>
#include <QPointF>

class Mutante : public Enemigo
{
public:
    Mutante(float x, float y);
    ~Mutante() = default;

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
void actualizarConJugador(QPointF jugador) override;
private:
    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;
    float anguloMovimiento;

    // Animación de tentáculos
    float anguloTentaculo1;
    float anguloTentaculo2;
    float anguloTentaculo3;
    float velocidadTentaculos;

    void dibujarTentaculos(QPainter &painter);
    void dibujarCuerpo(QPainter &painter);
    void dibujarCabeza(QPainter &painter);
    void dibujarBarraVida(QPainter &painter);
};

#endif // MUTANTE_H
