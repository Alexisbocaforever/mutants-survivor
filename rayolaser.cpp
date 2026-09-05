// rayolaser.cpp
#include "rayolaser.h"
#include <cstdlib>

RayoLaser::RayoLaser()
    : origenX(0), origenY(0), angulo(0), longitud(0), grosor(18.0f), danioPorTick(0),
    activo(false), avisando(true), timerAviso(0), timerActivo(0),
    anguloBase(0.0f), rangoOscilacion(0.0f), velocidadOscilacion(0.0f), sentidoOscilacion(1),
    velocidadActual(0.0f)
{
}

void RayoLaser::reiniciar(float ox, float oy, float anguloNuevo, float longitudNueva, float danioNuevo,
                          float tiempoAviso, float tiempoActivo, float grosorNuevo,
                          float rangoOscilacionNuevo, float velocidadOscilacionNueva)
{
    origenX = ox;
    origenY = oy;
    angulo = anguloNuevo;
    anguloBase = anguloNuevo;
    longitud = longitudNueva;
    danioPorTick = danioNuevo;
    grosor = grosorNuevo;

    rangoOscilacion = rangoOscilacionNuevo;
    velocidadOscilacion = velocidadOscilacionNueva;
    // 1 arranca girando para cualquiera de los dos lados al azar
    sentidoOscilacion = (std::rand() % 2 == 0) ? 1 : -1;
    // 2 arranca en velocidad cero y va acelerando, como si el rayo tomara impulso
    velocidadActual = 0.0f;

    activo = true;
    avisando = true;
    timerAviso = tiempoAviso;
    timerActivo = tiempoActivo;
}

void RayoLaser::actualizar()
{
    if (!activo) return;

    if (avisando) {
        timerAviso -= 0.016f;
        if (timerAviso <= 0.0f) avisando = false;
    } else {
        timerActivo -= 0.016f;
        if (timerActivo <= 0.0f) activo = false;

        // 1 mientras esta haciendo danio, si tiene rango de oscilacion barre de un lado
        //   al otro con un movimiento tipo pendulo en vez de velocidad constante
        if (rangoOscilacion > 0.0f) {
            // 2 acerca de a poco la velocidad real a la velocidad objetivo, asi el
            //   rayo toma impulso en vez de arrancar de golpe apenas rebota
            velocidadActual += (velocidadOscilacion - velocidadActual) * 0.12f;

            // 3 cuanto mas cerca esta del limite del barrido, mas frena, como un
            //   pendulo que pierde impulso al llegar a la punta del recorrido
            float diferencia = angulo - anguloBase;
            float cercaniaLimite = std::min(1.0f, std::fabs(diferencia) / rangoOscilacion);
            float factorFrenado = 1.0f - cercaniaLimite * 0.7f;

            angulo += velocidadActual * factorFrenado * sentidoOscilacion;

            diferencia = angulo - anguloBase;
            if (diferencia >= rangoOscilacion) {
                angulo = anguloBase + rangoOscilacion;
                // 4 al llegar al limite elige un sentido horario o antihorario al azar
                //   y vuelve a arrancar despacio para el lado nuevo
                sentidoOscilacion = (std::rand() % 2 == 0) ? 1 : -1;
                velocidadActual = velocidadOscilacion * 0.2f;
            } else if (diferencia <= -rangoOscilacion) {
                angulo = anguloBase - rangoOscilacion;
                sentidoOscilacion = (std::rand() % 2 == 0) ? 1 : -1;
                velocidadActual = velocidadOscilacion * 0.2f;
            }
        }
    }
}

bool RayoLaser::colisionaCon(float px, float py, float radio) const
{
    if (!activo || avisando) return false;

    float anguloRad = angulo * M_PI / 180.0f;
    float finX = origenX + std::cos(anguloRad) * longitud;
    float finY = origenY + std::sin(anguloRad) * longitud;

    float dx = finX - origenX;
    float dy = finY - origenY;
    float largoCuadrado = dx * dx + dy * dy;

    float t = 0.0f;
    if (largoCuadrado > 0.0f) {
        t = ((px - origenX) * dx + (py - origenY) * dy) / largoCuadrado;
        t = std::max(0.0f, std::min(1.0f, t));
    }

    float puntoX = origenX + t * dx;
    float puntoY = origenY + t * dy;
    float distX = px - puntoX;
    float distY = py - puntoY;
    float distancia = std::sqrt(distX * distX + distY * distY);

    return distancia < (grosor / 2.0f + radio);
}

void RayoLaser::dibujar(QPainter &painter)
{
    if (!activo) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(origenX, origenY);
    painter.rotate(angulo);

    if (avisando) {
        painter.setPen(QPen(QColor(255, 40, 40, 160), 2, Qt::DashLine));
        painter.drawLine(QPointF(0, 0), QPointF(longitud, 0));
    } else {
        painter.setPen(Qt::NoPen);

        QLinearGradient nucleo(0, -grosor / 2.0f, 0, grosor / 2.0f);
        nucleo.setColorAt(0, QColor(255, 255, 255, 0));
        nucleo.setColorAt(0.5, QColor(255, 240, 240, 255));
        nucleo.setColorAt(1, QColor(255, 255, 255, 0));
        painter.setBrush(nucleo);
        painter.drawRect(QRectF(0, -grosor / 2.0f, longitud, grosor));

        painter.setBrush(QColor(255, 30, 20, 210));
        painter.drawRect(QRectF(0, -grosor / 2.6f, longitud, grosor / 1.3f));

        painter.setBrush(QColor(255, 255, 255, 230));
        painter.drawRect(QRectF(0, -grosor / 6.0f, longitud, grosor / 3.0f));
    }

    painter.restore();
}
