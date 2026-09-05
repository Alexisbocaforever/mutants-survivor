#ifndef MECANICDEMON_H
#define MECANICDEMON_H

#include "enemigos.h"
#include "proyectilbuscador.h"
#include <QPainter>
#include <QPainterPath>
#include <vector>

// Estructura ligera para los proyectiles de disparo rápido (Plasma)
struct ProyectilPlasma {
    float x, y, velX, velY, angulo;
    bool activo = true;
    ProyectilPlasma(float _x, float _y, float _angulo) {
        x = _x; y = _y; angulo = _angulo;
        velX = 8.0f * std::cos(angulo * M_PI / 180.0f);
        velY = 8.0f * std::sin(angulo * M_PI / 180.0f);
    }
};

class MecanicDemon : public Enemigo {
public:
    MecanicDemon(float x, float y);
    ~MecanicDemon();
    void actualizarConJugador(QPointF jugador) override;
    void actualizar() override;
    void dibujar(QPainter &p) override;
    void actualizar(float jugadorX, float jugadorY);
private:
    enum Estado { CAMINANDO, DISPARANDO_PLASMA, LANZANDO_BUSCADORES };
    Estado estadoActual;

    // Vectores de munición
    std::vector<ProyectilBuscador*> misilesBuscadores;
    std::vector<ProyectilPlasma> disparosPlasma;

    // Dinámica y temporizadores
    float anguloMirada;
    float frameAnimacion;
    float timerRafagaPlasma;
    float timerBuscador;
    int contadorRafaga;

    // Paleta de colores (Basada en image_3054b9.jpg)
    QColor colorArmadura;
    QColor colorDorado;
    QColor colorCarne;
    QColor colorPlasma;
    QColor colorMetalOculto;

    // Métodos direccionales detallados
    void dibujarFrente(QPainter &p, float offsetCaminar);
    void dibujarEspalda(QPainter &p, float offsetCaminar);
    void dibujarIzquierda(QPainter &p, float offsetCaminar);
    void dibujarDerecha(QPainter &p, float offsetCaminar);

    // Partes biomecánicas reutilizables
    void dibujarCanon(QPainter &p, bool disparando, float retroceso);
    void dibujarPierna(QPainter &p, float anguloPierna, bool esTrasera);
    void dibujarGlowPlasma(QPainter &p, float x, float y, float radio);
};

#endif // MECANICDEMON_H
