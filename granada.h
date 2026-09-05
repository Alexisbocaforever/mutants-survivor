#ifndef GRANADA_H
#define GRANADA_H

#include "arma.h"
#include "proyectil.h"
#include <QList>
#include <cmath>

class Granada : public Arma
{
public:
    Granada();
    ~Granada() = default;

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
};

#endif // GRANADA_H
