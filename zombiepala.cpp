#include "zombiepala.h"
#include <cmath>
#include <cstdlib>

ZombiePala::ZombiePala(float x, float y)
    : Enemigo(x, y), frameAnimacion(0.0f),
    velocidadAnimacion(0.1f), animacionSubiendo(true), anguloMovimiento(0)
{
    ancho = 35.0f;
    alto = 35.0f;
    velocidad = 2.0f;
    sistemaVida = Vida(80.0f);
    danioContacto = 12.0f;

    // Selección aleatoria de skin (0 a 3)
    tipoSkin = std::rand() % 4;
}
void ZombiePala::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }
void ZombiePala::actualizar() {
    if (!vivo) {
        tiempoDesaparecer -= 0.016f;
        return;
    }
    if (animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
        if (frameAnimacion > 3.0f) animacionSubiendo = false;
    } else {
        frameAnimacion -= velocidadAnimacion;
        if (frameAnimacion < -3.0f) animacionSubiendo = true;
    }
}
void ZombiePala::obtenerColoresSkin(QColor &colorPiel, QColor &colorRopa, QColor &colorDetalle, QColor &colorOjos)
{
    switch(tipoSkin) {
    case 0:
        colorPiel = QColor(70, 110, 70);
        colorRopa = QColor(220, 110, 20);
        colorDetalle = QColor(240, 210, 40);
        colorOjos = QColor(220, 30, 30);
        break;
    case 1:
        colorPiel = QColor(90, 100, 95);
        colorRopa = QColor(40, 40, 50);
        colorDetalle = QColor(180, 20, 30);
        colorOjos = QColor(255, 200, 50);
        break;
    case 2:
        colorPiel = QColor(60, 100, 60);
        colorRopa = QColor(40, 70, 120);
        colorDetalle = QColor(120, 80, 40);
        colorOjos = QColor(200, 40, 40);
        break;
    case 3:
    default:
        colorPiel = QColor(80, 200, 90);
        colorRopa = QColor(30, 40, 35);
        colorDetalle = QColor(140, 255, 50);
        colorOjos = QColor(255, 255, 100);
        break;
    }
}
void ZombiePala::moverHacia(QPointF objetivo) {
    float dx = objetivo.x() - (pos_x + ancho / 2.0f);
    float dy = objetivo.y() - (pos_y + alto / 2.0f);
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist > 5.0f) {
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        mover((dx/dist)*velocidad, (dy/dist)*velocidad);
    }
}

void ZombiePala::dibujar(QPainter &p) {
    if (!estaVivo()) return;

    // 1 se elige una de 4 poses fijas segun hacia donde se mueve,
    //   ya no rota el cuerpo entero como antes
    if (anguloMovimiento >= -45 && anguloMovimiento < 45) {
        dibujarZombiePalaDerecha(p);
    } else if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        dibujarZombiePalaAbajo(p);
    } else if (anguloMovimiento >= -135 && anguloMovimiento < -45) {
        dibujarZombiePalaArriba(p);
    } else {
        dibujarZombiePalaIzquierda(p);
    }
}
void ZombiePala::dibujarZombiePalaAbajo(QPainter &p)
{
    p.save();
    p.translate(pos_x + ancho/2.0f, pos_y + alto/2.0f);
    p.setPen(Qt::NoPen);

    float oscilacion = frameAnimacion;

    QColor colorPiel, colorRopa, colorDetalle, colorOjos;
    obtenerColoresSkin(colorPiel, colorRopa, colorDetalle, colorOjos);

    p.setBrush(QColor(0, 0, 0, 80));
    p.drawEllipse(-12, -10, 24, 20);

    p.setBrush(colorPiel);
    p.drawRect(-13, -2 + oscilacion, 5, 9);

    p.setBrush(colorRopa);
    p.drawRect(-10, -7, 20, 14);

    if (tipoSkin == 0) {
        p.setBrush(QColor(220, 220, 220));
        p.drawRect(-10, -3, 20, 2);
        p.drawRect(-10, 2, 20, 2);
    } else if (tipoSkin == 1) {
        p.setBrush(QColor(200, 200, 200));
        p.drawRect(-2, -7, 4, 12);
        p.setBrush(colorDetalle);
        p.drawRect(-1, -6, 2, 9);
    } else if (tipoSkin == 3) {
        p.setBrush(colorDetalle);
        p.drawRect(-6, -4, 3, 3);
        p.drawRect(3, 1, 4, 3);
    }

    p.setBrush(colorPiel);
    p.drawEllipse(-8, -8, 16, 16);

    if (tipoSkin == 0) {
        p.setBrush(colorDetalle);
        p.drawRect(-9, -10, 18, 6);
        p.drawRect(-11, -5, 22, 2);
    } else if (tipoSkin == 2) {
        p.setBrush(colorDetalle);
        p.drawEllipse(-9, -9, 18, 12);
        p.drawRect(-9, -2, 18, 3);
    }

    p.setBrush(colorOjos);
    p.drawRect(-5, -4, 3, 3);
    p.drawRect(2, -4, 3, 3);
    p.setBrush(QColor(255, 255, 255, 180));
    p.drawRect(-4, -4, 1, 1);
    p.drawRect(3, -4, 1, 1);

    p.setBrush(QColor(150, 20, 20, 200));
    p.drawRect(-3, -7, 2, 3);

    // BRAZO DERECHO + PALA (tal cual la tenias)
    p.save();
    p.rotate(-15);

    p.setBrush(colorPiel);
    p.drawRect(7, -5 - oscilacion, 5, 11);

    p.setBrush(QColor(110, 75, 40));
    p.drawRect(10, -22 - oscilacion, 3, 32);
    p.setBrush(QColor(140, 95, 50));
    p.drawRect(10, -22 - oscilacion, 1, 32);

    p.setBrush(QColor(80, 50, 25));
    p.drawRect(8, 9 - oscilacion, 7, 2);

    QPolygonF palaBase, palaBrillo, palaOxido;
    palaBase << QPointF(7, -22 - oscilacion)
             << QPointF(16, -22 - oscilacion)
             << QPointF(18, -32 - oscilacion)
             << QPointF(12, -36 - oscilacion)
             << QPointF(5, -32 - oscilacion);
    p.setBrush(QColor(130, 135, 145));
    p.drawPolygon(palaBase);

    palaBrillo << QPointF(12, -36 - oscilacion)
               << QPointF(18, -32 - oscilacion)
               << QPointF(16, -30 - oscilacion)
               << QPointF(11, -33 - oscilacion);
    p.setBrush(QColor(210, 220, 235));
    p.drawPolygon(palaBrillo);

    palaOxido << QPointF(5, -32 - oscilacion)
              << QPointF(10, -32 - oscilacion)
              << QPointF(8, -26 - oscilacion);
    p.setBrush(QColor(140, 25, 25, 220));
    p.drawPolygon(palaOxido);

    p.restore();
    p.restore();
}

void ZombiePala::dibujarZombiePalaArriba(QPainter &p)
{
    p.save();
    p.translate(pos_x + ancho/2.0f, pos_y + alto/2.0f);
    p.setPen(Qt::NoPen);

    float oscilacion = frameAnimacion;

    QColor colorPiel, colorRopa, colorDetalle, colorOjos;
    obtenerColoresSkin(colorPiel, colorRopa, colorDetalle, colorOjos);

    p.setBrush(QColor(0, 0, 0, 80));
    p.drawEllipse(-12, -10, 24, 20);

    p.setBrush(colorPiel);
    p.drawRect(-13, -2 + oscilacion, 5, 9);

    // 1 torso de espaldas
    p.setBrush(colorRopa.darker(115));
    p.drawRect(-10, -7, 20, 14);

    if (tipoSkin == 0) {
        p.setBrush(QColor(220, 220, 220));
        p.drawRect(-10, -3, 20, 2);
        p.drawRect(-10, 2, 20, 2);
    }

    // 2 nuca, sin ojos
    p.setBrush(colorPiel);
    p.drawEllipse(-8, -8, 16, 16);

    if (tipoSkin == 0) {
        p.setBrush(colorDetalle);
        p.drawRect(-9, -10, 18, 6);
    } else if (tipoSkin == 2) {
        p.setBrush(colorDetalle);
        p.drawEllipse(-9, -9, 18, 10);
    }

    // 3 brazo con la pala, ahora detras del cuerpo
    p.save();
    p.rotate(15);

    p.setBrush(colorPiel);
    p.drawRect(7, -5 - oscilacion, 5, 11);

    p.setBrush(QColor(110, 75, 40));
    p.drawRect(10, -22 - oscilacion, 3, 32);

    QPolygonF pala;
    pala << QPointF(7, -22 - oscilacion)
         << QPointF(16, -22 - oscilacion)
         << QPointF(12, -36 - oscilacion)
         << QPointF(5, -32 - oscilacion);
    p.setBrush(QColor(130, 135, 145).darker(115));
    p.drawPolygon(pala);

    p.restore();
    p.restore();
}

void ZombiePala::dibujarZombiePalaDerecha(QPainter &p)
{
    p.save();
    p.translate(pos_x + ancho/2.0f, pos_y + alto/2.0f);
    p.setPen(Qt::NoPen);

    float oscilacion = frameAnimacion;

    QColor colorPiel, colorRopa, colorDetalle, colorOjos;
    obtenerColoresSkin(colorPiel, colorRopa, colorDetalle, colorOjos);

    p.setBrush(QColor(0, 0, 0, 80));
    p.drawEllipse(-12, -10, 24, 20);

    p.setBrush(colorRopa.darker(130));
    p.drawRect(-4 + oscilacion, 6, 6, 10);
    p.drawRect(-4 - oscilacion, 6, 6, 10);

    p.setBrush(colorRopa);
    p.drawRect(-6, -7, 14, 14);

    if (tipoSkin == 0) {
        p.setBrush(QColor(220, 220, 220));
        p.drawRect(-6, -3, 14, 2);
        p.drawRect(-6, 2, 14, 2);
    } else if (tipoSkin == 1) {
        p.setBrush(QColor(200, 200, 200));
        p.drawRect(2, -7, 3, 12);
        p.setBrush(colorDetalle);
        p.drawRect(3, -6, 1, 9);
    } else if (tipoSkin == 3) {
        p.setBrush(colorDetalle);
        p.drawRect(-3, -4, 3, 3);
    }

    p.setBrush(colorPiel);
    p.drawRect(-8, -2 + oscilacion, 5, 9);

    p.setBrush(colorPiel);
    p.drawEllipse(-2, -8, 14, 16);

    if (tipoSkin == 0) {
        p.setBrush(colorDetalle);
        p.drawRect(-3, -10, 15, 6);
        p.drawRect(-4, -5, 17, 2);
    } else if (tipoSkin == 2) {
        p.setBrush(colorDetalle);
        p.drawEllipse(-3, -9, 15, 12);
        p.drawRect(-3, -2, 15, 3);
    }

    p.setBrush(colorOjos);
    p.drawRect(6, -4, 3, 3);
    p.setBrush(QColor(255, 255, 255, 180));
    p.drawRect(7, -4, 1, 1);

    p.save();
    p.rotate(-10);

    p.setBrush(colorPiel);
    p.drawRect(6, -5 - oscilacion, 5, 11);

    p.setBrush(QColor(110, 75, 40));
    p.drawRect(9, -22 - oscilacion, 3, 30);

    QPolygonF pala;
    pala << QPointF(6, -22 - oscilacion)
         << QPointF(15, -24 - oscilacion)
         << QPointF(14, -34 - oscilacion)
         << QPointF(6, -32 - oscilacion);
    p.setBrush(QColor(130, 135, 145));
    p.drawPolygon(pala);
    p.setBrush(QColor(138, 3, 3));
    p.drawRect(7, -30 - oscilacion, 3, 3);

    p.restore();
    p.restore();
}

void ZombiePala::dibujarZombiePalaIzquierda(QPainter &p)
{
    p.save();
    p.translate(pos_x + ancho/2.0f, pos_y + alto/2.0f);
    p.setPen(Qt::NoPen);

    float oscilacion = frameAnimacion;

    QColor colorPiel, colorRopa, colorDetalle, colorOjos;
    obtenerColoresSkin(colorPiel, colorRopa, colorDetalle, colorOjos);

    p.setBrush(QColor(0, 0, 0, 80));
    p.drawEllipse(-12, -10, 24, 20);

    p.setBrush(colorRopa.darker(130));
    p.drawRect(-2 + oscilacion, 6, 6, 10);
    p.drawRect(-2 - oscilacion, 6, 6, 10);

    p.setBrush(colorRopa);
    p.drawRect(-8, -7, 14, 14);

    if (tipoSkin == 0) {
        p.setBrush(QColor(220, 220, 220));
        p.drawRect(-8, -3, 14, 2);
        p.drawRect(-8, 2, 14, 2);
    } else if (tipoSkin == 1) {
        p.setBrush(QColor(200, 200, 200));
        p.drawRect(-5, -7, 3, 12);
        p.setBrush(colorDetalle);
        p.drawRect(-4, -6, 1, 9);
    } else if (tipoSkin == 3) {
        p.setBrush(colorDetalle);
        p.drawRect(0, -4, 3, 3);
    }

    p.setBrush(colorPiel);
    p.drawRect(3, -2 + oscilacion, 5, 9);

    p.setBrush(colorPiel);
    p.drawEllipse(-12, -8, 14, 16);

    if (tipoSkin == 0) {
        p.setBrush(colorDetalle);
        p.drawRect(-12, -10, 15, 6);
        p.drawRect(-13, -5, 17, 2);
    } else if (tipoSkin == 2) {
        p.setBrush(colorDetalle);
        p.drawEllipse(-12, -9, 15, 12);
        p.drawRect(-12, -2, 15, 3);
    }

    p.setBrush(colorOjos);
    p.drawRect(-9, -4, 3, 3);
    p.setBrush(QColor(255, 255, 255, 180));
    p.drawRect(-8, -4, 1, 1);

    p.save();
    p.rotate(10);

    p.setBrush(colorPiel);
    p.drawRect(-11, -5 - oscilacion, 5, 11);

    p.setBrush(QColor(110, 75, 40));
    p.drawRect(-12, -22 - oscilacion, 3, 30);

    QPolygonF pala;
    pala << QPointF(-9, -22 - oscilacion)
         << QPointF(-18, -24 - oscilacion)
         << QPointF(-17, -34 - oscilacion)
         << QPointF(-9, -32 - oscilacion);
    p.setBrush(QColor(130, 135, 145));
    p.drawPolygon(pala);
    p.setBrush(QColor(138, 3, 3));
    p.drawRect(-10, -30 - oscilacion, 3, 3);

    p.restore();
    p.restore();
}
