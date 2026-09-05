// rayolaser.h
#ifndef RAYOLASER_H
#define RAYOLASER_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <cmath>
#include <algorithm>

class RayoLaser : public Dibujable_abstracto
{
public:
    RayoLaser();

    // 1 rangoOscilacion en 0 mantiene el rayo fijo, igual que antes
    // 2 rangoOscilacion mayor a 0 hace que el rayo barra entre anguloBase menos
    //   ese rango y anguloBase mas ese rango, cambiando de sentido al azar
    //   cada vez que llega a alguno de los dos extremos
    void reiniciar(float origenX, float origenY, float angulo, float longitud, float danioPorTick,
                   float tiempoAviso = 0.4f, float tiempoActivo = 0.5f, float grosor = 18.0f,
                   float rangoOscilacion = 0.0f, float velocidadOscilacion = 0.0f);

    void dibujar(QPainter &painter) override;
    void actualizar();

    float get_x() override { return origenX; }
    float get_y() override { return origenY; }
    float get_ancho() override { return longitud; }
    float get_alto() override { return grosor; }

    bool estaActivo() const { return activo; }
    void desactivar() { activo = false; }
    bool estaHaciendoDanio() const { return activo && !avisando; }
    float getDanio() const { return danioPorTick; }

    // Distancia mas corta entre el punto (px,py) y el segmento del rayo
    bool colisionaCon(float px, float py, float radio) const;

private:
    float origenX;
    float origenY;
    float angulo;
    float longitud;
    float grosor;
    float danioPorTick;

    bool activo;
    bool avisando;
    float timerAviso;
    float timerActivo;

    // 3 datos del barrido oscilante, se usan solo si rangoOscilacion es mayor a 0
    float anguloBase;
    float rangoOscilacion;
    float velocidadOscilacion;
    int sentidoOscilacion;
    // 4 velocidad real de este instante, se acerca de a poco a velocidadOscilacion
    // para lograr un movimiento tipo pendulo, frenando cerca de los extremos y
    // arrancando despacio otra vez despues de cada rebote
    float velocidadActual;
};

#endif // RAYOLASER_H
