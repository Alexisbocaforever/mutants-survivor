#include "cascarudo.h"
#include <cmath>
#include <QRandomGenerator>
Cascarudo::Cascarudo(float x, float y)
    : Enemigo(x, y), frameAnimacion(0.0f),
    velocidadAnimacion(0.08f), animacionSubiendo(true),
    anguloMovimiento(90.0f)
{
    ancho = 40.0f;
    alto = 40.0f;
    velocidad = 1.5f;
    sistemaVida = Vida(50.0f);  // Cascarudo con 50 HP
    danioContacto = 5.0f;       // Hace 5 de daño al contacto

    // --- SELECCIÓN ALEATORIA DE SKIN ---
    tipoSkin = QRandomGenerator::global()->bounded(4); // Genera número del 0 al 3

    switch(tipoSkin) {
    case 0: // Skin 0: BASE (El original gris)[cite: 40]
        colCaparazonOscuro = QColor(60, 60, 60);
        colCaparazonClaro = QColor(90, 90, 90);
        colBrillo = QColor(120, 120, 120);
        colPatas = QColor(30, 30, 30);
        colOjos = QColor(150, 0, 0);
        break;
    case 1: // Skin 1: TÓXICO (Mutado, verde radiactivo)
        colCaparazonOscuro = QColor(20, 50, 20); // Verde muy oscuro
        colCaparazonClaro = QColor(50, 180, 50); // Verde neón
        colBrillo = QColor(150, 255, 150);
        colPatas = QColor(15, 30, 15);
        colOjos = QColor(255, 255, 0);           // Ojos amarillos
        break;
    case 2: // Skin 2: MAGMA (Volcánico)
        colCaparazonOscuro = QColor(25, 20, 20); // Negro carbón
        colCaparazonClaro = QColor(200, 60, 0);  // Naranja ardiente
        colBrillo = QColor(255, 150, 50);        // Brillo de lava
        colPatas = QColor(15, 10, 10);
        colOjos = QColor(255, 255, 255);         // Ojos blancos incandescentes
        break;
    case 3: // Skin 3: ÉLITE ACORAZADO (Metálico azulado)
        colCaparazonOscuro = QColor(30, 40, 60); // Azul marino oscuro
        colCaparazonClaro = QColor(80, 120, 160); // Celeste acero
        colBrillo = QColor(180, 210, 240);
        colPatas = QColor(20, 25, 30);
        colOjos = QColor(0, 255, 255);           // Ojos cian brillante
        break;
    }
}
void Cascarudo::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }

void Cascarudo::dibujar(QPainter &painter)
{
    int pixelSize = 2;
    float offset = frameAnimacion * (pixelSize / 2.0f);

    // Normalizar ángulo entre 0 y 360
    float angulo = anguloMovimiento;
    while (angulo < 0) angulo += 360;
    while (angulo >= 360) angulo -= 360;

    // Determinar dirección basada en el ángulo (8 direcciones)
    if (angulo >= 337.5 || angulo < 22.5) {
        // Derecha (0°)
        dibujarDerecha(painter, offset);
    } else if (angulo >= 22.5 && angulo < 67.5) {
        // Abajo-Derecha (45°)
        dibujarDiagonal(painter, offset, 1);
    } else if (angulo >= 67.5 && angulo < 112.5) {
        // Abajo (90°)
        dibujarAbajo(painter, offset);
    } else if (angulo >= 112.5 && angulo < 157.5) {
        // Abajo-Izquierda (135°)
        dibujarDiagonal(painter, offset, 2);
    } else if (angulo >= 157.5 && angulo < 202.5) {
        // Izquierda (180°)
        dibujarIzquierda(painter, offset);
    } else if (angulo >= 202.5 && angulo < 247.5) {
        // Arriba-Izquierda (225°)
        dibujarDiagonal(painter, offset, 3);
    } else if (angulo >= 247.5 && angulo < 292.5) {
        // Arriba (270°)
        dibujarArriba(painter, offset);
    } else {
        // Arriba-Derecha (315°)
        dibujarDiagonal(painter, offset, 4);
    }
}

void Cascarudo::dibujarArriba(QPainter &painter, float offset)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);

    // Colores grises
    QColor caparazonOscuro(60, 60, 60);      // Gris oscuro
    QColor caparazonClaro(90, 90, 90);       // Gris medio
    QColor brillo(120, 120, 120);            // Gris claro para brillos
    QColor patas(30, 30, 30);                // Negro para patas
    QColor ojos(150, 0, 0);                  // Rojo para ojos

    // PATAS (3 pares)
    painter.setBrush(colPatas);
    // Par 1 (atrás)
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 15*pixelSize + offset, 2*pixelSize, 8*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 15*pixelSize - offset, 2*pixelSize, 8*pixelSize);

    // Par 2 (medio)
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 10*pixelSize - offset, 2*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 15*pixelSize, pos_y + 10*pixelSize + offset, 2*pixelSize, 6*pixelSize);

    // Par 3 (delante)
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 6*pixelSize + offset, 2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 6*pixelSize - offset, 2*pixelSize, 5*pixelSize);

    // CUERPO/CAPARAZÓN BASE
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 8*pixelSize, 10*pixelSize, 14*pixelSize);

    // CAPARAZÓN CON DETALLES (segmentos)
    painter.setBrush(caparazonClaro);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 9*pixelSize, 8*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 14*pixelSize, 8*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 19*pixelSize, 8*pixelSize, 3*pixelSize);

    // BRILLOS EN CAPARAZÓN
    painter.setBrush(brillo);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 10*pixelSize, 2*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 15*pixelSize, 2*pixelSize, 2*pixelSize);

    // CABEZA
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 4*pixelSize, 6*pixelSize, 5*pixelSize);

    // OJOS ROJOS BRILLANTES
    painter.setBrush(colOjos);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 5*pixelSize, 2*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 5*pixelSize, 2*pixelSize, 2*pixelSize);

    // ANTENAS
    painter.setBrush(colPatas);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 2*pixelSize, 1*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 2*pixelSize, 1*pixelSize, 3*pixelSize);

    // MANDÍBULAS
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
}

void Cascarudo::dibujarAbajo(QPainter &painter, float offset)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);


    // PATAS (3 pares) - vista desde atrás
    painter.setBrush(colPatas);
    // Par 1 (delante, ahora atrás en la vista)
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 2*pixelSize - offset, 2*pixelSize, 8*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 2*pixelSize + offset, 2*pixelSize, 8*pixelSize);

    // Par 2 (medio)
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 8*pixelSize + offset, 2*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 15*pixelSize, pos_y + 8*pixelSize - offset, 2*pixelSize, 6*pixelSize);

    // Par 3 (atrás)
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 13*pixelSize - offset, 2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 13*pixelSize + offset, 2*pixelSize, 5*pixelSize);

    // CUERPO/CAPARAZÓN (vista posterior)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 4*pixelSize, 10*pixelSize, 14*pixelSize);

    // SEGMENTOS DEL CAPARAZÓN (desde atrás)
    painter.setBrush(colCaparazonClaro);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 5*pixelSize, 8*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 9*pixelSize, 8*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 14*pixelSize, 8*pixelSize, 4*pixelSize);

    // BRILLOS
    painter.setBrush(colBrillo);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 10*pixelSize, 2*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 6*pixelSize, 2*pixelSize, 2*pixelSize);

    // PARTE TRASERA (abdomen)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 18*pixelSize, 6*pixelSize, 4*pixelSize);
}

void Cascarudo::dibujarIzquierda(QPainter &painter, float offset)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);


    // PATAS (solo un lado visible, 3 patas)
    painter.setBrush(colPatas);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 6*pixelSize + offset, 6*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 12*pixelSize - offset, 5*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 18*pixelSize + offset, 6*pixelSize, 2*pixelSize);

    // CUERPO (vista lateral)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 5*pixelSize, 12*pixelSize, 16*pixelSize);

    // SEGMENTOS LATERALES
    painter.setBrush(colCaparazonClaro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 6*pixelSize, 10*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 12*pixelSize, 10*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 17*pixelSize, 10*pixelSize, 3*pixelSize);

    // BRILLO
    painter.setBrush(colBrillo);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 13*pixelSize, 2*pixelSize, 2*pixelSize);

    // CABEZA (lateral)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 8*pixelSize, 5*pixelSize, 6*pixelSize);

    // OJO
    painter.setBrush(colOjos);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 10*pixelSize, 2*pixelSize, 2*pixelSize);

    // ANTENA
    painter.setBrush(colPatas);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
}

void Cascarudo::dibujarDerecha(QPainter &painter, float offset)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);



    // PATAS (solo un lado visible, 3 patas)
    painter.setBrush(colPatas);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 6*pixelSize - offset, 6*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 13*pixelSize, pos_y + 12*pixelSize + offset, 5*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 18*pixelSize - offset, 6*pixelSize, 2*pixelSize);

    // CUERPO (vista lateral)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 5*pixelSize, 12*pixelSize, 16*pixelSize);

    // SEGMENTOS LATERALES
    painter.setBrush(colCaparazonClaro);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 6*pixelSize, 10*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 12*pixelSize, 10*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 17*pixelSize, 10*pixelSize, 3*pixelSize);

    // BRILLO
    painter.setBrush(colBrillo);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 13*pixelSize, 2*pixelSize, 2*pixelSize);

    // CABEZA (lateral)
    painter.setBrush(colCaparazonOscuro);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 8*pixelSize, 5*pixelSize, 6*pixelSize);

    // OJO
    painter.setBrush(colOjos);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 10*pixelSize, 2*pixelSize, 2*pixelSize);

    // ANTENA
    painter.setBrush(colPatas);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
}

void Cascarudo::dibujarDiagonal(QPainter &painter, float offset, int direccion)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);

    bool mirandoDerecha = (direccion == 1 || direccion == 4);
    bool mirandoArriba = (direccion == 3 || direccion == 4);

    // PATAS (vista diagonal - 3 patas visibles)
    painter.setBrush(colPatas);
    if (mirandoDerecha) {
        painter.drawRect(pos_x + 14*pixelSize, pos_y + 7*pixelSize - offset, 4*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 14*pixelSize, pos_y + 13*pixelSize + offset, 4*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 14*pixelSize, pos_y + 19*pixelSize - offset, 4*pixelSize, 2*pixelSize);
    } else {
        painter.drawRect(pos_x + 2*pixelSize, pos_y + 7*pixelSize + offset, 4*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 2*pixelSize, pos_y + 13*pixelSize - offset, 4*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 2*pixelSize, pos_y + 19*pixelSize + offset, 4*pixelSize, 2*pixelSize);
    }

    // CUERPO (oval diagonal)
    painter.setBrush(colCaparazonOscuro);
    if (mirandoArriba) {
        painter.drawRect(pos_x + 6*pixelSize, pos_y + 6*pixelSize, 8*pixelSize, 14*pixelSize);
    } else {
        painter.drawRect(pos_x + 6*pixelSize, pos_y + 6*pixelSize, 8*pixelSize, 14*pixelSize);
    }

    // SEGMENTOS
    painter.setBrush(colCaparazonClaro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 8*pixelSize, 6*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 13*pixelSize, 6*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 17*pixelSize, 6*pixelSize, 2*pixelSize);

    // BRILLO
    painter.setBrush(colBrillo);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 9*pixelSize, 2*pixelSize, 2*pixelSize);

    // CABEZA
    painter.setBrush(colCaparazonOscuro);
    if (mirandoArriba) {
        painter.drawRect(pos_x + 7*pixelSize, pos_y + 3*pixelSize, 6*pixelSize, 4*pixelSize);
    } else {
        painter.drawRect(pos_x + 7*pixelSize, pos_y + 19*pixelSize, 6*pixelSize, 4*pixelSize);
    }

    // OJOS
    painter.setBrush(colOjos);
    if (mirandoArriba) {
        if (mirandoDerecha) {
            painter.drawRect(pos_x + 10*pixelSize, pos_y + 4*pixelSize, 2*pixelSize, 2*pixelSize);
        } else {
            painter.drawRect(pos_x + 8*pixelSize, pos_y + 4*pixelSize, 2*pixelSize, 2*pixelSize);
        }
    } else {
        if (mirandoDerecha) {
            painter.drawRect(pos_x + 10*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 2*pixelSize);
        } else {
            painter.drawRect(pos_x + 8*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 2*pixelSize);
        }
    }
}

void Cascarudo::actualizar()
{
    if (!vivo) {
        tiempoDesaparecer -= 0.016f; // Restar tiempo si está muerto
        return; // No animar ni mover si está muerto
    }
    // Animación de movimiento (patas)
    if (animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
    } else {
        frameAnimacion -= velocidadAnimacion;
    }

    if (frameAnimacion > 2.0f) {
        animacionSubiendo = false;
    }
    if (frameAnimacion < -2.0f) {
        animacionSubiendo = true;
    }
}

void Cascarudo::moverHacia(QPointF objetivo)
{
    // Calcular dirección hacia el objetivo
    float dx = objetivo.x() - (pos_x + ancho / 2.0f);
    float dy = objetivo.y() - (pos_y + alto / 2.0f);

    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia > 5.0f) {
        // Calcular ángulo de movimiento
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;

        // Normalizar y aplicar velocidad
        dx = (dx / distancia) * velocidad;
        dy = (dy / distancia) * velocidad;

        mover(dx, dy);
    }
}
