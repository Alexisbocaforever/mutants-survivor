// pulsar.h
#ifndef PULSAR_H
#define PULSAR_H

#include "enemigos.h"
#include "ondaexpansiva.h"
#include <QPainter>

class Pulsar : public Enemigo
{
public:
    Pulsar(float x, float y);
    ~Pulsar();

    void actualizar() override;
    void actualizar(float jugadorX, float jugadorY);
    void dibujar(QPainter &p) override;

    OndaExpansiva& getOnda() { return onda; }
    void actualizarConJugador(QPointF jugador) override;
private:
    float frameAnimacion;
    float timerPulso;
    bool cargando;
    float timerCarga;

    OndaExpansiva onda;

    QColor colorCascara;
    QColor colorMembrana;
    QColor colorNucleo;

    void dibujarCascara(QPainter &p, float escala);
    void dibujarNucleo(QPainter &p, float intensidad);
    void dibujarTentaculos(QPainter &p, float ondulacion);
};

#endif // PULSAR_H
