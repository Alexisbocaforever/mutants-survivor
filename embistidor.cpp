#include "embistidor.h"
#include <cmath>
#include <QPainterPath>
#include <QPolygonF>
#include <QBrush>
#include <QPen>

Embistidor::Embistidor(float x, float y, Skin skinElegida)
    : Enemigo(x, y), estadoActual(PERSIGUIENDO), miSkin(skinElegida),
    timerEstado(0.0f), frameAnimacion(0.0f), anguloMovimiento(0.0f)
{
    // Estadísticas de enemigo Mediano/Tanque
    this->sistemaVida.setVidaMaxima(250.0f);
    this->sistemaVida.restaurarCompleta();

    this->ancho = 70.0f;
    this->alto = 50.0f;

    // Stats de movimiento y daño
    this->velocidadNormal = 1.2f;
    this->velocidadCarga = 6.5f; // Muy rápido en carga
    this->danioNormal = 10.0f;
    this->danioCarga = 45.0f;    // Daño devastador si te agarra de lleno

    this->danioContacto = danioNormal; // Inicia con daño normal
    this->velocidad = velocidadNormal;

    configurarSkin();
}
void Embistidor::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }

void Embistidor::configurarSkin() {
    switch(miSkin) {
    case TOXICO: // Basado en la imagen de referencia
        colorPiel = QColor(40, 60, 90);       // Azul oscuro escamoso
        colorPielSecundario = QColor(60, 90, 110);
        colorCristal = QColor(200, 50, 200);  // Magenta/Púrpura
        colorBrillo = QColor(50, 255, 50);    // Verde tóxico
        colorOjos = QColor(255, 200, 0);      // Amarillo brillante
        break;
    case MAGMA:
        colorPiel = QColor(30, 30, 30);
        colorPielSecundario = QColor(50, 40, 40);
        colorCristal = QColor(255, 80, 0);
        colorBrillo = QColor(255, 150, 0);
        colorOjos = QColor(255, 50, 0);
        break;
    case ABISAL:
        colorPiel = QColor(10, 15, 25);
        colorPielSecundario = QColor(20, 30, 45);
        colorCristal = QColor(0, 150, 255);
        colorBrillo = QColor(0, 255, 255);
        colorOjos = Qt::white;
        break;
    }
}

void Embistidor::actualizar() { }

void Embistidor::actualizar(float jugadorX, float jugadorY) {
    if (!estaVivo()) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    frameAnimacion += 0.1f;
    if (frameAnimacion > M_PI * 2) frameAnimacion -= M_PI * 2;

    // MÁQUINA DE ESTADOS
    switch (estadoActual) {
    case PERSIGUIENDO:
        // Apunta y camina hacia el jugador
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

        pos_x += (dx / distancia) * velocidadNormal;
        pos_y += (dy / distancia) * velocidadNormal;
        danioContacto = danioNormal;

        // Si está a la distancia perfecta, se prepara para embestir
        if (distancia > 100.0f && distancia < 250.0f) {
            estadoActual = PREPARANDO;
            timerEstado = 0.0f;
        }
        break;

    case PREPARANDO:
        // Se queda quieto, "cargando" el ataque. Actualiza su ángulo para apuntar bien.
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

        timerEstado += 0.016f; // Asumiendo ~60fps
        if (timerEstado >= 1.2f) { // Tarda 1.2 segundos en prepararse (aviso al jugador)
            estadoActual = CARGANDO;
            timerEstado = 0.0f;
            // Guarda el vector direccional FIJO de la carga
            dirCargaX = dx / distancia;
            dirCargaY = dy / distancia;
            danioContacto = danioCarga; // Daño masivo activado
        }
        break;

    case CARGANDO:
        // Corre en línea recta sin importar dónde esté el jugador ahora (esquivable)
        pos_x += dirCargaX * velocidadCarga;
        pos_y += dirCargaY * velocidadCarga;

        timerEstado += 0.016f;
        // Termina la carga por tiempo (aprox 0.8 segundos de carga furiosa)
        if (timerEstado >= 0.8f) {
            estadoActual = FATIGADO;
            timerEstado = 0.0f;
            danioContacto = danioNormal;
        }
        break;

    case FATIGADO:
        // Se queda quieto jadeando/recuperándose
        timerEstado += 0.016f;
        if (timerEstado >= 2.5f) { // 2.5 segundos de ventana vulnerable para pegarle
            estadoActual = PERSIGUIENDO;
        }
        break;
    }
}

void Embistidor::dibujar(QPainter &p) {
    if (esCadaver()) return;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    float offsetCaminar = 0;
    if (estadoActual == PERSIGUIENDO || estadoActual == CARGANDO) {
        offsetCaminar = std::sin(frameAnimacion) * (estadoActual == CARGANDO ? 4.0f : 2.0f);
    } else if (estadoActual == FATIGADO) {
        // Jadeo (expande y contrae ligeramente)
        p.scale(1.0f + std::sin(frameAnimacion * 2.0f) * 0.05f, 1.0f);
    }

    // Efecto visual cuando está preparando la carga (tiembla)
    if (estadoActual == PREPARANDO) {
        p.translate((std::rand() % 3 - 1), (std::rand() % 3 - 1));
    }

    if (anguloMovimiento >= 315 || anguloMovimiento < 45) {
        dibujarDerecha(p, offsetCaminar);
    } else if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        dibujarAbajo(p, offsetCaminar);
    } else if (anguloMovimiento >= 135 && anguloMovimiento < 225) {
        dibujarIzquierda(p, offsetCaminar);
    } else {
        dibujarArriba(p, offsetCaminar);
    }

    p.restore();
}

void Embistidor::dibujarIzquierda(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);
    float animPiernas = std::sin(frameAnimacion * 2.0f);
    if (estadoActual == PREPARANDO || estadoActual == FATIGADO) animPiernas = 0;

    // Patas traseras (detras del cuerpo)
    p.setBrush(colorPielSecundario);
    p.drawRect(8 + animPiernas * 8, 8, 10, 20);
    p.drawRect(-14 + animPiernas * 8, 8, 10, 20);
    p.setBrush(colorPielSecundario.darker(120));
    p.drawRect(8 + animPiernas * 8, 24, 10, 6);
    p.drawRect(-14 + animPiernas * 8, 24, 10, 6);

    // Cuerpo macizo, en bloque (antes QPainterPath con curvas)
    p.setBrush(colorPiel);
    p.drawRect(-22, -22, 48, 32);
    p.setBrush(colorPiel.lighter(115));
    p.drawRect(-22, -22, 48, 5);
    p.setBrush(colorPiel.darker(115));
    p.drawRect(-22, 4, 48, 6);

    dibujarCristales(p, true);

    // Cabeza tipo martillo, en bloques escalonados
    p.setBrush(colorPielSecundario);
    p.drawRect(-40, -18, 20, 8);
    p.drawRect(-34, -10, 14, 12);
    p.setBrush(colorPielSecundario.darker(115));
    p.drawRect(-40, -12, 20, 3);

    // Ojo
    p.setBrush(colorOjos);
    p.drawRect(-38, -16, 4, 3);

    // Puntos bioluminiscentes
    p.setBrush(estadoActual == PREPARANDO ? Qt::white : colorBrillo);
    p.drawRect(9, -2, 3, 3);
    p.drawRect(-1, 3, 3, 3);
    p.drawRect(-11, -6, 3, 3);

    dibujarBaba(p);

    // Patas delanteras (delante del cuerpo)
    p.setBrush(colorPiel);
    p.drawRect(12 - animPiernas * 8, 10, 12, 24);
    p.drawRect(-18 - animPiernas * 8, 10, 12, 24);
    p.setBrush(colorPiel.darker(115));
    p.drawRect(12 - animPiernas * 8, 28, 12, 6);
    p.drawRect(-18 - animPiernas * 8, 28, 12, 6);
}

void Embistidor::dibujarDerecha(QPainter &p, float offset) {
    p.scale(-1, 1);
    dibujarIzquierda(p, offset);
    p.scale(-1, 1);
}

void Embistidor::dibujarAbajo(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    dibujarCristales(p, false);

    // Patas frontales
    p.setBrush(colorPiel);
    p.drawRect(-32, 10, 20, 22);
    p.drawRect(12, 10, 20, 22);
    p.setBrush(colorPiel.darker(115));
    p.drawRect(-32, 26, 20, 6);
    p.drawRect(12, 26, 20, 6);

    // Cuerpo
    p.setBrush(colorPiel);
    p.drawRect(-25, -18, 50, 38);
    p.setBrush(colorPiel.lighter(115));
    p.drawRect(-25, -18, 50, 5);

    // Cabeza tipo martillo, bien ancha (antes QPainterPath con quadTo)
    p.setBrush(colorPielSecundario);
    p.drawRect(-40, -18, 80, 12);
    p.drawRect(-25, -8, 50, 14);
    p.setBrush(colorPielSecundario.darker(115));
    p.drawRect(-40, -8, 80, 3);

    // Ojos en los extremos del martillo
    p.setBrush(colorOjos);
    p.drawRect(-38, -14, 5, 4);
    p.drawRect(33, -14, 5, 4);

    dibujarBaba(p);
}

void Embistidor::dibujarArriba(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    // Patas traseras gruesas
    p.setBrush(colorPielSecundario);
    p.drawRect(-30, 6, 18, 26);
    p.drawRect(12, 6, 18, 26);
    p.setBrush(colorPielSecundario.darker(115));
    p.drawRect(-30, 24, 18, 8);
    p.drawRect(12, 24, 18, 8);

    // Cuerpo
    p.setBrush(colorPiel);
    p.drawRect(-24, -22, 48, 40);
    p.setBrush(colorPiel.darker(115));
    p.drawRect(-24, 10, 48, 8);

    // Cristales enormes vistos desde atras
    dibujarCristales(p, false);
}

void Embistidor::dibujarCristales(QPainter &p, bool lateral) {
    p.setPen(Qt::NoPen);
    p.setBrush(colorCristal);

    // Pulso mas rapido y mas brillante en PREPARANDO: telegraph visual de la carga
    QColor base = (estadoActual == PREPARANDO) ? Qt::white : colorBrillo;
    float velocidadPulso = (estadoActual == PREPARANDO) ? 8.0f : 2.0f;
    int alphaPulso = 140 + (int)(std::abs(std::sin(frameAnimacion * velocidadPulso)) * 115);
    QColor colorPunta(base.red(), base.green(), base.blue(), alphaPulso);

    if (lateral) {
        p.drawRect(4, -34, 6, 20);
        p.drawRect(12, -42, 6, 26);
        p.drawRect(20, -32, 6, 18);

        p.setBrush(colorPunta);
        p.drawRect(4, -38, 6, 5);
        p.drawRect(12, -46, 6, 5);
        p.drawRect(20, -36, 6, 5);
    } else {
        p.drawRect(-22, -30, 6, 16);
        p.drawRect(-8, -40, 6, 24);
        p.drawRect(6, -40, 6, 24);
        p.drawRect(18, -30, 6, 16);

        p.setBrush(colorPunta);
        p.drawRect(-22, -34, 6, 5);
        p.drawRect(-8, -44, 6, 5);
        p.drawRect(6, -44, 6, 5);
        p.drawRect(18, -34, 6, 5);
    }
}

void Embistidor::dibujarBaba(QPainter &p) {
    p.setPen(Qt::NoPen);
    p.setBrush(colorBrillo);

    float goteo = std::fmod(frameAnimacion * 5.0f, 15.0f);
    p.drawRect(-27, 5, 3, 6 + goteo);
    p.drawRect(-28, 10 + goteo, 5, 4);
}
