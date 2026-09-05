#ifndef CARNICERO_H
#define CARNICERO_H

#include "enemigos.h"
#include <QPainter>

class carnicero : public Enemigo {
public:
    carnicero(float x, float y, int variante = 0);

    void actualizar() override;
    void dibujar(QPainter &p) override;
    void actualizar(float jugadorX, float jugadorY);
    void actualizarConJugador(QPointF jugador) override;
private:
    int varianteDiseno;
    bool enfurecido;

    // Variables de dinámica y animación
    float frameAnimacion;
    float anguloMovimiento;
    bool atacando;
    float timerAtaque;

    // Colores base de la armadura
    QColor colorArmadura;
    QColor colorBorde;
    QColor colorOjos;
    QColor colorRunas;

    // Inicialización
    void inicializarColores();

    // Métodos direccionales
    void dibujarFrente(QPainter &p, float offsetPiernas, float offsetArmas);
    void dibujarEspalda(QPainter &p, float offsetPiernas, float offsetArmas);
    void dibujarIzquierda(QPainter &p, float offsetPiernas, float offsetArmas);
    void dibujarDerecha(QPainter &p, float offsetPiernas, float offsetArmas);

    // Auxiliar para armas de W40k
    void dibujarArma(QPainter &p, bool esManoIzquierda);
};

#endif // CARNICERO_H
