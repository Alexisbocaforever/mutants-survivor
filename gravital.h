// gravital.h
#ifndef GRAVITAL_H
#define GRAVITAL_H

#include "enemigos.h"
#include "proyectil.h"
#include "rayolaser.h"
#include "proyectilcurvo.h"
#include <QPainter>
#include <QList>

// 1 fases de combate del gravital, cada una arranca cuando pierde otro 25 por ciento de vida
#define GRAVITAL_FASE_1_ESPIRAL 0
#define GRAVITAL_FASE_2_CRUZ    1
#define GRAVITAL_FASE_3_Y       2
#define GRAVITAL_FASE_4_LOCURA  3

// 2 direccion hacia donde mira el ojo y lado del cuerpo que queda vulnerable
#define GRAVITAL_DIR_FRENTE    0
#define GRAVITAL_DIR_IZQUIERDA 1
#define GRAVITAL_DIR_DERECHA   2
#define GRAVITAL_DIR_ESPALDA   3

// 3 cantidad fija de gajos en que se parte la esfera, como una naranja
#define GRAVITAL_CANT_GAJOS 4

// 4 grilla logica de pixeles que arma cada gajo, ver dibujarGajoPixelArt
#define GRAVITAL_GAJO_FILAS    10
#define GRAVITAL_GAJO_COLUMNAS 10

class Gravital : public Enemigo
{
public:
    Gravital(float x, float y);
    ~Gravital();

    void dibujar(QPainter &painter) override;
    void actualizar() override;

    QList<Proyectil*>& getProyectiles();
    QList<RayoLaser*>& getRayosLaser() { return rayosLaser; }
    QList<ProyectilCurvo*>& getProyectilesCurvos() { return proyectilesCurvos; }

    bool impactoEsVulnerable(float proyectilX, float proyectilY);

private:
    QList<Proyectil*> proyectiles;
    QList<RayoLaser*> rayosLaser;
    QList<ProyectilCurvo*> proyectilesCurvos;
    float timerCurvo;

    // 5 fase de escudo total, ya existia y se mantiene igual
    bool enEscudoTotal;
    float timerEscudoTotal;
    float timerParaEscudo;

    int faseActual;
    int direccionActual;

    // 6 cuantos cuartos de vida ya perdio, va de 0 sano a 3 furioso
    int cuartosPerdidos;
    // 7 apertura actual de los gajos, de 0.0 cerrado a 1.0 totalmente abierto como una naranja pelada
    float aperturaGajos;
    // 8 velocidad de interpolacion de la apertura para que el quiebre se vea progresivo y no salte de golpe
    float velocidadApertura;

    float anguloEspiral;
    int timerDisparo;
    int cadenciaActual;

    float frameAnimacion;
    float flotacionY;

    void evaluarFase();
    void ejecutarAtaque();
    void dispararEspiral();
    void dispararCruz();
    void dispararY();
    void dispararCurvo();

    void dibujarEsferaMetalica(QPainter &painter);
    void dibujarGajoPixelArt(QPainter &painter, float centroX, float centroY, int indiceGajo, float escalaPixel);
    void dibujarNucleoEnergia(QPainter &painter, float centroX, float centroY);
    void dibujarOjo(QPainter &painter);
    void dibujarEscudoInvulnerabilidad(QPainter &painter);
};

#endif // GRAVITAL_H
