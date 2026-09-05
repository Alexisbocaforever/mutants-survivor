// pulsar.cpp
#include "pulsar.h"
#include <algorithm>
#include <cmath>

Pulsar::Pulsar(float x, float y)
    : Enemigo(x, y), frameAnimacion(0.0f), timerPulso(3.0f), cargando(false), timerCarga(0.0f)
{
    this->sistemaVida.setVidaMaxima(250.0f);
    this->sistemaVida.restaurarCompleta();
    this->velocidad = 0.9f;
    this->danioContacto = 15.0f;
    this->ancho = 55.0f;
    this->alto = 55.0f;

    // 1 paleta nueva, criatura alienigena bioluminiscente violeta en vez del
    //   gris apagado de antes
    colorCascara = QColor(45, 30, 58);
    colorMembrana = QColor(150, 60, 190);
    colorNucleo = QColor(210, 40, 160);
}
void Pulsar::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }
Pulsar::~Pulsar() { }

// ============================================================
// la logica de ataque de aca abajo no se toco, sigue igual que antes
// ============================================================

void Pulsar::actualizar() { }

void Pulsar::actualizar(float jugadorX, float jugadorY)
{
    onda.actualizar();
    if (!estaVivo()) {
        onda.desactivar(); // Simplemente la desactivas
        return;
    }

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    frameAnimacion += 0.04f;
    if (frameAnimacion > M_PI * 2) frameAnimacion -= M_PI * 2;

    // Se mantiene a media distancia, ni pegado ni lejos
    if (!cargando) {
        if (distancia > 260.0f) {
            pos_x += (dx / distancia) * velocidad;
            pos_y += (dy / distancia) * velocidad;
        } else if (distancia < 160.0f) {
            pos_x -= (dx / distancia) * velocidad;
            pos_y -= (dy / distancia) * velocidad;
        }
    }

    timerPulso -= 0.016f;
    if (!cargando && timerPulso <= 0.0f) {
        cargando = true;
        timerCarga = 1.1f; // aviso antes de soltar la onda
    }

    if (cargando) {
        timerCarga -= 0.016f;
        if (timerCarga <= 0.0f) {
            onda.reiniciar(centroX, centroY, 8.0f, 320.0f, 6.5f, 26.0f);
            cargando = false;
            timerPulso = 4.5f;
        }
    }
}

// ============================================================
// de aca para abajo empieza el rediseno estetico pixel art
// la logica de ataque de arriba no se modifico en nada
// ============================================================

void Pulsar::dibujar(QPainter &p)
{
    if (esCadaver()) return; // NUEVO: movido arriba de todo, antes dibujaba la onda igual

    onda.dibujar(p);

    p.save();
    // 1 sin antialiasing para que los bloques de pixeles queden nitidos
    p.setRenderHint(QPainter::Antialiasing, false);
    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    float flotacion = std::sin(frameAnimacion * 1.5f) * 5.0f;
    p.translate(0, flotacion);

    float ondulacion = std::sin(frameAnimacion * 2.0f) * 4.0f;
    dibujarTentaculos(p, ondulacion);

    float escalaPalpito = 1.0f + std::sin(frameAnimacion * 3.0f) * 0.04f;
    dibujarCascara(p, escalaPalpito);

    float intensidad = cargando ? (1.0f - std::max(0.0f, timerCarga) / 1.1f) : 0.2f;
    dibujarNucleo(p, intensidad);

    p.restore();
}
// ============================================================
// tentaculos pixelados
// cada uno es una cadena de 5 bloques que se van angostando a medida
// que se alejan del cuerpo, alternando el tono de cascara y de membrana
// para que se note la segmentacion, y ondulan siguiendo el mismo
// parametro de ondulacion que antes
// ============================================================
void Pulsar::dibujarTentaculos(QPainter &p, float ondulacion)
{
    p.setPen(Qt::NoPen);
    const int segmentos = 5;

    for (int i = 0; i < 5; i++) {
        float rad = (360.0f / 5 * i) * M_PI / 180.0f;
        float dirX = std::cos(rad);
        float dirY = std::sin(rad);

        for (int s = 0; s < segmentos; s++) {
            float t = static_cast<float>(s) / (segmentos - 1);
            float radio = 19.0f + t * (18.0f + ondulacion);
            float ondaSeg = std::sin(t * 3.0f + ondulacion * 0.3f) * 3.0f * t;

            float px = dirX * radio + (-dirY) * ondaSeg;
            float py = dirY * radio + 12.0f * t + dirX * ondaSeg;

            float tam = 5.5f - t * 2.8f;
            QColor color = (s % 2 == 0) ? colorCascara.darker(125) : colorMembrana.darker(115);
            p.setBrush(color);
            p.drawRect(QRectF(px - tam / 2.0f, py - tam / 2.0f, tam, tam));
        }
    }
}

// ============================================================
// cascara pixelada
// grilla de 15 por 13 recortada en forma de ovalo achatado, con anillos
// concentricos mas claros para simular las costillas del caparazon, un
// borde exterior mas oscuro, una cresta superior de membrana violeta y
// una ventana circular vacia en el centro por donde se asoma el nucleo
// los 3 ojos amarillos se mantienen en la misma disposicion triangular
// que antes pero ahora son bloques cuadrados
// ============================================================
void Pulsar::dibujarCascara(QPainter &p, float escala)
{
    p.save();
    p.scale(escala, escala);

    const int columnas = 15;
    const int filas = 13;
    float pixel = 2.6f;
    float origenX = -columnas * pixel / 2.0f;
    float origenY = -filas * pixel / 2.0f - 2.0f;

    p.setPen(Qt::NoPen);
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            float nx = (columna - columnas / 2.0f + 0.5f) / (columnas / 2.0f);
            float ny = (fila - filas / 2.0f + 0.5f) / (filas / 2.0f);
            float distancia = nx * nx + ny * ny;
            if (distancia > 1.0f) continue; // 1 recorta la grilla con forma de ovalo

            // 2 ventana central por donde se asoma el nucleo de energia
            bool ventanaNucleo = (distancia < 0.16f && ny > -0.35f);
            if (ventanaNucleo) continue;

            bool esCresta = (ny < -0.55f && (columna % 2 == 0));
            bool esBorde = (distancia > 0.82f);
            bool esAnillo = (std::fmod(distancia, 0.14f) < 0.045f);

            QColor color = colorCascara;
            if (esCresta) color = colorMembrana.darker(110);
            else if (esBorde) color = colorCascara.darker(155);
            else if (esAnillo) color = colorMembrana.lighter(120);

            p.setBrush(color);
            p.drawRect(QRectF(origenX + columna * pixel, origenY + fila * pixel, pixel + 0.5f, pixel + 0.5f));
        }
    }

    // 3 tres ojos amarillos en bloques, misma disposicion triangular de siempre
    QPointF posOjos[3] = { QPointF(-9, -3), QPointF(9, -3), QPointF(0, 8) };
    for (auto &o : posOjos) {
        p.setBrush(Qt::yellow);
        p.drawRect(QRectF(o.x() - 2.3f, o.y() - 2.3f, 4.6f, 4.6f));
        p.setBrush(Qt::black);
        p.drawRect(QRectF(o.x() - 1.0f, o.y() - 1.0f, 2.0f, 2.0f));
    }

    p.restore();
}

// ============================================================
// nucleo pixelado
// el halo ya no es un gradiente radial sino anillos cuadrados
// concentricos que crecen con la intensidad de carga, y en el centro
// hay un pequeno cristal de energia armado con bloques en forma de
// rombo que se ilumina cuanto mas cerca esta de soltar la onda
// ============================================================
void Pulsar::dibujarNucleo(QPainter &p, float intensidad)
{
    QColor nucleoColor = colorNucleo;
    nucleoColor.setRed(std::min(255, nucleoColor.red() + (int)(60 * intensidad)));
    nucleoColor.setGreen(std::min(255, nucleoColor.green() + (int)(90 * intensidad)));
    nucleoColor.setBlue(std::min(255, nucleoColor.blue() + (int)(30 * intensidad)));

    p.setPen(Qt::NoPen);

    // 1 halo pixelado en anillos cuadrados concentricos
    for (int anillo = 4; anillo >= 1; anillo--) {
        int alfa = 22 * (5 - anillo);
        QColor c = nucleoColor;
        c.setAlpha(alfa);
        p.setBrush(c);
        float r = (7.0f + intensidad * 7.0f) * anillo * 0.6f;
        p.drawRect(QRectF(-r, -r, r * 2.0f, r * 2.0f));
    }

    // 2 cristal central en forma de rombo, armado con bloques pixelados
    float tam = 3.2f;
    QPointF celdas[] = {
        {0, -2 * tam}, {-tam, -tam}, {tam, -tam},
        {-tam * 1.5f, 0}, {0, 0}, {tam * 1.5f, 0},
        {-tam, tam}, {tam, tam}, {0, 2 * tam}
    };
    p.setBrush(nucleoColor);
    for (auto &c : celdas) {
        p.drawRect(QRectF(c.x() - tam / 2.0f, c.y() - tam / 2.0f, tam, tam));
    }

    p.setBrush(nucleoColor.lighter(160));
    p.drawRect(QRectF(-tam / 2.0f, -2 * tam - tam / 2.0f, tam, tam));
}
