#include "pared.h"
#include <QLinearGradient>
#include <QtMath>
#include <cmath>

Pared::Pared()
    : x_pared(0.0f), y_pared(0.0f),
    ancho_pared(0.0f), alto_pared(0.0f), vida(100.0f), tipo_pared(0)
{
}

Pared::Pared(float x, float y, float ancho, float alto, int tipo)
    : x_pared(x), y_pared(y),
    ancho_pared(ancho), alto_pared(alto), vida(100.0f), tipo_pared(tipo)
{
}

float Pared::get_x()
{
    return x_pared;
}

float Pared::get_y()
{
    return y_pared;
}

float Pared::get_ancho()
{
    return ancho_pared;
}

float Pared::get_alto()
{
    return alto_pared;
}

void Pared::dibujar(QPainter &p)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);

    // Corregido: tipo_pared en lugar de tipoPared
    if(tipo_pared == 1) {
        dibujarVallaLimite(p);
    } else {
        dibujarMuroBase(p);
    }

    dibujarDanio(p);

    p.restore();
}

void Pared::dibujarMuroBase(QPainter &p)
{
    QRectF rect(get_x(), get_y(), get_ancho(), get_alto());

    // 1. bloque de hormigon con leve degradado
    QLinearGradient degradado(rect.topLeft(), rect.bottomLeft());
    degradado.setColorAt(0.0, QColor(120, 120, 118));
    degradado.setColorAt(1.0, QColor(85, 85, 82));
    p.setPen(Qt::NoPen);
    p.setBrush(degradado);
    p.drawRect(rect);

    // 2. juntas horizontales
    p.setPen(QPen(QColor(55, 55, 52), 1));
    int cantidadJuntas = (int)(get_alto() / 14.0f);
    for(int i = 1; i <= cantidadJuntas; i++) {
        float y = get_y() + i * 14.0f;
        if(y >= get_y() + get_alto()) break;
        p.drawLine(QPointF(get_x(), y), QPointF(get_x() + get_ancho(), y));
    }

    // 3. bulones metalicos
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(170, 170, 165));
    float paso = 18.0f;
    int cantidadBulones = (int)(get_ancho() / paso);
    for(int i = 0; i <= cantidadBulones; i++) {
        float bx = get_x() + 6.0f + i * paso;
        if(bx > get_x() + get_ancho() - 4.0f) break;
        p.drawEllipse(QPointF(bx, get_y() + 4.0f), 1.6f, 1.6f);
    }

    // 4. alambre de puas
    p.setPen(QPen(QColor(70, 70, 68), 1.4f));
    float xIni = get_x();
    float xFin = get_x() + get_ancho();
    float yPua = get_y() - 3.0f;
    float pasoPua = 10.0f;
    for(float x = xIni; x < xFin; x += pasoPua) {
        p.drawLine(QPointF(x, yPua), QPointF(x + pasoPua * 0.6f, yPua - 4.0f));
        p.drawLine(QPointF(x + pasoPua * 0.6f, yPua - 4.0f), QPointF(x + pasoPua, yPua));
        p.drawLine(QPointF(x + pasoPua * 0.3f, yPua - 2.0f), QPointF(x + pasoPua * 0.3f + 2.0f, yPua - 4.0f));
    }
}

void Pared::dibujarVallaLimite(QPainter &p)
{
    QRectF rect(get_x(), get_y(), get_ancho(), get_alto());

    p.setClipRect(rect);

    // 1. fondo tenue
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(55, 60, 55, 160));
    p.drawRect(rect);

    // 2. postes de sosten
    p.setBrush(QColor(60, 55, 45));
    float pasoPoste = 40.0f;
    int cantidadPostes = (int)(get_ancho() / pasoPoste) + 1;
    bool horizontal = get_ancho() >= get_alto();
    if(horizontal) {
        for(int i = 0; i <= cantidadPostes; i++) {
            float px = get_x() + i * pasoPoste;
            if(px > get_x() + get_ancho()) break;
            p.drawRect(QRectF(px - 1.5f, get_y(), 3.0f, get_alto()));
        }
    } else {
        int cantidadPostesV = (int)(get_alto() / pasoPoste) + 1;
        for(int i = 0; i <= cantidadPostesV; i++) {
            float py = get_y() + i * pasoPoste;
            if(py > get_y() + get_alto()) break;
            p.drawRect(QRectF(get_x(), py - 1.5f, get_ancho(), 3.0f));
        }
    }

    // 3. malla romboidal
    p.setPen(QPen(QColor(150, 150, 140, 190), 1));
    float pasoMalla = 8.0f;
    for(float d = -get_alto(); d < get_ancho(); d += pasoMalla) {
        p.drawLine(QPointF(get_x() + d, get_y()), QPointF(get_x() + d + get_alto(), get_y() + get_alto()));
    }
    for(float d = -get_alto(); d < get_ancho(); d += pasoMalla) {
        p.drawLine(QPointF(get_x() + d + get_alto(), get_y()), QPointF(get_x() + d, get_y() + get_alto()));
    }
}

void Pared::dibujarDanio(QPainter &p)
{
    if(vida >= 100.0f) return;

    QRectF rect(get_x(), get_y(), get_ancho(), get_alto());
    float porcentajeDanio = 1.0f - (vida / 100.0f);

    // 1. mancha de tizne
    int alphaTizne = (int)(120 * porcentajeDanio);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(30, 25, 20, alphaTizne));
    p.drawRect(rect);

    // 2. grietas
    p.setPen(QPen(QColor(20, 15, 15, 200), 1.2f));
    int cantidadGrietas = (int)(porcentajeDanio * 6.0f);
    for(int i = 0; i < cantidadGrietas; i++) {
        float semilla = std::fmod(x_pared * 3.17f + y_pared * 5.31f + i * 91.7f, 100.0f);
        float ox = get_x() + std::fmod(semilla * 7.0f, get_ancho());
        float oy = get_y() + std::fmod(semilla * 13.0f, get_alto());

        QPointF punto(ox, oy);
        for(int t = 0; t < 3; t++) {
            float anguloGrieta = std::fmod(semilla * (t + 1) * 47.0f, 360.0f) * M_PI / 180.0f;
            QPointF siguiente = punto + QPointF(cos(anguloGrieta) * 6.0f, sin(anguloGrieta) * 6.0f);
            p.drawLine(punto, siguiente);
            punto = siguiente;
        }
    }

    // 3. borde critico
    if (vida < 30.0f) {
        p.setPen(QPen(QColor(255, 60, 40, 200), 1.5f, Qt::DashLine));
        p.setBrush(Qt::NoBrush);
        p.drawRect(rect.adjusted(2, 2, -2, -2));
    }
}
