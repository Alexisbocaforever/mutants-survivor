#ifndef PARED_H
#define PARED_H

#include "dibujable_abstracto.h"
#include <QPainter>

class Pared : public Dibujable_abstracto
{
public:
    Pared();
    Pared(float x, float y, float ancho, float alto, int tipo);

    // Implementación de métodos virtuales
    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;
    void dibujar(QPainter &p) override;

    // Variables miembros
    float x_pared;
    float y_pared;
    float ancho_pared;
    float alto_pared;
    float vida;
    int tipo_pared; // 0 = hormigón, 1 = valla

private:
    // Métodos privados que faltaban declarar
    void dibujarMuroBase(QPainter &p);
    void dibujarVallaLimite(QPainter &p);
    void dibujarDanio(QPainter &p);
};

#endif // PARED_H
