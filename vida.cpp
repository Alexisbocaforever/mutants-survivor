#include "vida.h"
#include <algorithm>

Vida::Vida(float vidaMaxima)
    : vidaMaxima(vidaMaxima), vidaActual(vidaMaxima)
{
}

void Vida::recibirDanio(float cantidad)
{
    vidaActual -= cantidad;
    if (vidaActual < 0.0f) {
        vidaActual = 0.0f;
    }
}

void Vida::curar(float cantidad)
{
    vidaActual += cantidad;
    if (vidaActual > vidaMaxima) {
        vidaActual = vidaMaxima;
    }
}

void Vida::restaurarCompleta()
{
    vidaActual = vidaMaxima;
}

float Vida::getVidaActual() const
{
    return vidaActual;
}

float Vida::getVidaMaxima() const
{
    return vidaMaxima;
}

float Vida::getPorcentajeVida() const
{
    if (vidaMaxima <= 0) return 0.0f;
    return (vidaActual / vidaMaxima) * 100.0f;
}

bool Vida::estaVivo() const
{
    return vidaActual > 0.0f;
}

bool Vida::estaCritico() const
{
    return getPorcentajeVida() < 25.0f && estaVivo();
}

void Vida::setVidaMaxima(float nuevaVidaMaxima)
{
    vidaMaxima = nuevaVidaMaxima;
    if (vidaActual > vidaMaxima) {
        vidaActual = vidaMaxima;
    }
}
