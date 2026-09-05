#include "pistola.h"
#include "poolentidades.h"
#include <QPainter>
// pistola.cpp

Pistola::Pistola() : cadenciaDisparo(15), contadorCadencia(0), poolProyectiles(40)
{
    ancho = 20.0f;
    alto = 15.0f;
    municionMaxima = 15;
    municionMaximaBase = 15;
    municionActual = 15;
    tiempoRecargaMax = 1.5f;
}

void Pistola::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        Proyectil* p = poolProyectiles.obtener();
        p->reiniciar(pos_x, pos_y, angulo, 10.0f, 10.0f, false, 0.0f, multiplicadorRango);
        proyectiles.append(p);
        contadorCadencia = cadenciaDisparo;
        municionActual--;
    }
}

void Pistola::actualizarProyectiles()
{
    for (int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if (!proyectiles[i]->estaActivo()) {
            poolProyectiles.liberar(proyectiles[i]);
            proyectiles.removeAt(i);
        }
    }
}


// ... (Constructor y demás métodos se mantienen igual[cite: 36]) ...

void Pistola::dibujar(QPainter &painter)
{
    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // --- ARMA BASE DETALLADA ---
    // Corredera (Parte superior metálica)
    painter.setBrush(QColor(50, 50, 55));
    painter.drawRect(-4, -4, 18, 5);

    // Detalles de la corredera (ranuras de agarre)
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(-2, -4, 1, 4);
    painter.drawRect(0, -4, 1, 4);

    // Armazón y guardamonte (Polímero)
    painter.setBrush(QColor(40, 40, 45));
    painter.drawRect(-4, 1, 14, 3);

    // Guardamonte (alrededor del gatillo)
    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(2, 4, 6, 2);
    // Gatillo
    painter.setBrush(QColor(100, 100, 110)); //[cite: 36]
    painter.drawRect(4, 2, 2, 3);

    // Empuñadura
    painter.setBrush(QColor(25, 25, 30));
    painter.drawRect(-6, 1, 6, 9);
    // Textura de la empuñadura
    painter.setBrush(QColor(45, 45, 50));
    painter.drawRect(-4, 2, 2, 7);

    // Mira de hierro clásica (Punto amarillo original[cite: 36])
    painter.setBrush(QColor(200, 200, 50)); //[cite: 36]
    painter.drawRect(12, -6, 2, 2);


    // --- MEJORAS VISUALES POR MUNICIÓN ---
    if (municionMaxima > 15 && municionMaxima < 24) { // 15 es la munición base[cite: 36]
        // Nivel 1: Módulo Láser Táctico
        painter.setBrush(QColor(20, 20, 20));
        painter.drawRect(6, 4, 8, 3); // Caja del láser bajo el cañón
        // Haz de láser rojo (transparente)
        painter.setBrush(QColor(255, 0, 0, 100));
        painter.drawRect(14, 5, 30, 1);
    }
    else if (municionMaxima >= 24) {
        // Nivel 2: Operaciones Especiales (Cargador extendido, Silenciador, Red Dot)

        // Cargador extendido
        painter.setBrush(QColor(35, 35, 40));
        painter.drawRect(-6, 10, 5, 6);
        painter.setBrush(QColor(10, 10, 10)); // Base del cargador
        painter.drawRect(-7, 15, 7, 2);

        // Silenciador
        painter.setBrush(QColor(25, 25, 30));
        painter.drawRect(14, -4, 12, 6);
        // Detalles del silenciador
        painter.setPen(QPen(QColor(15, 15, 15), 1));
        painter.drawLine(17, -4, 17, 1);
        painter.drawLine(21, -4, 21, 1);
        painter.setPen(Qt::NoPen);

        // Mira Micro Red Dot
        painter.setBrush(QColor(30, 30, 30));
        painter.drawRect(-2, -7, 6, 4); // Montura
        painter.setBrush(QColor(255, 50, 50)); // Lente rojo
        painter.drawRect(2, -6, 1, 2);
    }

    painter.restore();
}

void Pistola::actualizar(float posX, float posY, QPointF mouseWorld)
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

    // Recarga automática cuando se acaba la munición
    if (municionActual <= 0 && !recargando) {
        iniciarRecarga();
    }
}


QList<Proyectil*>& Pistola::getProyectiles()
{
    return proyectiles;
}

float Pistola::get_x() { return pos_x; }
float Pistola::get_y() { return pos_y; }
float Pistola::get_ancho() { return ancho; }
float Pistola::get_alto() { return alto; }
