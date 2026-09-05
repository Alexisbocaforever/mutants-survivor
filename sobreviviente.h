#ifndef SOBREVIVIENTE_H
#define SOBREVIVIENTE_H

#include "dibujable_abstracto.h"
#include "arma.h"
#include "pistola.h"
#include "fal.h"
#include "vida.h"
#include <QPainter>
#include <QPointF>

#define SKIN_SURVIVOR 0
#define SKIN_MILITAR 1
#define SKIN_RUNNER 2
#define SKIN_Argentine 3
#define SKIN_Exterminador 4

// 1 direcciones para el sistema de 4 vistas, compartido por las 5 skins
#define SOBREVIVIENTE_DIR_ABAJO     0
#define SOBREVIVIENTE_DIR_ARRIBA    1
#define SOBREVIVIENTE_DIR_IZQUIERDA 2
#define SOBREVIVIENTE_DIR_DERECHA   3


          struct EstadisticasPersonaje {
    float velocidadMovimiento;     // Multiplicador de velocidad
    float velocidadRecarga;        // Multiplicador de recarga (menor = más rápido)
    float vidaMaxima;              // Vida máxima
    float resistenciaDanio;        // Reduce daño recibido (0.0 = sin reducción, 0.2 = 20% menos)
};

class Sobreviviente : public Dibujable_abstracto
{
public:
    Sobreviviente(float x = 0, float y = 0);
    ~Sobreviviente();

    void dibujar(QPainter &painter) override;

    void setPosicion(float x, float y);
    void actualizarAnimacion(bool seMueve, float dx, float dy);
    void actualizar(QPointF mouseWorld);
    void disparar();
    void cambiarArma(int tipoArma);

    void recibirDanio(float danio);
    bool estaVivo() const;
    Vida& getVida();

    // Sistema de skins y stats
    void setSkin(int nuevaSkin);
    int getSkin() const;
    EstadisticasPersonaje getStats() const;
    float getVelocidadMovimiento() const;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    Arma* getArma() const;
    void reiniciarArmas(); // NUEVO
private:
    float pos_x;
    float pos_y;
    float ancho = 30.0f;
    float alto = 40.0f;
    float frameAnimacion = 0.0f;
    float velocidadAnimacion = 0.05f;
    bool animacionSubiendo = true;
    int direccionMovimiento = SOBREVIVIENTE_DIR_ABAJO;
    float faseCaminata = 0.0f;

    int timerDanio; // <--- NUEVO: Temporizador para el destello rojo
    Arma* arma;
    Arma* armasPoseidas[8]; // NUEVO: una instancia persistente por tipo, para no perder las balas al cambiar
    int armaActual;
    Vida sistemaVida;
    int skinActual;
    EstadisticasPersonaje stats;

    void aplicarEstadisticas();

    void dibujarSurvivor(QPainter &painter);
    void dibujarSurvivorAbajo(QPainter &painter);
    void dibujarSurvivorArriba(QPainter &painter);
    void dibujarSurvivorIzquierda(QPainter &painter);
    void dibujarSurvivorDerecha(QPainter &painter);

    void dibujarMilitar(QPainter &painter);
    void dibujarMilitarAbajo(QPainter &painter);
    void dibujarMilitarArriba(QPainter &painter);
    void dibujarMilitarIzquierda(QPainter &painter);
    void dibujarMilitarDerecha(QPainter &painter);

    void dibujarRunner(QPainter &painter);
    void dibujarRunnerAbajo(QPainter &painter);
    void dibujarRunnerArriba(QPainter &painter);
    void dibujarRunnerIzquierda(QPainter &painter);
    void dibujarRunnerDerecha(QPainter &painter);

    void dibujarArgentine(QPainter &painter);
    void dibujarArgentineAbajo(QPainter &painter);
    void dibujarArgentineArriba(QPainter &painter);
    void dibujarArgentineIzquierda(QPainter &painter);
    void dibujarArgentineDerecha(QPainter &painter);

    void dibujarExterminador(QPainter &painter);
    void dibujarExterminadorAbajo(QPainter &painter);
    void dibujarExterminadorArriba(QPainter &painter);
    void dibujarExterminadorIzquierda(QPainter &painter);
    void dibujarExterminadorDerecha(QPainter &painter);
};

#endif // SOBREVIVIENTE_H
