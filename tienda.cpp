#include "tienda.h"
#include <cmath>

Tienda::Tienda(float x, float y, TipoTienda tipo)
    : pos_x(x), pos_y(y), ancho(150.0f), alto(130.0f), // ¡Tamaño casi duplicado!
    rangoInteraccion(180.0f), tipo(tipo) // Rango de interacción más generoso
{
}

void Tienda::dibujar(QPainter &painter)
{
    // Sombra base para darle profundidad a todas las tiendas
    painter.save();
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setPen(Qt::NoPen);
    painter.drawRect(pos_x + 8, pos_y + 8, ancho, alto);
    painter.restore();

    if (tipo == TIENDA_ARMAS) {
        dibujarTiendaArmas(painter);
    } else if (tipo == TIENDA_VIDA) {
        dibujarTiendaVida(painter);
    } else if (tipo == TIENDA_CHICHES) {
        dibujarTiendaChiches(painter);
    } else if (tipo == TIENDA_EXPANSION) {
        dibujarTiendaExpansion(painter);
    }
}

void Tienda::dibujarTiendaArmas(QPainter &painter)
{
    painter.save();

    // Edificio base (Muros de ladrillo/concreto rústico)
    QColor colorPared(160, 140, 120);
    painter.setBrush(colorPared);
    painter.setPen(QPen(QColor(100, 80, 60), 3));
    painter.drawRect(pos_x, pos_y, ancho, alto);

    // Techo a dos aguas clásico
    QColor colorTecho(130, 60, 50);
    painter.setBrush(colorTecho);
    QPolygonF techo;
    techo << QPointF(pos_x - 15, pos_y)
          << QPointF(pos_x + ancho + 15, pos_y)
          << QPointF(pos_x + ancho / 2, pos_y - 45);
    painter.drawPolygon(techo);

    // Puerta doble reforzada
    painter.setBrush(QColor(70, 50, 40));
    painter.drawRect(pos_x + ancho/2 - 25, pos_y + alto - 70, 50, 70);
    painter.setPen(QPen(QColor(40, 30, 20), 2));
    painter.drawLine(pos_x + ancho/2, pos_y + alto - 70, pos_x + ancho/2, pos_y + alto);

    // Ventana con rejas
    painter.setBrush(QColor(100, 150, 200, 150));
    painter.drawRect(pos_x + 20, pos_y + 40, 40, 40);
    painter.setPen(QPen(QColor(50, 50, 50), 3));
    painter.drawLine(pos_x + 30, pos_y + 40, pos_x + 30, pos_y + 80);
    painter.drawLine(pos_x + 50, pos_y + 40, pos_x + 50, pos_y + 80);

    dibujarCartel(painter, "FERRETERÍA\nPIPO");

    painter.restore();
}

void Tienda::dibujarTiendaVida(QPainter &painter)
{
    painter.save();

    // Edificio base (Clínica moderna y limpia)
    QColor colorPared(245, 245, 250);
    painter.setBrush(colorPared);
    painter.setPen(QPen(QColor(180, 180, 200), 3));
    painter.drawRect(pos_x, pos_y, ancho, alto);

    // Techo plano moderno
    painter.setBrush(QColor(150, 160, 170));
    painter.drawRect(pos_x - 5, pos_y - 12, ancho + 10, 12);

    // Gran cruz médica en la pared
    painter.setBrush(QColor(220, 40, 40));
    painter.setPen(Qt::NoPen);
    painter.drawRect(pos_x + ancho/2 - 25, pos_y + 20, 50, 16);
    painter.drawRect(pos_x + ancho/2 - 8, pos_y + 5, 16, 46);

    // Puerta automática de cristal
    painter.setBrush(QColor(200, 240, 255, 200));
    painter.setPen(QPen(QColor(150, 180, 200), 2));
    painter.drawRect(pos_x + ancho/2 - 35, pos_y + alto - 60, 70, 60);
    painter.drawLine(pos_x + ancho/2, pos_y + alto - 60, pos_x + ancho/2, pos_y + alto);

    dibujarCartel(painter, "CLÍNICA\nMALDONADO");

    painter.restore();
}

void Tienda::dibujarTiendaChiches(QPainter &painter)
{
    painter.save();

    // Edificio vibrante
    QColor colorPared(255, 190, 80);
    painter.setBrush(colorPared);
    painter.setPen(QPen(QColor(220, 140, 30), 3));
    painter.drawRect(pos_x, pos_y, ancho, alto);

    // Toldo de kiosco a rayas
    float anchoFranja = (ancho + 20) / 6.0f;
    painter.setPen(Qt::NoPen);
    for(int i = 0; i < 6; i++) {
        if(i % 2 == 0) painter.setBrush(QColor(255, 80, 80)); // Rojo
        else painter.setBrush(QColor(255, 250, 200));        // Crema
        QPolygonF franja;
        franja << QPointF(pos_x - 10 + (i * anchoFranja), pos_y)
               << QPointF(pos_x - 10 + ((i+1) * anchoFranja), pos_y)
               << QPointF(pos_x - 10 + ((i+1) * anchoFranja), pos_y + 25)
               << QPointF(pos_x - 10 + (i * anchoFranja), pos_y + 25);
        painter.drawPolygon(franja);
    }

    // Vidriera grande con "estantes"
    painter.setBrush(QColor(180, 230, 255, 150));
    painter.setPen(QPen(QColor(100, 150, 200), 3));
    painter.drawRect(pos_x + 15, pos_y + 45, 70, 60);
    // Estantes
    painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
    painter.drawLine(pos_x + 15, pos_y + 65, pos_x + 85, pos_y + 65);
    painter.drawLine(pos_x + 15, pos_y + 85, pos_x + 85, pos_y + 85);

    // Puerta colorida
    painter.setBrush(QColor(100, 200, 150));
    painter.setPen(QPen(QColor(60, 150, 100), 3));
    painter.drawRect(pos_x + 95, pos_y + alto - 65, 40, 65);

    dibujarCartel(painter, "CHICHES\nY MÁS");

    painter.restore();
}

void Tienda::dibujarTiendaExpansion(QPainter &painter)
{
    painter.save();

    // Almacén industrial
    QColor colorPared(110, 140, 170);
    painter.setBrush(colorPared);
    painter.setPen(QPen(QColor(60, 80, 110), 3));
    painter.drawRect(pos_x, pos_y, ancho, alto);

    // Techo industrial con bordes
    painter.setBrush(QColor(70, 90, 110));
    painter.drawRect(pos_x - 10, pos_y - 15, ancho + 20, 15);

    // Detalles del techo industrial
    painter.setPen(QPen(QColor(50, 70, 90), 3));
    for(int i = 0; i <= 4; i++) {
        float x_linea = pos_x + (i * (ancho/4));
        painter.drawLine(x_linea, pos_y - 15, x_linea, pos_y);
    }

    // Gran persiana metálica (puerta de garaje)
    painter.setBrush(QColor(180, 190, 200));
    painter.setPen(QPen(QColor(100, 110, 120), 2));
    painter.drawRect(pos_x + 25, pos_y + 40, ancho - 50, alto - 40);

    // Líneas horizontales de la persiana
    for(int i = 1; i < 10; i++) {
        float y_linea = pos_y + 40 + (i * 9);
        painter.drawLine(pos_x + 25, y_linea, pos_x + ancho - 25, y_linea);
    }

    // Símbolo de construcción amarillo
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 200, 50));
    painter.drawRect(pos_x + ancho/2 - 10, pos_y + 15, 20, 5);
    painter.drawRect(pos_x + ancho/2 - 10, pos_y + 15, 5, 20);

    dibujarCartel(painter, "TALLER UY");

    painter.restore();
}

void Tienda::dibujarCartel(QPainter &painter, QString texto)
{
    painter.save();

    // Dimensiones del cartel aumentadas
    float cartelAncho = 130;
    float cartelAlto = 40;
    float cartelX = pos_x + (ancho / 2) - (cartelAncho / 2);
    float cartelY = pos_y - 55;

    // Símil madera/chapa oscura
    painter.setBrush(QColor(50, 50, 60, 220));
    painter.setPen(QPen(QColor(180, 180, 190), 3));
    painter.drawRoundedRect(cartelX, cartelY, cartelAncho, cartelAlto, 8, 8);

    // Texto más grande
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(QRectF(cartelX, cartelY, cartelAncho, cartelAlto),
                     Qt::AlignCenter, texto);

    painter.restore();
}

bool Tienda::estaEnRango(float jugadorX, float jugadorY) const
{
    float centroTiendaX = pos_x + ancho / 2.0f;
    float centroTiendaY = pos_y + alto / 2.0f;

    float dx = jugadorX - centroTiendaX;
    float dy = jugadorY - centroTiendaY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    return distancia <= rangoInteraccion;
}
