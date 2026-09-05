// zombietanque.h
#ifndef ZOMBIETANQUE_H
#define ZOMBIETANQUE_H

#include "enemigos.h"
#include "proyectilbuscador.h"
#include "rayolaser.h" // NUEVO

struct ParticulaLlamaTanque {
    float x, y, vx, vy, vida, vidaMax, tam;
};

class ZombieTanque : public Enemigo {
public:
    ZombieTanque(float x, float y);
    ~ZombieTanque();

    void actualizar() override;
    void actualizar(float jugadorX, float jugadorY);
    void dibujar(QPainter &p) override;

    std::vector<ProyectilBuscador*>& getMisiles() { return misiles; }

    bool estaLanzandoLlamas() const { return lanzandoLlamas; }
    QList<QPointF> getAreaLlamarada() const;
    float getDanioLlamarada() const { return 6.0f; }

    const RayoLaser& getRayoLaser() const { return rayoLaser; } // NUEVO
    void actualizarConJugador(QPointF jugador) override;
private:
    float anguloMovimiento;
    float frameAnimacion;
    float timerDisparo;

    float timerLanzallamasTanque;
    float duracionLlamarada;
    bool lanzandoLlamas;
    float anguloLlamarada;
    QList<ParticulaLlamaTanque> particulasLlama;

    float timerRayoLaser; // NUEVO
    RayoLaser rayoLaser;  // NUEVO

    std::vector<ProyectilBuscador*> misiles;

    QColor colorMetal;
    QColor colorMetalOxidado;
    QColor colorCerebro;
    QColor colorSangre;
    QColor colorOrugas;
    QColor colorCarneMuerta;

    void dibujarIzquierda(QPainter &p, float offsetOrugas);
    void dibujarDerecha(QPainter &p, float offsetOrugas);
    void dibujarArriba(QPainter &p, float offsetOrugas);
    void dibujarAbajo(QPainter &p, float offsetOrugas);

    void dibujarCerebro(QPainter &p, float escalaPalpito, bool enFuria);
    void dibujarCanon(QPainter &p, float angulo, float largo);
    void dibujarDetallesSangre(QPainter &p);
    void dibujarOrugasLaterales(QPainter &p, float offsetOrugas);
    void dibujarOrugasFrontales(QPainter &p, float offsetOrugas);
    void dibujarFranjasPeligro(QPainter &p, float x, float y, float ancho, float alto);

    // 1 relleno solido pixelado que se usa recortado dentro de un QPainterPath
    //   o QRect para dar la textura de chapa pixel art a todas las piezas del tanque
    void dibujarBloquePixelado(QPainter &p, float x, float y, float ancho, float alto,
                               QColor colorBase, float pixel);

    void dispararLlamarada(float jugadorX, float jugadorY);
    void actualizarLlamarada();
    void dibujarLlamarada(QPainter &p);

    float velocidadAnimacion;
    bool animacionSubiendo;


    int tipoSkin;

    void obtenerColoresSkin(QColor &colorPiel, QColor &colorRopa, QColor &colorDetalle, QColor &colorOjos); // NUEVO
    void dibujarZombiePalaAbajo(QPainter &p);     // NUEVO
    void dibujarZombiePalaArriba(QPainter &p);    // NUEVO
    void dibujarZombiePalaIzquierda(QPainter &p); // NUEVO
    void dibujarZombiePalaDerecha(QPainter &p);   // NUEVO
};

#endif // ZOMBIETANQUE_H
