#ifndef EMBISTIDOR_H
#define EMBISTIDOR_H

#include "enemigos.h"
#include <QPainter>
#include <QColor>

class Embistidor : public Enemigo {
public:
    // Enum para las distintas paletas de colores (Skins)
    enum Skin {
        TOXICO,    // Basado en image_3287f2.jpg (Azul, cristales magenta, baba verde)
        MAGMA,     // Piel de obsidiana, cristales de fuego
        ABISAL     // Piel oscura, bioluminiscencia cian intensa
    };

    Embistidor(float x, float y, Skin skinElegida = TOXICO);
    ~Embistidor() = default;

    void actualizar() override;
    void actualizar(float jugadorX, float jugadorY);
    void dibujar(QPainter &p) override;
    void actualizarConJugador(QPointF jugador) override;
private:
    // Máquina de estados
    enum Estado { PERSIGUIENDO, PREPARANDO, CARGANDO, FATIGADO };
    Estado estadoActual;

    Skin miSkin;

    // Estadísticas dinámicas
    float danioNormal;
    float danioCarga;
    float velocidadNormal;
    float velocidadCarga;

    // Temporizadores y control de movimiento
    float timerEstado;
    float frameAnimacion;
    float anguloMovimiento;

    // Vector de la embestida (para ir en línea recta)
    float dirCargaX;
    float dirCargaY;

    // Colores del Skin
    QColor colorPiel;
    QColor colorPielSecundario;
    QColor colorCristal;
    QColor colorBrillo;
    QColor colorOjos;

    void configurarSkin();

    // Métodos de dibujo (4 direcciones)
    void dibujarIzquierda(QPainter &p, float offsetCaminar);
    void dibujarDerecha(QPainter &p, float offsetCaminar);
    void dibujarAbajo(QPainter &p, float offsetCaminar);   // antes dibujarFrente
    void dibujarArriba(QPainter &p, float offsetCaminar);  // antes dibujarEspalda

    // Partes reutilizables del dibujo
    void dibujarCristales(QPainter &p, bool lateral);
    void dibujarBaba(QPainter &p);
};

#endif // EMBISTIDOR_H
