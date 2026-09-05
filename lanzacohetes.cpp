#include "lanzacohetes.h"
#include <QPainter>

Lanzacohetes::Lanzacohetes() : cadenciaDisparo(90), contadorCadencia(0), poolProyectiles(10) // Pocos misiles en pantalla
{
    ancho = 35.0f;
    alto = 15.0f;
    municionMaxima = 4;
    municionMaximaBase = 4;
    municionActual = 4;
    tiempoRecargaMax = 3.5f; // Recarga súper lenta
}

void Lanzacohetes::disparar()
{
    if (contadorCadencia == 0 && municionActual > 0 && !recargando) {
        Proyectil* p = poolProyectiles.obtener();

        // Atributos de reinicio especiales:
        // Velocidad = 8.0f (lento), Daño directo = 100.0f, esExplosivo = true, RadioExplosion = 120.0f
        p->reiniciar(pos_x, pos_y, angulo, 6.0f, 60.0f, true, 90.0f * multiplicadorRadioExplosion, multiplicadorRango);
        proyectiles.append(p);

        contadorCadencia = cadenciaDisparo;
        municionActual--;
    }
}

void Lanzacohetes::actualizarProyectiles()
{
    for (int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if (!proyectiles[i]->estaActivo()) {
            poolProyectiles.liberar(proyectiles[i]);
            proyectiles.removeAt(i);
        }
    }
}

void Lanzacohetes::dibujar(QPainter &painter)
{
    painter.save();
    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // Tubo del lanzacohetes
    painter.setBrush(QColor(60, 80, 60)); // Verde militar
    painter.drawRect(-15, -6, 35, 12);

    // Boca del cañón (más ancha)
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(20, -7, 6, 14);

    // Escape trasero
    painter.drawRect(-18, -5, 3, 10);

    // Mira y agarres
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(0, 6, 4, 6);
    painter.drawRect(8, -10, 4, 4);

    painter.restore();
}

void Lanzacohetes::actualizar(float posX, float posY, QPointF mouseWorld)
{
    pos_x = posX;
    pos_y = posY;

    float dx = mouseWorld.x() - pos_x;
    float dy = mouseWorld.y() - pos_y;
    angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    if (contadorCadencia > 0) contadorCadencia--;
    actualizarRecarga();

    if (municionActual <= 0 && !recargando) {
        iniciarRecarga();
    }
}

QList<Proyectil*>& Lanzacohetes::getProyectiles() { return proyectiles; }
float Lanzacohetes::get_x() { return pos_x; }
float Lanzacohetes::get_y() { return pos_y; }
float Lanzacohetes::get_ancho() { return ancho; }
float Lanzacohetes::get_alto() { return alto; }
