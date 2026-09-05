#ifndef ESCOPETA_H
#define ESCOPETA_H

#include "arma.h"
#include "poolentidades.h"
#include "proyectil.h"
#include <QList>
#include <cmath>
#include <cstdlib> // Para rand()
#include <algorithm>
class Escopeta : public Arma
{
public:
    Escopeta();
    ~Escopeta() = default;

    void dibujar(QPainter &painter) override;
    void actualizar(float posX, float posY, QPointF mouseWorld) override;
    void disparar() override;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    QList<Proyectil*>& getProyectiles();
    void actualizarProyectiles() override;
    void setNivelCadenciaEscopeta(int n) { nivelCadenciaEscopeta = n; cadenciaDisparo = std::max(20, 45 - n * 6); }
    int getNivelCadenciaEscopeta() const { return nivelCadenciaEscopeta; }

private:
    QList<Proyectil*> proyectiles;
    int cadenciaDisparo;
    int contadorCadencia;
    int perdigonesPorDisparo;
    PoolEntidades<Proyectil> poolProyectiles;
    int nivelCadenciaEscopeta;
};

#endif // ESCOPETA_H
