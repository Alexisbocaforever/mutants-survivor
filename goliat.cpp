#include "goliat.h"
#include <cmath>

Goliat::Goliat(float x, float y)
    : Enemigo(x, y), frameAnimacion(0.0f),
    velocidadAnimacion(0.05f), animacionSubiendo(true),
    anguloMovimiento(90.0f)
{

    ancho = 60.0f;
    alto = 60.0f;
    velocidad = 0.8f;
    sistemaVida = Vida(150.0f);  // Goliat con 150 HP
    danioContacto = 15.0f;        // Hace 15 de daño al contacto
}

void Goliat::dibujar(QPainter &painter)
{
    int pixelSize = 3;
    float offset = frameAnimacion * (pixelSize / 2.0f);

    // Normalizar ángulo entre 0 y 360
    float angulo = anguloMovimiento;
    while (angulo < 0) angulo += 360;
    while (angulo >= 360) angulo -= 360;

    // Determinar dirección basada en el ángulo (8 direcciones)
    if (angulo >= 337.5 || angulo < 22.5) {
        dibujarDerecha(painter, offset);
    } else if (angulo >= 22.5 && angulo < 67.5) {
        dibujarDiagonal(painter, offset, 1);
    } else if (angulo >= 67.5 && angulo < 112.5) {
        dibujarAbajo(painter, offset);
    } else if (angulo >= 112.5 && angulo < 157.5) {
        dibujarDiagonal(painter, offset, 2);
    } else if (angulo >= 157.5 && angulo < 202.5) {
        dibujarIzquierda(painter, offset);
    } else if (angulo >= 202.5 && angulo < 247.5) {
        dibujarDiagonal(painter, offset, 3);
    } else if (angulo >= 247.5 && angulo < 292.5) {
        dibujarArriba(painter, offset);
    } else {
        dibujarDiagonal(painter, offset, 4);
    }
}
void Goliat::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }
void Goliat::dibujarArriba(QPainter &painter, float offset)
{
    int pixelSize = 3;
    painter.setPen(Qt::NoPen);

    // Colores base
    QColor cuerpoOscuro(40, 40, 50);        // Gris muy oscuro con tinte azul
    QColor cuerpoMedio(55, 55, 65);         // Gris oscuro medio
    QColor armadura(70, 70, 80);            // Gris armadura
    QColor patas(25, 25, 25);               // Negro casi puro
    QColor ojos(200, 50, 0);                // Rojo naranja brillante

    // Nuevos colores de detalle (píxeles de luz, escamas y sombra)
    QColor luzArmadura(95, 95, 110);        // Borde superior brillante
    QColor escamas(85, 85, 100);            // Patrón de escamas
    QColor brilloOjos(255, 200, 50);        // Núcleo brillante en el ojo
    QColor articulacion(45, 45, 55);        // Detalle en patas

    // PATAS TRASERAS (4 pares)
    painter.setBrush(patas);
    // Par 1
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 22*pixelSize + offset, 3*pixelSize, 10*pixelSize);
    painter.drawRect(pos_x + 25*pixelSize, pos_y + 22*pixelSize - offset, 3*pixelSize, 10*pixelSize);
    // Par 2
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 16*pixelSize - offset, 3*pixelSize, 8*pixelSize);
    painter.drawRect(pos_x + 24*pixelSize, pos_y + 16*pixelSize + offset, 3*pixelSize, 8*pixelSize);
    // Par 3
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 10*pixelSize + offset, 3*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 22*pixelSize, pos_y + 10*pixelSize - offset, 3*pixelSize, 7*pixelSize);
    // Par 4
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 6*pixelSize - offset, 3*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 21*pixelSize, pos_y + 6*pixelSize + offset, 3*pixelSize, 6*pixelSize);

    // Detalle articulación en patas
    painter.setBrush(articulacion);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 22*pixelSize + offset, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 25*pixelSize, pos_y + 22*pixelSize - offset, 3*pixelSize, 1*pixelSize);

    // CUERPO PRINCIPAL
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 10*pixelSize, 16*pixelSize, 20*pixelSize);

    // PLACAS DE ARMADURA
    painter.setBrush(armadura);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 11*pixelSize, 14*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 18*pixelSize, 14*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 24*pixelSize, 14*pixelSize, 5*pixelSize);

    // --- DETALLE: BRILLO EN BORDES DE PLACAS ---
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 11*pixelSize, 14*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 18*pixelSize, 14*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 24*pixelSize, 14*pixelSize, 1*pixelSize);

    // --- DETALLE: PATRÓN DE ESCAMAS EN LA ARMADURA ---
    painter.setBrush(escamas);
    // Placa 1
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 13*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 13*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 18*pixelSize, pos_y + 13*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 15*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 15*pixelSize, 2*pixelSize, 1*pixelSize);
    // Placa 2
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 18*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 21*pixelSize, 2*pixelSize, 1*pixelSize);
    // Placa 3
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 26*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 17*pixelSize, pos_y + 26*pixelSize, 2*pixelSize, 1*pixelSize);

    // RANURAS BASE
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 13*pixelSize, 1*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 19*pixelSize, pos_y + 13*pixelSize, 1*pixelSize, 2*pixelSize);

    // CABEZA
    painter.setBrush(cuerpoMedio);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 4*pixelSize, 12*pixelSize, 7*pixelSize);
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 4*pixelSize, 12*pixelSize, 1*pixelSize);

    // OJOS
    painter.setBrush(ojos);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 6*pixelSize, 3*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 6*pixelSize, 3*pixelSize, 3*pixelSize);
    // --- DETALLE: PUPILA/NÚCLEO BRILLANTE EN EL OJO ---
    painter.setBrush(brilloOjos);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 7*pixelSize, 1*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 17*pixelSize, pos_y + 7*pixelSize, 1*pixelSize, 1*pixelSize);

    // CUERNOS/ESPINAS
    painter.setBrush(patas);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 2*pixelSize, 2*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 19*pixelSize, pos_y + 2*pixelSize, 2*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 1*pixelSize, 2*pixelSize, 4*pixelSize);
    // Punta afilada en cuernos
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 2*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 19*pixelSize, pos_y + 2*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 1*pixelSize, 2*pixelSize, 1*pixelSize);

    // MANDÍBULAS
    painter.setBrush(patas);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 10*pixelSize, 3*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 17*pixelSize, pos_y + 10*pixelSize, 3*pixelSize, 2*pixelSize);
}

void Goliat::dibujarAbajo(QPainter &painter, float offset)
{
    int pixelSize = 3;
    painter.setPen(Qt::NoPen);

    QColor cuerpoOscuro(40, 40, 50);
    QColor armadura(70, 70, 80);
    QColor patas(25, 25, 25);
    QColor luzArmadura(95, 95, 110);
    QColor escamas(85, 85, 100);

    // PATAS
    painter.setBrush(patas);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 2*pixelSize - offset, 3*pixelSize, 10*pixelSize);
    painter.drawRect(pos_x + 25*pixelSize, pos_y + 2*pixelSize + offset, 3*pixelSize, 10*pixelSize);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 10*pixelSize + offset, 3*pixelSize, 8*pixelSize);
    painter.drawRect(pos_x + 24*pixelSize, pos_y + 10*pixelSize - offset, 3*pixelSize, 8*pixelSize);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 16*pixelSize - offset, 3*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 22*pixelSize, pos_y + 16*pixelSize + offset, 3*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 22*pixelSize + offset, 3*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 21*pixelSize, pos_y + 22*pixelSize - offset, 3*pixelSize, 6*pixelSize);

    // CUERPO
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 5*pixelSize, 16*pixelSize, 20*pixelSize);

    // SEGMENTOS DE ARMADURA
    painter.setBrush(armadura);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 6*pixelSize, 14*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 12*pixelSize, 14*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 18*pixelSize, 14*pixelSize, 6*pixelSize);

    // --- DETALLE: LUCES DE BORDE Y ESCAMAS ---
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 6*pixelSize, 14*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 12*pixelSize, 14*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 18*pixelSize, 14*pixelSize, 1*pixelSize);

    painter.setBrush(escamas);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 17*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 14*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 18*pixelSize, pos_y + 20*pixelSize, 2*pixelSize, 1*pixelSize);

    // PARTE TRASERA (abdomen grande)
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 25*pixelSize, 10*pixelSize, 5*pixelSize);

    // Textura en abdomen
    painter.setBrush(escamas);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 26*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + 26*pixelSize, 2*pixelSize, 1*pixelSize);

    // Espinas traseras
    painter.setBrush(patas);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 27*pixelSize, 2*pixelSize, 2*pixelSize);
    painter.drawRect(pos_x + 19*pixelSize, pos_y + 27*pixelSize, 2*pixelSize, 2*pixelSize);
}

void Goliat::dibujarIzquierda(QPainter &painter, float offset)
{
    int pixelSize = 3;
    painter.setPen(Qt::NoPen);

    QColor cuerpoOscuro(40, 40, 50);
    QColor cuerpoMedio(55, 55, 65);
    QColor armadura(70, 70, 80);
    QColor patas(25, 25, 25);
    QColor ojos(200, 50, 0);
    QColor luzArmadura(95, 95, 110);
    QColor escamas(85, 85, 100);
    QColor brilloOjos(255, 200, 50);

    // PATAS
    painter.setBrush(patas);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 8*pixelSize + offset, 8*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 16*pixelSize - offset, 7*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 24*pixelSize + offset, 8*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 32*pixelSize - offset, 7*pixelSize, 3*pixelSize);

    // CUERPO
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 6*pixelSize, 18*pixelSize, 26*pixelSize);

    // SEGMENTOS LATERALES
    painter.setBrush(armadura);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 8*pixelSize, 16*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 16*pixelSize, 16*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 23*pixelSize, 16*pixelSize, 7*pixelSize);

    // --- DETALLE: LUCES LATERALES Y ESCAMAS ---
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 8*pixelSize, 16*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 16*pixelSize, 16*pixelSize, 1*pixelSize);

    painter.setBrush(escamas);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 11*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 18*pixelSize, pos_y + 11*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 19*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 26*pixelSize, 3*pixelSize, 1*pixelSize);

    // CABEZA
    painter.setBrush(cuerpoMedio);
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 10*pixelSize, 7*pixelSize, 8*pixelSize);

    // OJO Y DETALLE
    painter.setBrush(ojos);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 13*pixelSize, 3*pixelSize, 3*pixelSize);
    painter.setBrush(brilloOjos);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 14*pixelSize, 1*pixelSize, 1*pixelSize);

    // CUERNO LATERAL
    painter.setBrush(patas);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 10*pixelSize, 3*pixelSize, 2*pixelSize);
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 10*pixelSize, 1*pixelSize, 2*pixelSize);
}

void Goliat::dibujarDerecha(QPainter &painter, float offset)
{
    int pixelSize = 3;
    painter.setPen(Qt::NoPen);

    QColor cuerpoOscuro(40, 40, 50);
    QColor cuerpoMedio(55, 55, 65);
    QColor armadura(70, 70, 80);
    QColor patas(25, 25, 25);
    QColor ojos(200, 50, 0);
    QColor luzArmadura(95, 95, 110);
    QColor escamas(85, 85, 100);
    QColor brilloOjos(255, 200, 50);

    // PATAS
    painter.setBrush(patas);
    painter.drawRect(pos_x + 20*pixelSize, pos_y + 8*pixelSize - offset, 8*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 21*pixelSize, pos_y + 16*pixelSize + offset, 7*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 20*pixelSize, pos_y + 24*pixelSize - offset, 8*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 20*pixelSize, pos_y + 32*pixelSize + offset, 7*pixelSize, 3*pixelSize);

    // CUERPO
    painter.setBrush(cuerpoOscuro);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 6*pixelSize, 18*pixelSize, 26*pixelSize);

    // SEGMENTOS LATERALES
    painter.setBrush(armadura);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 8*pixelSize, 16*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 16*pixelSize, 16*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 23*pixelSize, 16*pixelSize, 7*pixelSize);

    // --- DETALLE: LUCES LATERALES Y ESCAMAS ---
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 8*pixelSize, 16*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 16*pixelSize, 16*pixelSize, 1*pixelSize);

    painter.setBrush(escamas);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 11*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 13*pixelSize, pos_y + 11*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 19*pixelSize, 3*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 26*pixelSize, 3*pixelSize, 1*pixelSize);

    // CABEZA
    painter.setBrush(cuerpoMedio);
    painter.drawRect(pos_x + 20*pixelSize, pos_y + 10*pixelSize, 7*pixelSize, 8*pixelSize);

    // OJO Y DETALLE
    painter.setBrush(ojos);
    painter.drawRect(pos_x + 22*pixelSize, pos_y + 13*pixelSize, 3*pixelSize, 3*pixelSize);
    painter.setBrush(brilloOjos);
    painter.drawRect(pos_x + 24*pixelSize, pos_y + 14*pixelSize, 1*pixelSize, 1*pixelSize);

    // CUERNO LATERAL
    painter.setBrush(patas);
    painter.drawRect(pos_x + 25*pixelSize, pos_y + 10*pixelSize, 3*pixelSize, 2*pixelSize);
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 27*pixelSize, pos_y + 10*pixelSize, 1*pixelSize, 2*pixelSize);
}

void Goliat::dibujarDiagonal(QPainter &painter, float offset, int direccion)
{
    int pixelSize = 3;
    painter.setPen(Qt::NoPen);

    QColor cuerpoOscuro(40, 40, 50);
    QColor cuerpoMedio(55, 55, 65);
    QColor armadura(70, 70, 80);
    QColor patas(25, 25, 25);
    QColor ojos(200, 50, 0);
    QColor luzArmadura(95, 95, 110);
    QColor escamas(85, 85, 100);
    QColor brilloOjos(255, 200, 50);

    bool mirandoDerecha = (direccion == 1 || direccion == 4);
    bool mirandoArriba = (direccion == 3 || direccion == 4);

    // PATAS
    painter.setBrush(patas);
    if (mirandoDerecha) {
        painter.drawRect(pos_x + 22*pixelSize, pos_y + 9*pixelSize - offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 22*pixelSize, pos_y + 17*pixelSize + offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 22*pixelSize, pos_y + 25*pixelSize - offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 22*pixelSize, pos_y + 33*pixelSize + offset, 5*pixelSize, 3*pixelSize);
    } else {
        painter.drawRect(pos_x + 3*pixelSize, pos_y + 9*pixelSize + offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 3*pixelSize, pos_y + 17*pixelSize - offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 3*pixelSize, pos_y + 25*pixelSize + offset, 5*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 3*pixelSize, pos_y + 33*pixelSize - offset, 5*pixelSize, 3*pixelSize);
    }

    // CUERPO
    painter.setBrush(cuerpoOscuro);
    if (mirandoArriba) {
        painter.drawRect(pos_x + 9*pixelSize, pos_y + 8*pixelSize, 12*pixelSize, 20*pixelSize);
    } else {
        painter.drawRect(pos_x + 9*pixelSize, pos_y + 10*pixelSize, 12*pixelSize, 20*pixelSize);
    }

    // SEGMENTOS
    painter.setBrush(armadura);
    int offsetY = mirandoArriba ? 0 : 2;
    painter.drawRect(pos_x + 10*pixelSize, pos_y + (10 + offsetY)*pixelSize, 10*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + (17 + offsetY)*pixelSize, 10*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + (23 + offsetY)*pixelSize, 10*pixelSize, 4*pixelSize);

    // --- DETALLE: ESCAMAS Y BRILLOS EN DIAGONAL ---
    painter.setBrush(luzArmadura);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + (10 + offsetY)*pixelSize, 10*pixelSize, 1*pixelSize);

    painter.setBrush(escamas);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + (12 + offsetY)*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 16*pixelSize, pos_y + (12 + offsetY)*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + (19 + offsetY)*pixelSize, 2*pixelSize, 1*pixelSize);

    // CABEZA
    painter.setBrush(cuerpoMedio);
    if (mirandoArriba) {
        painter.drawRect(pos_x + 10*pixelSize, pos_y + 4*pixelSize, 10*pixelSize, 5*pixelSize);
    } else {
        painter.drawRect(pos_x + 10*pixelSize, pos_y + 30*pixelSize, 10*pixelSize, 5*pixelSize);
    }

    // OJOS Y DETALLE
    painter.setBrush(ojos);
    int ojoX = mirandoDerecha ? 15 : 12;
    int ojoY = mirandoArriba ? 5 : 31;
    painter.drawRect(pos_x + ojoX*pixelSize, pos_y + ojoY*pixelSize, 3*pixelSize, 3*pixelSize);

    painter.setBrush(brilloOjos);
    painter.drawRect(pos_x + (ojoX + 1)*pixelSize, pos_y + (ojoY + 1)*pixelSize, 1*pixelSize, 1*pixelSize);

    // CUERNO
    painter.setBrush(patas);
    if (mirandoArriba) {
        painter.drawRect(pos_x + 13*pixelSize, pos_y + 2*pixelSize, 3*pixelSize, 3*pixelSize);
        painter.setBrush(luzArmadura);
        painter.drawRect(pos_x + 14*pixelSize, pos_y + 2*pixelSize, 1*pixelSize, 1*pixelSize);
    }
}

void Goliat::actualizar()
{
    if (!vivo) {
        tiempoDesaparecer -= 0.016f; // Restar tiempo si está muerto
        return; // No animar ni mover si está muerto
    }
    // Animación de movimiento
    if (animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
    } else {
        frameAnimacion -= velocidadAnimacion;
    }

    if (frameAnimacion > 1.5f) {
        animacionSubiendo = false;
    }
    if (frameAnimacion < -1.5f) {
        animacionSubiendo = true;
    }
}

void Goliat::moverHacia(QPointF objetivo)
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
