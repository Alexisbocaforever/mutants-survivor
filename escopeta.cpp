#include "escopeta.h"
#include <QPainter>

Escopeta::Escopeta() : cadenciaDisparo(45), contadorCadencia(0), perdigonesPorDisparo(6),
    poolProyectiles(60), nivelCadenciaEscopeta(0)
{
    ancho = 30.0f;
    alto = 12.0f;
    municionMaxima = 8;
    municionMaximaBase = 8;
    municionActual = 8;
    tiempoRecargaMax = 2.5f; // Tarda más en recargar
}

void Escopeta::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        // Dispara múltiples perdigones
        for(int i = 0; i < perdigonesPorDisparo; i++) {
            Proyectil* p = poolProyectiles.obtener();

            // Dispersión aleatoria entre -15 y +15 grados
            float dispersion = (rand() % 31) - 15.0f;
            float anguloFinal = angulo + dispersion;

            // Perdigones viajan a velocidad 10, con 10 de daño cada uno
            p->reiniciar(pos_x, pos_y, anguloFinal, 10.0f, 10.0f, false, 0.0f, multiplicadorRango);
            proyectiles.append(p);
        }

        contadorCadencia = cadenciaDisparo;
        municionActual--;
    }
}

void Escopeta::actualizarProyectiles()
{
    for (int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if (!proyectiles[i]->estaActivo()) {
            poolProyectiles.liberar(proyectiles[i]);
            proyectiles.removeAt(i);
        }
    }
}

void Escopeta::dibujar(QPainter &painter)
{
    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // Cañón doble
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(5, -4, 25, 4);
    painter.drawRect(5, 0, 25, 4);

    // Culata y agarre de madera
    painter.setBrush(QColor(100, 50, 20));
    painter.drawRect(-15, -3, 20, 8);
    painter.drawRect(-15, 5, 8, 5);

    // SKIN nivel 1: riel de recarga rapida bajo el cañon
    if (nivelCadenciaEscopeta >= 1) {
        painter.setBrush(QColor(40, 40, 40));
        painter.drawRect(5, 4, 22, 3);
        painter.setPen(QPen(QColor(80, 80, 80), 1));
        for (int i = 0; i < 5; i++) painter.drawLine(7 + i * 4, 4, 7 + i * 4, 7);
        painter.setPen(Qt::NoPen);
    }

    // SKIN nivel 2: vainas de repuesto a la vista, cadencia turbo
    if (nivelCadenciaEscopeta >= 2) {
        painter.setBrush(QColor(200, 60, 30));
        for (int i = 0; i < 3; i++) {
            painter.drawRect(-13 + i * 4, -9, 3, 6);
        }
    }

    painter.restore();
}

void Escopeta::actualizar(float posX, float posY, QPointF mouseWorld)
{
    pos_x = posX;
    pos_y = posY;

    float dx = mouseWorld.x() - pos_x;
    float dy = mouseWorld.y() - pos_y;
    angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    if (contadorCadencia > 0) contadorCadencia--;
    actualizarRecarga();

    if (municionActual <= 0 && !recargando) {
        iniciarRecarga();
    }
}

QList<Proyectil*>& Escopeta::getProyectiles() { return proyectiles; }
float Escopeta::get_x() { return pos_x; }
float Escopeta::get_y() { return pos_y; }
float Escopeta::get_ancho() { return ancho; }
float Escopeta::get_alto() { return alto; }
