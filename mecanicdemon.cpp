#include "mecanicdemon.h"
#include <cmath>
#include <QPen>
#include <QBrush>
#include <QRadialGradient>

MecanicDemon::MecanicDemon(float x, float y)
    : Enemigo(x, y), estadoActual(CAMINANDO), anguloMirada(90.0f),
    frameAnimacion(0.0f), timerRafagaPlasma(0.0f), timerBuscador(0.0f), contadorRafaga(0)
{
    // Estadísticas de un minijefe / enemigo pesado
    this->sistemaVida.setVidaMaxima(800.0f);
    this->sistemaVida.restaurarCompleta();
    this->velocidad = 0.6f; // Movimiento pesado y lento
    this->danioContacto = 30.0f;

    // Hitbox masiva
    this->ancho = 120.0f;
    this->alto = 100.0f;

    // Colores basados en la imagen proporcionada
    colorArmadura = QColor(130, 20, 30);   // Rojo oscuro
    colorDorado = QColor(218, 165, 32);    // Ribetes dorados
    colorCarne = QColor(180, 110, 120);    // Piel mutada/expuesta
    colorPlasma = QColor(0, 200, 255);     // Brillo cian de las armas
    colorMetalOculto = QColor(50, 50, 55); // Cables y esqueleto interno
}

MecanicDemon::~MecanicDemon() {
    for (auto m : misilesBuscadores) {
        delete m;
    }
    misilesBuscadores.clear();
}
void MecanicDemon::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }
void MecanicDemon::actualizar() { }

void MecanicDemon::actualizar(float jugadorX, float jugadorY) {
    if (!estaVivo()) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    anguloMirada = std::atan2(dy, dx) * 180.0f / M_PI;
    if (anguloMirada < 0) anguloMirada += 360.0f;

    // IA y Máquina de Estados
    timerRafagaPlasma += 0.016f;
    timerBuscador += 0.016f;
    frameAnimacion += 0.05f; // Ciclo de caminata lento

    if (distancia < 400.0f && timerBuscador >= 6.0f) {
        estadoActual = LANZANDO_BUSCADORES;
        // Lanza 2 buscadores (uno desde cada "hombro")
        misilesBuscadores.push_back(new ProyectilBuscador(centroX - 20, centroY - 30, anguloMirada - 30, 4.0f, 25.0f));
        misilesBuscadores.push_back(new ProyectilBuscador(centroX + 20, centroY - 30, anguloMirada + 30, 4.0f, 25.0f));
        timerBuscador = 0.0f;
    }
    else if (distancia < 300.0f && timerRafagaPlasma >= 2.0f) {
        estadoActual = DISPARANDO_PLASMA;
        // Ráfaga tipo ametralladora (dispara cada pocos frames)
        if (contadorRafaga < 6) {
            float ang_variado = anguloMirada + (std::rand() % 10 - 5);
            disparosPlasma.push_back(ProyectilPlasma(centroX, centroY, ang_variado));
            contadorRafaga++;
            timerRafagaPlasma -= 0.1f; // Pequeño delay entre disparos de la ráfaga
        } else {
            contadorRafaga = 0;
            timerRafagaPlasma = 0.0f; // Resetea el ciclo
        }
    }
    else {
        estadoActual = CAMINANDO;
        // Solo avanza si está a más de 150 px de distancia (mantiene la línea de fuego)
        if (distancia > 150.0f) {
            pos_x += (dx / distancia) * velocidad;
            pos_y += (dy / distancia) * velocidad;
        }
    }

    // Actualizar proyectiles Buscadores
    for (int i = 0; i < misilesBuscadores.size(); i++) {
        misilesBuscadores[i]->actualizarHacia(QPointF(jugadorX, jugadorY));
        misilesBuscadores[i]->actualizar();
        if (!misilesBuscadores[i]->estaActivo()) {
            delete misilesBuscadores[i];
            misilesBuscadores.erase(misilesBuscadores.begin() + i);
            i--;
        }
    }

    // Actualizar proyectiles de Plasma normales
    for (int i = 0; i < disparosPlasma.size(); i++) {
        disparosPlasma[i].x += disparosPlasma[i].velX;
        disparosPlasma[i].y += disparosPlasma[i].velY;
        if (disparosPlasma[i].x < 0 || disparosPlasma[i].x > 2000 ||
            disparosPlasma[i].y < 0 || disparosPlasma[i].y > 2000) {
            disparosPlasma.erase(disparosPlasma.begin() + i);
            i--;
        }
    }
}

void MecanicDemon::dibujar(QPainter &p) {
    if (esCadaver()) return;
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(Qt::NoPen);

    // Ataques independientes de la bestia, tambien en pixeles
    for (auto& plasma : disparosPlasma) {
        p.setBrush(colorPlasma);
        p.drawRect(plasma.x - 4, plasma.y - 4, 8, 8);
        p.setBrush(Qt::white);
        p.drawRect(plasma.x - 2, plasma.y - 2, 4, 4);
    }
    for (auto buscador : misilesBuscadores) {
        buscador->dibujar(p);
    }

    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    float offsetCaminar = std::sin(frameAnimacion) * 5.0f;

    if (anguloMirada >= 315 || anguloMirada < 45) {
        dibujarDerecha(p, offsetCaminar);
    } else if (anguloMirada >= 45 && anguloMirada < 135) {
        dibujarFrente(p, offsetCaminar);
    } else if (anguloMirada >= 135 && anguloMirada < 225) {
        dibujarIzquierda(p, offsetCaminar);
    } else {
        dibujarEspalda(p, offsetCaminar);
    }

    p.restore();
}

void MecanicDemon::dibujarIzquierda(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);
    float animPiernas = std::sin(frameAnimacion * 2.0f);

    // Cables traseros, escalonados en pixeles (antes curvas cubicTo)
    p.setBrush(colorPlasma);
    p.drawRect(20, 18, 6, 4);
    p.drawRect(28, 22, 6, 4);
    p.drawRect(36, 20, 6, 4);
    p.drawRect(44, 24, 6, 4);
    p.setBrush(colorMetalOculto);
    p.drawRect(30, 26, 6, 4);
    p.drawRect(38, 32, 6, 4);
    p.drawRect(46, 40, 6, 4);
    p.drawRect(50, 50, 6, 4);

    dibujarPierna(p, -animPiernas * 15, true);

    // Torso, bloque solido con capas (antes caparazon con cubicTo)
    p.setBrush(colorArmadura);
    p.drawRect(-40, -40, 90, 85);
    p.setBrush(colorArmadura.lighter(115));
    p.drawRect(-40, -40, 90, 8);
    p.setBrush(colorDorado);
    p.drawRect(-40, -8, 90, 4);
    p.drawRect(-40, 8, 90, 4);
    p.setBrush(colorArmadura.darker(115));
    p.drawRect(-40, 30, 90, 15);

    // Chimeneas (arreglado: antes tenian alto negativo)
    p.setBrush(QColor(180, 140, 40));
    p.drawRect(5, -65, 12, 20);
    p.drawRect(20, -62, 10, 15);
    p.setBrush(QColor(255, 140, 0));
    p.drawRect(7, -70, 8, 6);
    p.drawRect(22, -68, 6, 5);

    // Cuello y cabeza
    p.setBrush(colorCarne);
    p.drawRect(-55, -14, 22, 18);
    p.setBrush(colorArmadura);
    p.drawRect(-70, -20, 20, 22);
    p.setBrush(colorDorado);
    p.drawRect(-70, -20, 20, 4);

    // Ojo
    p.setBrush(Qt::yellow);
    p.drawRect(-64, -12, 6, 4);
    p.setBrush(Qt::white);
    p.drawRect(-62, -11, 2, 2);

    // Dientes
    p.setBrush(Qt::white);
    p.drawRect(-68, -4, 4, 6);
    p.drawRect(-62, -4, 4, 6);

    // Cuerno escalonado (antes quadTo diagonal)
    p.setBrush(QColor(40, 40, 40));
    p.drawRect(-52, -30, 5, 10);
    p.drawRect(-47, -36, 5, 10);
    p.drawRect(-42, -42, 5, 8);

    dibujarPierna(p, animPiernas * 15, false);

    float retroceso = (estadoActual == DISPARANDO_PLASMA) ? (std::rand() % 5) : 0;
    p.translate(-40 + retroceso, 15);
    dibujarCanon(p, estadoActual == DISPARANDO_PLASMA, retroceso);
    p.translate(40 - retroceso, -15);
}

void MecanicDemon::dibujarDerecha(QPainter &p, float offset) {
    p.scale(-1, 1);
    dibujarIzquierda(p, offset);
    p.scale(-1, 1);
}

void MecanicDemon::dibujarFrente(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    // Torso, bloque (antes elipse con patas sueltas por fuera del cuerpo)
    p.setBrush(colorArmadura);
    p.drawRect(-45, -40, 90, 65);
    p.setBrush(colorArmadura.lighter(115));
    p.drawRect(-45, -40, 90, 6);
    p.setBrush(colorDorado);
    p.drawRect(-45, 15, 90, 4);

    // Patas, ahora pegadas debajo del torso
    p.setBrush(colorArmadura);
    p.drawRect(-38, 19, 22, 30);
    p.drawRect(16, 19, 22, 30);
    p.setBrush(colorDorado);
    p.drawRect(-38, 19, 22, 4);
    p.drawRect(16, 19, 22, 4);

    // Tubos cian del pecho
    p.setBrush(colorPlasma);
    p.drawRect(-30, 0, 6, 15);
    p.drawRect(24, 0, 6, 15);

    // Chimeneas
    p.setBrush(QColor(180, 140, 40));
    p.drawRect(-15, -60, 10, 25);
    p.drawRect(5, -60, 10, 25);
    p.setBrush(QColor(255, 140, 0));
    p.drawRect(-14, -65, 8, 6);
    p.drawRect(6, -65, 8, 6);

    // Cabeza
    p.setBrush(colorCarne);
    p.drawRect(-20, -28, 40, 36);
    p.setBrush(colorArmadura);
    p.drawRect(-15, -25, 30, 30);
    p.setBrush(colorDorado);
    p.drawRect(-15, -25, 30, 4);

    // Ojos
    p.setBrush(Qt::yellow);
    p.drawRect(-8, -15, 6, 6);
    p.drawRect(2, -15, 6, 6);
    p.setBrush(Qt::white);
    p.drawRect(-7, -14, 2, 2);
    p.drawRect(3, -14, 2, 2);

    // Cuernos escalonados (antes quadTo diagonal)
    p.setBrush(QColor(40, 40, 40));
    p.drawRect(-30, -32, 5, 12);
    p.drawRect(-25, -38, 5, 10);
    p.drawRect(25, -32, 5, 12);
    p.drawRect(20, -38, 5, 10);

    // Cañones gemelos
    float retrocesoIzq = (estadoActual == DISPARANDO_PLASMA) ? (std::rand() % 4) : 0;
    float retrocesoDer = (estadoActual == DISPARANDO_PLASMA) ? (std::rand() % 4) : 0;

    p.save();
    p.translate(50, 10 - retrocesoIzq);
    p.rotate(90);
    dibujarCanon(p, estadoActual == DISPARANDO_PLASMA, retrocesoIzq);
    p.restore();

    p.save();
    p.translate(-50, 10 - retrocesoDer);
    p.rotate(90);
    dibujarCanon(p, estadoActual == DISPARANDO_PLASMA, retrocesoDer);
    p.restore();
}

void MecanicDemon::dibujarEspalda(QPainter &p, float offset) {
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    p.setBrush(colorArmadura);
    p.drawRect(-55, -45, 110, 90);
    p.setBrush(colorArmadura.darker(115));
    p.drawRect(-55, 30, 110, 15);

    // Espinas dorsales, en bloques (antes triangulos QPolygonF)
    p.setBrush(colorDorado);
    for(int i = -30; i <= 30; i += 15) {
        p.drawRect(i - 3, -55, 6, 10);
        p.drawRect(i - 2, -60, 4, 6);
    }

    p.setBrush(QColor(180, 140, 40));
    p.drawRect(-25, -20, 18, 35);
    p.drawRect(7, -20, 18, 35);
    p.setBrush(QColor(60, 60, 60));
    p.drawRect(-25, -20, 18, 4);
    p.drawRect(7, -20, 18, 4);

    p.setBrush(colorPlasma);
    p.drawRect(-24, 8, 6, 8);
    p.drawRect(18, 8, 6, 8);
    p.setBrush(colorMetalOculto);
    p.drawRect(-14, 16, 6, 6);
    p.drawRect(8, 16, 6, 6);

    // Patas, ahora pegadas al torso (antes en -65/+40, bien afuera del cuerpo)
    p.setBrush(colorArmadura);
    p.drawRect(-35, 30, 22, 32);
    p.drawRect(13, 30, 22, 32);
    p.setBrush(colorDorado);
    p.drawRect(-35, 30, 22, 4);
    p.drawRect(13, 30, 22, 4);
}

void MecanicDemon::dibujarCanon(QPainter &p, bool disparando, float retroceso) {
    p.save();
    p.setPen(Qt::NoPen);

    p.setBrush(colorArmadura);
    p.drawRect(6, -10, 16, 20);
    p.setBrush(colorDorado);
    p.drawRect(6, -10, 16, 3);

    p.setBrush(QColor(40, 40, 45));
    p.drawRect(-30 + retroceso, -12, 45, 24);
    p.setBrush(QColor(60, 60, 65));
    p.drawRect(-30 + retroceso, -12, 45, 4);

    for (int i = 0; i < 4; i++) {
        p.setBrush(colorPlasma);
        p.drawRect(-25 + retroceso + (i*8), -10, 6, 20);
    }

    p.setBrush(QColor(20, 20, 20));
    p.drawRect(-45 + retroceso, -10, 15, 6);
    p.drawRect(-45 + retroceso, 4, 15, 6);

    if (disparando) {
        dibujarGlowPlasma(p, -50 + retroceso, -7, 12);
        dibujarGlowPlasma(p, -50 + retroceso, 7, 12);

        p.setBrush(QColor(255, 200, 0));
        p.drawRect(-65 + retroceso, -9, 20, 4);
        p.drawRect(-65 + retroceso, 5, 20, 4);
    }

    p.restore();
}

void MecanicDemon::dibujarPierna(QPainter &p, float anguloPierna, bool esTrasera) {
    p.save();
    p.translate(esTrasera ? 25 : 15, 15);
    p.rotate(anguloPierna);
    p.setPen(Qt::NoPen);

    p.setBrush(colorCarne);
    p.drawRect(-4, 0, 18, 22);
    p.setBrush(colorCarne.darker(120));
    p.drawRect(-4, 16, 18, 6);

    p.translate(5, 25);
    p.rotate(-anguloPierna * 1.5f);

    p.setBrush(colorArmadura);
    p.drawRect(-8, -5, 16, 25);
    p.setBrush(colorDorado);
    p.drawRect(-8, -5, 16, 3);

    // Garras (antes quadTo diagonal)
    p.setBrush(QColor(100, 100, 100));
    p.drawRect(-16, 22, 8, 6);
    p.drawRect(-4, 24, 8, 6);

    p.restore();
}

void MecanicDemon::dibujarGlowPlasma(QPainter &p, float x, float y, float radio) {
    p.setPen(Qt::NoPen);
    int r = (int)radio;
    p.setBrush(QColor(colorPlasma.red(), colorPlasma.green(), colorPlasma.blue(), 100));
    p.drawRect(x - r, y - r, r * 2, r * 2);
    p.setBrush(colorPlasma);
    p.drawRect(x - r/2, y - r/2, r, r);
    p.setBrush(Qt::white);
    p.drawRect(x - r/4, y - r/4, r/2, r/2);
}
