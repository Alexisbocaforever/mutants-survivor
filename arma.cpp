#include "arma.h"

Arma::Arma() : pos_x(0), pos_y(0), angulo(0), ancho(0), alto(0),
    municionActual(0), municionMaxima(10), municionMaximaBase(10),
    tiempoRecarga(0.0f), tiempoRecargaMax(2.0f),
    multiplicadorRecarga(1.0f), recargando(false),
    multiplicadorRango(1.0f), multiplicadorRadioExplosion(1.0f)
{
}

void Arma::setAngulo(float angulo)
{
    this->angulo = angulo;
}

float Arma::getAngulo() const
{
    return angulo;
}

void Arma::setMultiplicadorRecarga(float multiplicador)
{
    multiplicadorRecarga = multiplicador;
}

void Arma::setMunicionMaxima(int maxima)
{
    int diferencia = maxima - municionMaxima;
    municionMaxima = maxima;
    municionActual += diferencia; // Agregar la diferencia a la munición actual
    if(municionActual > municionMaxima) municionActual = municionMaxima;
}

void Arma::setMultiplicadorRango(float multiplicador)
{
    multiplicadorRango = multiplicador;
}

void Arma::setMultiplicadorRadioExplosion(float multiplicador)
{
    multiplicadorRadioExplosion = multiplicador;
}

bool Arma::estaRecargando() const
{
    return recargando;
}

int Arma::getMunicionActual() const
{
    return municionActual;
}

int Arma::getMunicionMaxima() const
{
    return municionMaxima;
}

float Arma::getProgresoRecarga() const
{
    if (!recargando || tiempoRecargaMax <= 0) return 1.0f;
    return 1.0f - (tiempoRecarga / tiempoRecargaMax);
}

void Arma::iniciarRecarga()
{
    if (recargando) return;
    recargando = true;
    tiempoRecarga = tiempoRecargaMax * multiplicadorRecarga;
}

void Arma::actualizarRecarga()
{
    if (!recargando) return;

    tiempoRecarga -= 0.016f;

    if (tiempoRecarga <= 0) {
        tiempoRecarga = 0;
        recargando = false;
        municionActual = municionMaxima;
    }
}
#define _USE_MATH_DEFINES
