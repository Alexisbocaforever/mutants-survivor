#include "fal.h"
#include <QPainter>

Fal::Fal() : cadenciaDisparo(8), contadorCadencia(0)
{
    ancho = 35.0f;
    alto = 10.0f;
    municionMaxima = 30;
    municionMaximaBase = 30;
    municionActual = 30;
    tiempoRecargaMax = 2.0f;
}

void Fal::dibujar(QPainter &painter)
{
    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // --- ARMA BASE DETALLADA ---
    // Cañón principal
    painter.setBrush(QColor(40, 40, 45));
    painter.drawRect(0, -3, 30, 5);

    // Guardamanos de polímero texturizado / madera
    painter.setBrush(QColor(60, 45, 30));
    painter.drawRect(4, -4, 16, 7);

    // Ranuras de ventilación del guardamanos
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(6, -2, 2, 3);
    painter.drawRect(10, -2, 2, 3);
    painter.drawRect(14, -2, 2, 3);

    // Cuerpo principal y cajón de mecanismos
    painter.setBrush(QColor(50, 50, 55));
    painter.drawRect(-12, -5, 16, 11);

    // Cargador curvado
    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(-6, 6, 7, 10);
    // Relieves del cargador
    painter.setBrush(QColor(50, 50, 55));
    painter.drawRect(-5, 7, 1, 8);
    painter.drawRect(-2, 7, 1, 8);

    // Culata ergonómica
    painter.setBrush(QColor(60, 45, 30));
    painter.drawRect(-22, -4, 10, 6);
    painter.drawRect(-24, -2, 4, 7); // Apoyo del hombro
    // Cantonera de goma
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(-26, -3, 2, 9);

    // Empuñadura
    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(-10, 5, 4, 7);

    // --- MEJORAS VISUALES POR RANGO ---
    if (multiplicadorRango > 1.0f && multiplicadorRango < 1.4f) {
        // Nivel 1: Mira telescópica chica (ACOG)
        painter.setBrush(QColor(20, 20, 25));
        painter.drawRect(-4, -9, 10, 4);      // Montura y tubo
        painter.drawPolygon(QPolygonF() << QPointF(6, -9) << QPointF(8, -11) << QPointF(8, -5) << QPointF(6, -5)); // Campana
        // Lente azulado
        painter.setBrush(QColor(50, 150, 255, 200));
        painter.drawRect(7, -10, 1, 4);
    }
    else if (multiplicadorRango >= 1.4f) {
        // Nivel 2: Mira Francotirador pesada, Silenciador y Grip
        // Mira pesada
        painter.setBrush(QColor(15, 15, 20));
        painter.drawRect(-6, -10, 20, 5); // Tubo largo
        painter.drawRect(-8, -12, 4, 9);  // Ocular trasero
        painter.drawRect(12, -12, 6, 9);  // Objetivo delantero
        // Lente de rubí anti-reflejo
        painter.setBrush(QColor(255, 50, 50, 200));
        painter.drawRect(16, -11, 2, 7);

        // Silenciador táctico en el cañón
        painter.setBrush(QColor(25, 25, 30));
        painter.drawRect(30, -5, 14, 9);
        // Detalles del silenciador (ranuras)
        painter.setPen(QPen(QColor(10, 10, 10), 1));
        for(int i = 0; i < 4; i++) {
            painter.drawLine(33 + (i*3), -5, 33 + (i*3), 3);
        }
        painter.setPen(Qt::NoPen);

        // Empuñadura frontal (Grip)
        painter.setBrush(QColor(20, 20, 20));
        painter.drawRect(12, 3, 4, 8);
    } else {
        // Sin mejoras: Mira de hierro clásica
        painter.setBrush(QColor(150, 150, 160));
        painter.drawRect(26, -6, 2, 3);
    }

    painter.restore();
}

void Fal::actualizar(float posX, float posY, QPointF mouseWorld)
{
    pos_x = posX;
    pos_y = posY;

    float dx = mouseWorld.x() - pos_x;
    float dy = mouseWorld.y() - pos_y;
    angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    if (contadorCadencia > 0) {
        contadorCadencia--;
    }

    actualizarRecarga();

    if (municionActual <= 0 && !recargando) {
        iniciarRecarga();
    }
}

void Fal::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        // En lugar de: Proyectil* p = new Proyectil(pos_x, pos_y, angulo, 15.0f, 15.0f);
        Proyectil* p = new Proyectil();
        p->reiniciar(pos_x, pos_y, angulo, 10.0f, 10.0f, false, 0.0f, multiplicadorRango);

        proyectiles.append(p);
        contadorCadencia = cadenciaDisparo;
        municionActual--;
    }
}

void Fal::actualizarProyectiles()
{
    for (int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if (!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
}

QList<Proyectil*>& Fal::getProyectiles()
{
    return proyectiles;
}

float Fal::get_x() { return pos_x; }
float Fal::get_y() { return pos_y; }
float Fal::get_ancho() { return ancho; }
float Fal::get_alto() { return alto; }
