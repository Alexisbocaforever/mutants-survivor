#include "carnicero.h"
#include <cmath>
#include <QPolygonF>
#include <QBrush>
#include <QPen>
#include <QRadialGradient>

carnicero::carnicero(float x, float y, int variante)
    : Enemigo(x, y), varianteDiseno(variante), enfurecido(false),
    frameAnimacion(0.0f), anguloMovimiento(90.0f), atacando(false), timerAtaque(0.0f)
{
    this->sistemaVida.setVidaMaxima(180.0f);
    this->sistemaVida.restaurarCompleta();
    this->velocidad = 2.5f;
    this->danioContacto = 35.0f;
    this->ancho = 48.0f;
    this->alto = 48.0f;
    inicializarColores();
}
void carnicero::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }

void carnicero::inicializarColores() {
    switch(varianteDiseno) {
    case 0: // Devoradores de Mundos (Clásico)
        colorArmadura = QColor(160, 20, 20); // Rojo sangre oscuro
        colorBorde = QColor(218, 165, 32);   // Bronce/Dorado oscuro
        colorOjos = Qt::green;
        colorRunas = QColor(255, 200, 0);
        break;
    case 1: // Portadores de la Palabra (Oscuros)
        colorArmadura = QColor(80, 10, 10);
        colorBorde = QColor(170, 170, 170);  // Acero
        colorOjos = QColor(255, 100, 0);
        colorRunas = QColor(200, 200, 200);
        break;
    case 2: // Khorne Demoníaco
        colorArmadura = QColor(200, 30, 30);
        colorBorde = QColor(255, 200, 0);
        colorOjos = Qt::cyan;
        colorRunas = QColor(255, 100, 0);
        break;
    default:
        colorArmadura = QColor(160, 20, 20);
        colorBorde = QColor(218, 165, 32);
        colorOjos = Qt::green;
        colorRunas = QColor(255, 200, 0);
        break;
    }
}

void carnicero::actualizar() {
    // Sobrecarga vacía requerida por la clase abstracta
}

void carnicero::actualizar(float jugadorX, float jugadorY) {
    if (!estaVivo()) return;

    // Furia al llegar al 50% de la vida
    if (!enfurecido && sistemaVida.getVidaActual() <= (sistemaVida.getVidaMaxima() * 0.5f)) {
        enfurecido = true;
        velocidad = 4.8f;
        danioContacto = 50.0f;
    }

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > 0) {
        // Movimiento base
        pos_x += (dx / distancia) * velocidad;
        pos_y += (dy / distancia) * velocidad;

        // Calcular ángulo de movimiento en grados (0-360)
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

        // Ciclo de animación para caminar
        frameAnimacion += 0.15f * (enfurecido ? 1.5f : 1.0f);
        if (frameAnimacion > M_PI * 2) frameAnimacion -= M_PI * 2;
    }

    // Lógica de animación de ataque cuerpo a cuerpo
    if (distancia < 55.0f) {
        atacando = true;
        timerAtaque += 0.3f * (enfurecido ? 1.5f : 1.0f);
    } else {
        atacando = false;
        timerAtaque = 0.0f;
    }
}

void carnicero::dibujar(QPainter &p) {
 if (esCadaver()) return;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    // Aura de Furia, en bloques concentricos en vez de gradiente radial
    if (enfurecido) {
        p.setPen(Qt::NoPen);
        int alpha = 70 + (int)(std::abs(std::sin(frameAnimacion * 4)) * 60);
        int r = (int)(ancho * 0.7f);
        p.setBrush(QColor(255, 40, 0, alpha));
        p.drawRect(-r, -r, r * 2, r * 2);
        p.setBrush(QColor(255, 120, 0, alpha / 2));
        p.drawRect(-r + 6, -r + 6, r * 2 - 12, r * 2 - 12);
    }

    float offsetPiernas = std::sin(frameAnimacion) * 8.0f;
    float offsetArmas = std::cos(frameAnimacion) * 3.0f;

    if (atacando) offsetArmas = std::abs(std::sin(timerAtaque)) * 18.0f;

    if (anguloMovimiento >= 315 || anguloMovimiento < 45) {
        dibujarDerecha(p, offsetPiernas, offsetArmas);
    } else if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        dibujarFrente(p, offsetPiernas, offsetArmas);
    } else if (anguloMovimiento >= 135 && anguloMovimiento < 225) {
        dibujarIzquierda(p, offsetPiernas, offsetArmas);
    } else {
        dibujarEspalda(p, offsetPiernas, offsetArmas);
    }

    p.restore();
}

void carnicero::dibujarFrente(QPainter &p, float offsetPiernas, float offsetArmas) {
    p.setPen(Qt::NoPen);

    // 1. PIERNAS, bloques solidos sin redondeo
    p.setBrush(QBrush(colorArmadura.darker()));
    p.drawRect(-12, 10 - offsetPiernas, 10, 16);
    p.drawRect(2, 10 + offsetPiernas, 10, 16);
    p.setBrush(QBrush(colorBorde));
    p.drawRect(-12, 22 - offsetPiernas, 10, 4);
    p.drawRect(2, 22 + offsetPiernas, 10, 4);

    // 2. TORSO
    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-16, -12, 32, 26);
    p.setBrush(QBrush(colorArmadura.lighter(115)));
    p.drawRect(-16, -12, 32, 4);
    p.setBrush(QBrush(colorArmadura.darker(120)));
    p.drawRect(-16, 8, 32, 6);

    // Runa de Khorne, cruz en bloques pixelados
    p.setBrush(QBrush(colorRunas));
    p.drawRect(-2, -2, 4, 4);
    p.drawRect(-6, 2, 4, 4);
    p.drawRect(2, 2, 4, 4);
    p.drawRect(-2, 6, 4, 4);
    p.drawRect(-6, -2, 4, 4);
    p.drawRect(2, -2, 4, 4);

    // 3. BRAZOS Y ARMAS
    p.translate(-22, -2 + offsetArmas);
    dibujarArma(p, true);
    p.translate(22, 2 - offsetArmas);

    p.translate(22, -2 - offsetArmas);
    if (varianteDiseno == 2) dibujarArma(p, false);
    else {
        p.setBrush(QBrush(colorArmadura));
        p.drawRect(-6, -6, 12, 12);
        p.setBrush(QBrush(colorBorde));
        p.drawRect(-6, -6, 12, 3);
    }
    p.translate(-22, 2 + offsetArmas);

    // 4. HOMBRERAS en bloque, sin drawChord
    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-28, -18, 16, 14);
    p.drawRect(12, -18, 16, 14);
    p.setBrush(QBrush(colorBorde));
    p.drawRect(-28, -18, 16, 3);
    p.drawRect(12, -18, 16, 3);

    // 5. CASCO Y CRESTA
    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-8, -22, 16, 14);

    p.setBrush(Qt::black);
    p.drawRect(-6, -16, 12, 4);
    p.setBrush(QBrush(colorOjos));
    p.drawRect(-5, -15, 4, 2);
    p.drawRect(1, -15, 4, 2);

    // Cresta escalonada (sin diagonales, pura silueta de pixeles)
    p.setBrush(QBrush(colorBorde));
    p.drawRect(-14, -30, 4, 8);
    p.drawRect(-10, -26, 4, 6);
    p.drawRect(10, -30, 4, 8);
    p.drawRect(6, -26, 4, 6);
}

void carnicero::dibujarEspalda(QPainter &p, float offsetPiernas, float offsetArmas) {
    p.setPen(Qt::NoPen);

    p.setBrush(QBrush(colorArmadura.darker()));
    p.drawRect(-12, 10 + offsetPiernas, 10, 16);
    p.drawRect(2, 10 - offsetPiernas, 10, 16);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-24, -6 + offsetArmas, 8, 8);
    p.drawRect(16, -6 - offsetArmas, 8, 8);

    p.setBrush(QBrush(colorArmadura.darker(110)));
    p.drawRect(-14, -12, 28, 26);

    // Power pack (mochila dorsal)
    p.setBrush(QBrush(QColor(50, 50, 50)));
    p.drawRect(-10, -20, 20, 20);
    p.setBrush(QBrush(colorBorde));
    p.drawRect(-18, -14, 8, 8);
    p.drawRect(10, -14, 8, 8);
    p.setBrush(Qt::black);
    p.drawRect(-16, -12, 4, 4);
    p.drawRect(12, -12, 4, 4);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-28, -18, 16, 14);
    p.drawRect(12, -18, 16, 14);
    p.drawRect(-8, -22, 16, 14); // Nuca

    p.setBrush(QBrush(colorBorde));
    p.drawRect(-14, -30, 4, 8);
    p.drawRect(-10, -26, 4, 6);
    p.drawRect(10, -30, 4, 8);
    p.drawRect(6, -26, 4, 6);
}

void carnicero::dibujarIzquierda(QPainter &p, float offsetPiernas, float offsetArmas) {
    p.setPen(Qt::NoPen);

    p.setBrush(QBrush(colorArmadura.darker()));
    p.drawRect(-6, 10 + offsetPiernas, 8, 16);
    p.drawRect(-2, 10 - offsetPiernas, 8, 16);

    p.setBrush(QBrush(QColor(50, 50, 50)));
    p.drawRect(4, -14, 10, 16);
    p.setBrush(QBrush(colorBorde));
    p.drawRect(11, -9, 6, 6);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-12, -12, 18, 26);
    p.setBrush(QBrush(colorArmadura.lighter(115)));
    p.drawRect(-12, -12, 18, 4);

    p.translate(-14, 2 + offsetArmas);
    dibujarArma(p, true);
    p.translate(14, -2 - offsetArmas);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-10, -16, 14, 18);

    p.drawRect(-14, -22, 14, 14);
    p.setBrush(Qt::black);
    p.drawRect(-19, -17, 6, 4);
    p.setBrush(QBrush(colorOjos));
    p.drawRect(-19, -17, 3, 2);

    p.setBrush(QBrush(colorBorde));
    p.drawRect(-4, -32, 4, 8);
    p.drawRect(0, -28, 4, 6);
}

void carnicero::dibujarDerecha(QPainter &p, float offsetPiernas, float offsetArmas) {
    p.setPen(Qt::NoPen);

    p.setBrush(QBrush(colorArmadura.darker()));
    p.drawRect(-2, 10 + offsetPiernas, 8, 16);
    p.drawRect(-6, 10 - offsetPiernas, 8, 16);

    p.setBrush(QBrush(QColor(50, 50, 50)));
    p.drawRect(-14, -14, 10, 16);
    p.setBrush(QBrush(colorBorde));
    p.drawRect(-17, -9, 6, 6);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-6, -12, 18, 26);
    p.setBrush(QBrush(colorArmadura.lighter(115)));
    p.drawRect(-6, -12, 18, 4);

    p.translate(14, 2 + offsetArmas);
    p.scale(-1, 1);
    dibujarArma(p, true);
    p.scale(-1, 1);
    p.translate(-14, -2 - offsetArmas);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-4, -16, 14, 18);

    p.drawRect(0, -22, 14, 14);
    p.setBrush(Qt::black);
    p.drawRect(13, -17, 6, 4);
    p.setBrush(QBrush(colorOjos));
    p.drawRect(16, -17, 3, 2);

    p.setBrush(QBrush(colorBorde));
    p.drawRect(0, -32, 4, 8);
    p.drawRect(-4, -28, 4, 6);
}

void carnicero::dibujarArma(QPainter &p, bool esManoIzquierda) {
    p.setPen(Qt::NoPen);

    p.setBrush(QBrush(colorArmadura));
    p.drawRect(-4, -4, 8, 8);

    if (varianteDiseno == 0 || varianteDiseno == 2) {
        // Hacha sierra, silueta escalonada en vez de poligono diagonal
        p.setBrush(QBrush(QColor(80, 40, 20)));
        p.drawRect(-2, -22, 4, 26);

        p.setBrush(QBrush(QColor(160, 160, 160)));
        p.drawRect(-2, -30, 16, 8);
        p.drawRect(6, -26, 10, 16);

        p.setBrush(QBrush(QColor(90, 90, 90)));
        for (int i = 0; i < 4; i++) {
            p.drawRect(14, -28 + i * 5, 4, 3);
        }
    }
    else if (varianteDiseno == 1) {
        p.setBrush(QBrush(QColor(60, 60, 60)));
        p.drawRect(-3, -32, 6, 36);

        p.setBrush(QBrush(QColor(200, 200, 200)));
        for (int i = -30; i < 4; i += 6) p.drawRect(3, i, 5, 3);

        p.setBrush(QBrush(colorBorde));
        p.drawRect(6, -32, 3, 34);
    }
}
