#include "zombiearmado.h"
#include <cmath>
#include <cstdlib>

ZombieArmado::ZombieArmado(float x, float y, int tipo)
    : Enemigo(x, y), tipoArma(tipo), anguloMovimiento(0), anguloArma(0),
    contadorCadencia(0), rangoDeteccion(400.0f), rangoDisparo(350.0f),
    frameAnimacion(0.0f), velocidadAnimacion(0.06f), animacionSubiendo(true)
{
    ancho = 35.0f;
    alto = 35.0f;

    // Skin aleatoria
    tipoSkin = std::rand() % 4;

    // Configuración según tipo de arma
    if(tipoArma == 1) { // Pistola
        velocidad = 1.5f;
        sistemaVida = Vida(60.0f);
        danioContacto = 8.0f;
        cadenciaDisparo = 40;
    }
    else if(tipoArma == 2) { // Granadas
        velocidad = 1.2f;
        sistemaVida = Vida(80.0f);
        danioContacto = 10.0f;
        cadenciaDisparo = 120;
    }
    else if(tipoArma == 3) { // Lanzacohetes
        velocidad = 1.0f;
        sistemaVida = Vida(100.0f);
        danioContacto = 12.0f;
        cadenciaDisparo = 150;
    }
}
void ZombieArmado::actualizarConJugador(QPointF jugador) {
    moverHacia(jugador);
    dispararHacia(jugador);
    actualizarProyectiles();
}
void ZombieArmado::dibujarProyectilesPropios(QPainter &painter) {
    for(auto* p : proyectiles) if(p && p->estaActivo()) p->dibujar(painter);
}
ZombieArmado::~ZombieArmado()
{
    qDeleteAll(proyectiles);
    proyectiles.clear();
}

void ZombieArmado::dibujar(QPainter &painter)
{
    if(!estaVivo()) return;

    // 1 el cuerpo usa una de 4 poses fijas segun hacia donde apunta,
    //   ya no rota el cuerpo entero como antes
    if (anguloArma >= -45 && anguloArma < 45) {
        dibujarZombieDerecha(painter);
    } else if (anguloArma >= 45 && anguloArma < 135) {
        dibujarZombieAbajo(painter);
    } else if (anguloArma >= -135 && anguloArma < -45) {
        dibujarZombieArriba(painter);
    } else {
        dibujarZombieIzquierda(painter);
    }

    // 2 el arma si sigue rotando, para que apunte exacto hacia el objetivo
    painter.save();
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    painter.translate(centroX, centroY);
    painter.rotate(anguloArma);
    painter.translate(-centroX, -centroY);
    dibujarArma(painter);
    painter.restore();

    // Barra de vida
    if(sistemaVida.estaVivo()) {
        float barraAncho = 35.0f;
        float barraAlto = 4.0f;
        float barraX = pos_x;
        float barraY = pos_y - 10.0f;

        painter.setBrush(QColor(40, 0, 0, 200));
        painter.setPen(Qt::NoPen);
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);

        float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
        painter.setBrush(QColor(220, 40, 40));
        painter.drawRect(barraX, barraY, barraAncho * porcentaje, barraAlto);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);
    }
}
void ZombieArmado::dibujarZombieAbajo(QPainter &painter)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);
    float offset = frameAnimacion * pixelSize;

    QColor colorPiel, colorChaleco, colorCasco, colorOjos;
    obtenerColoresSkin(colorPiel, colorChaleco, colorCasco, colorOjos);

    painter.setBrush(QColor(0, 0, 0, 70));
    painter.drawEllipse(pos_x + 3*pixelSize, pos_y + 13*pixelSize, 12*pixelSize, 6*pixelSize);

    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 11*pixelSize + offset, 3*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 11*pixelSize - offset, 3*pixelSize, 5*pixelSize);

    painter.setBrush(colorChaleco);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 5*pixelSize, 10*pixelSize, 7*pixelSize);

    if (tipoSkin == 0 || tipoSkin == 1) {
        painter.setBrush(QColor(20, 20, 20, 180));
        painter.drawRect(pos_x + 5*pixelSize, pos_y + 6*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 10*pixelSize, pos_y + 6*pixelSize, 3*pixelSize, 2*pixelSize);
    } else if (tipoSkin == 2) {
        painter.setBrush(QColor(200, 160, 40));
        painter.drawRect(pos_x + 5*pixelSize, pos_y + 6*pixelSize, 2*pixelSize, 1*pixelSize);
        painter.drawRect(pos_x + 8*pixelSize, pos_y + 8*pixelSize, 2*pixelSize, 1*pixelSize);
        painter.drawRect(pos_x + 11*pixelSize, pos_y + 10*pixelSize, 2*pixelSize, 1*pixelSize);
    } else if (tipoSkin == 3) {
        painter.setBrush(QColor(160, 20, 20));
        painter.drawRect(pos_x + 6*pixelSize, pos_y + 7*pixelSize, 2*pixelSize, 4*pixelSize);
    }

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 6*pixelSize - offset, 2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 6*pixelSize + offset, 2*pixelSize, 5*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 1*pixelSize, 8*pixelSize, 5*pixelSize);

    if (tipoSkin == 0 || tipoSkin == 1) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 4*pixelSize, pos_y + 0*pixelSize, 10*pixelSize, 3*pixelSize);
        painter.drawRect(pos_x + 4*pixelSize, pos_y + 2*pixelSize, 10*pixelSize, 1*pixelSize);
    } else if (tipoSkin == 2) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 5*pixelSize, pos_y + 1*pixelSize, 8*pixelSize, 2*pixelSize);
        painter.drawRect(pos_x + 13*pixelSize, pos_y + 2*pixelSize, 2*pixelSize, 2*pixelSize);
    }

    painter.setBrush(colorOjos);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 3*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 3*pixelSize, 2*pixelSize, 1*pixelSize);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 3*pixelSize, 1*pixelSize, 1*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 3*pixelSize, 1*pixelSize, 1*pixelSize);
}

void ZombieArmado::dibujarZombieArriba(QPainter &painter)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);
    float offset = frameAnimacion * pixelSize;

    QColor colorPiel, colorChaleco, colorCasco, colorOjos;
    obtenerColoresSkin(colorPiel, colorChaleco, colorCasco, colorOjos);

    painter.setBrush(QColor(0, 0, 0, 70));
    painter.drawEllipse(pos_x + 3*pixelSize, pos_y + 13*pixelSize, 12*pixelSize, 6*pixelSize);

    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 11*pixelSize + offset, 3*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 11*pixelSize - offset, 3*pixelSize, 5*pixelSize);

    // 1 torso de espaldas, con tiras del chaleco marcando la columna
    painter.setBrush(colorChaleco.darker(115));
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 5*pixelSize, 10*pixelSize, 7*pixelSize);
    painter.setBrush(colorChaleco.darker(140));
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 5*pixelSize, 2*pixelSize, 7*pixelSize);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 5*pixelSize, 2*pixelSize, 7*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 6*pixelSize - offset, 2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 14*pixelSize, pos_y + 6*pixelSize + offset, 2*pixelSize, 5*pixelSize);

    // 2 nuca, sin ojos visibles
    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 1*pixelSize, 8*pixelSize, 5*pixelSize);

    if (tipoSkin == 0 || tipoSkin == 1) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 4*pixelSize, pos_y + 0*pixelSize, 10*pixelSize, 4*pixelSize);
    } else if (tipoSkin == 2) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 5*pixelSize, pos_y + 0*pixelSize, 8*pixelSize, 3*pixelSize);
    }
}

void ZombieArmado::dibujarZombieDerecha(QPainter &painter)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);
    float offset = frameAnimacion * pixelSize;

    QColor colorPiel, colorChaleco, colorCasco, colorOjos;
    obtenerColoresSkin(colorPiel, colorChaleco, colorCasco, colorOjos);

    painter.setBrush(QColor(0, 0, 0, 70));
    painter.drawEllipse(pos_x + 3*pixelSize, pos_y + 13*pixelSize, 12*pixelSize, 6*pixelSize);

    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(pos_x + 7*pixelSize + offset, pos_y + 11*pixelSize, 3*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 7*pixelSize - offset, pos_y + 11*pixelSize, 3*pixelSize, 5*pixelSize);

    painter.setBrush(colorChaleco);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 5*pixelSize, 7*pixelSize, 7*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 6*pixelSize, pos_y + 6*pixelSize - offset, 2*pixelSize, 5*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 7*pixelSize, pos_y + 1*pixelSize, 7*pixelSize, 5*pixelSize);

    if (tipoSkin == 0 || tipoSkin == 1) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 6*pixelSize, pos_y + 0*pixelSize, 8*pixelSize, 3*pixelSize);
    } else if (tipoSkin == 2) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 7*pixelSize, pos_y + 1*pixelSize, 7*pixelSize, 2*pixelSize);
    }

    painter.setBrush(colorOjos);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 3*pixelSize, 2*pixelSize, 1*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 12*pixelSize + offset*0.5f, pos_y + 6*pixelSize, 2*pixelSize, 5*pixelSize);
}

void ZombieArmado::dibujarZombieIzquierda(QPainter &painter)
{
    int pixelSize = 2;
    painter.setPen(Qt::NoPen);
    float offset = frameAnimacion * pixelSize;

    QColor colorPiel, colorChaleco, colorCasco, colorOjos;
    obtenerColoresSkin(colorPiel, colorChaleco, colorCasco, colorOjos);

    painter.setBrush(QColor(0, 0, 0, 70));
    painter.drawEllipse(pos_x + 3*pixelSize, pos_y + 13*pixelSize, 12*pixelSize, 6*pixelSize);

    painter.setBrush(QColor(30, 30, 35));
    painter.drawRect(pos_x + 8*pixelSize + offset, pos_y + 11*pixelSize, 3*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 8*pixelSize - offset, pos_y + 11*pixelSize, 3*pixelSize, 5*pixelSize);

    painter.setBrush(colorChaleco);
    painter.drawRect(pos_x + 5*pixelSize, pos_y + 5*pixelSize, 7*pixelSize, 7*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 10*pixelSize, pos_y + 6*pixelSize - offset, 2*pixelSize, 5*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 1*pixelSize, 7*pixelSize, 5*pixelSize);

    if (tipoSkin == 0 || tipoSkin == 1) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 4*pixelSize, pos_y + 0*pixelSize, 8*pixelSize, 3*pixelSize);
    } else if (tipoSkin == 2) {
        painter.setBrush(colorCasco);
        painter.drawRect(pos_x + 4*pixelSize, pos_y + 1*pixelSize, 7*pixelSize, 2*pixelSize);
    }

    painter.setBrush(colorOjos);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 3*pixelSize, 2*pixelSize, 1*pixelSize);

    painter.setBrush(colorPiel);
    painter.drawRect(pos_x + 4*pixelSize - offset*0.5f, pos_y + 6*pixelSize, 2*pixelSize, 5*pixelSize);
}


void ZombieArmado::dibujarArma(QPainter &painter)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    // Pincel dinámico sobre el eje traducido del zombie
    if(tipoArma == 1) { // Pistola táctica
        // Cuerpo metálico
        painter.setBrush(QColor(30, 30, 35));
        painter.drawRect(centroX + 2, centroY - 2, 14, 4);
        // Luz del metal
        painter.setBrush(QColor(90, 95, 105));
        painter.drawRect(centroX + 4, centroY - 2, 10, 1);
        // Empuñadura
        painter.setBrush(QColor(60, 40, 20));
        painter.drawRect(centroX, centroY, 4, 5);
    }
    else if(tipoArma == 2) { // Lanzagranadas
        // Cañón ancho
        painter.setBrush(QColor(50, 65, 50));
        painter.drawRect(centroX - 2, centroY - 4, 18, 7);
        // Cañón interior oscuro
        painter.setBrush(QColor(20, 20, 20));
        painter.drawRect(centroX + 14, centroY - 3, 3, 5);
        // Tambor de granadas
        painter.setBrush(QColor(80, 80, 85));
        painter.drawEllipse(centroX + 2, centroY - 3, 6, 6);
    }
    else if(tipoArma == 3) { // Lanzacohetes RPG
        // Tubo lanzador pesado
        painter.setBrush(QColor(60, 75, 50));
        painter.drawRect(centroX - 6, centroY - 5, 24, 8);
        // Estructura metálica del frente
        painter.setBrush(QColor(30, 30, 30));
        painter.drawRect(centroX + 16, centroY - 6, 3, 10);
        // Ojiva / Cohete asomándose
        painter.setBrush(QColor(180, 50, 40)); // Punta roja del cohete
        QPolygonF cohete;
        cohete << QPointF(centroX + 19, centroY - 4)
               << QPointF(centroX + 24, centroY)
               << QPointF(centroX + 19, centroY + 4);
        painter.drawPolygon(cohete);
        // Mirilla
        painter.setBrush(QColor(200, 200, 50));
        painter.drawRect(centroX + 4, centroY - 7, 3, 2);
    }
}

void ZombieArmado::actualizar()
{
    if (!vivo) {
        tiempoDesaparecer -= 0.016f;
        return;
    }
    if(animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
        if(frameAnimacion > 1.5f) animacionSubiendo = false;
    } else {
        frameAnimacion -= velocidadAnimacion;
        if(frameAnimacion < -1.5f) animacionSubiendo = true;
    }

    if(contadorCadencia > 0) {
        contadorCadencia--;
    }
}

void ZombieArmado::moverHacia(QPointF objetivo)
{
    float dx = objetivo.x() - (pos_x + ancho / 2.0f);
    float dy = objetivo.y() - (pos_y + alto / 2.0f);
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia > 0) {
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        anguloArma = anguloMovimiento;

        if(distancia > rangoDisparo) {
            dx = (dx / distancia) * velocidad;
            dy = (dy / distancia) * velocidad;
            mover(dx, dy);
        }
    }
}

void ZombieArmado::dispararHacia(QPointF objetivo)
{
    if(contadorCadencia > 0) return;

    float dx = objetivo.x() - (pos_x + ancho / 2.0f);
    float dy = objetivo.y() - (pos_y + alto / 2.0f);
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia <= rangoDisparo && distancia > 50.0f) {
        crearProyectil(objetivo);
        contadorCadencia = cadenciaDisparo;
    }
}

void ZombieArmado::crearProyectil(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    Proyectil* p = nullptr;

    if(tipoArma == 1) {
        p = new Proyectil();
        p->reiniciar(centroX, centroY, angulo, 10.0f, 10.0f);
    }
    else if(tipoArma == 2) {
        p = new Proyectil();
        p->reiniciar(centroX, centroY, angulo, 5.0f, 40.0f, true, 80.0f);
    }
    else if(tipoArma == 3) {
        p = new Proyectil();
        p->reiniciar(centroX, centroY, angulo, 7.0f, 60.0f, true, 100.0f);
    }

    if(p) {
        proyectiles.append(p);
    }
}

void ZombieArmado::actualizarProyectiles()
{
    for(int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if(!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
}

QList<Proyectil*>& ZombieArmado::getProyectiles()
{
    return proyectiles;
}
void ZombieArmado::obtenerColoresSkin(QColor &colorPiel, QColor &colorChaleco, QColor &colorCasco, QColor &colorOjos)
{
    switch(tipoSkin) {
    case 0: // Mercenario Militar (Camuflado Verde)
        colorPiel = QColor(70, 110, 70);
        colorChaleco = QColor(50, 75, 45);
        colorCasco = QColor(65, 85, 55);
        colorOjos = QColor(255, 50, 50);
        break;
    case 1: // SWAT / Fuerzas Especiales
        colorPiel = QColor(80, 95, 85);
        colorChaleco = QColor(30, 30, 35);
        colorCasco = QColor(20, 20, 25);
        colorOjos = QColor(255, 200, 0);
        break;
    case 2: // Superviviente (Bandana Roja)
        colorPiel = QColor(85, 120, 80);
        colorChaleco = QColor(100, 70, 45);
        colorCasco = QColor(190, 30, 30);
        colorOjos = QColor(220, 30, 30);
        break;
    case 3: // Cientifico infectado
    default:
        colorPiel = QColor(100, 150, 120);
        colorChaleco = QColor(200, 205, 210);
        colorCasco = QColor(100, 150, 120);
        colorOjos = QColor(120, 255, 50);
        break;
    }
}
