#include "basemilitar.h"
#include "pared.h"          // AGREGADO: Necesario para instanciar Pared
#include <QPainterPath>     // CORREGIDO: Mayúsculas estándar de Qt
#include <QRadialGradient>
#include <QFont>            // AGREGADO: Necesario para setFont
#include <QtMath>
#include <cmath>

BaseMilitar::BaseMilitar() : margenExterno(200.0f), nivelNieve(0)
{
    x_escenario = 200.0f;
    y_escenario = 200.0f;
    ancho_escenario = 600.0f;
    alto_escenario = 600.0f;

    crearValla(); // ya genera la decoracion y los parches de nieve
}

void BaseMilitar::establecerNivelNieve(int nivel)
{
    int nivelAcotado = nivel;
    if(nivelAcotado < 0) nivelAcotado = 0;
    if(nivelAcotado > 15) nivelAcotado = 15; // ANTES: 10, debe coincidir con NIVEL_BOSS_FINAL
    nivelNieve = nivelAcotado;
}
void BaseMilitar::generarParchesNieve()
{
    parchesNieve.clear();
    moteadoNieveFinal.clear();

    float xMinMapa = get_x() - margenExterno;
    float yMinMapa = get_y() - margenExterno;
    float anchoMapa = get_ancho() + 2 * margenExterno;
    float altoMapa = get_alto() + 2 * margenExterno;

    // 1 subido de 700 a 1400 parches, y parches un poco mas grandes
    for(int i = 0; i < 1400; i++) {
        ParcheNieve parche;
        parche.x = xMinMapa + (rand() % (int)anchoMapa);
        parche.y = yMinMapa + (rand() % (int)altoMapa);
        parche.tam = 8.0f + (rand() % 18);
        parche.umbralNivel = 1 + (rand() % 15);
        parche.tono = rand() % 20;
        parchesNieve.append(parche);
    }

    for(int i = 0; i < 250; i++) {
        float mx = xMinMapa + (rand() % (int)anchoMapa);
        float my = yMinMapa + (rand() % (int)altoMapa);
        moteadoNieveFinal.append(QPointF(mx, my));
    }
}
void BaseMilitar::dibujarNieveAcumulada(QPainter &p)
{
    if(nivelNieve <= 0) return;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);

    // 1 sombra celeste tenue debajo de cada parche, da sensacion de volumen
    for(const ParcheNieve &parche : parchesNieve) {
        if(parche.umbralNivel <= nivelNieve) {
            dibujarBloquePixel(p, parche.x + 2.0f, parche.y + 2.0f, parche.tam, QColor(170, 190, 210, 90));
        }
    }

    // 2 el parche de nieve en si, con variacion de blanco para textura
    for(const ParcheNieve &parche : parchesNieve) {
        if(parche.umbralNivel <= nivelNieve) {
            int base = 235 + parche.tono;
            if(base > 255) base = 255;
            dibujarBloquePixel(p, parche.x, parche.y, parche.tam, QColor(base, base, 250, 215));
        }
    }

    // 3 gorros de nieve posados sobre la decoracion existente del mapa
    for(const ElementoDecorativoMapa &e : decoracionMapa) {
        int umbralDecor = 1 + ((int)(e.x + e.y) % 10);
        if(umbralDecor <= nivelNieve) {
            dibujarBloquePixel(p, e.x - e.tam * 0.3f, e.y - e.tam * 0.3f, e.tam * 0.5f, QColor(245, 248, 252, 200));
        }
    }

    // 4 en el nivel final el mapa queda completamente tapado, con moteado fijo
    if(nivelNieve >= 15) {
        float xMinMapa = get_x() - margenExterno;
        float yMinMapa = get_y() - margenExterno;
        float anchoMapa = get_ancho() + 2 * margenExterno;
        float altoMapa = get_alto() + 2 * margenExterno;
        p.fillRect(QRectF(xMinMapa, yMinMapa, anchoMapa, altoMapa), QColor(240, 245, 250, 235));

        for(const QPointF &punto : moteadoNieveFinal) {
            dibujarBloquePixel(p, punto.x(), punto.y(), 3.0f, QColor(210, 220, 230, 120));
        }
    }

    p.restore();
}
void BaseMilitar::crearValla()
{
    float grosor = 10.0f;

    qDeleteAll(paredes);
    paredes.clear();

    // Paredes perimetrales interiores (hormigón)
    agregarPared(new Pared(get_x() - grosor, get_y() - grosor, get_ancho() + 2*grosor, grosor, 0));
    agregarPared(new Pared(get_x() - grosor, get_y() + get_alto(), get_ancho() + 2*grosor, grosor, 0));
    agregarPared(new Pared(get_x() - grosor, get_y() - grosor, grosor, get_alto() + 2*grosor, 0));
    agregarPared(new Pared(get_x() + get_ancho(), get_y() - grosor, grosor, get_alto() + 2*grosor, 0));

    // Límites exteriores (valla)
    float xMinMapa = get_x() - margenExterno;
    float yMinMapa = get_y() - margenExterno;
    float anchoMapa = get_ancho() + 2 * margenExterno;
    float altoMapa = get_alto() + 2 * margenExterno;

    agregarPared(new Pared(xMinMapa, yMinMapa - grosor, anchoMapa, grosor, 1));
    agregarPared(new Pared(xMinMapa, yMinMapa + altoMapa, anchoMapa, grosor, 1));
    agregarPared(new Pared(xMinMapa - grosor, yMinMapa, grosor, altoMapa, 1));
    agregarPared(new Pared(xMinMapa + anchoMapa, yMinMapa, grosor, altoMapa, 1));

    generarDecoracionMapa();
    generarParchesNieve();
}
void BaseMilitar::generarDecoracionMapa()
{
    decoracionMapa.clear();

    float xMinMapa = get_x() - margenExterno;
    float yMinMapa = get_y() - margenExterno;
    float anchoMapa = get_ancho() + 2 * margenExterno;
    float altoMapa = get_alto() + 2 * margenExterno;

    // Decoraciones exteriores proporcionadas
    for(int i = 0; i < 20; i++) {
        ElementoDecorativoMapa e;
        e.x = xMinMapa + (rand() % (int)anchoMapa);
        e.y = yMinMapa + (rand() % (int)altoMapa);
        e.tam = 40.0f + (rand() % 40);
        e.angulo = rand() % 360;
        e.tipo = 3;
        decoracionMapa.append(e);
    }
    for(int i = 0; i < 150; i++) {
        ElementoDecorativoMapa e;
        e.x = xMinMapa + (rand() % (int)anchoMapa);
        e.y = yMinMapa + (rand() % (int)altoMapa);
        e.tam = 3.0f + (rand() % 20) / 10.0f;
        e.angulo = rand() % 360;
        e.tipo = 0; // Pasto
        decoracionMapa.append(e);
    }

    // NUEVO: estos dos tipos ya estaban implementados en dibujarTerrenoExterior
    // pero nunca se generaban, asi que jamas se veian en el mapa
    for(int i = 0; i < 15; i++) {
        ElementoDecorativoMapa e;
        e.x = xMinMapa + (rand() % (int)anchoMapa);
        e.y = yMinMapa + (rand() % (int)altoMapa);
        e.tam = 8.0f + (rand() % 10);
        e.angulo = rand() % 360;
        e.tipo = 1; // Rocas
        decoracionMapa.append(e);
    }
    for(int i = 0; i < 12; i++) {
        ElementoDecorativoMapa e;
        e.x = xMinMapa + (rand() % (int)anchoMapa);
        e.y = yMinMapa + (rand() % (int)altoMapa);
        e.tam = 10.0f + (rand() % 15);
        e.angulo = rand() % 360;
        e.tipo = 2; // Ramas secas / grietas
        decoracionMapa.append(e);
    }
}

float BaseMilitar::redondearAPixel(float valor, float tamPixel)
{
    // 1. cuantizar un valor continuo al tamanio de pixel dado
    return std::floor(valor / tamPixel) * tamPixel;
}

void BaseMilitar::dibujarBloquePixel(QPainter &p, float x, float y, float tam, const QColor &color)
{
    // 1. unidad basica del estilo pixel art: un bloque cuadrado sin antialiasing
    p.fillRect(QRectF(redondearAPixel(x, tam), redondearAPixel(y, tam), tam, tam), color);
}

void BaseMilitar::dibujarCirculoPixelado(QPainter &p, float cx, float cy, float radio, float tamPixel, const QColor &color)
{
    // 1. aproximar un circulo usando bloques cuadrados en vez de una elipse suave
    for(float y = -radio; y <= radio; y += tamPixel) {
        for(float x = -radio; x <= radio; x += tamPixel) {
            float dist = std::sqrt(x * x + y * y);
            if(dist <= radio) {
                dibujarBloquePixel(p, cx + x, cy + y, tamPixel, color);
            }
        }
    }
}

void BaseMilitar::dibujarCaminoTierra(QPainter &p)
{
    float centroX = get_x() + get_ancho() / 2.0f;
    float yInicio = get_y() + get_alto();
    float yFin = get_y() + get_alto() + margenExterno;

    float anchoInicio = 78.0f;
    float anchoFin = 48.0f;
    float tamPixel = 6.0f;

    // 1. franjas horizontales de pixel en vez de un path con curvas suaves
    int filas = (int)((yFin - yInicio) / tamPixel);
    if(filas < 1) filas = 1;

    p.setPen(Qt::NoPen);
    for(int i = 0; i < filas; i++) {
        float t = (float)i / filas;
        float y = yInicio + i * tamPixel;
        float anchoActual = anchoInicio + (anchoFin - anchoInicio) * t;
        // 2. ondulacion escalonada, cuantizada al tamanio de pixel en vez de una curva continua
        float ondulacion = redondearAPixel(sin(t * 3.14159f * 2.0f) * 10.0f, tamPixel);

        float xIzq = redondearAPixel(centroX - anchoActual / 2.0f + ondulacion, tamPixel);
        float xDer = redondearAPixel(centroX + anchoActual / 2.0f + ondulacion, tamPixel);

        // 3. borde mas oscuro de un pixel de ancho a cada lado, relleno claro al medio
        p.fillRect(QRectF(xIzq, y, tamPixel, tamPixel), QColor(108, 86, 60, 220));
        p.fillRect(QRectF(xIzq + tamPixel, y, xDer - xIzq - 2.0f * tamPixel, tamPixel), QColor(140, 112, 82, 210));
        p.fillRect(QRectF(xDer - tamPixel, y, tamPixel, tamPixel), QColor(108, 86, 60, 220));
    }

    // 4. piedritas como bloques cuadrados en vez de elipses suaves
    for(int i = 1; i < filas; i += 3) {
        float t = (float)i / filas;
        float y = yInicio + i * tamPixel;
        float ondulacion = redondearAPixel(sin(t * 3.14159f * 2.0f) * 10.0f, tamPixel);
        dibujarBloquePixel(p, centroX + ondulacion - 14.0f, y, 4.0f, QColor(165, 134, 98, 210));
        dibujarBloquePixel(p, centroX + ondulacion + 10.0f, y + 4.0f, 3.0f, QColor(92, 73, 50, 210));
    }
}

void BaseMilitar::dibujarTerrenoExterior(QPainter &p)
{
    p.setPen(Qt::NoPen);

    for(const ElementoDecorativoMapa &e : decoracionMapa) {
        if(e.tipo == 3) {
            // 1. parche de tierra como racimo de bloques limitado a una silueta ovalada
            QColor colorParche(90, 98, 78, 100);
            float tamPixel = e.tam / 4.0f;
            for(float dy = -e.tam * 0.6f; dy <= e.tam * 0.6f; dy += tamPixel) {
                for(float dx = -e.tam; dx <= e.tam; dx += tamPixel) {
                    if((dx * dx) / (e.tam * e.tam) + (dy * dy) / ((e.tam * 0.6f) * (e.tam * 0.6f)) <= 1.0f) {
                        dibujarBloquePixel(p, e.x + dx, e.y + dy, tamPixel, colorParche);
                    }
                }
            }
        }
        else if(e.tipo == 0) {
            // 2. brote de pasto como bloques apilados en vez de lineas finas con antialiasing
            float base = e.angulo * M_PI / 180.0f;
            for(int b = -1; b <= 1; b++) {
                float ang = base + b * 0.35f;
                float px = e.x + cos(ang) * e.tam;
                float py = e.y - fabs(sin(ang)) * e.tam - e.tam * 0.5f;
                dibujarBloquePixel(p, (e.x + px) / 2.0f, (e.y + py) / 2.0f, 1.8f, QColor(70, 115, 52));
                dibujarBloquePixel(p, px, py, 1.8f, QColor(90, 145, 68));
            }
        }
        else if(e.tipo == 1) {
            // 3. roca armada con bloques cuadrados de distinto tono, sin degrade circular
            float tamPixel = e.tam / 3.0f;
            dibujarBloquePixel(p, e.x - tamPixel, e.y, tamPixel * 2.0f, QColor(105, 103, 95));
            dibujarBloquePixel(p, e.x - tamPixel * 2.0f, e.y + tamPixel * 0.5f, tamPixel, QColor(88, 86, 79));
            dibujarBloquePixel(p, e.x + tamPixel, e.y + tamPixel * 0.5f, tamPixel, QColor(88, 86, 79));
            dibujarBloquePixel(p, e.x - tamPixel * 0.5f, e.y - tamPixel * 0.5f, tamPixel, QColor(138, 135, 124));
        }
        else {
            // 4. rama seca como segmentos de bloques en vez de lineas de 1px
            float base = e.angulo * M_PI / 180.0f;
            dibujarBloquePixel(p, e.x, e.y, 2.4f, QColor(95, 75, 45));
            for(int r = 0; r < 5; r++) {
                float ang = base + r * (2.0f * M_PI / 5.0f);
                float px = e.x + cos(ang) * e.tam;
                float py = e.y + sin(ang) * e.tam;
                dibujarBloquePixel(p, (e.x + px) / 2.0f, (e.y + py) / 2.0f, 1.8f, QColor(90, 70, 42));
                dibujarBloquePixel(p, px, py, 1.6f, QColor(78, 60, 36));
            }
        }
    }
}

void BaseMilitar::dibujarSueloInterior(QPainter &p)
{
    // 1. fondo de asfalto texturado con bloques en vez de un relleno plano
    p.setPen(Qt::NoPen);
    float tamAsfalto = 14.0f;
    for(float by = get_y(); by < get_y() + get_alto(); by += tamAsfalto) {
        for(float bx = get_x(); bx < get_x() + get_ancho(); bx += tamAsfalto) {
            int variacion = ((int)(bx / tamAsfalto) * 7 + (int)(by / tamAsfalto) * 13) % 5;
            QColor colorBase = (variacion == 0) ? QColor(68, 68, 73) : QColor(76, 76, 81);
            p.fillRect(QRectF(bx, by, tamAsfalto, tamAsfalto), colorBase);
        }
    }

    // Grid de losas de concreto (ya son lineas rectas, se mantienen)
    p.setPen(QPen(QColor(60, 60, 65), 2));
    float pasoLosa = 100.0f;
    for(float x = get_x(); x <= get_x() + get_ancho(); x += pasoLosa) {
        p.drawLine(QPointF(x, get_y()), QPointF(x, get_y() + get_alto()));
    }
    for(float y = get_y(); y <= get_y() + get_alto(); y += pasoLosa) {
        p.drawLine(QPointF(get_x(), y), QPointF(get_x() + get_ancho(), y));
    }

    // 2. Helipuerto Central con anillos pixelados en vez de elipses suaves
    float cx = get_x() + get_ancho() / 2.0f;
    float cy = get_y() + get_alto() / 2.0f;
    float tamAnillo = 6.0f;

    dibujarCirculoPixelado(p, cx, cy, 90.0f, tamAnillo, QColor(220, 200, 40));
    dibujarCirculoPixelado(p, cx, cy, 80.0f, tamAnillo, QColor(65, 70, 75));
    dibujarCirculoPixelado(p, cx, cy, 74.0f, tamAnillo, QColor(220, 220, 220));
    dibujarCirculoPixelado(p, cx, cy, 68.0f, tamAnillo, QColor(65, 70, 75));

    // 3. letra "H" armada con bloques en vez de texto suavizado
    float tamLetra = 10.0f;
    QColor blancoH(255, 255, 255);
    for(int fila = -4; fila <= 4; fila++) {
        dibujarBloquePixel(p, cx - 30.0f, cy + fila * tamLetra, tamLetra, blancoH);
        dibujarBloquePixel(p, cx + 20.0f, cy + fila * tamLetra, tamLetra, blancoH);
    }
    for(int col = -2; col <= 1; col++) {
        dibujarBloquePixel(p, cx + col * tamLetra, cy, tamLetra, blancoH);
    }

    // Zonas de estacionamiento militar (lineas rectas, se mantienen)
    p.setPen(QPen(QColor(200, 200, 200), 3));
    for(int i = 0; i < 5; i++) {
        float px = get_x() + 50.0f + i * 80.0f;
        float py = get_y() + 30.0f;
        p.drawLine(QPointF(px, py), QPointF(px, py + 100.0f));
    }
    p.drawLine(QPointF(get_x() + 50.0f, get_y() + 30.0f), QPointF(get_x() + 50.0f + 4*80.0f, get_y() + 30.0f));

    // 4. Marcas de peligro como bloques alternados amarillo/negro en vez de linea punteada suave
    p.setPen(Qt::NoPen);
    float yPeligro = get_y() + 44.0f;
    float xIniPeligro = get_x() + get_ancho() - 200.0f;
    float xFinPeligro = get_x() + get_ancho() - 30.0f;
    float tamRaya = 12.0f;
    bool amarillo = true;
    for(float x = xIniPeligro; x < xFinPeligro; x += tamRaya) {
        p.fillRect(QRectF(x, yPeligro, tamRaya, tamRaya), amarillo ? QColor(220, 200, 0) : QColor(20, 20, 20));
        amarillo = !amarillo;
    }
}

void BaseMilitar::dibujarDecoracionInterior(QPainter &p)
{
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(QPen(QColor(30, 40, 30), 2));

    // Barracón Norte (Reducido)
    p.setBrush(QColor(100, 110, 95));
    p.drawRect(get_x() + 50.0f, get_y() + get_alto() - 150.0f, 250.0f, 100.0f);
    p.setBrush(QColor(90, 100, 85)); // Techo
    p.drawRect(get_x() + 60.0f, get_y() + get_alto() - 140.0f, 230.0f, 80.0f);

    // 1. tablones del techo en franjas de pixel alternadas en vez de un relleno liso
    float techoNorteX = get_x() + 60.0f;
    float techoNorteY = get_y() + get_alto() - 140.0f;
    p.setPen(Qt::NoPen);
    for(int i = 0; i < 12; i++) {
        QColor tablon = (i % 2 == 0) ? QColor(84, 94, 80) : QColor(96, 106, 91);
        p.fillRect(QRectF(techoNorteX + i * 19.2f, techoNorteY, 19.2f, 80.0f), tablon);
    }
    // 2. ventanas y puerta pixeladas
    p.fillRect(QRectF(techoNorteX + 25.0f, techoNorteY + 20.0f, 26.0f, 26.0f), QColor(140, 190, 210, 210));
    p.fillRect(QRectF(techoNorteX + 25.0f, techoNorteY + 20.0f, 26.0f, 4.0f), QColor(50, 45, 35));
    p.fillRect(QRectF(techoNorteX + 90.0f, techoNorteY + 20.0f, 26.0f, 26.0f), QColor(140, 190, 210, 210));
    p.fillRect(QRectF(techoNorteX + 90.0f, techoNorteY + 20.0f, 26.0f, 4.0f), QColor(50, 45, 35));
    p.fillRect(QRectF(techoNorteX + 170.0f, techoNorteY + 30.0f, 34.0f, 50.0f), QColor(60, 50, 38));
    p.fillRect(QRectF(techoNorteX + 170.0f, techoNorteY + 30.0f, 34.0f, 6.0f), QColor(45, 38, 28));

    // Barracón Este
    p.setPen(QPen(QColor(30, 40, 30), 2));
    p.setBrush(QColor(100, 110, 95));
    p.drawRect(get_x() + get_ancho() - 150.0f, get_y() + get_alto() - 280.0f, 120.0f, 200.0f);
    p.setBrush(QColor(90, 100, 85)); // Techo
    p.drawRect(get_x() + get_ancho() - 140.0f, get_y() + get_alto() - 270.0f, 100.0f, 180.0f);

    // 3. tablones y ventanas del barracon este
    float techoEsteX = get_x() + get_ancho() - 140.0f;
    float techoEsteY = get_y() + get_alto() - 270.0f;
    p.setPen(Qt::NoPen);
    for(int i = 0; i < 9; i++) {
        QColor tablon = (i % 2 == 0) ? QColor(84, 94, 80) : QColor(96, 106, 91);
        p.fillRect(QRectF(techoEsteX, techoEsteY + i * 20.0f, 100.0f, 20.0f), tablon);
    }
    p.fillRect(QRectF(techoEsteX + 30.0f, techoEsteY + 25.0f, 26.0f, 26.0f), QColor(140, 190, 210, 210));
    p.fillRect(QRectF(techoEsteX + 30.0f, techoEsteY + 25.0f, 26.0f, 4.0f), QColor(50, 45, 35));
    p.fillRect(QRectF(techoEsteX + 30.0f, techoEsteY + 110.0f, 26.0f, 26.0f), QColor(140, 190, 210, 210));
    p.fillRect(QRectF(techoEsteX + 30.0f, techoEsteY + 110.0f, 26.0f, 4.0f), QColor(50, 45, 35));

    // Cajas de suministros
    p.setPen(QPen(QColor(50, 40, 20), 2));
    p.setBrush(QColor(140, 110, 70));
    float boxStartX = get_x() + get_ancho() - 180.0f;
    float boxStartY = get_y() + 100.0f;

    for(int fila = 0; fila < 4; fila++) {
        for(int col = 0; col < 3; col++) {
            p.drawRect(boxStartX + col * 25.0f, boxStartY + fila * 25.0f, 20.0f, 20.0f);
            // 4. veta de madera como bloques escalonados en vez de una diagonal fina
            p.setPen(Qt::NoPen);
            p.fillRect(QRectF(boxStartX + col * 25.0f + 2.0f, boxStartY + fila * 25.0f + 2.0f, 4.0f, 4.0f), QColor(110, 85, 50));
            p.fillRect(QRectF(boxStartX + col * 25.0f + 8.0f, boxStartY + fila * 25.0f + 8.0f, 4.0f, 4.0f), QColor(110, 85, 50));
            p.fillRect(QRectF(boxStartX + col * 25.0f + 14.0f, boxStartY + fila * 25.0f + 14.0f, 4.0f, 4.0f), QColor(110, 85, 50));
            p.setPen(QPen(QColor(50, 40, 20), 2));
        }
    }

    // Contenedores Reforzados
    p.setPen(Qt::NoPen);
    for(int i = 0; i < 15; i++) {
        // 5. chapa corrugada del contenedor azul, franjas verticales de pixel
        QColor franja = (i % 2 == 0) ? QColor(60, 80, 118) : QColor(78, 100, 140);
        p.fillRect(QRectF(get_x() + 50.0f + i * 10.0f, get_y() + 200.0f, 10.0f, 50.0f), franja);
    }
    p.fillRect(QRectF(get_x() + 110.0f, get_y() + 208.0f, 30.0f, 34.0f), QColor(30, 40, 60));
    p.fillRect(QRectF(get_x() + 122.0f, get_y() + 222.0f, 6.0f, 6.0f), QColor(200, 190, 60));
    p.setPen(QPen(QColor(20, 30, 50), 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(get_x() + 50.0f, get_y() + 200.0f, 150.0f, 50.0f);

    p.setPen(Qt::NoPen);
    for(int i = 0; i < 15; i++) {
        // 6. chapa corrugada del contenedor rojo
        QColor franja = (i % 2 == 0) ? QColor(110, 58, 58) : QColor(142, 82, 82);
        p.fillRect(QRectF(get_x() + 50.0f + i * 10.0f, get_y() + 260.0f, 10.0f, 50.0f), franja);
    }
    p.fillRect(QRectF(get_x() + 110.0f, get_y() + 268.0f, 30.0f, 34.0f), QColor(70, 30, 30));
    p.fillRect(QRectF(get_x() + 122.0f, get_y() + 282.0f, 6.0f, 6.0f), QColor(200, 190, 60));
    p.setPen(QPen(QColor(20, 30, 50), 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(get_x() + 50.0f, get_y() + 260.0f, 150.0f, 50.0f);
}

void BaseMilitar::dibujarVinieta(QPainter &p)
{
    float xMinMapa = get_x() - margenExterno;
    float yMinMapa = get_y() - margenExterno;
    float anchoMapa = get_ancho() + 2 * margenExterno;
    float altoMapa = get_alto() + 2 * margenExterno;

    p.setRenderHint(QPainter::Antialiasing, false);
    p.setPen(Qt::NoPen);

    // 1. anillos rectangulares escalonados en vez de un QRadialGradient suave
    int anillos = 7;
    float grosorAnillo = 26.0f;
    for(int i = 0; i < anillos; i++) {
        float inset = i * grosorAnillo;
        int alpha = 14 * (anillos - i);
        if(alpha > 98) alpha = 98;

        QRectF exterior(xMinMapa + inset, yMinMapa + inset,
                        anchoMapa - 2.0f * inset, altoMapa - 2.0f * inset);
        if(exterior.width() <= 0.0f || exterior.height() <= 0.0f) break;

        QPainterPath anillo;
        anillo.addRect(exterior);
        QRectF interior = exterior.adjusted(grosorAnillo, grosorAnillo, -grosorAnillo, -grosorAnillo);
        if(interior.width() > 0.0f && interior.height() > 0.0f) {
            anillo.addRect(interior);
        }
        anillo.setFillRule(Qt::OddEvenFill);

        p.fillPath(anillo, QColor(0, 0, 0, alpha));
    }
}

void BaseMilitar::dibujarDetallesAdicionales(QPainter &p)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);

    // Torre de vigilancia (esquina noroeste)
    float torreX = get_x() + 30.0f;
    float torreY = get_y() + 30.0f;
    p.setPen(QPen(QColor(40, 35, 25), 2));
    p.setBrush(QColor(90, 75, 55));
    p.drawRect(QRectF(torreX - 4, torreY, 8, 70));
    p.drawRect(QRectF(torreX + 40, torreY, 8, 70));
    p.setBrush(QColor(70, 60, 45));
    p.drawRect(QRectF(torreX - 15, torreY - 30, 65, 32));

    // 1. techo escalonado (piramide de bloques) en vez de triangulo con antialiasing
    p.setPen(Qt::NoPen);
    float techoBaseY = torreY - 30.0f;
    float techoAncho = 74.0f;
    int filasTecho = 5;
    float alturaFila = 4.0f;
    for(int i = 0; i < filasTecho; i++) {
        float reduccion = i * (techoAncho / (2.0f * filasTecho));
        float ry = techoBaseY - (i + 1) * alturaFila;
        p.fillRect(QRectF(torreX - 20.0f + reduccion, ry, techoAncho - 2.0f * reduccion, alturaFila),
                   (i % 2 == 0) ? QColor(50, 45, 35) : QColor(60, 54, 42));
    }
    p.fillRect(QRectF(torreX + 11, torreY - 26, 8, 8), QColor(255, 60, 40, 200));

    // 2. Mastil con bandera armada en bloques, ondulacion escalonada en vez de poligono suave
    float mastilX = get_x() + get_ancho() - 60.0f;
    float mastilY = get_y() + 60.0f;
    p.setPen(QPen(QColor(60, 60, 60), 4));
    p.drawLine(QPointF(mastilX, mastilY), QPointF(mastilX, mastilY - 120.0f));
    p.setPen(Qt::NoPen);

    float tamPixelBandera = 6.0f;
    int filasBandera = 5;
    int colsBandera = 8;
    for(int fila = 0; fila < filasBandera; fila++) {
        int recorte = (fila == 2) ? 2 : ((fila == 1 || fila == 3) ? 1 : 0);
        for(int col = 0; col < colsBandera - recorte; col++) {
            QColor colorBandera = (col % 2 == 0) ? QColor(60, 90, 160) : QColor(72, 102, 174);
            p.fillRect(QRectF(mastilX + col * tamPixelBandera,
                              mastilY - 122.0f + fila * tamPixelBandera,
                              tamPixelBandera, tamPixelBandera), colorBandera);
        }
    }

    // 3. Muro de sacos de arena como bloques apilados en vez de elipses
    float sacosX = get_x() + get_ancho() - 220.0f;
    float sacosY = get_y() + get_alto() - 60.0f;
    for(int i = 0; i < 7; i++) {
        QColor colorSaco = (i % 2 == 0) ? QColor(180, 150, 100) : QColor(168, 138, 90);
        p.fillRect(QRectF(sacosX + i * 18.0f, sacosY, 18.0f, 13.0f), colorSaco);
        p.fillRect(QRectF(sacosX + i * 18.0f, sacosY, 18.0f, 3.0f), QColor(120, 95, 60));
    }
    for(int i = 0; i < 6; i++) {
        QColor colorSaco = (i % 2 == 0) ? QColor(190, 158, 106) : QColor(174, 144, 94);
        p.fillRect(QRectF(sacosX + 9.0f + i * 18.0f, sacosY - 10.0f, 18.0f, 13.0f), colorSaco);
        p.fillRect(QRectF(sacosX + 9.0f + i * 18.0f, sacosY - 10.0f, 18.0f, 3.0f), QColor(130, 105, 68));
    }

    // 4. Bidones de combustible con bandas de pixel adicionales
    p.fillRect(QRectF(get_x() + 240.0f, get_y() + 40.0f, 22.0f, 30.0f), QColor(180, 60, 40));
    p.fillRect(QRectF(get_x() + 240.0f, get_y() + 40.0f, 22.0f, 6.0f), QColor(150, 48, 32));
    p.fillRect(QRectF(get_x() + 240.0f, get_y() + 64.0f, 22.0f, 6.0f), QColor(150, 48, 32));
    p.fillRect(QRectF(get_x() + 265.0f, get_y() + 45.0f, 22.0f, 30.0f), QColor(180, 60, 40));
    p.fillRect(QRectF(get_x() + 265.0f, get_y() + 45.0f, 22.0f, 6.0f), QColor(150, 48, 32));
    p.fillRect(QRectF(get_x() + 265.0f, get_y() + 69.0f, 22.0f, 6.0f), QColor(150, 48, 32));
    p.fillRect(QRectF(get_x() + 240.0f, get_y() + 44.0f, 22.0f, 4.0f), QColor(230, 200, 40));
    p.fillRect(QRectF(get_x() + 265.0f, get_y() + 49.0f, 22.0f, 4.0f), QColor(230, 200, 40));
    p.fillRect(QRectF(get_x() + 246.0f, get_y() + 52.0f, 4.0f, 4.0f), QColor(230, 200, 40));
    p.fillRect(QRectF(get_x() + 271.0f, get_y() + 57.0f, 4.0f, 4.0f), QColor(230, 200, 40));

    // 5. Marcas de neumaticos como bloques discontinuos en vez de una linea con antialiasing
    QPointF inicio1(get_x() + 100.0f, get_y() + get_alto() - 200.0f);
    QPointF fin1(get_x() + 300.0f, get_y() + get_alto() - 40.0f);
    QPointF inicio2(get_x() + 115.0f, get_y() + get_alto() - 200.0f);
    QPointF fin2(get_x() + 315.0f, get_y() + get_alto() - 40.0f);
    int pasosMarca = 16;
    for(int i = 0; i < pasosMarca; i += 2) {
        float t = (float)i / pasosMarca;
        QPointF pt1 = inicio1 + (fin1 - inicio1) * t;
        QPointF pt2 = inicio2 + (fin2 - inicio2) * t;
        p.fillRect(QRectF(pt1.x() - 3, pt1.y() - 3, 6, 6), QColor(30, 30, 30, 130));
        p.fillRect(QRectF(pt2.x() - 3, pt2.y() - 3, 6, 6), QColor(30, 30, 30, 130));
    }

    p.restore();
}

void BaseMilitar::dibujarPerro(QPainter &p, float x, float y, float escala, float angulo)
{
    p.save();
    p.translate(x, y);
    p.rotate(angulo);
    p.scale(escala, escala);
    p.setRenderHint(QPainter::Antialiasing, false);

    QColor doradoClaro(255, 220, 150);
    QColor doradoMedio(235, 180, 95);
    QColor doradoOscuro(195, 140, 70);
    QColor contorno(80, 55, 30);
    QColor crema(255, 240, 210);

    float pix = 4.0f;
    p.setPen(Qt::NoPen);

    // 1. Sombra de contacto, racimo de bloques limitado a una silueta ovalada
    for(float by = 24.0f; by <= 36.0f; by += pix) {
        for(float bx = -34.0f; bx <= 34.0f; bx += pix) {
            if((bx * bx) / (34.0f * 34.0f) + ((by - 30.0f) * (by - 30.0f)) / (10.0f * 10.0f) <= 1.0f) {
                dibujarBloquePixel(p, bx, by, pix, QColor(20, 30, 10, 70));
            }
        }
    }

    // 2. Cola, escalones de bloques en vez de curvas Bezier
    QColor tonosCola[3] = { doradoOscuro, doradoMedio, doradoOscuro };
    float colaX[7] = { -24.0f, -30.0f, -34.0f, -36.0f, -34.0f, -30.0f, -26.0f };
    float colaY[7] = {   4.0f,  -2.0f,  -8.0f, -16.0f, -24.0f, -30.0f, -28.0f };
    for(int i = 0; i < 7; i++) {
        dibujarBloquePixel(p, colaX[i], colaY[i], pix + 2.0f, tonosCola[i % 3]);
        dibujarBloquePixel(p, colaX[i] - pix, colaY[i], pix, contorno);
    }

    // 3. Patas traseras, mas oscuras, atras del cuerpo
    for(int pata = 0; pata < 2; pata++) {
        float px0 = (pata == 0) ? -20.0f : 10.0f;
        for(int fila = 0; fila < 4; fila++) {
            QColor tono = (fila % 2 == 0) ? doradoOscuro : QColor(172, 122, 60);
            dibujarBloquePixel(p, px0, 14.0f + fila * pix, 9.0f, tono);
        }
        dibujarBloquePixel(p, px0, 14.0f + 4.0f * pix, 9.0f, contorno);
    }

    // 4. Cuerpo principal, bloque ovalado con bandas de pixel en vez de degrade radial
    float cuerpoX0 = -30.0f, cuerpoY0 = -14.0f, cuerpoAncho = 60.0f, cuerpoAlto = 30.0f;
    for(float by = cuerpoY0; by < cuerpoY0 + cuerpoAlto; by += pix) {
        for(float bx = cuerpoX0; bx < cuerpoX0 + cuerpoAncho; bx += pix) {
            float ex = (bx + pix / 2.0f - (cuerpoX0 + cuerpoAncho / 2.0f)) / (cuerpoAncho / 2.0f);
            float ey = (by + pix / 2.0f - (cuerpoY0 + cuerpoAlto / 2.0f)) / (cuerpoAlto / 2.0f);
            if(ex * ex + ey * ey <= 1.0f) {
                int franjaFila = (int)((by - cuerpoY0) / pix);
                QColor tono = (franjaFila % 2 == 0) ? doradoMedio : doradoClaro;
                if(by > cuerpoY0 + cuerpoAlto * 0.6f) tono = crema; // panza clara
                dibujarBloquePixel(p, bx, by, pix, tono);
            }
        }
    }
    dibujarBloquePixel(p, cuerpoX0, cuerpoY0 + 8.0f, pix, contorno);
    dibujarBloquePixel(p, cuerpoX0 + cuerpoAncho - pix, cuerpoY0 + 4.0f, pix, contorno);

    // 5. Patas delanteras, mas claras, encima del cuerpo
    for(int pata = 0; pata < 2; pata++) {
        float px0 = (pata == 0) ? -12.0f : 16.0f;
        for(int fila = 0; fila < 4; fila++) {
            QColor tono = (fila % 2 == 0) ? doradoMedio : doradoClaro;
            dibujarBloquePixel(p, px0, 16.0f + fila * pix, 9.0f, tono);
        }
        dibujarBloquePixel(p, px0, 16.0f + 4.0f * pix, 9.0f, contorno);
    }

    // 6. Gorguera de pelo en el cuello
    for(int i = 0; i < 4; i++) {
        dibujarBloquePixel(p, 20.0f + (i % 2) * pix, -6.0f + i * pix, pix, doradoClaro);
    }

    // 7. Oreja trasera, bloques detras de la cabeza
    for(int i = 0; i < 4; i++) {
        dibujarBloquePixel(p, 28.0f, -18.0f + i * pix, pix + 2.0f, doradoOscuro);
    }

    // 8. Cabeza, bloque cuadrado con silueta redondeada por pixeles
    float cabezaX0 = 18.0f, cabezaY0 = -26.0f, cabezaTam = 26.0f;
    for(float by = cabezaY0; by < cabezaY0 + cabezaTam; by += pix) {
        for(float bx = cabezaX0; bx < cabezaX0 + cabezaTam; bx += pix) {
            float ex = (bx + pix / 2.0f - (cabezaX0 + cabezaTam / 2.0f)) / (cabezaTam / 2.0f);
            float ey = (by + pix / 2.0f - (cabezaY0 + cabezaTam / 2.0f)) / (cabezaTam / 2.0f);
            if(ex * ex + ey * ey <= 1.0f) {
                int franjaFila = (int)((by - cabezaY0) / pix);
                dibujarBloquePixel(p, bx, by, pix, (franjaFila % 2 == 0) ? doradoMedio : doradoClaro);
            }
        }
    }

    // 9. Oreja delantera, cae sobre la cabeza
    for(int i = 0; i < 5; i++) {
        dibujarBloquePixel(p, 14.0f, -22.0f + i * pix, pix + 2.0f, doradoMedio);
    }
    dibujarBloquePixel(p, 16.0f, -14.0f, pix, QColor(200, 140, 130));

    // 10. Cejas
    dibujarBloquePixel(p, 24.0f, -22.0f, pix, doradoOscuro);
    dibujarBloquePixel(p, 34.0f, -22.0f, pix, doradoOscuro);

    // 11. Hocico
    float hocicoX0 = 40.0f, hocicoY0 = -10.0f;
    for(int fila = 0; fila < 3; fila++) {
        for(int col = 0; col < 4; col++) {
            dibujarBloquePixel(p, hocicoX0 + col * pix, hocicoY0 + fila * pix, pix, crema);
        }
    }

    // 12. Nariz
    dibujarBloquePixel(p, hocicoX0 + 4.0f * pix, hocicoY0 + pix, pix, QColor(25, 20, 20));

    // 13. Boca y lengua
    dibujarBloquePixel(p, hocicoX0, hocicoY0 + 2.0f * pix, pix, QColor(60, 40, 30));
    dibujarBloquePixel(p, hocicoX0 - pix, hocicoY0 + 2.0f * pix, pix, QColor(235, 110, 130));

    // 14. Ojo con brillo
    dibujarBloquePixel(p, 30.0f, -16.0f, pix, QColor(15, 15, 15));
    dibujarBloquePixel(p, 31.0f, -17.0f, 2.0f, Qt::white);

    // 15. Collar con chapita
    for(int i = 0; i < 5; i++) {
        dibujarBloquePixel(p, 16.0f + i * pix, 6.0f, pix, QColor(190, 45, 45));
    }
    dibujarBloquePixel(p, 22.0f, 10.0f, pix, QColor(230, 190, 60));

    p.restore();
}

void BaseMilitar::dibujarPerros(QPainter &p)
{
    // Reubicados en el hueco entre los contenedores y el barracon norte,
    // ahora que son mas grandes necesitan mas espacio libre alrededor
    float baseX = get_x() + 70.0f;
    float baseY = get_y() + 340.0f;

    dibujarPerro(p, baseX, baseY, 1.0f, 15.0f);
    dibujarPerro(p, baseX + 60.0f, baseY + 55.0f, 0.9f, -20.0f);
}

void BaseMilitar::dibujar(QPainter &p)
{
    float xMinMapa = get_x() - margenExterno;
    float yMinMapa = get_y() - margenExterno;
    float anchoMapa = get_ancho() + 2 * margenExterno;
    float altoMapa = get_alto() + 2 * margenExterno;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, false);

    QColor colorExterior(100, 110, 90);
    p.setBrush(colorExterior);
    p.setPen(Qt::NoPen);
    p.drawRect(xMinMapa, yMinMapa, anchoMapa, altoMapa);

    // Asume que aún tienes implementadas estas funciones de terreno exterior
    dibujarCaminoTierra(p);
    dibujarTerrenoExterior(p);
    dibujarSueloInterior(p);

    p.restore();

    dibujarParedes(p);
    dibujarDecoracionInterior(p);
    dibujarDetallesAdicionales(p);
    dibujarPerros(p);

    dibujarNieveAcumulada(p); // NUEVO

    p.save();
    dibujarVinieta(p);
    p.restore();
}


float BaseMilitar::get_x() { return x_escenario; }
float BaseMilitar::get_y() { return y_escenario; }
float BaseMilitar::get_ancho() { return ancho_escenario; }
float BaseMilitar::get_alto() { return alto_escenario; }

QPointF BaseMilitar::getPuntoSpawnInterno() const
{
    return QPointF(x_escenario + ancho_escenario / 2.0f,
                   y_escenario + alto_escenario / 2.0f);
}

QPointF BaseMilitar::getPuntoSpawnExterno() const
{
    return QPointF(x_escenario + ancho_escenario / 2.0f,
                   y_escenario + alto_escenario + 100);
}

void BaseMilitar::expandirDerecha(float cantidad)
{
    margenExterno += cantidad;
    crearValla();
}

void BaseMilitar::expandirIzquierda(float cantidad)
{
    margenExterno += cantidad;
    crearValla();
}

void BaseMilitar::expandirArriba(float cantidad)
{
    margenExterno += cantidad;
    crearValla();
}

void BaseMilitar::expandirAbajo(float cantidad)
{
    margenExterno += cantidad;
    crearValla();
}
