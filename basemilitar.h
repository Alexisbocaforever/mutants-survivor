#ifndef BASEMILITAR_H
#define BASEMILITAR_H

#include "escenario.h"
#include <QPainter>
#include <QVector> // Necesario para guardar la decoración

struct ElementoDecorativoMapa {
    float x;
    float y;
    float tam;
    float angulo;
    int tipo;
};

// NUEVO: parche de nieve acumulada en el mapa
struct ParcheNieve {
    float x;
    float y;
    float tam;
    int umbralNivel;
    int tono; // variacion de blanco, para textura
};

class BaseMilitar : public Escenario
{
public:
    BaseMilitar();
    ~BaseMilitar() = default;

    void dibujar(QPainter &p) override;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    QPointF getPuntoSpawnInterno() const;
    QPointF getPuntoSpawnExterno() const;

    // Funciones para expansión
    void expandirDerecha(float cantidad);
    void expandirIzquierda(float cantidad);
    void expandirArriba(float cantidad);
    void expandirAbajo(float cantidad);

    float getMargenExterno() const { return margenExterno; }
    void establecerNivelNieve(int nivel); // NUEVO

private:
    void crearValla();

    // TODAS estas funciones están implementadas en tu .cpp pero faltaban aquí
    void generarDecoracionMapa();
    void dibujarCaminoTierra(QPainter &p);
    void dibujarTerrenoExterior(QPainter &p);
    void dibujarSueloInterior(QPainter &p);
    void dibujarDecoracionInterior(QPainter &p);
    void dibujarVinieta(QPainter &p);
    void dibujarDetallesAdicionales(QPainter &p); // NUEVO
    void dibujarPerros(QPainter &p);              // NUEVO
    void dibujarPerro(QPainter &p, float x, float y, float escala, float angulo); // NUEVO

    void generarParchesNieve();          // NUEVO
    void dibujarNieveAcumulada(QPainter &p); // NUEVO
    // Helpers estilo pixel art
    void dibujarBloquePixel(QPainter &p, float x, float y, float tam, const QColor &color);
    void dibujarCirculoPixelado(QPainter &p, float cx, float cy, float radio, float tamPixel, const QColor &color);
    float redondearAPixel(float valor, float tamPixel);

    float margenExterno;

    // Lista para almacenar la decoración generada
    QVector<ElementoDecorativoMapa> decoracionMapa;
    QVector<ParcheNieve> parchesNieve;
    QVector<QPointF> moteadoNieveFinal; // NUEVO: textura fija del nivel 10

    int nivelNieve;                    // NUEVO
};

#endif // BASEMILITAR_H
