#ifndef LANZALLAMAS_H
#define LANZALLAMAS_H

#include "arma.h"
#include "proyectil.h"
#include <QList>
#include <QPainter>
#include <cmath>

// Estructura para las partículas de fuego
struct ParticulaFuego {
    float x;
    float y;
    float vel_x;
    float vel_y;
    float tiempoVida;
    float tiempoVidaMax;
    float tamano;
    float angulo;

    ParticulaFuego(float px, float py, float vx, float vy, float vida, float tam, float ang)
        : x(px), y(py), vel_x(vx), vel_y(vy),
        tiempoVida(vida), tiempoVidaMax(vida), tamano(tam), angulo(ang) {}
};

class Lanzallamas : public Arma
{
public:
    Lanzallamas();
    ~Lanzallamas();

    void dibujar(QPainter &painter) override;
    void actualizar(float posX, float posY, QPointF mouseWorld) override;
    void disparar() override;
    void actualizarProyectiles() override;
    QList<Proyectil*>& getProyectiles() override;

    float get_x() override;
    float get_y() override;
    float get_ancho() override;
    float get_alto() override;

    // Método para obtener enemigos en área de efecto
    QList<QPointF> getAreaEfecto() const;

    bool estaDisparando() const { return disparando; }
    float getDanioPorTick() const { return danioTick; }
    // lanzallamas.h, agregar en la parte publica
    void setDanioTick(float danio) { danioTick = danio; }
    void setAlcanceMaximo(float alcance) { alcanceMaximo = alcance; }
    int getNivelDanioLanzallamas() const { return nivelDanioLanzallamas; }
    int getNivelAlcanceLanzallamas() const { return nivelAlcanceLanzallamas; }

    void setNivelDanioLanzallamas(int nivel) { nivelDanioLanzallamas = nivel; }     // NUEVO
    void setNivelAlcanceLanzallamas(int nivel) { nivelAlcanceLanzallamas = nivel; } // NUEVO
private:
    QList<Proyectil*> proyectiles; // No usado, pero necesario por la interfaz
    QList<ParticulaFuego*> particulas;

    bool disparando;
    float anguloDisparo;
    float alcanceMaximo;
    float anchoConoInicial;
    float anchoConoFinal;
    float danioTick;
    float tiempoEntreParticulas;
    float contadorParticulas;

    void crearParticulasFuego(float origenX, float origenY);
    void actualizarParticulas();
    void dibujarLlamarada(QPainter &painter);
    int nivelDanioLanzallamas;
    int nivelAlcanceLanzallamas;
};

#endif // LANZALLAMAS_H
