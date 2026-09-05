#ifndef FAL_H
#define FAL_H

#include "arma.h"
#include "proyectil.h"
#include <QList>
#include <cmath>
#include "poolentidades.h"
class Fal : public Arma
{
public:
    Fal();
    ~Fal() = default;

    void dibujar(QPainter &painter) override;
    void actualizar(float posX, float posY, QPointF mouseWorld) override;
    void disparar() override;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    QList<Proyectil*>& getProyectiles();
    void actualizarProyectiles();

private:
    QList<Proyectil*> proyectiles;
    int cadenciaDisparo;
    int contadorCadencia;
    PoolEntidades<Proyectil> poolProyectiles;
};

#endif // FAL_H
