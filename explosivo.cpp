#include "explosivo.h"
#include <cmath>
#include <cstdlib>
#include <QRadialGradient>
#include <QColor>

Explosivo::Explosivo(float x, float y, float radio)
    : pos_x(x), pos_y(y), radio(radio), tiempoMax(1.3f), tiempoVida(1.3f)
{
    // tiempoMax mas largo que antes para dejar tiempo a que el humo se disipe
    // despues de que la bola de fuego ya desaparecio
    generarFragmentos();
}

void Explosivo::generarFragmentos()
{
    // 1. generar chispas rapidas y brillantes
    int cantidadChispas = 12;
    for(int i = 0; i < cantidadChispas; i++) {
        FragmentoExplosion f;
        f.angulo = (rand() % 360) * M_PI / 180.0f;
        f.velocidad = radio * (0.09f + (rand() % 12) / 100.0f);
        f.distanciaActual = 0.0f;
        f.tam = 1.5f + (rand() % 20) / 10.0f;
        f.tipo = 0;
        f.rotacionActual = 0.0f;
        f.velocidadRotacion = 0.0f;
        fragmentos.append(f);
    }

    // 2. generar escombros que giran y caen un poco mas lento
    int cantidadEscombros = 7;
    for(int i = 0; i < cantidadEscombros; i++) {
        FragmentoExplosion f;
        f.angulo = (rand() % 360) * M_PI / 180.0f;
        f.velocidad = radio * (0.05f + (rand() % 8) / 100.0f);
        f.distanciaActual = 0.0f;
        f.tam = 2.0f + (rand() % 30) / 10.0f;
        f.tipo = 2;
        f.rotacionActual = rand() % 360;
        f.velocidadRotacion = -6.0f + (rand() % 120) / 10.0f;
        fragmentos.append(f);
    }

    // 3. generar puffs de humo, se mueven poco y quedan flotando arriba
    int cantidadHumo = 6;
    for(int i = 0; i < cantidadHumo; i++) {
        FragmentoExplosion f;
        f.angulo = (rand() % 360) * M_PI / 180.0f;
        f.velocidad = radio * (0.015f + (rand() % 4) / 100.0f);
        f.distanciaActual = 0.0f;
        f.tam = radio * (0.25f + (rand() % 20) / 100.0f);
        f.tipo = 1;
        f.rotacionActual = 0.0f;
        f.velocidadRotacion = 0.0f;
        fragmentos.append(f);
    }
}

void Explosivo::actualizar()
{
    tiempoVida -= 0.016f; // Aprox 60 FPS

    for(int i = 0; i < fragmentos.size(); i++) {
        FragmentoExplosion &f = fragmentos[i];
        f.distanciaActual += f.velocidad;
        f.rotacionActual += f.velocidadRotacion;

        // Las chispas y los escombros frenan un poco con el tiempo
        if(f.tipo == 0 || f.tipo == 2) {
            f.velocidad *= 0.94f;
        }
    }
}

void Explosivo::dibujar(QPainter &painter)
{
    if (tiempoVida <= 0) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Progreso general de 0.0 (inicio) a 1.0 (fin del efecto completo)
    float progreso = 1.0f - (tiempoVida / tiempoMax);

    // 1. flash blanco muy breve, marca el instante del impacto
    dibujarFlashInicial(painter, progreso);

    // 2. onda de choque expandiendose
    dibujarOndaDeChoque(painter, progreso);

    // 3. bola de fuego central con degradado
    dibujarBolaDeFuego(painter, progreso);

    // 4. chispas, escombros y humo
    dibujarFragmentos(painter, progreso);

    painter.restore();
}

void Explosivo::dibujarFlashInicial(QPainter &painter, float progreso)
{
    float duracionFlash = 0.12f;
    if(progreso >= duracionFlash) return;

    float subProgreso = progreso / duracionFlash;
    int alpha = (int)(255 * (1.0f - subProgreso));
    // El flash ya no supera el radio real de la explosion (antes llegaba a 1.5x)
    float radioFlash = radio * (0.65f + 0.35f * subProgreso);

    QRadialGradient gradiente(pos_x, pos_y, radioFlash);
    gradiente.setColorAt(0.0, QColor(255, 255, 240, alpha));
    gradiente.setColorAt(0.5, QColor(255, 240, 180, alpha / 2));
    gradiente.setColorAt(1.0, QColor(255, 220, 140, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradiente);
    painter.drawEllipse(QPointF(pos_x, pos_y), radioFlash, radioFlash);
}

void Explosivo::dibujarOndaDeChoque(QPainter &painter, float progreso)
{
    float duracionOnda = 0.4f;
    if(progreso >= duracionOnda) return;

    float subProgreso = progreso / duracionOnda;
    // El radio maximo de la onda ahora es exactamente "radio", antes llegaba a 1.6x
    // y hacia ver la explosion mas grande de lo que en verdad hacia daño
    float radioOnda = radio * (0.3f + 0.7f * subProgreso);
    int alpha = (int)(200 * (1.0f - subProgreso));
    float grosor = 4.0f * (1.0f - subProgreso) + 1.0f;

    painter.setPen(QPen(QColor(255, 210, 120, alpha), grosor));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(pos_x, pos_y), radioOnda, radioOnda);
}

void Explosivo::dibujarBolaDeFuego(QPainter &painter, float progreso)
{
    float duracionFuego = 0.55f;
    float subProgreso = progreso / duracionFuego;
    if(subProgreso > 1.0f) subProgreso = 1.0f;

    // El radio crece rapido al principio y se frena hacia el final (ease out)
    float crecimiento = 1.0f - (1.0f - subProgreso) * (1.0f - subProgreso);
    // El radio maximo de la bola de fuego ahora es exactamente "radio",
    // antes llegaba a 1.1x y no coincidia con el area que realmente hace daño
    float radioAnimado = radio * (0.25f + 0.75f * crecimiento);

    int alpha = (int)(255 * (1.0f - subProgreso));
    if(alpha < 0) alpha = 0;
    if(alpha > 255) alpha = 255;

    // Nucleo blanco amarillento en el centro, borde rojo oscuro humeante
    QRadialGradient gradienteFuego(pos_x, pos_y, radioAnimado);
    gradienteFuego.setColorAt(0.0, QColor(255, 250, 210, alpha));
    gradienteFuego.setColorAt(0.25, QColor(255, 200, 60, alpha));
    gradienteFuego.setColorAt(0.55, QColor(255, 110, 20, (int)(alpha * 0.9f)));
    gradienteFuego.setColorAt(0.8, QColor(180, 30, 10, (int)(alpha * 0.7f)));
    gradienteFuego.setColorAt(1.0, QColor(60, 20, 20, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradienteFuego);
    painter.drawEllipse(QPointF(pos_x, pos_y), radioAnimado, radioAnimado);
}

void Explosivo::dibujarFragmentos(QPainter &painter, float progreso)
{
    painter.setPen(Qt::NoPen);

    for(const FragmentoExplosion &f : fragmentos) {
        float x = pos_x + cos(f.angulo) * f.distanciaActual;
        float y = pos_y + sin(f.angulo) * f.distanciaActual;

        if(f.tipo == 0) {
            // Chispa, brillo breve que va de blanco a naranja
            float duracion = 0.5f;
            float subProgreso = progreso / duracion;
            if(subProgreso >= 1.0f) continue;
            int alpha = (int)(255 * (1.0f - subProgreso));
            QColor colorChispa(255, (int)(230 - 120 * subProgreso), 40, alpha);
            painter.setBrush(colorChispa);
            float tamActual = f.tam * (1.0f - 0.5f * subProgreso);
            painter.drawEllipse(QPointF(x, y), tamActual, tamActual);
        }
        else if(f.tipo == 2) {
            // Escombro, cuadrado oscuro que gira y se apaga un poco mas lento
            float duracion = 0.7f;
            float subProgreso = progreso / duracion;
            if(subProgreso >= 1.0f) continue;
            int alpha = (int)(220 * (1.0f - subProgreso));
            QColor colorEscombro(70, 55, 45, alpha);
            painter.save();
            painter.translate(x, y);
            painter.rotate(f.rotacionActual);
            painter.setBrush(colorEscombro);
            painter.drawRect(QRectF(-f.tam / 2.0f, -f.tam / 2.0f, f.tam, f.tam));
            painter.restore();
        }
        else {
            // Humo, se expande despacio y flota hacia arriba durante toda la vida del efecto
            float yHumo = y - progreso * radio * 0.6f;
            float tamHumo = f.tam * (0.6f + 0.7f * progreso);
            int alpha = (int)(110 * (1.0f - progreso));
            if(alpha < 0) alpha = 0;
            QColor colorHumo(90, 85, 80, alpha);
            painter.setBrush(colorHumo);
            painter.drawEllipse(QPointF(x, yHumo), tamHumo, tamHumo);
        }
    }
}
