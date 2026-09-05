#ifndef PISTOLA_H
#define PISTOLA_H

#include "arma.h"
#include "poolentidades.h"
#include "proyectil.h"
#include <QList>
#include <cmath>

class Pistola : public Arma
{
public:
    Pistola();
    ~Pistola() = default;

    void dibujar(QPainter &painter) override;
    void actualizar(float posX, float posY, QPointF mouseWorld) override;
    void disparar() override;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    QList<Proyectil*>& getProyectiles();
    void actualizarProyectiles() override;

private:
    QList<Proyectil*> proyectiles;
    int cadenciaDisparo;
    int contadorCadencia;
    PoolEntidades<Proyectil> poolProyectiles;
};

#endif // PISTOLA_H
