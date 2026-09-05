#ifndef INVOCADOR_H
#define INVOCADOR_H

#include "enemigos.h"
#include <QPainter>
#include <vector>
#include "proyectilbuscador.h"

// 1 cada cuanto tiempo el invocador intenta invocar berserkers khorne
#define INVOCADOR_INTERVALO_INVOCACION_BERSERKER 20.0f
// 2 cuantos berserkers khorne aparecen por invocacion
#define INVOCADOR_CANT_BERSERKERS_POR_INVOCACION 2

class Invocador : public Enemigo
{
public:
    Invocador(float x, float y);
    ~Invocador() override = default;

    void dibujar(QPainter &painter) override;
    void actualizarConJugador(QPointF jugador) override;
    void actualizar() override;
    void actualizar(qreal xJugador, qreal yJugador);
    void dibujarProyectilesPropios(QPainter &painter) override;
    void moverHacia(QPointF objetivo);

    std::vector<ProyectilBuscador>& getProyectiles();

    // 3 devuelve true una sola vez cada 20 segundos, momento en el que el
    //   dueno de la lista de enemigos debe crear los berserkers y agregarlos
    //   a sus propias listas, el invocador no los crea ni los administra
    bool consumirDeseoDeInvocarBerserkers();

private:
    float anguloCuerpo;
    float frameAnimacion;        // respiración / flotación
    float velocidadAnimacion;
    bool animacionSubiendo;
    float cooldownDisparo;

    // animación de movimiento
    float frameMovimiento;       // oscila cuando se mueve (0.0 → 1.0)
    bool enMovimiento;           // indica si está caminando

    std::vector<ProyectilBuscador> proyectiles;

    // 4 cuenta regresiva propia para la invocacion de berserkers khorne
    float timerInvocacionBerserker;

    void dibujarArriba(QPainter &painter, float cx, float cy, int p, float fase);
    void dibujarAbajo(QPainter &painter, float cx, float cy, int p, float fase);
    void dibujarDerecha(QPainter &painter, float cx, float cy, int p, float fase);
    void dibujarIzquierda(QPainter &painter, float cx, float cy, int p, float fase);
};

#endif // INVOCADOR_H
