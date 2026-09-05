#include "mutante.h"
#include <cmath>

Mutante::Mutante(float x, float y)
    : Enemigo(x, y), frameAnimacion(0.0f),
    velocidadAnimacion(0.08f), animacionSubiendo(true),
    anguloMovimiento(90.0f), anguloTentaculo1(0.0f),
    anguloTentaculo2(120.0f), anguloTentaculo3(240.0f),
    velocidadTentaculos(0.15f)
{
    ancho = 50.0f;
    alto = 60.0f;
    velocidad = 1.8f;
    sistemaVida = Vida(300.0f);  // Jefe con mucha vida
    danioContacto = 40.0f;       // Daño alto por ser jefe
}
void Mutante::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }
void Mutante::dibujar(QPainter &painter)
{
    if (!estaVivo()) return;

    int pixelSize = 2;
    float offset = frameAnimacion * (pixelSize / 2.0f);

    painter.save();
    painter.setPen(Qt::NoPen);

    // Dibujar componentes en orden: tentáculos -> cuerpo -> cabeza
    dibujarTentaculos(painter);
    dibujarCuerpo(painter);
    dibujarCabeza(painter);
    dibujarBarraVida(painter);

    painter.restore();
}

void Mutante::dibujarTentaculos(QPainter &painter)
{
    int pixelSize = 2;
    QColor violetaTentaculo(100, 50, 150);
    QColor violetaOscuro(80, 40, 120);

    painter.setBrush(violetaTentaculo);

    // Tentáculo 1 (izquierdo) - con animación ondulante
    float desvioT1 = sin(anguloTentaculo1 * M_PI / 180.0f) * 2;
    painter.drawRect(pos_x + 2*pixelSize + desvioT1, pos_y + 12*pixelSize, 4*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 1*pixelSize + desvioT1, pos_y + 15*pixelSize, 3*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 0*pixelSize + desvioT1*1.5f, pos_y + 19*pixelSize, 2*pixelSize, 3*pixelSize);

    // Tentáculo 2 (centro-superior) - con animación ondulante
    float desvioT2 = sin(anguloTentaculo2 * M_PI / 180.0f) * 2;
    painter.drawRect(pos_x + 11*pixelSize, pos_y + 6*pixelSize + desvioT2, 6*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 3*pixelSize + desvioT2*1.5f, 4*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 13*pixelSize, pos_y + 0*pixelSize + desvioT2*2, 2*pixelSize, 3*pixelSize);

    // Tentáculo 3 (derecho) - con animación ondulante
    float desvioT3 = sin(anguloTentaculo3 * M_PI / 180.0f) * 2;
    painter.drawRect(pos_x + 22*pixelSize - desvioT3, pos_y + 13*pixelSize, 4*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 24*pixelSize - desvioT3, pos_y + 16*pixelSize, 3*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 26*pixelSize - desvioT3*1.5f, pos_y + 20*pixelSize, 2*pixelSize, 3*pixelSize);

    // Puntas de tentáculos (más oscuras)
    painter.setBrush(violetaOscuro);
    painter.drawEllipse(QPointF(pos_x + 1*pixelSize + desvioT1*1.5f, pos_y + 21*pixelSize), 2, 2);
    painter.drawEllipse(QPointF(pos_x + 14*pixelSize, pos_y + 1*pixelSize + desvioT2*2), 2, 2);
    painter.drawEllipse(QPointF(pos_x + 27*pixelSize - desvioT3*1.5f, pos_y + 22*pixelSize), 2, 2);
}

void Mutante::dibujarCuerpo(QPainter &painter)
{
    int pixelSize = 2;
    float offset = frameAnimacion * (pixelSize / 2.0f);

    QColor violetaOscuro(80, 40, 120);
    QColor violetaClaro(120, 60, 180);
    QColor negro(20, 10, 30);

    // PIERNAS (con animación de movimiento)
    painter.setBrush(violetaOscuro);
    painter.drawRect(pos_x + 8*pixelSize, pos_y + 22*pixelSize + offset, 5*pixelSize, 12*pixelSize);
    painter.drawRect(pos_x + 15*pixelSize, pos_y + 22*pixelSize - offset, 5*pixelSize, 12*pixelSize);

    // Pies más oscuros
    painter.setBrush(negro);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 33*pixelSize + offset, 6*pixelSize, 3*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 33*pixelSize - offset, 6*pixelSize, 3*pixelSize);

    // CUERPO PRINCIPAL (grande y robusto)
    painter.setBrush(violetaClaro);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 10*pixelSize, 16*pixelSize, 14*pixelSize);

    // Detalles del cuerpo (manchas más oscuras)
    painter.setBrush(violetaOscuro);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 13*pixelSize, 4*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 15*pixelSize, pos_y + 16*pixelSize, 4*pixelSize, 4*pixelSize);
}

void Mutante::dibujarCabeza(QPainter &painter)
{
    int pixelSize = 2;
    QColor violetaOscuro(80, 40, 120);

    // CABEZA / OJO CENTRAL
    painter.setBrush(violetaOscuro);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 8*pixelSize, 8*pixelSize, 6*pixelSize);

    // Ojo brillante (rojo)
    painter.setBrush(QColor(200, 0, 0));
    painter.drawEllipse(QPointF(pos_x + 14*pixelSize, pos_y + 11*pixelSize), 3, 3);
    painter.setBrush(QColor(255, 50, 50));
    painter.drawEllipse(QPointF(pos_x + 14*pixelSize, pos_y + 11*pixelSize), 1.5, 1.5);
}

void Mutante::dibujarBarraVida(QPainter &painter)
{
    if (!sistemaVida.estaVivo()) return;

    float barraAncho = 50.0f;
    float barraAlto = 5.0f;
    float barraX = pos_x;
    float barraY = pos_y - 10.0f;

    painter.setBrush(QColor(100, 0, 0));
    painter.drawRect(barraX, barraY, barraAncho, barraAlto);

    float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
    QColor colorVida = sistemaVida.estaCritico() ? QColor(200, 50, 0) : QColor(150, 0, 200);
    painter.setBrush(colorVida);
    painter.drawRect(barraX, barraY, barraAncho * porcentaje, barraAlto);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(barraX, barraY, barraAncho, barraAlto);
}

void Mutante::actualizar()
{
    if (!vivo) {
        tiempoDesaparecer -= 0.016f; // Restar tiempo si está muerto
        return; // No animar ni mover si está muerto
    }
    // Actualizar animación de piernas
    if (animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
    } else {
        frameAnimacion -= velocidadAnimacion;
    }

    if (frameAnimacion > 3.0f) {
        animacionSubiendo = false;
    }
    if (frameAnimacion < -3.0f) {
        animacionSubiendo = true;
    }

    // Actualizar animación de tentáculos (cada uno con fase diferente)
    anguloTentaculo1 += velocidadTentaculos * 60;
    anguloTentaculo2 += velocidadTentaculos * 60;
    anguloTentaculo3 += velocidadTentaculos * 60;

    if (anguloTentaculo1 >= 360.0f) anguloTentaculo1 -= 360.0f;
    if (anguloTentaculo2 >= 360.0f) anguloTentaculo2 -= 360.0f;
    if (anguloTentaculo3 >= 360.0f) anguloTentaculo3 -= 360.0f;
}

void Mutante::moverHacia(QPointF objetivo)
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
#define _USE_MATH_DEFINES
