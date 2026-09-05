#include "murcielago.h"
#include <cmath>
#include <QRandomGenerator>
Murcielago::Murcielago(float x, float y)
    : Enemigo(x, y), frameAlas(0), alasBajando(true)
{
    ancho = 25.0f;
    alto = 20.0f;
    velocidad = 1.5f;           // MUY RÁPIDO[cite: 42]
    sistemaVida = Vida(25.0f);  // Muy poca vida[cite: 42]
    danioContacto = 8.0f;       // Daño bajo pero rápido[cite: 42]

    tipoSkin = QRandomGenerator::global()->bounded(4);

    switch(tipoSkin) {
    case 0: // Skin 0: BASE (Púrpura y amarillo)[cite: 42]
        colCuerpo = QColor(40, 0, 60);
        colAlas = QColor(60, 20, 80);
        colOjos = Qt::yellow;
        break;
    case 1: // Skin 1: ALBINO CAVERNÍCOLA (Pálido y ciego)
        colCuerpo = QColor(200, 200, 210); // Gris casi blanco
        colAlas = QColor(230, 180, 180);   // Membrana rosada
        colOjos = QColor(255, 50, 50);     // Ojos rojos inyectados
        break;
    case 2: // Skin 2: VAMPIRO (Oscuro y sangriento)
        colCuerpo = QColor(20, 20, 20);    // Negro azabache
        colAlas = QColor(120, 0, 0);       // Alas carmesí oscuro
        colOjos = Qt::white;               // Ojos blancos fríos
        break;
    case 3: // Skin 3: ESPECTRAL (Fantasmagórico, casi transparente)
        colCuerpo = QColor(0, 150, 200, 180); // Cian translúcido (Con canal Alpha)
        colAlas = QColor(100, 220, 255, 120); // Alas celestes translúcidas
        colOjos = QColor(150, 255, 150);      // Ojos verde neón
        break;
    }
}
void Murcielago::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }
void Murcielago::actualizar() {
    if (!vivo) {
        tiempoDesaparecer -= 0.016f; // Restar tiempo si está muerto
        return; // No animar ni mover si está muerto
    }
    // Aleteo rápido
    if (alasBajando) {
        frameAlas += 1.0f;
        if (frameAlas > 5.0f) alasBajando = false;
    } else {
        frameAlas -= 1.0f;
        if (frameAlas < -5.0f) alasBajando = true;
    }
}

void Murcielago::moverHacia(QPointF objetivo) {
    float dx = objetivo.x() - (pos_x + ancho / 2.0f);
    float dy = objetivo.y() - (pos_y + alto / 2.0f);
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist > 0) {
        // Movimiento errático: agregamos un pequeño seno al movimiento
        float wobble = sin(frameAlas) * 2.0f;
        mover((dx/dist)*velocidad + wobble, (dy/dist)*velocidad);
    }
}

void Murcielago::dibujar(QPainter &p) {
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(pos_x + ancho/2, pos_y + alto/2);

    // Cuerpo
    p.setBrush(colCuerpo); // <--- Variable
    p.setPen(Qt::NoPen);
    p.drawEllipse(-6, -6, 12, 12);

    // Orejas
    QPolygonF orejaIzq; orejaIzq << QPointF(-4, -5) << QPointF(-6, -12) << QPointF(-2, -5);
    QPolygonF orejaDer; orejaDer << QPointF(2, -5) << QPointF(6, -12) << QPointF(4, -5);
    p.drawPolygon(orejaIzq);
    p.drawPolygon(orejaDer);

    // Alas
    QPolygonF alaIzq;
    alaIzq << QPointF(-5, 0) << QPointF(-20, frameAlas * 2) << QPointF(-5, 8);
    QPolygonF alaDer;
    alaDer << QPointF(5, 0) << QPointF(20, frameAlas * 2) << QPointF(5, 8);

    p.setBrush(colAlas); // <--- Variable
    p.drawPolygon(alaIzq);
    p.drawPolygon(alaDer);

    // Ojos
    p.setBrush(colOjos); // <--- Variable
    p.drawEllipse(-3, -2, 2, 2);
    p.drawEllipse(1, -2, 2, 2);

    p.restore();
}
