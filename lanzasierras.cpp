// lanzasierras.cpp
#include "lanzasierras.h"
#include "qpainterpath.h"
#include <algorithm>
#include <cmath>

LanzaSierras::LanzaSierras()
    : cadenciaDisparo(55), contadorCadencia(0), danio(35.0f), radioSierra(14.0f),
    nivelCadenciaSierra(0), nivelTamanioSierra(0), anguloCargada(0.0f)
{
    ancho = 22.0f;  // antes 28
    alto = 12.0f;   // antes 14
    municionMaxima = 6;
    municionMaximaBase = 6;
    municionActual = 6;
    tiempoRecargaMax = 2.5f;
}
LanzaSierras::~LanzaSierras()
{
    qDeleteAll(sierras);
    sierras.clear();
}

void LanzaSierras::actualizar(float posX, float posY, QPointF mouseWorld)
{
    pos_x = posX;
    pos_y = posY;

    float dx = mouseWorld.x() - pos_x;
    float dy = mouseWorld.y() - pos_y;
    angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    actualizarRecarga();
    anguloCargada += 6.0f;
    if (anguloCargada > 360.0f) anguloCargada -= 360.0f;

    if (contadorCadencia > 0) contadorCadencia--;
}

void LanzaSierras::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        SierraVoladora* s = new SierraVoladora();
        float distancia = 260.0f * multiplicadorRango;
        s->reiniciar(pos_x, pos_y, angulo, 9.0f, danio, distancia, radioSierra);
        sierras.append(s);

        contadorCadencia = cadenciaDisparo;
        municionActual--;

        if (municionActual <= 0) iniciarRecarga();
    }
}

void LanzaSierras::actualizarProyectiles()
{
    for (int i = sierras.size() - 1; i >= 0; i--) {
        sierras[i]->actualizar(QPointF(pos_x, pos_y));
        if (!sierras[i]->estaActivo()) {
            delete sierras[i];
            sierras.removeAt(i);
        }
    }
}

QList<Proyectil*>& LanzaSierras::getProyectiles()
{
    return proyectiles;
}

void LanzaSierras::dibujar(QPainter &painter)
{
    for (auto* s : sierras) s->dibujar(painter);

    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    // Carcasa principal ahusada
    QLinearGradient cuerpoGrad(0, -6, 0, 6);
    cuerpoGrad.setColorAt(0, QColor(90, 92, 98));
    cuerpoGrad.setColorAt(0.5, QColor(55, 57, 62));
    cuerpoGrad.setColorAt(1, QColor(28, 29, 33));
    painter.setBrush(cuerpoGrad);

    QPainterPath cuerpo;
    cuerpo.moveTo(-9, -6);
    cuerpo.lineTo(4, -5);
    cuerpo.lineTo(7, -3);
    cuerpo.lineTo(7, 3);
    cuerpo.lineTo(4, 5);
    cuerpo.lineTo(-9, 6);
    cuerpo.closeSubpath();
    painter.drawPath(cuerpo);

    // Franja de peligro, misma identidad visual que el resto del juego
    painter.setBrush(QColor(230, 190, 20));
    painter.drawRect(-8, -2, 3, 4);
    painter.setBrush(QColor(25, 25, 25));
    painter.drawRect(-5, -2, 2, 4);

    // Empuñadura
    painter.setBrush(QColor(35, 32, 30));
    painter.drawRoundedRect(-8, 3, 5, 6, 2, 2);

    // Rieles guia por donde sale disparada la sierra
    painter.setBrush(QColor(70, 72, 78));
    painter.drawRect(5, -6, 8, 2);
    painter.drawRect(5, 4, 8, 2);

    // Camara frontal: la sierra de repuesto, girando siempre en reposo
    float radioCamara = 5.0f + nivelTamanioSierra * 1.3f;
    painter.save();
    painter.translate(9, 0);
    painter.rotate(anguloCargada);

    QRadialGradient discoGrad(0, 0, radioCamara);
    discoGrad.setColorAt(0, QColor(220, 222, 225));
    discoGrad.setColorAt(0.65, QColor(140, 142, 148));
    discoGrad.setColorAt(1, QColor(70, 72, 78));
    painter.setBrush(discoGrad);
    painter.setPen(QPen(QColor(20, 20, 22), 1));
    painter.drawEllipse(QPointF(0, 0), radioCamara, radioCamara);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(190, 192, 196));
    int dientes = 6 + nivelTamanioSierra;
    for (int i = 0; i < dientes; i++) {
        float a = (360.0f / dientes) * i * M_PI / 180.0f;
        QPolygonF diente;
        diente << QPointF(std::cos(a) * radioCamara, std::sin(a) * radioCamara)
               << QPointF(std::cos(a + 0.12f) * (radioCamara + 2.5f), std::sin(a + 0.12f) * (radioCamara + 2.5f))
               << QPointF(std::cos(a - 0.12f) * (radioCamara + 2.5f), std::sin(a - 0.12f) * (radioCamara + 2.5f));
        painter.drawPolygon(diente);
    }
    painter.setBrush(QColor(40, 40, 44));
    painter.drawEllipse(QPointF(0, 0), radioCamara * 0.3f, radioCamara * 0.3f);
    painter.restore();

    // Aro de contencion alrededor de la camara
    painter.setPen(QPen(QColor(15, 15, 17), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(9, 0), radioCamara + 3.0f, radioCamara + 3.0f);
    painter.setPen(Qt::NoPen);

    // SKIN nivel 1 de cadencia: modulo turbo en la parte trasera
    if (nivelCadenciaSierra >= 1) {
        painter.setBrush(QColor(150, 40, 40));
        painter.drawRoundedRect(-13, -4, 5, 8, 1, 1);
        painter.setPen(QPen(QColor(30, 30, 30), 1));
        painter.drawLine(-13, -1, -8, -1);
        painter.drawLine(-13, 1, -8, 1);
        painter.setPen(Qt::NoPen);
    }

    // SKIN nivel 2 de cadencia: intake turbo brillando, pulsa con el giro
    if (nivelCadenciaSierra >= 2) {
        float pulso = 0.6f + std::sin(anguloCargada * M_PI / 180.0f * 4.0f) * 0.4f;
        QRadialGradient turbo(-15, 0, 4);
        turbo.setColorAt(0, QColor(255, 200, 60, (int)(220 * pulso)));
        turbo.setColorAt(1, QColor(255, 120, 0, 0));
        painter.setBrush(turbo);
        painter.drawEllipse(QPointF(-15, 0), 4.0f, 4.0f);
    }

    painter.restore();
}
