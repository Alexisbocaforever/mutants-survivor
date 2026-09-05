#include "invocador.h"
#include <cmath>

Invocador::Invocador(float x, float y)
    : Enemigo(x, y),
    anguloCuerpo(90.0f),
    frameAnimacion(0.0f), velocidadAnimacion(0.06f), animacionSubiendo(true),
    cooldownDisparo(0.0f),
    frameMovimiento(0.0f), enMovimiento(false),
    timerInvocacionBerserker(INVOCADOR_INTERVALO_INVOCACION_BERSERKER)
{
    ancho = 60.0f;
    alto = 64.0f;
    velocidad = 1.0f;
    sistemaVida = Vida(250.0f);
    danioContacto = 20.0f;
}

void Invocador::actualizar()
{
    // 1 esta funcion la llama el loop principal para TODOS los enemigos sin
    //   importar el tipo, y ademas se llama a actualizar(x,y) por separado
    //   para el invocador, asi que si esta vivo no hace falta nada aca, todo
    //   el trabajo por frame vive en actualizar(x,y). la unica excepcion es
    //   el cadaver, porque una vez muerto actualizar(x,y) ya no se llama mas
    //   y esta es la unica funcion que sigue corriendo para descontarle el
    //   tiempo de desaparicion
    if(!vivo) {
        tiempoDesaparecer -= 0.016f;
    }
}
void Invocador::actualizarConJugador(QPointF jugador) {
    moverHacia(jugador);
    actualizar(jugador.x(), jugador.y());
}
void Invocador::dibujarProyectilesPropios(QPainter &painter) {
    for(auto& p : proyectiles) p.dibujar(painter);
}
void Invocador::actualizar(qreal xJugador, qreal yJugador)
{
    if(!vivo) return;

    // 2 respiracion / flotacion, animacion pasiva
    if(animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
        if(frameAnimacion > 2.0f) animacionSubiendo = false;
    } else {
        frameAnimacion -= velocidadAnimacion;
        if(frameAnimacion < -2.0f) animacionSubiendo = true;
    }

    // 3 animacion de piernas al moverse
    if(enMovimiento) {
        frameMovimiento += 0.12f;          // velocidad de zancada
        if(frameMovimiento > 1.0f)
            frameMovimiento -= 1.0f;
    } else {
        // vuelve suavemente a la posicion neutra
        frameMovimiento *= 0.8f;
        if(fabs(frameMovimiento) < 0.01f) frameMovimiento = 0.0f;
    }

    // 4 cuenta regresiva para la proxima invocacion de berserkers khorne
    if (timerInvocacionBerserker > 0.0f) {
        timerInvocacionBerserker -= 0.016f;
    }

    QPointF posJugador(xJugador, yJugador);

    if(cooldownDisparo > 0) {
        cooldownDisparo -= 0.016f;
    } else {
        float dx = xJugador - (pos_x + ancho/2.0f);
        float dy = yJugador - (pos_y + alto/2.0f);
        float distancia = std::sqrt(dx*dx + dy*dy);

        if(distancia > 0 && distancia < 400.0f) {
            float angulo = std::atan2(dy, dx) * 180.0f / M_PI;
            proyectiles.push_back(ProyectilBuscador(pos_x + ancho/2.0f,
                                                    pos_y + alto/2.0f,
                                                    angulo, 3.0f, 15.0f));
            cooldownDisparo = 2.0f;
        }
    }

    // CORRECCIÓN: primero movemos, luego reorientamos
    for (auto it = proyectiles.begin(); it != proyectiles.end(); ) {
        it->actualizar();                 // mueve y descuenta vida
        it->actualizarHacia(posJugador);  // corrige dirección
        if (!it->estaActivo()) {
            it = proyectiles.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<ProyectilBuscador>& Invocador::getProyectiles()
{
    return proyectiles;
}

bool Invocador::consumirDeseoDeInvocarBerserkers()
{
    // 1 mientras este vivo, cada 20 segundos devuelve true una sola vez
    //   y reinicia la cuenta regresiva para la proxima invocacion
    if (!vivo) return false;

    if (timerInvocacionBerserker <= 0.0f) {
        timerInvocacionBerserker = INVOCADOR_INTERVALO_INVOCACION_BERSERKER;
        return true;
    }
    return false;
}

void Invocador::moverHacia(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia > 0) {
        anguloCuerpo = std::atan2(dy, dx) * 180.0f / M_PI;

        if(distancia > 200.0f) {
            mover((dx / distancia) * velocidad, (dy / distancia) * velocidad);
            enMovimiento = true;
        } else if (distancia < 150.0f) {
            mover(-(dx / distancia) * velocidad * 0.5f, -(dy / distancia) * velocidad * 0.5f);
            enMovimiento = true;
        } else {
            enMovimiento = false;  // parado en rango medio
        }
    }
}

// =================================================
// DIBUJO PRINCIPAL
// =================================================
void Invocador::dibujar(QPainter &painter)
{
    if(!estaVivo()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    float angulo = anguloCuerpo;
    while (angulo < 0) angulo += 360;
    while (angulo >= 360) angulo -= 360;

    int p = 2; // escala de píxeles
    float respY = frameAnimacion;             // flotación vertical
    float fase = frameAnimacion * 1.8f;       // para runas mágicas
    float cx = pos_x + ancho / 2.0f;
    float cy = pos_y + alto / 2.0f + respY;

    // Elegir vista según ángulo
    if (angulo >= 315 || angulo < 45) {
        dibujarDerecha(painter, cx, cy, p, fase);
    } else if (angulo >= 45 && angulo < 135) {
        dibujarAbajo(painter, cx, cy, p, fase);
    } else if (angulo >= 135 && angulo < 225) {
        dibujarIzquierda(painter, cx, cy, p, fase);
    } else {
        dibujarArriba(painter, cx, cy, p, fase);
    }

    painter.restore();
}

// ========================================================
// VISTA FRONTAL (mirando al jugador)
// ========================================================
void Invocador::dibujarAbajo(QPainter &painter, float cx, float cy, int p, float fase)
{
    QColor azulCaos(15, 35, 100);
    QColor azulLuz(30, 60, 160);
    QColor amarillo(220, 200, 40);       // toque amarillo / dorado
    QColor azulOscuro(15, 25, 70);       // para el tabardo
    QColor cianRuna(0, 255, 255);
    painter.setPen(Qt::NoPen);

    // --- Bastón de Tzeentch (detrás) ---
    float bX = cx + 16*p;
    float bY = cy - 14*p;
    painter.setBrush(QColor(40, 30, 20));
    painter.drawRect(bX, bY, 2*p, 28*p);            // mango
    // Cabeza del bastón: ojo de Tzeentch
    painter.setBrush(amarillo);
    painter.drawRect(bX - 3*p, bY - 6*p, 8*p, 6*p);   // base amarilla
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(bX - 2*p, bY - 5*p, 6*p, 4*p);   // pupila
    painter.setBrush(cianRuna);
    painter.drawRect(bX, bY - 4*p, 2*p, 2*p);         // punto central
    // Destello mágico
    painter.setBrush(QColor(0, 255, 255, 180));
    painter.drawRect(bX + 2*p, bY - 6*p, 2*p, 2*p);

    // --- Brazos traseros (sujetando el bastón) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 14*p, cy, 4*p, 8*p);
    painter.drawRect(cx + 10*p, cy, 4*p, 8*p);

    // --- Cresta / Tocado (casco original) ---
    painter.setBrush(amarillo);
    painter.drawRect(cx - 12*p, cy - 16*p, 24*p, 12*p);   // base del tocado
    painter.setBrush(azulLuz);
    painter.drawRect(cx - 10*p, cy - 15*p, 4*p, 10*p);    // detalle azul
    painter.drawRect(cx - 2*p, cy - 16*p, 4*p, 10*p);
    painter.drawRect(cx + 6*p, cy - 15*p, 4*p, 10*p);

    // --- Cabeza y Ojos (yelmo simple) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 6*p, cy - 10*p, 12*p, 10*p);
    painter.setBrush(QColor(20, 255, 230));               // ojos cyan
    painter.drawRect(cx - 3*p, cy - 6*p, 2*p, 2*p);
    painter.drawRect(cx + 1*p, cy - 6*p, 2*p, 2*p);

    // --- Torso con ornamentos amarillos ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 8*p, cy, 16*p, 14*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 8*p, cy, 16*p, 2*p);            // banda pectoral
    // Hombreras amarillas
    painter.drawRect(cx - 10*p, cy, 3*p, 6*p);
    painter.drawRect(cx + 7*p, cy, 3*p, 6*p);
    painter.setBrush(QColor(150, 130, 20));
    painter.drawRect(cx - 9*p, cy + 1*p, 1*p, 1*p);
    painter.drawRect(cx + 8*p, cy + 1*p, 1*p, 1*p);

    // --- Tabardo, mas ancho que el torso (antes quedaba angosto y el
    // cuerpo se veia "estrangulado" en la cintura) ---
    painter.setBrush(azulOscuro);
    painter.drawRect(cx - 10*p, cy + 14*p, 20*p, 10*p);
    painter.setBrush(cianRuna);
    painter.drawRect(cx - 2*p, cy + 16*p, 4*p, 4*p);       // runa en el pecho del tabardo
    painter.setBrush(amarillo);
    painter.drawRect(cx - 10*p, cy + 22*p, 20*p, 2*p);     // borde inferior

    // --- Piernas, alineadas con el ancho nuevo del tabardo, con
    // balanceo suave en vez del salto binario que tenia antes ---
    float despX = std::sin(frameMovimiento * 2.0f * M_PI) * (float)p;
    // Pierna izquierda
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 9*p - despX, cy + 24*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 9*p - despX, cy + 24*p, 6*p, 2*p);
    // Pierna derecha
    painter.setBrush(azulCaos);
    painter.drawRect(cx + 3*p + despX, cy + 24*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx + 3*p + despX, cy + 24*p, 6*p, 2*p);

    // --- Brazos delanteros (inferiores) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 12*p, cy + 8*p, 4*p, 8*p);
    painter.drawRect(cx + 8*p, cy + 8*p, 4*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 12*p, cy + 14*p, 4*p, 3*p);     // guantelete izquierdo
    painter.drawRect(cx + 8*p, cy + 14*p, 4*p, 3*p);      // guantelete derecho

    // --- Runas flotantes mágicas ---
    float r1y = cy - 18*p + 3*p * sin(fase);
    float r2y = cy - 2*p + 3*p * sin(fase + 1.2f);
    painter.setBrush(QColor(0, 255, 255, 200));
    painter.drawRect(bX - 4*p, r1y, 2*p, 2*p);
    painter.drawRect(bX + 4*p, r2y, 2*p, 2*p);
}

// ========================================================
// VISTA TRASERA (mirando hacia arriba)
// ========================================================
void Invocador::dibujarArriba(QPainter &painter, float cx, float cy, int p, float fase)
{
    QColor azulCaos(15, 35, 100);
    QColor azulLuz(30, 60, 160);
    QColor amarillo(220, 200, 40);
    QColor azulOscuro(15, 25, 70);
    QColor cianRuna(0, 255, 255);
    painter.setPen(Qt::NoPen);

    // --- Bastón (asoma por encima del hombro, visto desde atrás) ---
    float bX = cx + 14*p;
    float bY = cy - 14*p;
    painter.setBrush(QColor(40, 30, 20));
    painter.drawRect(bX, bY, 2*p, 26*p);
    painter.setBrush(amarillo);
    painter.drawRect(bX - 3*p, bY - 6*p, 8*p, 6*p);
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(bX - 2*p, bY - 5*p, 6*p, 4*p);
    painter.setBrush(cianRuna);
    painter.drawRect(bX, bY - 4*p, 2*p, 2*p);
    painter.setBrush(QColor(0, 255, 255, 180));
    painter.drawRect(bX + 2*p, bY - 6*p, 2*p, 2*p);

    // --- Capa (azul oscuro con borde amarillo) ---
    painter.setBrush(azulOscuro);
    painter.drawRect(cx - 7*p, cy + 2*p, 14*p, 22*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 7*p, cy + 22*p, 14*p, 2*p);

    // --- Torso (espalda de la armadura) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 8*p, cy, 16*p, 10*p);

    // --- Cresta / Tocado (visto desde atrás) ---
    painter.setBrush(amarillo);
    painter.drawRect(cx - 12*p, cy - 16*p, 24*p, 12*p);
    painter.setBrush(azulLuz);
    painter.drawRect(cx - 10*p, cy - 15*p, 4*p, 10*p);
    painter.drawRect(cx - 2*p, cy - 16*p, 4*p, 10*p);
    painter.drawRect(cx + 6*p, cy - 15*p, 4*p, 10*p);

    // --- Brazos superiores (detrás) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 14*p, cy, 4*p, 8*p);
    painter.drawRect(cx + 10*p, cy, 4*p, 8*p);

    // --- Piernas (animación desde atrás) ---
    int piernaFase = (int)(frameMovimiento * 2.0f) % 2;
    int despX = piernaFase * p;
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 6*p - despX, cy + 24*p, 5*p, 8*p);
    painter.drawRect(cx + 1*p + despX, cy + 24*p, 5*p, 8*p);

    // --- Runas flotantes ---
    float r1y = cy - 18*p + 3*p * sin(fase);
    float r2y = cy - 2*p + 3*p * sin(fase + 1.2f);
    painter.setBrush(QColor(0, 255, 255, 200));
    painter.drawRect(bX - 4*p, r1y, 2*p, 2*p);
    painter.drawRect(bX + 4*p, r2y, 2*p, 2*p);
}

// ========================================================
// VISTA LATERAL DERECHA
// ========================================================
void Invocador::dibujarDerecha(QPainter &painter, float cx, float cy, int p, float fase)
{
    QColor azulCaos(15, 35, 100);
    QColor amarillo(220, 200, 40);
    QColor azulOscuro(15, 25, 70);
    QColor cianRuna(0, 255, 255);
    painter.setPen(Qt::NoPen);

    // --- Bastón (en la mano del frente) ---
    float bX = cx + 8*p;
    float bY = cy - 12*p;
    painter.setBrush(QColor(40, 30, 20));
    painter.drawRect(bX, bY, 2*p, 26*p);
    painter.setBrush(amarillo);
    painter.drawRect(bX - 2*p, bY - 6*p, 6*p, 6*p);
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(bX - 1*p, bY - 5*p, 4*p, 4*p);
    painter.setBrush(cianRuna);
    painter.drawRect(bX, bY - 3*p, 2*p, 2*p);
    painter.setBrush(QColor(0, 255, 255, 180));
    painter.drawRect(bX + 3*p, bY - 7*p, 2*p, 2*p);

    // --- Brazo que sujeta el bastón ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx, cy + 2*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx, cy + 8*p, 6*p, 3*p);

    // --- Cresta / Tocado lateral ---
    painter.setBrush(amarillo);
    painter.drawRect(cx - 4*p, cy - 16*p, 8*p, 12*p);

    // --- Cabeza y ojo (yelmo simple) ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 4*p, cy - 10*p, 8*p, 10*p);
    painter.setBrush(QColor(20, 255, 230));
    painter.drawRect(cx + 2*p, cy - 6*p, 2*p, 2*p);

    // --- Torso con banda amarilla ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 5*p, cy, 10*p, 14*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 5*p, cy + 4*p, 10*p, 2*p);

    // --- Tabardo lateral (azul oscuro) ---
    painter.setBrush(azulOscuro);
    painter.drawRect(cx - 3*p, cy + 14*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 3*p, cy + 20*p, 6*p, 2*p);

    // --- Piernas laterales (animación) ---
    int piernaFase = (int)(frameMovimiento * 2.0f) % 2;
    int despY = piernaFase * p;   // desplazamiento vertical para simular zancada lateral
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 4*p, cy + 22*p + despY, 4*p, 8*p);
    painter.drawRect(cx + 1*p, cy + 22*p - despY, 4*p, 8*p);

    // --- Runas flotantes ---
    float r1y = cy - 18*p + 3*p * sin(fase);
    painter.setBrush(QColor(0, 255, 255, 200));
    painter.drawRect(bX - 3*p, r1y, 2*p, 2*p);
}

// ========================================================
// VISTA LATERAL IZQUIERDA (espejo de la derecha)
// ========================================================
void Invocador::dibujarIzquierda(QPainter &painter, float cx, float cy, int p, float fase)
{
    QColor azulCaos(15, 35, 100);
    QColor amarillo(220, 200, 40);
    QColor azulOscuro(15, 25, 70);
    QColor cianRuna(0, 255, 255);
    painter.setPen(Qt::NoPen);

    // --- Bastón (en la mano izquierda) ---
    float bX = cx - 10*p;
    float bY = cy - 12*p;
    painter.setBrush(QColor(40, 30, 20));
    painter.drawRect(bX, bY, 2*p, 26*p);
    painter.setBrush(amarillo);
    painter.drawRect(bX - 4*p, bY - 6*p, 6*p, 6*p);
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(bX - 3*p, bY - 5*p, 4*p, 4*p);
    painter.setBrush(cianRuna);
    painter.drawRect(bX - 2*p, bY - 3*p, 2*p, 2*p);
    painter.setBrush(QColor(0, 255, 255, 180));
    painter.drawRect(bX - 5*p, bY - 7*p, 2*p, 2*p);

    // --- Brazo delantero ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 6*p, cy + 2*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 6*p, cy + 8*p, 6*p, 3*p);

    // --- Cresta / Tocado lateral ---
    painter.setBrush(amarillo);
    painter.drawRect(cx - 4*p, cy - 16*p, 8*p, 12*p);

    // --- Cabeza y ojo ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 4*p, cy - 10*p, 8*p, 10*p);
    painter.setBrush(QColor(20, 255, 230));
    painter.drawRect(cx - 4*p, cy - 6*p, 2*p, 2*p);

    // --- Torso ---
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 5*p, cy, 10*p, 14*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 5*p, cy + 4*p, 10*p, 2*p);

    // --- Tabardo ---
    painter.setBrush(azulOscuro);
    painter.drawRect(cx - 3*p, cy + 14*p, 6*p, 8*p);
    painter.setBrush(amarillo);
    painter.drawRect(cx - 3*p, cy + 20*p, 6*p, 2*p);

    // --- Piernas (animación) ---
    int piernaFase = (int)(frameMovimiento * 2.0f) % 2;
    int despY = piernaFase * p;
    painter.setBrush(azulCaos);
    painter.drawRect(cx - 4*p, cy + 22*p + despY, 4*p, 8*p);
    painter.drawRect(cx + 1*p, cy + 22*p - despY, 4*p, 8*p);

    // --- Runas flotantes ---
    float r1y = cy - 18*p + 3*p * sin(fase);
    painter.setBrush(QColor(0, 255, 255, 200));
    painter.drawRect(bX + 1*p, r1y, 2*p, 2*p);
}
