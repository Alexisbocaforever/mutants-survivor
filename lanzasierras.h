// lanzasierras.h
#ifndef LANZASIERRAS_H
#define LANZASIERRAS_H

#include "arma.h"
#include "sierravoladora.h"
#include <QList>

class LanzaSierras : public Arma
{
public:
    LanzaSierras();
    ~LanzaSierras();

    void dibujar(QPainter &painter) override;
    void actualizar(float posX, float posY, QPointF mouseWorld) override;
    void disparar() override;
    void actualizarProyectiles() override;
    QList<Proyectil*>& getProyectiles() override; // vacia, la interfaz la pide igual

    float get_x() override { return pos_x; }
    float get_y() override { return pos_y; }
    float get_ancho() override { return ancho; }
    float get_alto() override { return alto; }

    QList<SierraVoladora*>& getSierras() { return sierras; }

    void setDanio(float d) { danio = d; }
    void setNivelCadenciaSierra(int n) { nivelCadenciaSierra = n; cadenciaDisparo = std::max(20, 55 - n * 8); }
    void setNivelTamanioSierra(int n) { nivelTamanioSierra = n; radioSierra = 14.0f + n * 3.0f; }
    int getNivelCadenciaSierra() const { return nivelCadenciaSierra; }
    int getNivelTamanioSierra() const { return nivelTamanioSierra; }

private:
    QList<Proyectil*> proyectiles;
    QList<SierraVoladora*> sierras;
    float anguloCargada;
    int cadenciaDisparo;
    int contadorCadencia;
    float danio;
    float radioSierra;
    int nivelCadenciaSierra;
    int nivelTamanioSierra;
};

#endif // LANZASIERRAS_H
