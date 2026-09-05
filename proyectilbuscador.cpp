#include "proyectilbuscador.h"

ProyectilBuscador::ProyectilBuscador(float x, float y, float angulo, float velocidad, float danio)
    : pos_x(x), pos_y(y), angulo(angulo), danio(danio), activo(true),
    tiempoVida(3.5f), tiempoVidaMax(3.5f), velocidadGiro(2.0f)
{
    ancho = 8.0f;
    alto = 8.0f;

    vel_x = velocidad * cos(angulo * M_PI / 180.0f);
    vel_y = velocidad * sin(angulo * M_PI / 180.0f);
}

void ProyectilBuscador::dibujar(QPainter &painter)
{
    if(!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.translate(pos_x, pos_y);

    // Aura exterior brillante MAGENTA/ROSA (proyectil buscador)
    QRadialGradient aura(0, 0, 15);
    aura.setColorAt(0, QColor(255, 100, 255, 250));
    aura.setColorAt(0.4, QColor(200, 50, 200, 200));
    aura.setColorAt(0.7, QColor(150, 0, 150, 100));
    aura.setColorAt(1, QColor(100, 0, 100, 0));
    painter.setBrush(aura);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(0, 0), 15, 15);

    // Núcleo brillante central
    QRadialGradient nucleo(0, 0, 8);
    nucleo.setColorAt(0, QColor(255, 220, 255));
    nucleo.setColorAt(0.5, QColor(255, 150, 255));
    nucleo.setColorAt(1, QColor(200, 100, 200));
    painter.setBrush(nucleo);
    painter.drawEllipse(QPointF(0, 0), 8, 8);

    // Punto central super brillante
    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(QPointF(0, 0), 3, 3);

    // Destellos en cruz
    painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
    painter.drawLine(-10, 0, 10, 0);
    painter.drawLine(0, -10, 0, 10);

    // Trail/estela detrás (curva porque va girando)
    painter.setPen(QPen(QColor(255, 100, 255, 150), 3));
    float estelaX = -vel_x * 2.0f;
    float estelaY = -vel_y * 2.0f;
    painter.drawLine(0, 0, estelaX, estelaY);

    painter.restore();
}

void ProyectilBuscador::actualizar()
{
    if(!activo) return;

    pos_x += vel_x;
    pos_y += vel_y;

    tiempoVida -= 0.016f; // ~60 FPS
    if(tiempoVida <= 0) {
        activo = false;
    }
}

void ProyectilBuscador::actualizarHacia(QPointF objetivo)
{
    if(!activo) return;

    // Calcular ángulo hacia el objetivo
    float dx = objetivo.x() - pos_x;
    float dy = objetivo.y() - pos_y;
    float anguloObjetivo = std::atan2(dy, dx) * 180.0f / M_PI;

    // Girar gradualmente hacia el objetivo
    float diferencia = anguloObjetivo - angulo;

    // Normalizar diferencia entre -180 y 180
    while(diferencia > 180.0f) diferencia -= 360.0f;
    while(diferencia < -180.0f) diferencia += 360.0f;

    // Aplicar giro gradual
    if(std::abs(diferencia) < velocidadGiro) {
        angulo = anguloObjetivo;
    } else {
        angulo += (diferencia > 0 ? velocidadGiro : -velocidadGiro);
    }

    // Actualizar velocidades según nuevo ángulo
    float velocidadActual = std::sqrt(vel_x * vel_x + vel_y * vel_y);
    vel_x = velocidadActual * cos(angulo * M_PI / 180.0f);
    vel_y = velocidadActual * sin(angulo * M_PI / 180.0f);
}
