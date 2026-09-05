#ifndef XHAAL_H
#define XHAAL_H

#include "enemigos.h"
#include "proyectil.h"
#include "proyectilbuscador.h"
#include <QPainter>
#include <QPointF>
#include <QList>
#include <QString>

struct TextoFlotante {
    QString texto;
    float posX;
    float posY;
    float tiempoVida;
    int alpha;
};

// Particula usada para el efecto visual del teletransporte
struct ParticulaTeleport {
    float x;
    float y;
    float vx;
    float vy;
    float vida;
    float vidaMax;
    float tam;
};

class Xhaal : public Enemigo
{
public:
    Xhaal(float x, float y);
    ~Xhaal();

    void dibujar(QPainter &painter) override;
    void actualizar() override;
    void moverHacia(QPointF objetivo);
    void dispararHacia(QPointF objetivo, QPointF velocidadJugador);
    void teletransportarse(QPointF jugador);

    QList<Proyectil*>& getProyectiles();
    QList<ProyectilBuscador*>& getProyectilesBuscadores();
    void actualizarProyectiles(QPointF posJugador);
    void dibujarProyectilesPropios(QPainter &painter) override;
private:
    // Sistema de combate
    float anguloArma;
    int cadenciaDisparo;
    int contadorCadencia;
    float rangoDeteccion;
    float rangoDisparo;

    QList<Proyectil*> proyectiles;
    QList<ProyectilBuscador*> proyectilesBuscadores;

    // Sistema de teletransporte
    float timerTeletransporte;
    float cooldownTeletransporte;
    bool puedeTransportarse;
    float timerEfectoTeletransporte; // Duracion del resplandor tras teletransportarse

    // Animacion
    float frameAnimacion;
    float velocidadAnimacion;
    bool animacionSubiendo;

    // Animacion de brazos (cada brazo oscila con fase y velocidad propia)
    float anguloBrazos[6];
    float velocidadBrazos[6];

    // Animacion de ojos
    float parpadeo;
    bool pestañasCerradas;

    // Mirada hacia el objetivo (para que los ojos sigan al jugador)
    float objetivoX;
    float objetivoY;
    bool tieneObjetivo;

    // Pulso del nucleo/torso, se acelera cuando la vida es baja
    float pulsoNucleo;

    // Particulas del teletransporte
    QList<ParticulaTeleport*> particulasTeleport;

    // Textos flotantes
    QList<TextoFlotante*> textosFlotantes;
    float timerTextos;
    int indiceTextoActual;
    QStringList mensajes;

    // Sistema de ataques especiales
    int tipoAtaqueActual;
    float timerCambioAtaque;

    void crearProyectil(QPointF objetivo);
    void crearProyectilesBuscadores(QPointF objetivo);
    void crearProyectilPredictivo(QPointF objetivo, QPointF velocidadJugador);
    void crearProyectilesCruz(); // NUEVO: Ataque en cruz al teletransportarse
    void generarTextoAleatorio();
    void actualizarTextos();
    void dibujarTextos(QPainter &painter);

    void crearParticulasTeleport(float x, float y);
    void actualizarParticulasTeleport();
    void dibujarParticulasTeleport(QPainter &painter);
    void dibujarAuraFuria(QPainter &painter);
};

#endif // XHAAL_H
