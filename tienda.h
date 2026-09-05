#ifndef TIENDA_H
#define TIENDA_H

#include "dibujable_abstracto.h"
#include <QPainter>
#include <QString>

enum TipoTienda {
    TIENDA_ARMAS,
    TIENDA_VIDA,
    TIENDA_CHICHES,
    TIENDA_EXPANSION  // NUEVO
};

class Tienda : public Dibujable_abstracto
{
public:
    Tienda(float x, float y, TipoTienda tipo);
    ~Tienda() = default;

    void dibujar(QPainter &painter) override;
    bool estaEnRango(float jugadorX, float jugadorY) const;

    float get_x() override { return pos_x; }
    float get_y() override { return pos_y; }
    float get_ancho() override { return ancho; }
    float get_alto() override { return alto; }

    TipoTienda getTipo() const { return tipo; }

private:
    float pos_x;
    float pos_y;
    float ancho;
    float alto;
    float rangoInteraccion;
    TipoTienda tipo;

    void dibujarTiendaArmas(QPainter &painter);
    void dibujarTiendaVida(QPainter &painter);
    void dibujarTiendaChiches(QPainter &painter);
    void dibujarTiendaExpansion(QPainter &painter);  // NUEVO
    void dibujarCartel(QPainter &painter, QString texto);
};

#endif // TIENDA_H
