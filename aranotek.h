#ifndef ARANOTEK_H
#define ARANOTEK_H

#include "enemigos.h"
#include "proyectil.h"
#include <QPainter>
#include <QPointF>
#include <QList>

class AranoTek : public Enemigo
{
public:
    AranoTek(float x, float y);
    ~AranoTek();
void actualizarConJugador(QPointF jugador) override;
    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
    void dispararHacia(QPointF objetivo);

    QList<Proyectil*>& getProyectiles();
    void actualizarProyectiles();

private:
    float anguloCohete;
    float anguloAmetralladora;
    float anguloCuerpo; // Controla hacia dónde mira el torso de orco

    int cadenciaCohete;
    int contadorCadenciaCohete;
    int cadenciaEntreBalas;
    int cadenciaEntreRafagas;
    int contadorCadenciaAmetralladora;
    int cantidadRafaga;
    int balasRestantesRafaga;

    float rangoDeteccion;
    float rangoDisparo;

    QList<Proyectil*> proyectiles;

    float cicloPatas;
    float velocidadCicloPatas;
    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;
    float destelloCohete;
    float destelloAmetralladora;

    void dibujarPatas(QPainter &painter);
    void dibujarCuerpo(QPainter &painter, int direccion);
    void dibujarBrazoLanzacohetes(QPainter &painter, int direccion);
    void dibujarBrazoAmetralladora(QPainter &painter, int direccion);

    void dispararCohete(QPointF objetivo);
    void dispararBala(QPointF objetivo);
};

#endif // ARANOTEK_H
