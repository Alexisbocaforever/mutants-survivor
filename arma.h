#ifndef ARMA_H
#define ARMA_H

#include "dibujable_abstracto.h"
#include "proyectil.h"
#include <QPointF>
#include <QList>

class Arma : public Dibujable_abstracto
{
public:
    Arma();
    virtual ~Arma() = default;

    virtual void actualizar(float posX, float posY, QPointF mouseWorld) = 0;
    virtual void disparar() = 0;
    virtual void actualizarProyectiles() = 0;
    virtual QList<Proyectil*>& getProyectiles() = 0;

    void setAngulo(float angulo);
    float getAngulo() const;

    // Sistema de recarga
    void setMultiplicadorRecarga(float multiplicador);
    bool estaRecargando() const;
    int getMunicionActual() const;
    int getMunicionMaxima() const;
    float getProgresoRecarga() const;

    // NUEVOS: Sistema de mejoras
    void setMunicionMaxima(int maxima);
    void setMultiplicadorRango(float multiplicador);
    void setMultiplicadorRadioExplosion(float multiplicador);

    float getMultiplicadorRango() const { return multiplicadorRango; }
    float getMultiplicadorRadioExplosion() const { return multiplicadorRadioExplosion; }

protected:
    float pos_x;
    float pos_y;
    float angulo;
    float ancho;
    float alto;

    // Sistema de munición y recarga
    int municionActual;
    int municionMaxima;
    int municionMaximaBase;  // NUEVO: Para recordar el valor original
    float tiempoRecarga;
    float tiempoRecargaMax;
    float multiplicadorRecarga;
    bool recargando;

    // NUEVOS: Multiplicadores de mejoras
    float multiplicadorRango;
    float multiplicadorRadioExplosion;

    void iniciarRecarga();
    void actualizarRecarga();
};

#endif // ARMA_H
