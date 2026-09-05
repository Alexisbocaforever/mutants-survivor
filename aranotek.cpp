#include "aranotek.h"
#include <cmath>

AranoTek::AranoTek(float x, float y)
    : Enemigo(x, y),
    anguloCohete(0), anguloAmetralladora(0), anguloCuerpo(90.0f),
    cadenciaCohete(190), contadorCadenciaCohete(60),
    cadenciaEntreBalas(6), cadenciaEntreRafagas(100),
    contadorCadenciaAmetralladora(30),
    cantidadRafaga(5), balasRestantesRafaga(5),
    rangoDeteccion(500.0f), rangoDisparo(430.0f),
    cicloPatas(0.0f), velocidadCicloPatas(0.16f),
    frameAnimacion(0.0f), velocidadAnimacion(0.05f), animacionSubiendo(true),
    destelloCohete(0.0f), destelloAmetralladora(0.0f)
{
    ancho = 56.0f;
    alto = 56.0f;
    velocidad = 1.2f;
    sistemaVida = Vida(170.0f);
    danioContacto = 14.0f;
}

AranoTek::~AranoTek()
{
    qDeleteAll(proyectiles);
    proyectiles.clear();
}
void AranoTek::actualizarConJugador(QPointF jugador) { moverHacia(jugador); }
void AranoTek::dibujar(QPainter &painter)
{
    if(!estaVivo()) return;

    painter.save();

    // ESTILO PIXEL ART: Desactivamos el Antialiasing para bordes duros
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Normalizar ángulo del cuerpo
    float angulo = anguloCuerpo;
    while (angulo < 0) angulo += 360;
    while (angulo >= 360) angulo -= 360;

    // Determinar dirección: 0: Der, 1: Abajo (Frente), 2: Izq, 3: Arriba (Espalda)
    int direccion = 1;
    if (angulo >= 315 || angulo < 45) direccion = 0;
    else if (angulo >= 45 && angulo < 135) direccion = 1;
    else if (angulo >= 135 && angulo < 225) direccion = 2;
    else direccion = 3;

    // --- ORDEN DE CAPAS SEGÚN PERSPECTIVA ---
    dibujarPatas(painter); // Las patas siempre van de fondo

    if (direccion == 3) {
        // Mira Arriba: Brazos al fondo, cuerpo tapa los hombros
        dibujarBrazoLanzacohetes(painter, direccion);
        dibujarBrazoAmetralladora(painter, direccion);
        dibujarCuerpo(painter, direccion);
    } else if (direccion == 0) {
        // Mira Derecha: Brazo izq oculto atrás, luego cuerpo, luego brazo der al frente
        dibujarBrazoLanzacohetes(painter, direccion);
        dibujarCuerpo(painter, direccion);
        dibujarBrazoAmetralladora(painter, direccion);
    } else if (direccion == 2) {
        // Mira Izquierda: Brazo der atrás, cuerpo, brazo izq al frente
        dibujarBrazoAmetralladora(painter, direccion);
        dibujarCuerpo(painter, direccion);
        dibujarBrazoLanzacohetes(painter, direccion);
    } else {
        // Mira Abajo (Frente): Cuerpo de fondo, brazos al frente
        dibujarCuerpo(painter, direccion);
        dibujarBrazoLanzacohetes(painter, direccion);
        dibujarBrazoAmetralladora(painter, direccion);
    }

    painter.restore();

    // Barra de vida
    if(sistemaVida.estaVivo()) {
        float barraAncho = 56.0f;
        float barraAlto = 5.0f;
        float barraX = pos_x;
        float barraY = pos_y - 12.0f;

        painter.setBrush(QColor(90, 0, 0));
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);
        float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
        painter.setBrush(QColor(200, 30, 20));
        painter.drawRect(barraX, barraY, barraAncho * porcentaje, barraAlto);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);
    }
}

void AranoTek::dibujarPatas(QPainter &painter)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float anguloAnclaje[8] = {
        -150.0f, -105.0f, 105.0f, 150.0f,
        -75.0f,  -30.0f,   30.0f,  75.0f
    };

    QColor metalClaro(150, 150, 155);
    QColor metalOscuro(80, 80, 85);
    QColor junta(60, 200, 210);

    // Usar FlatCap y MiterJoin sin Antialiasing crea líneas gruesas pixeladas
    QPen penOscuro(metalOscuro, 4.0f, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    QPen penClaro(metalClaro, 4.0f, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);

    for(int i = 0; i < 8; i++) {
        float fase = cicloPatas + ((i % 2 == 0) ? 0.0f : (float)M_PI);
        float levante = std::max(0.0f, (float)sin(fase)) * 6.0f;
        float avance = (float)cos(fase) * 5.0f;

        float anguloBase = anguloAnclaje[i] * M_PI / 180.0f;
        float largoPata = 30.0f;

        float hipX = centroX + cos(anguloBase) * 14.0f;
        float hipY = centroY + sin(anguloBase) * 10.0f;

        float rodillaX = hipX + cos(anguloBase) * largoPata * 0.55f + avance * 0.4f;
        float rodillaY = hipY + sin(anguloBase) * largoPata * 0.3f - 8.0f;

        float pieX = hipX + cos(anguloBase) * largoPata + avance;
        float pieY = hipY + sin(anguloBase) * largoPata * 0.55f + 10.0f - levante;

        // Lineas cuadradas simulando pixeles
        painter.setPen(penOscuro);
        painter.drawLine(QPointF(hipX, hipY), QPointF(rodillaX, rodillaY));
        painter.setPen(penClaro);
        painter.drawLine(QPointF(rodillaX, rodillaY), QPointF(pieX, pieY));

        // Juntas cuadradas en vez de círculos
        painter.setPen(Qt::NoPen);
        painter.setBrush(junta);
        painter.drawRect(rodillaX - 2, rodillaY - 2, 4, 4);
        painter.setBrush(metalOscuro);
        painter.drawRect(pieX - 2, pieY - 2, 5, 5);
    }
}

void AranoTek::dibujarCuerpo(QPainter &painter, int direccion)
{
    int p = 2; // Tamaño de píxel
    float respY = sin(frameAnimacion) * 1.5f; // Animación de respiración

    QColor metalPlaca(95, 98, 100);
    QColor metalPlacaOscuro(65, 68, 70);
    QColor pielOrco(70, 110, 55);
    QColor pielOscuro(48, 82, 40);
    QColor ojos(210, 30, 20);
    QColor colmillo(235, 230, 215);
    QColor arnes(50, 50, 50);

    painter.setPen(Qt::NoPen);
    float cx = pos_x + ancho / 2.0f;
    float cy = pos_y + alto / 2.0f + 5.0f;

    // 1. BASE MECÁNICA (Tórax) - Bloque sólido
    painter.setBrush(metalPlacaOscuro);
    painter.drawRect(cx - 10*p, cy - 4*p, 20*p, 8*p);
    painter.setBrush(metalPlaca);
    painter.drawRect(cx - 8*p, cy - 5*p, 16*p, 6*p);
    painter.setBrush(QColor(50, 200, 210)); // Luces
    painter.drawRect(cx - 6*p, cy - 2*p, 2*p, 2*p);
    painter.drawRect(cx + 4*p, cy - 2*p, 2*p, 2*p);

    // 2. TORSO DE ORCO (Sprites Direccionales)
    float orcoY = cy - 12*p + respY;

    if (direccion == 1) { // --- ABAJO (Mirando al frente) ---
        // Abdomen
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 6*p, orcoY, 12*p, 10*p);
        // Pectorales (Sombra)
        painter.setBrush(pielOscuro);
        painter.drawRect(cx - 5*p, orcoY + 6*p, 4*p, 2*p);
        painter.drawRect(cx + 1*p, orcoY + 6*p, 4*p, 2*p);
        // Arnés
        painter.setBrush(arnes);
        painter.drawRect(cx - 4*p, orcoY, 2*p, 10*p);
        painter.drawRect(cx + 2*p, orcoY, 2*p, 10*p);
        // Cabeza
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 4*p, orcoY - 6*p, 8*p, 7*p);
        // Ojos y Colmillos
        painter.setBrush(ojos);
        painter.drawRect(cx - 3*p, orcoY - 3*p, 2*p, 1*p);
        painter.drawRect(cx + 1*p, orcoY - 3*p, 2*p, 1*p);
        painter.setBrush(colmillo);
        painter.drawRect(cx - 3*p, orcoY, 1*p, 2*p);
        painter.drawRect(cx + 2*p, orcoY, 1*p, 2*p);
    }
    else if (direccion == 3) { // --- ARRIBA (De espaldas) ---
        // Espalda
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 6*p, orcoY, 12*p, 10*p);
        painter.setBrush(pielOscuro);
        painter.drawRect(cx - 2*p, orcoY + 2*p, 4*p, 6*p);
        // Arnés trasero
        painter.setBrush(arnes);
        painter.drawRect(cx - 4*p, orcoY, 8*p, 2*p);
        // Nuca
        painter.setBrush(pielOscuro);
        painter.drawRect(cx - 4*p, orcoY - 6*p, 8*p, 7*p);
    }
    else if (direccion == 0) { // --- DERECHA (Perfil) ---
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 4*p, orcoY, 8*p, 10*p);
        painter.setBrush(pielOscuro);
        painter.drawRect(cx, orcoY + 6*p, 3*p, 2*p);
        painter.setBrush(arnes);
        painter.drawRect(cx - 1*p, orcoY, 2*p, 10*p);
        // Cabeza perfil der
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 2*p, orcoY - 6*p, 6*p, 7*p);
        painter.setBrush(ojos);
        painter.drawRect(cx + 2*p, orcoY - 3*p, 2*p, 1*p);
        painter.setBrush(colmillo);
        painter.drawRect(cx + 2*p, orcoY, 1*p, 2*p);
    }
    else if (direccion == 2) { // --- IZQUIERDA (Perfil) ---
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 4*p, orcoY, 8*p, 10*p);
        painter.setBrush(pielOscuro);
        painter.drawRect(cx - 3*p, orcoY + 6*p, 3*p, 2*p);
        painter.setBrush(arnes);
        painter.drawRect(cx - 1*p, orcoY, 2*p, 10*p);
        // Cabeza perfil izq
        painter.setBrush(pielOrco);
        painter.drawRect(cx - 4*p, orcoY - 6*p, 6*p, 7*p);
        painter.setBrush(ojos);
        painter.drawRect(cx - 4*p, orcoY - 3*p, 2*p, 1*p);
        painter.setBrush(colmillo);
        painter.drawRect(cx - 3*p, orcoY, 1*p, 2*p);
    }
}

void AranoTek::dibujarBrazoLanzacohetes(QPainter &painter, int direccion)
{
    int p = 2;
    painter.save();

    float hombroX = pos_x + ancho * 0.30f;
    float hombroY = pos_y + alto * 0.42f;

    // Desfase de perspectiva 3D
    if(direccion == 0) hombroX -= 5.0f;
    if(direccion == 2) hombroX += 5.0f;

    painter.translate(hombroX, hombroY);
    painter.rotate(anguloCohete);

    painter.setPen(Qt::NoPen);

    // Tubo lanzacohetes (Hecho con bloques cuadrados)
    painter.setBrush(QColor(55, 70, 55));
    painter.drawRect(-3*p, -3*p, 15*p, 6*p);
    painter.setBrush(QColor(35, 45, 35));
    painter.drawRect(10*p, -4*p, 3*p, 8*p);

    // Cabeza del cohete
    painter.setBrush(QColor(90, 30, 25));
    painter.drawRect(13*p, -1*p, 3*p, 2*p);

    if(destelloCohete > 0.0f) {
        int alpha = (int)(255 * (destelloCohete / 0.18f));
        painter.setBrush(QColor(255, 200, 80, alpha));
        painter.drawRect(16*p, -2*p, 4*p, 4*p);
    }

    painter.restore();
}

void AranoTek::dibujarBrazoAmetralladora(QPainter &painter, int direccion)
{
    int p = 2;
    painter.save();

    float hombroX = pos_x + ancho * 0.70f;
    float hombroY = pos_y + alto * 0.42f;

    // Desfase de perspectiva 3D
    if(direccion == 0) hombroX -= 5.0f;
    if(direccion == 2) hombroX += 5.0f;

    painter.translate(hombroX, hombroY);
    painter.rotate(anguloAmetralladora);

    painter.setPen(Qt::NoPen);

    // Receptor y cargador (Hecho con bloques cuadrados)
    painter.setBrush(QColor(40, 40, 42));
    painter.drawRect(-3*p, -2*p, 7*p, 5*p);
    painter.setBrush(QColor(30, 30, 32));
    painter.drawRect(-1*p, 3*p, 3*p, 4*p);

    // Cañon
    painter.setBrush(QColor(55, 55, 58));
    painter.drawRect(4*p, -1*p, 10*p, 2*p);

    if(destelloAmetralladora > 0.0f) {
        int alpha = (int)(255 * (destelloAmetralladora / 0.08f));
        painter.setBrush(QColor(255, 230, 140, alpha));
        painter.drawRect(14*p, -1*p, 3*p, 2*p);
    }

    painter.restore();
}

void AranoTek::actualizar()
{
    if(!vivo) {
        tiempoDesaparecer -= 0.016f;
        return;
    }

    cicloPatas += velocidadCicloPatas;
    if(cicloPatas > 2.0f * M_PI) cicloPatas -= 2.0f * M_PI;

    if(animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
        if(frameAnimacion > 1.5f) animacionSubiendo = false;
    } else {
        frameAnimacion -= velocidadAnimacion;
        if(frameAnimacion < -1.5f) animacionSubiendo = true;
    }

    if(contadorCadenciaCohete > 0) contadorCadenciaCohete--;
    if(contadorCadenciaAmetralladora > 0) contadorCadenciaAmetralladora--;

    if(destelloCohete > 0.0f) destelloCohete -= 0.016f;
    if(destelloAmetralladora > 0.0f) destelloAmetralladora -= 0.016f;
}

void AranoTek::moverHacia(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia > 0) {
        // Actualiza la rotación del cuerpo para que mire hacia donde camina
        anguloCuerpo = std::atan2(dy, dx) * 180.0f / M_PI;

        if(distancia > rangoDisparo * 0.75f) {
            dx = (dx / distancia) * velocidad;
            dy = (dy / distancia) * velocidad;
            mover(dx, dy);
        }
        else if(distancia < rangoDisparo * 0.45f) {
            dx = -(dx / distancia) * velocidad * 0.7f;
            dy = -(dy / distancia) * velocidad * 0.7f;
            mover(dx, dy);
        }
    }
}

void AranoTek::dispararHacia(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia <= rangoDeteccion) {
        float anguloObjetivo = std::atan2(dy, dx) * 180.0f / M_PI;
        anguloCohete = anguloObjetivo;
        anguloAmetralladora = anguloObjetivo;
        anguloCuerpo = anguloObjetivo; // Obliga al torso a mirar al jugador al apuntar
    }

    if(distancia <= rangoDisparo && distancia > 40.0f) {
        dispararCohete(objetivo);
        dispararBala(objetivo);
    }
}

void AranoTek::dispararCohete(QPointF objetivo)
{
    if(contadorCadenciaCohete > 0) return;
    float centroX = pos_x + ancho * 0.30f;
    float centroY = pos_y + alto * 0.42f;
    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    Proyectil* p = new Proyectil();
    p->reiniciar(centroX, centroY, angulo, 6.5f, 50.0f, true, 85.0f);
    proyectiles.append(p);


    destelloCohete = 0.18f;
    contadorCadenciaCohete = cadenciaCohete;
}

void AranoTek::dispararBala(QPointF objetivo)
{
    if(contadorCadenciaAmetralladora > 0) return;
    float centroX = pos_x + ancho * 0.70f;
    float centroY = pos_y + alto * 0.42f;
    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;

    float anguloBase = std::atan2(dy, dx) * 180.0f / M_PI;
    float dispersion = -4.0f + (rand() % 80) / 10.0f;
    float angulo = anguloBase + dispersion;

    Proyectil* p = new Proyectil();
    p->reiniciar(centroX, centroY, angulo, 12.0f, 6.0f, false, 0.0f);
    proyectiles.append(p);


    destelloAmetralladora = 0.08f;
    balasRestantesRafaga--;

    if(balasRestantesRafaga > 0) {
        contadorCadenciaAmetralladora = cadenciaEntreBalas;
    } else {
        balasRestantesRafaga = cantidadRafaga;
        contadorCadenciaAmetralladora = cadenciaEntreRafagas;
    }
}

void AranoTek::actualizarProyectiles()
{
    for(int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if(!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
}

QList<Proyectil*>& AranoTek::getProyectiles()
{
    return proyectiles;
}
