#ifndef EXPLOSIVO_H
#define EXPLOSIVO_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <QVector>

// Tipo de fragmento generado por la explosion
// 0 = chispa, 1 = humo, 2 = escombro
// Se usa int en lugar de un enum para mantener el estilo del resto del proyecto

struct FragmentoExplosion {
    float angulo;
    float velocidad;
    float distanciaActual;
    float tam;
    int tipo;
    float rotacionActual;
    float velocidadRotacion;
};

class Explosivo : public Dibujable_abstracto
{
public:
    // El radio definirá qué tan grande se dibuja la explosión
    Explosivo(float x, float y, float radio);
    ~Explosivo() = default;

    void dibujar(QPainter &painter) override;
    void actualizar();

    float get_x() override { return pos_x; }
    float get_y() override { return pos_y; }
    float get_ancho() override { return radio * 2; }
    float get_alto() override { return radio * 2; }

    // Para saber cuándo eliminar el dibujo
    bool terminada() const { return tiempoVida <= 0; }

private:
    float pos_x;
    float pos_y;
    float radio;
    float tiempoVida; // Contador regresivo
    float tiempoMax;  // Duración total del efecto completo

    QVector<FragmentoExplosion> fragmentos;

    void generarFragmentos();
    void dibujarFlashInicial(QPainter &painter, float progreso);
    void dibujarOndaDeChoque(QPainter &painter, float progreso);
    void dibujarBolaDeFuego(QPainter &painter, float progreso);
    void dibujarFragmentos(QPainter &painter, float progreso);
};

#endif // EXPLOSIVO_H
