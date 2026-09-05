#ifndef ZOMBIEARMADO_H
#define ZOMBIEARMADO_H

#include "enemigos.h"
#include "proyectil.h"
#include <QPainter>
#include <QPointF>
#include <QList>

class ZombieArmado : public Enemigo
{
public:
    ZombieArmado(float x, float y, int tipoArma);
    ~ZombieArmado();

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
    void dispararHacia(QPointF objetivo);
    void dibujarProyectilesPropios(QPainter &painter) override;
    QList<Proyectil*>& getProyectiles();
    void actualizarProyectiles();
    void actualizarConJugador(QPointF jugador) override;


private:
    int tipoArma; // 1=Pistola, 2=Granadas, 3=Lanzacohetes
    int tipoSkin; // Variante de skin (0..3)

    float anguloMovimiento;
    float anguloArma;

    int cadenciaDisparo;
    int contadorCadencia;
    float rangoDeteccion;
    float rangoDisparo;

    QList<Proyectil*> proyectiles;

    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;

    void obtenerColoresSkin(QColor &colorPiel, QColor &colorChaleco, QColor &colorCasco, QColor &colorOjos); // NUEVO
    void dibujarZombieAbajo(QPainter &painter);    // NUEVO, antes era dibujarZombie
    void dibujarZombieArriba(QPainter &painter);   // NUEVO
    void dibujarZombieIzquierda(QPainter &painter); // NUEVO
    void dibujarZombieDerecha(QPainter &painter);   // NUEVO
    void dibujarArma(QPainter &painter);
    void crearProyectil(QPointF objetivo);
};

#endif // ZOMBIEARMADO_H
