#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include "dibujable_abstracto.h"
#include "vida.h"
#include <algorithm>
class Enemigo : public Dibujable_abstracto
{
public:
    virtual void actualizarConJugador(QPointF jugador) {}
    virtual void dibujarProyectilesPropios(QPainter &painter) {}
    Enemigo();
    Enemigo(float x, float y);
    virtual ~Enemigo() = default;

    virtual void actualizar() = 0;
    virtual void mover(float dx, float dy);

    void teletransportar(float x, float y) { pos_x = x; pos_y = y; }
    void setFactorMapa(float factor) { factorMapa = factor; }
    float getFactorMapa() const { return factorMapa; }

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    bool estaVivo() const;
    void recibirDanio(float danio);
    Vida& getVida();

    float getDanioContacto() const;

    // NUEVO: Sistema de multiplicador de velocidad
    void multiplicarVelocidad(float multiplicador);
    // Debajo de float getDanioContacto() const;
    void multiplicarDanio(float multiplicador); // NUEVO

    void multiplicarVida(float multiplicador); // NUEVO: Escalar vida
    bool esCadaver() const;                    // NUEVO: Para saber si está en los 3s de espera
    bool debeDesaparecer() const;
    float getTiempoQuemandose() const { return tiempoQuemandose; }
    void incrementarQuemado(float dt) { tiempoQuemandose = std::min(tiempoQuemandose + dt, 3.0f); }
    void decaerQuemado(float dt) { tiempoQuemandose = std::max(0.0f, tiempoQuemandose - dt * 0.6f); }

protected:
    float factorMapa = 1.0f;
    float pos_x;
    float pos_y;
    float ancho;
    float alto;
    Vida sistemaVida;
    float velocidad;
    bool vivo;
    float danioContacto;
    float tiempoDesaparecer; // Contador de 3 segundos
    float tiempoQuemandose;
};

#endif // ENEMIGOS_H
