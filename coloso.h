#ifndef COLOSO_H
#define COLOSO_H

#include "enemigos.h"
#include <QPainter>
#include <QColor>
#include <QList>

#define COLOSO_PERSIGUIENDO      0
#define COLOSO_PREPARANDO_CARGA  1
#define COLOSO_CARGANDO          2
#define COLOSO_PREPARANDO_GOLPE  3
#define COLOSO_GOLPEANDO         4
#define COLOSO_FATIGADO          5

#define COLOSO_CARGAS_POR_SECUENCIA 6

struct RastroAireColoso {
    float x, y, angulo, longitud, vida, vidaMax;
};

class Coloso : public Enemigo
{
public:
    Coloso(float x, float y);
    ~Coloso() = default;

    void actualizar() override;
    void actualizar(float jugadorX, float jugadorY);
    void actualizarConJugador(QPointF jugador) override;
    void dibujar(QPainter &p) override;

    bool consumirGolpeArea();
    float getRadioGolpe() const { return radioGolpe; }
    float getDanioGolpe() const { return danioGolpe; }
    float getCentroGolpeX() const { return pos_x + ancho / 2.0f; }
    float getCentroGolpeY() const { return pos_y + alto / 2.0f; }

private:
    int estadoActual;

    float timerEstado;
    float frameAnimacion;
    float anguloMovimiento;

    // Variables dinámicas para animar "todo"
    float animHombros;
    float animRespiracion;

    float velocidadNormal;
    float velocidadCarga;
    float danioNormal;
    float danioCarga;
    float danioGolpe;
    float radioGolpe;

    int cargasRestantes;
    float dirCargaX;
    float dirCargaY;
    bool golpeAplicado;
    float aperturaGarras;

    QList<RastroAireColoso> rastros;

    QColor colorPiel;
    QColor colorPielSecundario;
    QColor colorHueso;
    QColor colorOjos;
    QColor colorBrillo;
    QColor colorCarne;
    QColor colorManto;

    void dibujarAbajo(QPainter &p, float offset);
    void dibujarArriba(QPainter &p, float offset);
    void dibujarIzquierda(QPainter &p, float offset);
    void dibujarDerecha(QPainter &p, float offset);

    void dibujarBrazo(QPainter &p, float hombroX, float hombroY, float anguloHombro,
                      float anguloCodoBase, float largoSuperior, float largoInferior,
                      float grosor, QColor colorBase, float signoCurva);

    void dibujarParBrazos(QPainter &p, float hombroXMag, float hombroY,
                          float anguloHombroDerecha, float anguloCodoDerecha,
                          float largoSuperior, float largoInferior,
                          float grosor, QColor colorBase);

    void dibujarGarraCurva(QPainter &p, float grosorBase, float apertura, float signoCurva);
    void dibujarPata(QPainter &p, float baseX, float baseY, float altoPata, float anchoPata, QColor colorBase, float faseDesfase);
    void dibujarRastrosAire(QPainter &p);
    void actualizarRastrosAire();
    void iniciarGolpeArea();
    void dibujarDetallesEspalda(QPainter &p, float dirY);

    // Nueva función para inyectar detalles de pixel art HD
    void dibujarClustersPixeles(QPainter &p, float ancho, float alto, QColor color);

    // Rediseno tipo Ultralisco: cuerno curvo grande reutilizado en las 4 vistas
    void dibujarCuernoCurvo(QPainter &p, float baseX, float baseY, float signo, QColor colorBase, float escala = 1.0f);

    // Patrones de ataque: telegrafos visuales por estado (aro de advertencia, estela de embestida, circulo de impacto)
    void dibujarAuraAtaque(QPainter &p);
};

#endif // COLOSO_H
