#include "granada.h"
#include <QPainter>
Granada::Granada() : cadenciaDisparo(60), contadorCadencia(0)
{
    ancho = 25.0f;
    alto = 15.0f;
    municionMaxima = 6;
    municionMaximaBase = 6;  // Guardar base
    municionActual = 6;
    tiempoRecargaMax = 2.5f;
}

void Granada::dibujar(QPainter &painter)
{
    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // --- ARMA BASE DETALLADA (Estilo "Thumper" clásico) ---
    // Cañón principal grueso (Verde oliva[cite: 35])
    painter.setBrush(QColor(60, 80, 60)); //[cite: 35]
    painter.drawRect(-2, -6, 20, 12);

    // Detalles del cañón
    painter.setBrush(QColor(40, 60, 40));
    painter.drawRect(16, -7, 4, 14); // Refuerzo en la boca del cañón

    // Bisagra central (donde se quiebra para recargar)
    painter.setBrush(QColor(40, 40, 45));
    painter.drawRect(-6, -4, 6, 10);
    painter.setBrush(QColor(20, 20, 20));
    painter.drawEllipse(-4, -1, 4, 4); // Perno

    // Culata de madera (Marrón[cite: 35])
    painter.setBrush(QColor(80, 60, 40)); //[cite: 35]
    painter.drawRect(-18, -4, 12, 8);
    painter.drawPolygon(QPolygonF() << QPointF(-18, -4) << QPointF(-24, -2) << QPointF(-24, 8) << QPointF(-18, 4));
    // Cantonera de la culata
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(-26, -2, 2, 10);

    // Gatillo y guardamonte[cite: 35]
    painter.setBrush(QColor(100, 100, 110)); //[cite: 35]
    painter.drawRect(-8, 4, 4, 6);
    painter.setBrush(QColor(20, 20, 20)); // Hueco del guardamonte
    painter.drawRect(-7, 5, 2, 4);

    // --- MEJORAS VISUALES POR RADIO DE EXPLOSIÓN ---
    if (multiplicadorRadioExplosion > 1.0f && multiplicadorRadioExplosion < 1.4f) {
        // Nivel 1: Mira de hoja alzada y empuñadura táctica
        // Mira de cuadrante (Leaf sight)
        painter.setBrush(QColor(30, 30, 35));
        painter.drawRect(2, -14, 2, 8); // Base vertical
        painter.drawRect(0, -14, 6, 2); // Apuntador horizontal

        // Empuñadura inferior
        painter.setBrush(QColor(25, 25, 25));
        painter.drawRect(8, 6, 4, 7);
    }
    else if (multiplicadorRadioExplosion >= 1.4f) {
        // Nivel 2: Versión "Heavy Demolition"

        // Armadura de cañón (Gris oscuro metálico)
        painter.setBrush(QColor(45, 45, 50));
        painter.drawRect(4, -8, 14, 4); // Arriba
        painter.drawRect(4, 4, 14, 4);  // Abajo

        // Orificios de refrigeración del blindaje
        painter.setBrush(QColor(15, 15, 15));
        for(int i=0; i<3; i++) {
            painter.drawRect(6 + (i*4), -7, 2, 2);
            painter.drawRect(6 + (i*4), 5, 2, 2);
        }

        // Mira óptica de trayectoria
        painter.setBrush(QColor(20, 20, 20));
        painter.drawRect(0, -12, 10, 6);
        painter.setBrush(QColor(0, 255, 255)); // Pantalla cyan balística
        painter.drawRect(6, -11, 2, 4);

        // Agarre frontal pesado
        painter.setBrush(QColor(40, 40, 45));
        painter.drawRect(10, 6, 6, 8);
    } else {
        // Mira simple acostada (Base)
        painter.setBrush(QColor(30, 30, 35));
        painter.drawRect(2, -7, 8, 2);
    }

    painter.restore();
}

void Granada::actualizar(float posX, float posY, QPointF mouseWorld)
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

void Granada::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        // Aplicar multiplicador de radio de explosión
        float radioBase = 120.0f;
        float radioFinal = radioBase * multiplicadorRadioExplosion;

        // Crear proyectil con radio mejorado
        Proyectil* p = new Proyectil();
        p->reiniciar(pos_x, pos_y, angulo, 6.0f, 60.0f, true, 90.0f * multiplicadorRadioExplosion, multiplicadorRango);
        proyectiles.append(p);

        contadorCadencia = cadenciaDisparo;
        municionActual--;
    }
}

void Granada::actualizarProyectiles()
{
    for (int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if (!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
}

QList<Proyectil*>& Granada::getProyectiles()
{
    return proyectiles;
}

float Granada::get_x() { return pos_x; }
float Granada::get_y() { return pos_y; }
float Granada::get_ancho() { return ancho; }
float Granada::get_alto() { return alto; }
