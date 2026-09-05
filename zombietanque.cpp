// zombietanque.cpp

#include "qpainterpath.h"
#include "qpoint.h"
#include "zombietanque.h"
#include <cmath>
#include <algorithm>

ZombieTanque::ZombieTanque(float x, float y)
    : Enemigo(x, y), anguloMovimiento(0.0f), frameAnimacion(0.0f), timerDisparo(0.0f),
    timerLanzallamasTanque(3.0f), duracionLlamarada(0.0f), lanzandoLlamas(false), anguloLlamarada(0.0f),
    timerRayoLaser(4.0f)
{
    this->sistemaVida.setVidaMaxima(800.0f);
    this->sistemaVida.restaurarCompleta();

    this->velocidad = 0.4f;
    this->danioContacto = 30.0f;

    this->ancho = 140.0f;
    this->alto = 120.0f;

    colorMetal = QColor(70, 80, 70);
    colorMetalOxidado = QColor(100, 60, 40);
    colorCerebro = QColor(220, 110, 150);
    colorSangre = QColor(138, 3, 3);
    colorOrugas = QColor(30, 30, 30);
    colorCarneMuerta = QColor(100, 120, 90);
}
ZombieTanque::~ZombieTanque() {
    for (auto p : misiles) delete p;
    misiles.clear();
}
void ZombieTanque::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }

// ============================================================
// las funciones de aca abajo son las de ataque y logica de combate
// no se tocaron, siguen exactamente igual que antes
// ============================================================

void ZombieTanque::actualizar() { }

void ZombieTanque::actualizar(float jugadorX, float jugadorY) {
    if (!estaVivo()) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
    if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

    frameAnimacion += 0.05f;
    if (frameAnimacion > M_PI * 2) frameAnimacion -= M_PI * 2;

    if (distancia > 80.0f) {
        pos_x += (dx / distancia) * velocidad;
        pos_y += (dy / distancia) * velocidad;
    }

    timerDisparo += 0.016f;
    if (timerDisparo >= 5.0f && distancia < 600.0f) {
        float spawnX = centroX + std::cos(anguloMovimiento * M_PI / 180.0f) * 60.0f;
        float spawnY = centroY + std::sin(anguloMovimiento * M_PI / 180.0f) * 60.0f;
        misiles.push_back(new ProyectilBuscador(spawnX, spawnY, anguloMovimiento, 3.5f, 20.0f));
        timerDisparo = 0.0f;
    }

    for (size_t i = 0; i < misiles.size(); i++) {
        misiles[i]->actualizarHacia(QPointF(jugadorX, jugadorY));
        misiles[i]->actualizar();
        if (!misiles[i]->estaActivo()) {
            delete misiles[i];
            misiles.erase(misiles.begin() + i);
            i--;
        }
    }

    timerLanzallamasTanque -= 0.016f;
    if (!lanzandoLlamas && timerLanzallamasTanque <= 0.0f && distancia < 260.0f) {
        dispararLlamarada(jugadorX, jugadorY);
        timerLanzallamasTanque = 5.5f;
    }
    actualizarLlamarada();

    // NUEVO: rayo laser de largo alcance
    timerRayoLaser -= 0.016f;
    if (!rayoLaser.estaActivo() && timerRayoLaser <= 0.0f && distancia < 500.0f) {
        float anguloRayo = std::atan2(dy, dx) * 180.0f / M_PI;
        rayoLaser.reiniciar(centroX, centroY, anguloRayo, 500.0f, 10.0f);
        timerRayoLaser = 7.0f;
    }
    rayoLaser.actualizar();
}

void ZombieTanque::dispararLlamarada(float jugadorX, float jugadorY)
{
    lanzandoLlamas = true;
    duracionLlamarada = 1.4f;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    anguloLlamarada = std::atan2(jugadorY - centroY, jugadorX - centroX) * 180.0f / M_PI;
}

void ZombieTanque::actualizarLlamarada()
{
    if (lanzandoLlamas) {
        duracionLlamarada -= 0.016f;
        if (duracionLlamarada <= 0.0f) {
            lanzandoLlamas = false;
        } else {
            float centroX = pos_x + ancho / 2.0f;
            float centroY = pos_y + alto / 2.0f;

            for (int i = 0; i < 3; i++) {
                float dispersion = -18.0f + (rand() % 361) / 10.0f;
                float anguloParticula = (anguloLlamarada + dispersion) * M_PI / 180.0f;
                float velocidad = 6.0f + (rand() % 40) / 10.0f;

                ParticulaLlamaTanque part;
                part.x = centroX;
                part.y = centroY;
                part.vx = std::cos(anguloParticula) * velocidad;
                part.vy = std::sin(anguloParticula) * velocidad;
                part.vida = 0.4f;
                part.vidaMax = 0.4f;
                part.tam = 6.0f + (rand() % 6);
                particulasLlama.append(part);
            }
        }
    }

    for (int i = particulasLlama.size() - 1; i >= 0; i--) {
        particulasLlama[i].x += particulasLlama[i].vx;
        particulasLlama[i].y += particulasLlama[i].vy;
        particulasLlama[i].vida -= 0.016f;
        if (particulasLlama[i].vida <= 0.0f) {
            particulasLlama.removeAt(i);
        }
    }
}

QList<QPointF> ZombieTanque::getAreaLlamarada() const
{
    QList<QPointF> puntos;
    for (const auto& part : particulasLlama) puntos.append(QPointF(part.x, part.y));
    return puntos;
}

void ZombieTanque::dibujarLlamarada(QPainter &p)
{
    if (particulasLlama.isEmpty()) return;

    p.save();
    p.setPen(Qt::NoPen);
    for (const auto& part : particulasLlama) {
        float alpha = part.vida / part.vidaMax;
        p.setBrush(QColor(255, 90, 20, (int)(150 * alpha)));
        p.drawEllipse(QPointF(part.x, part.y), part.tam, part.tam);
        p.setBrush(QColor(255, 220, 80, (int)(220 * alpha)));
        p.drawEllipse(QPointF(part.x, part.y), part.tam * 0.5f, part.tam * 0.5f);
    }
    p.restore();
}

// ============================================================
// de aca para abajo empieza el rediseno estetico pixel art
// la logica de ataque de arriba no se modifico en nada
// ============================================================

void ZombieTanque::dibujar(QPainter &p) {
    if (esCadaver()) return;

    p.save();
    // 1 sin antialiasing para que los bloques de pixeles se vean nitidos y no
    //   se difuminen los bordes, asi se mantiene la estetica pixel art
    p.setRenderHint(QPainter::Antialiasing, false);

    for (auto misil : misiles) misil->dibujar(p);
    dibujarLlamarada(p);
    rayoLaser.dibujar(p); // NUEVO

    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    float offsetOrugas = std::fmod(frameAnimacion * 10.0f, 15.0f);
    float escalaPalpito = 1.0f + std::sin(frameAnimacion * 3.0f) * 0.05f;

    if (anguloMovimiento >= 315 || anguloMovimiento < 45) {
        dibujarDerecha(p, offsetOrugas);
    } else if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        dibujarAbajo(p, offsetOrugas);
    } else if (anguloMovimiento >= 135 && anguloMovimiento < 225) {
        dibujarIzquierda(p, offsetOrugas);
    } else {
        dibujarArriba(p, offsetOrugas);
    }

    dibujarCerebro(p, escalaPalpito, lanzandoLlamas);

    p.restore();
}

// ============================================================
// relleno pixelado reutilizable
// dibuja un bloque solido armado con una grilla de cuadrados del tamano
// "pixel", alternando el color base con un tono mas claro y otro mas
// oscuro para simular el ruido de una chapa metalica pixel art
// se usa siempre puesto dentro de un setClipPath o setClipRect previo,
// asi el bloque queda recortado con la silueta real de la pieza (chasis,
// torreta, carne, etc) sin necesidad de repetir la forma cada vez
// ============================================================
void ZombieTanque::dibujarBloquePixelado(QPainter &p, float x, float y, float ancho, float alto,
                                         QColor colorBase, float pixel)
{
    QColor tonoClaro = colorBase.lighter(125);
    QColor tonoOscuro = colorBase.darker(125);

    int columnas = static_cast<int>(std::ceil(ancho / pixel)) + 1;
    int filas = static_cast<int>(std::ceil(alto / pixel)) + 1;

    p.setPen(Qt::NoPen);
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            float px = x + columna * pixel;
            float py = y + fila * pixel;

            bool esClaro = ((fila + columna * 2) % 9 == 0);
            bool esOscuro = ((fila * 3 + columna) % 11 == 0);

            QColor color = esClaro ? tonoClaro : (esOscuro ? tonoOscuro : colorBase);
            p.setBrush(color);
            p.drawRect(QRectF(px, py, pixel + 0.6f, pixel + 0.6f));
        }
    }
}

void ZombieTanque::dibujarFranjasPeligro(QPainter &p, float x, float y, float ancho, float alto)
{
    // 1 franjas de peligro en bloques cuadrados alternados, mas acorde
    //   al resto del diseno pixel art que la version diagonal anterior
    p.save();
    p.setClipRect(x, y, ancho, alto);
    p.setPen(Qt::NoPen);

    float tam = alto;
    int columnas = static_cast<int>(std::ceil(ancho / tam)) + 2;
    for (int i = -1; i < columnas; i++) {
        bool esAmarillo = (i % 2 == 0);
        p.setBrush(esAmarillo ? QColor(230, 190, 20, 200) : QColor(20, 20, 20, 200));
        p.drawRect(QRectF(x + i * tam, y, tam, alto));
    }
    p.restore();
}

void ZombieTanque::dibujarOrugasLaterales(QPainter &p, float offsetOrugas)
{
    QPainterPath orugaPath;
    orugaPath.addRoundedRect(-55, -20, 110, 40, 8, 8);

    p.save();
    p.setClipPath(orugaPath);
    dibujarBloquePixelado(p, -55, -20, 110, 40, QColor(35, 35, 35), 5.0f);
    p.restore();

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::black, 3));
    p.drawPath(orugaPath);

    // 1 ruedas cuadradas en vez de circulos, siguiendo la estetica pixel art
    p.setPen(QPen(Qt::black, 2));
    for (int i = -45; i <= 45; i += 22) {
        p.setBrush(QColor(60, 60, 65));
        p.drawRect(QRectF(i - 11, -11, 22, 22));
        p.setBrush(QColor(100, 100, 105));
        p.drawRect(QRectF(i - 4, -4, 8, 8));
    }

    p.setPen(QPen(QColor(120, 120, 120), 3));
    for (int i = -45; i <= 45; i += 15) {
        float x = i - offsetOrugas;
        if (x >= -50 && x <= 50) p.drawLine(QPointF(x, -20), QPointF(x, -15));
    }
}

void ZombieTanque::dibujarOrugasFrontales(QPainter &p, float offsetOrugas)
{
    QPainterPath orugaIzq, orugaDer;
    orugaIzq.addRoundedRect(-60, -25, 30, 70, 8, 8);
    orugaDer.addRoundedRect(30, -25, 30, 70, 8, 8);

    p.save();
    p.setClipPath(orugaIzq);
    dibujarBloquePixelado(p, -60, -25, 30, 70, QColor(35, 35, 35), 5.0f);
    p.restore();
    p.save();
    p.setClipPath(orugaDer);
    dibujarBloquePixelado(p, 30, -25, 30, 70, QColor(35, 35, 35), 5.0f);
    p.restore();

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::black, 3));
    p.drawPath(orugaIzq);
    p.drawPath(orugaDer);

    p.setPen(QPen(QColor(120, 120, 120), 3));
    for (int y = -20; y <= 40; y += 15) {
        float yLine = y + offsetOrugas;
        if (yLine >= -25 && yLine <= 45) {
            p.drawLine(QPointF(-60, yLine), QPointF(-30, yLine));
            p.drawLine(QPointF(30, yLine), QPointF(60, yLine));
        }
    }
}

void ZombieTanque::dibujarIzquierda(QPainter &p, float offsetOrugas) {
    dibujarOrugasLaterales(p, offsetOrugas);

    QPainterPath chasisPath;
    chasisPath.addRoundedRect(-45, -30, 90, 50, 6, 6);
    p.save();
    p.setClipPath(chasisPath);
    dibujarBloquePixelado(p, -45, -30, 90, 50, colorMetal.lighter(125), 5.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorMetalOxidado, 3));
    p.drawPath(chasisPath);

    dibujarFranjasPeligro(p, -45, 12, 90, 8);

    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    QPointF remaches[] = {{-35, -20}, {35, -20}, {-35, 5}, {35, 5}};
    for (auto &r : remaches) {
        p.drawRect(QRectF(r.x() - 3, r.y() - 3, 6, 6));
        p.setBrush(QColor(150, 150, 150));
        p.drawRect(QRectF(r.x() - 1.5, r.y() - 1.5, 3, 3));
        p.setBrush(Qt::black);
    }

    p.setPen(QPen(colorMetal.lighter(200), 1));
    p.drawLine(-45, -29, 44, -29);

    dibujarCanon(p, 180.0f, 55);

    QPainterPath carnePath;
    carnePath.addEllipse(QPointF(0, -5), 35, 20);
    p.save();
    p.setClipPath(carnePath);
    dibujarBloquePixelado(p, -35, -25, 70, 40, colorCarneMuerta, 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorSangre.darker(120), 1));
    p.drawEllipse(QPointF(0, -5), 35, 20);

    dibujarDetallesSangre(p);
}

void ZombieTanque::dibujarDerecha(QPainter &p, float offsetOrugas) {
    dibujarOrugasLaterales(p, offsetOrugas);

    QPainterPath chasisPath;
    chasisPath.addRoundedRect(-45, -30, 90, 50, 6, 6);
    p.save();
    p.setClipPath(chasisPath);
    dibujarBloquePixelado(p, -45, -30, 90, 50, colorMetal.darker(125), 5.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorMetalOxidado, 3));
    p.drawPath(chasisPath);

    dibujarFranjasPeligro(p, -45, 12, 90, 8);

    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    QPointF remaches[] = {{-35, -20}, {35, -20}, {-35, 5}, {35, 5}};
    for (auto &r : remaches) {
        p.drawRect(QRectF(r.x() - 3, r.y() - 3, 6, 6));
        p.setBrush(QColor(150, 150, 150));
        p.drawRect(QRectF(r.x() - 1.5, r.y() - 1.5, 3, 3));
        p.setBrush(Qt::black);
    }

    p.setPen(QPen(colorMetal.lighter(200), 1));
    p.drawLine(-44, -29, 45, -29);

    dibujarCanon(p, 0.0f, 55);

    QPainterPath carnePath;
    carnePath.addEllipse(QPointF(0, -5), 35, 20);
    p.save();
    p.setClipPath(carnePath);
    dibujarBloquePixelado(p, -35, -25, 70, 40, colorCarneMuerta, 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorSangre.darker(120), 1));
    p.drawEllipse(QPointF(0, -5), 35, 20);

    dibujarDetallesSangre(p);
}

void ZombieTanque::dibujarAbajo(QPainter &p, float offsetOrugas) {
    dibujarOrugasFrontales(p, offsetOrugas);

    QPainterPath chasis;
    chasis.moveTo(-40, -25);
    chasis.lineTo(40, -25);
    chasis.lineTo(45, 30);
    chasis.lineTo(-45, 30);
    chasis.closeSubpath();

    p.save();
    p.setClipPath(chasis);
    dibujarBloquePixelado(p, -45, -25, 90, 55, colorMetal.lighter(115), 5.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorMetalOxidado, 4));
    p.drawPath(chasis);

    dibujarFranjasPeligro(p, -40, -25, 80, 8);

    p.setBrush(QColor(15, 15, 15));
    p.setPen(QPen(colorMetalOxidado, 1));
    p.drawRect(-20, 10, 40, 12);
    for (int i = -15; i <= 15; i += 5) p.drawLine(i, 10, i, 22);

    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    p.drawRect(QRectF(-27, -17, 4, 4));
    p.drawRect(QRectF(23, -17, 4, 4));
    p.drawRect(QRectF(-27, 18, 4, 4));
    p.drawRect(QRectF(23, 18, 4, 4));

    dibujarCanon(p, 90.0f, 35);

    QPainterPath carneClip;
    carneClip.addEllipse(QPointF(-25, -20), 15, 10);
    carneClip.addEllipse(QPointF(25, -20), 15, 10);
    p.save();
    p.setClipPath(carneClip);
    dibujarBloquePixelado(p, -40, -30, 90, 20, colorCarneMuerta, 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorSangre.darker(120), 1));
    p.drawEllipse(QPointF(-25, -20), 15, 10);
    p.drawEllipse(QPointF(25, -20), 15, 10);

    dibujarDetallesSangre(p);
}

void ZombieTanque::dibujarArriba(QPainter &p, float offsetOrugas) {
    dibujarOrugasFrontales(p, offsetOrugas);

    QRectF chasisRect(-40, -25, 80, 55);
    p.save();
    p.setClipRect(chasisRect);
    dibujarBloquePixelado(p, -40, -25, 80, 55, colorMetal.darker(135), 5.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorMetalOxidado, 4));
    p.drawRect(chasisRect);

    dibujarFranjasPeligro(p, -40, -25, 80, 8);

    p.setBrush(QColor(25, 25, 25));
    p.setPen(QPen(Qt::black, 2));
    p.drawRect(-30, 15, 15, 25);
    p.drawRect(15, 15, 15, 25);

    // Humo animado, ahora en bloques cuadrados para mantener el estilo pixel art
    float subeHumo = std::fmod(frameAnimacion * 20.0f, 20.0f);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(130, 130, 130, (int)(130 * (1.0f - subeHumo / 20.0f))));
    p.drawRect(QRectF(-27, 30 - subeHumo, 10, 10));
    p.drawRect(QRectF(17, 30 - subeHumo, 10, 10));
    p.setBrush(QColor(255, 150, 50, 100));
    p.drawRect(QRectF(-25, 37, 6, 6));
    p.drawRect(QRectF(19, 37, 6, 6));

    dibujarCanon(p, 270.0f, 35);

    QPainterPath carnePath;
    carnePath.addEllipse(QPointF(0, -20), 30, 15);
    p.save();
    p.setClipPath(carnePath);
    dibujarBloquePixelado(p, -30, -35, 60, 30, colorCarneMuerta, 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(colorSangre.darker(120), 1));
    p.drawEllipse(QPointF(0, -20), 30, 15);

    dibujarDetallesSangre(p);
}

void ZombieTanque::dibujarCanon(QPainter &p, float angulo, float largo) {
    p.save();
    p.translate(0, -5);
    p.rotate(angulo);

    QPainterPath basePath;
    basePath.addEllipse(QPointF(10, 0), 18, 18);
    p.save();
    p.setClipPath(basePath);
    dibujarBloquePixelado(p, -8, -18, 36, 36, QColor(160, 55, 70), 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::black, 2));
    p.drawEllipse(QPointF(10, 0), 18, 18);

    QRectF tuboRect(15, -14, largo, 28);
    p.save();
    p.setClipRect(tuboRect);
    dibujarBloquePixelado(p, 15, -14, largo, 28, QColor(105, 110, 105), 4.0f);
    p.restore();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::black, 2));
    p.drawRect(tuboRect);

    p.setBrush(QColor(20, 20, 20));
    p.setPen(QPen(Qt::black, 1));
    p.drawRect(QRectF(15 + largo, -16, 12, 32));

    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    p.drawRect(QRectF(15 + largo + 6, -8, 6, 16));

    p.restore();
}

void ZombieTanque::dibujarDetallesSangre(QPainter &p) {
    // 1 manchas de sangre y oxido en bloques cuadrados en vez de manchas
    //   ovaladas, para que combinen con el resto de la chapa pixelada
    p.save();
    p.setPen(Qt::NoPen);

    QColor sangre(140, 0, 15, 200);
    QColor oxido(90, 60, 30, 150);

    p.setBrush(sangre);
    p.drawRect(QRectF(-24, -13, 8, 6));
    p.drawRect(QRectF(11, 1, 10, 8));
    p.drawRect(QRectF(-34, 10, 8, 10));
    p.drawRect(QRectF(7, -19, 6, 9));

    p.setBrush(oxido);
    p.drawRect(QRectF(-18, 16, 12, 8));
    p.drawRect(QRectF(20, -8, 10, 6));

    p.setBrush(sangre);
    p.drawRect(QRectF(-7, -15, 4, 20));
    p.drawRect(QRectF(19, -8, 4, 16));

    p.restore();
}

// ============================================================
// cerebro zombie, ahora armado con una grilla de pixeles en vez de
// gradientes suaves, recortada en forma ovalada
// paleta: colorCerebro (o naranja si esta en furia) para la masa
// principal, un tono mas claro para el brillo superior, y un tono
// violaceo oscuro para las lineas de los pliegues
// el halo ya no es un gradiente radial sino anillos cuadrados
// concentricos, y los ojos que aparecen segun el punto de vista
// son bloques cuadrados en vez de circulos
// ============================================================
void ZombieTanque::dibujarCerebro(QPainter &p, float escala, bool enFuria) {
    p.save();
    p.translate(0, -40);
    p.scale(escala, escala);

    // 1 halo pixelado en anillos concentricos
    QColor colorHalo = enFuria ? QColor(255, 100, 0) : QColor(255, 50, 100);
    p.setPen(Qt::NoPen);
    for (int anillo = 5; anillo >= 1; anillo--) {
        int alfa = 16 * (6 - anillo);
        QColor c = colorHalo;
        c.setAlpha(alfa);
        p.setBrush(c);
        float r = anillo * 12.0f;
        p.drawRect(QRectF(-r, -r * 0.8f, r * 2.0f, r * 1.6f));
    }

    // 2 cerebro pixelado, grilla recortada con forma ovalada
    QColor colorBase = enFuria ? QColor(230, 90, 40) : colorCerebro;
    QColor colorClaro = colorBase.lighter(150);
    QColor colorPliegue(90, 20, 40);

    const int columnas = 16;
    const int filas = 10;
    float pixel = 5.5f;
    float origenX = -columnas * pixel / 2.0f;
    float origenY = -filas * pixel / 2.0f;

    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            float nx = (columna - columnas / 2.0f + 0.5f) / (columnas / 2.0f);
            float ny = (fila - filas / 2.0f + 0.5f) / (filas / 2.0f);
            if (nx * nx + ny * ny > 1.0f) continue; // 3 recorta la grilla con forma ovalada

            bool esPliegue = ((fila % 3 == 1) && (columna % 2 == 0));
            bool esBorde = (nx * nx + ny * ny > 0.72f);
            bool esBrillo = (fila < filas / 3 && (columna + fila) % 4 == 0);

            QColor color = colorBase;
            if (esPliegue) color = colorPliegue;
            else if (esBrillo) color = colorClaro;
            else if (esBorde) color = colorBase.darker(140);

            p.setBrush(color);
            p.drawRect(QRectF(origenX + columna * pixel, origenY + fila * pixel, pixel + 0.6f, pixel + 0.6f));
        }
    }

    // 4 tallo cerebral pixelado en la base
    p.setBrush(colorBase.darker(160));
    p.drawRect(QRectF(-pixel, filas * 0.4f * pixel, pixel * 2.0f, pixel * 2.5f));

    // 5 ojos en bloques cuadrados segun el punto de vista actual
    if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        p.setBrush(Qt::cyan);
        p.setPen(QPen(Qt::black, 1));
        p.drawRect(QRectF(-18, 14, 8, 8));
        p.drawRect(QRectF(10, 14, 8, 8));
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        p.drawRect(QRectF(-16, 16, 3, 3));
        p.drawRect(QRectF(12, 16, 3, 3));
    } else if (anguloMovimiento >= 225 && anguloMovimiento < 315) {
        p.setBrush(Qt::magenta);
        p.setPen(QPen(Qt::black, 1));
        p.drawRect(QRectF(-16, -22, 7, 7));
        p.drawRect(QRectF(9, -22, 7, 7));
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::white);
        p.drawRect(QRectF(-15, -21, 2, 2));
        p.drawRect(QRectF(10, -21, 2, 2));
    }

    p.restore();
}
