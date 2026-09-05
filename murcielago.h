#ifndef MURCIELAGO_H
#define MURCIELAGO_H

#include "enemigos.h"
#include <QPainter>

class Murcielago : public Enemigo
{
public:
    Murcielago(float x, float y);
    ~Murcielago() = default;

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
void actualizarConJugador(QPointF jugador) override;
private:
    float frameAlas; // Para aletear
    bool alasBajando;
    int tipoSkin;
    QColor colCuerpo;
    QColor colAlas;
    QColor colOjos;
};

#endif // MURCIELAGO_H
