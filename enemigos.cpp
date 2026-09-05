#include "enemigos.h"

Enemigo::Enemigo()
    : pos_x(0.0f), pos_y(0.0f), ancho(30.0f), alto(30.0f),
    sistemaVida(100.0f), velocidad(1.0f), vivo(true), danioContacto(10.0f),
    tiempoDesaparecer(3.0f), tiempoQuemandose(0.0f)
{
}

Enemigo::Enemigo(float x, float y)
    : pos_x(x), pos_y(y), ancho(30.0f), alto(30.0f),
    sistemaVida(100.0f), velocidad(1.0f), vivo(true), danioContacto(10.0f),
    tiempoDesaparecer(3.0f), tiempoQuemandose(0.0f)
{
}

void Enemigo::mover(float dx, float dy)
{
    pos_x += dx;
    pos_y += dy;
}

float Enemigo::get_x()
{
    return pos_x;
}

float Enemigo::get_y()
{
    return pos_y;
}

float Enemigo::get_ancho()
{
    return ancho;
}

float Enemigo::get_alto()
{
    return alto;
}

bool Enemigo::estaVivo() const
{
    return vivo && sistemaVida.estaVivo();
}

void Enemigo::recibirDanio(float danio)
{
    sistemaVida.recibirDanio(danio);
    if (!sistemaVida.estaVivo()) {
        vivo = false;
    }
}

Vida& Enemigo::getVida()
{
    return sistemaVida;
}

float Enemigo::getDanioContacto() const
{
    return danioContacto;
}

void Enemigo::multiplicarVelocidad(float multiplicador)
{
    velocidad *= multiplicador;
}
void Enemigo::multiplicarDanio(float multiplicador)
{
    danioContacto *= multiplicador;
}

void Enemigo::multiplicarVida(float multiplicador) {
    float vidaBase = sistemaVida.getVidaMaxima();
    sistemaVida.setVidaMaxima(vidaBase * multiplicador);
    sistemaVida.restaurarCompleta();
}

bool Enemigo::esCadaver() const {
    return !vivo; // Si "vivo" es false, es un cadáver visible
}

bool Enemigo::debeDesaparecer() const {
    return !vivo && tiempoDesaparecer <= 0;
}
