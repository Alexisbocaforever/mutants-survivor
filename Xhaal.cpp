#include "Xhaal.h"
#include <cmath>
#include <cstdlib>
#include <QRadialGradient>

Xhaal::Xhaal(float x, float y)
    : Enemigo(x, y), anguloArma(0), contadorCadencia(0),
    rangoDeteccion(600.0f), rangoDisparo(500.0f),
    frameAnimacion(0.0f), velocidadAnimacion(0.08f), animacionSubiendo(true),
    timerTeletransporte(0.0f), cooldownTeletransporte(8.0f), puedeTransportarse(true),
    timerEfectoTeletransporte(0.0f),
    parpadeo(0.0f), pestañasCerradas(false),
    objetivoX(0.0f), objetivoY(0.0f), tieneObjetivo(false),
    pulsoNucleo(0.0f),
    timerTextos(0.0f), indiceTextoActual(0),
    tipoAtaqueActual(0), timerCambioAtaque(0.0f)
{
    ancho = 60.0f;
    alto = 70.0f;
    velocidad = 3.5f;
    sistemaVida.setVidaMaxima(2000.0f);
    sistemaVida.restaurarCompleta();
    danioContacto = 45.0f; // REDUCIDO de 50.0 a 45.0
    cadenciaDisparo = 30;

    for(int i = 0; i < 6; i++) {
        anguloBrazos[i] = (rand() % 360);
        velocidadBrazos[i] = 1.0f + (rand() % 20) / 10.0f;
    }

    mensajes << "I am the lord of everything"
             << "I don like this world"
             << "You are anything"
             << "I want this world only for me ";
}

Xhaal::~Xhaal()
{
    qDeleteAll(proyectiles);
    proyectiles.clear();
    qDeleteAll(proyectilesBuscadores);
    proyectilesBuscadores.clear();
    qDeleteAll(textosFlotantes);
    textosFlotantes.clear();
    qDeleteAll(particulasTeleport);
    particulasTeleport.clear();
}

void Xhaal::dibujarProyectilesPropios(QPainter &painter) {
    for(auto* p : proyectiles) if(p && p->estaActivo()) p->dibujar(painter);
    for(auto* pb : proyectilesBuscadores) if(pb && pb->estaActivo()) pb->dibujar(painter);
}
void Xhaal::dibujar(QPainter &painter)
{
    if(!estaVivo()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    int pixelSize = 2;
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    // 1. aura de furia cuando la vida esta baja, se dibuja primero para quedar detras del cuerpo
    dibujarAuraFuria(painter);

    // 2. anillo de energia residual justo despues de teletransportarse
    if(timerEfectoTeletransporte > 0.0f) {
        float progreso = 1.0f - (timerEfectoTeletransporte / 0.4f);
        float radioAnillo = 15.0f + progreso * 75.0f;
        int alphaAnillo = (int)(220 * (1.0f - progreso));
        if(alphaAnillo < 0) alphaAnillo = 0;

        painter.setPen(QPen(QColor(190, 60, 255, alphaAnillo), 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPointF(centroX, centroY), radioAnillo, radioAnillo);
    }

    // 3. oscilacion independiente de cada uno de los 6 brazos
    float offsetBrazo[6];
    float amplitudNivel[3] = {3.0f, 2.2f, 1.5f};
    for(int i = 0; i < 6; i++) {
        float amplitud = amplitudNivel[i / 2];
        offsetBrazo[i] = sin(anguloBrazos[i] * M_PI / 180.0f) * amplitud;
    }

    // 4. respiracion sutil del torso
    float respiracion = sin(frameAnimacion * 0.4f) * 1.2f;

    painter.setPen(Qt::NoPen);

    // Colores estilo "Manos" - Ser deforme y monstruoso
    QColor piel(240, 235, 230); // Piel muy palida, casi cadaverica
    QColor sombra(180, 175, 170);
    QColor ojos(10, 10, 10);
    QColor pupila(200, 0, 0); // Rojo sangre

    // === CUERPO DEFORME (como Manos del comic) ===
    painter.setBrush(piel);

    // Torso irregular y grotesco, con leve respiracion vertical
    painter.drawRect(pos_x + 3*pixelSize, pos_y + 8*pixelSize + respiracion, 10*pixelSize, 12*pixelSize);

    // Protuberancias del cuerpo (deformidades)
    painter.setBrush(sombra);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 10*pixelSize + respiracion, 3*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 12*pixelSize + respiracion, 3*pixelSize, 3*pixelSize);

    // Nucleo pulsante visible entre las protuberancias, referencia visual de la vida del jefe
    {
        float pulso = 0.5f + 0.5f * sin(pulsoNucleo);
        int alphaNucleo = (int)(140 + 100 * pulso);
        float radioNucleo = pixelSize * (1.6f + 0.5f * pulso);
        QColor colorNucleo(255, (int)(40 + 60 * (1.0f - pulso)), 40, alphaNucleo);
        painter.setBrush(colorNucleo);
        painter.drawEllipse(QPointF(pos_x + 8*pixelSize, pos_y + 13*pixelSize + respiracion), radioNucleo, radioNucleo);
    }

    // === 6 BRAZOS DEFORMES CON MANOS GROTESCAS ===
    painter.setBrush(piel);

    // NIVEL 1 - Brazos superiores (mas largos), brazo 0 izquierdo, brazo 1 derecho
    painter.drawRect(pos_x + 1*pixelSize, pos_y + 8*pixelSize + offsetBrazo[0], 2*pixelSize, 7*pixelSize);
    // Mano izquierda GRANDE con 8 dedos largos
    painter.drawRect(pos_x - 1*pixelSize, pos_y + 15*pixelSize + offsetBrazo[0], 4*pixelSize, 3*pixelSize);
    for(int d = 0; d < 8; d++) {
        painter.drawRect(pos_x + (-1.5f - d*0.4f)*pixelSize, pos_y + 18*pixelSize + offsetBrazo[0],
                         pixelSize*0.7f, 4*pixelSize);
    }

    painter.drawRect(pos_x + 13*pixelSize, pos_y + 8*pixelSize + offsetBrazo[1], 2*pixelSize, 7*pixelSize);
    // Mano derecha GRANDE con 8 dedos largos
    painter.drawRect(pos_x + 13*pixelSize, pos_y + 15*pixelSize + offsetBrazo[1], 4*pixelSize, 3*pixelSize);
    for(int d = 0; d < 8; d++) {
        painter.drawRect(pos_x + (13.5f + d*0.4f)*pixelSize, pos_y + 18*pixelSize + offsetBrazo[1],
                         pixelSize*0.7f, 4*pixelSize);
    }

    // NIVEL 2 - Brazos medios (saliendo del torso), brazo 2 izquierdo, brazo 3 derecho
    painter.drawRect(pos_x + 0.5f*pixelSize, pos_y + 11*pixelSize + offsetBrazo[2],
                     2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x - 0.5f*pixelSize, pos_y + 16*pixelSize + offsetBrazo[2],
                     3*pixelSize, 2.5f*pixelSize);
    for(int d = 0; d < 7; d++) {
        painter.drawRect(pos_x + (-1.2f - d*0.35f)*pixelSize, pos_y + 18.5f*pixelSize + offsetBrazo[2],
                         pixelSize*0.6f, 3*pixelSize);
    }

    painter.drawRect(pos_x + 13.5f*pixelSize, pos_y + 11*pixelSize + offsetBrazo[3],
                     2*pixelSize, 5*pixelSize);
    painter.drawRect(pos_x + 13.5f*pixelSize, pos_y + 16*pixelSize + offsetBrazo[3],
                     3*pixelSize, 2.5f*pixelSize);
    for(int d = 0; d < 7; d++) {
        painter.drawRect(pos_x + (13.7f + d*0.35f)*pixelSize, pos_y + 18.5f*pixelSize + offsetBrazo[3],
                         pixelSize*0.6f, 3*pixelSize);
    }

    // NIVEL 3 - Brazos inferiores (mas cortos y retorcidos), brazo 4 izquierdo, brazo 5 derecho
    painter.drawRect(pos_x + 2*pixelSize, pos_y + 14*pixelSize + offsetBrazo[4],
                     2*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 1*pixelSize, pos_y + 18*pixelSize + offsetBrazo[4],
                     3*pixelSize, 2*pixelSize);
    for(int d = 0; d < 6; d++) {
        painter.drawRect(pos_x + (0.5f - d*0.3f)*pixelSize, pos_y + 20*pixelSize + offsetBrazo[4],
                         pixelSize*0.5f, 2.5f*pixelSize);
    }

    painter.drawRect(pos_x + 12*pixelSize, pos_y + 14*pixelSize + offsetBrazo[5],
                     2*pixelSize, 4*pixelSize);
    painter.drawRect(pos_x + 12*pixelSize, pos_y + 18*pixelSize + offsetBrazo[5],
                     3*pixelSize, 2*pixelSize);
    for(int d = 0; d < 6; d++) {
        painter.drawRect(pos_x + (12.5f + d*0.3f)*pixelSize, pos_y + 20*pixelSize + offsetBrazo[5],
                         pixelSize*0.5f, 2.5f*pixelSize);
    }

    // === PIERNAS DEFORMES ===
    painter.setBrush(piel);
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 20*pixelSize + respiracion*0.3f,
                     3*pixelSize, 6*pixelSize);
    painter.drawRect(pos_x + 9*pixelSize, pos_y + 20*pixelSize + respiracion*0.3f,
                     3*pixelSize, 6*pixelSize);

    // === CABEZA GROTESCA (estilo Manos) ===
    // Cabeza ovalada y deformada
    painter.drawRect(pos_x + 4*pixelSize, pos_y + 1*pixelSize + respiracion*0.5f, 8*pixelSize, 8*pixelSize);

    // Protuberancia craneal (caracteristica de Manos)
    painter.drawRect(pos_x + 5*pixelSize, pos_y - 1*pixelSize + respiracion*0.5f, 6*pixelSize, 3*pixelSize);

    // === MULTIPLES OJOS ASIMETRICOS (como Manos), ahora siguen al objetivo ===
    float posOjosX[] = {5.5f, 7.5f, 9.5f, 6.0f, 8.5f, 10.5f, 5.0f, 7.0f, 9.0f, 11.0f};
    float posOjosY[] = {3.0f, 2.5f, 3.5f, 4.5f, 4.0f, 5.0f, 6.0f, 6.5f, 6.0f, 5.5f};

    // 1. calcular hacia donde miran las pupilas
    float miradaX = 0.0f;
    float miradaY = 0.0f;
    if(tieneObjetivo) {
        float dx = objetivoX - centroX;
        float dy = objetivoY - centroY;
        float dist = std::sqrt(dx*dx + dy*dy);
        if(dist > 1.0f) {
            miradaX = (dx / dist) * pixelSize * 0.22f;
            miradaY = (dy / dist) * pixelSize * 0.22f;
        }
    }

    for(int i = 0; i < 10; i++) {
        float ojoX = pos_x + posOjosX[i]*pixelSize;
        float ojoY = pos_y + posOjosY[i]*pixelSize + respiracion*0.5f;

        if(!pestañasCerradas) {
            painter.setBrush(Qt::white);
            painter.drawEllipse(QPointF(ojoX, ojoY), pixelSize*0.7f, pixelSize*0.9f);

            painter.setBrush(pupila);
            painter.drawEllipse(QPointF(ojoX + miradaX, ojoY + miradaY), pixelSize*0.35f, pixelSize*0.45f);
        } else {
            painter.setPen(QPen(ojos, 1.5));
            painter.drawLine(QPointF(ojoX - pixelSize*0.6f, ojoY),
                             QPointF(ojoX + pixelSize*0.6f, ojoY));
            painter.setPen(Qt::NoPen);
        }
    }

    // === TEXTURA/DETALLES (venas, cicatrices) ===
    painter.setPen(QPen(sombra, 1));
    for(int i = 0; i < 5; i++) {
        float venaY = pos_y + (10 + i*2)*pixelSize + respiracion;
        painter.drawLine(pos_x + 5*pixelSize, venaY, pos_x + 11*pixelSize, venaY);
    }
    painter.setPen(Qt::NoPen);

    painter.restore();

    // Particulas de teletransporte, se dibujan en coordenadas de mundo, no dependen de pos_x/pos_y actuales
    dibujarParticulasTeleport(painter);

    // === BARRA DE VIDA (ESPECIAL PARA BOSS) ===
    if(sistemaVida.estaVivo()) {
        float barraAncho = 100.0f;
        float barraAlto = 8.0f;
        float barraX = pos_x - 10.0f;
        float barraY = pos_y - 20.0f;

        // Fondo negro
        painter.fillRect(barraX, barraY, barraAncho, barraAlto, QColor(0, 0, 0, 200));

        // Vida (degradado rojo-amarillo)
        float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
        QLinearGradient gradient(barraX, barraY, barraX + barraAncho * porcentaje, barraY);
        gradient.setColorAt(0, QColor(255, 0, 0));
        gradient.setColorAt(1, QColor(255, 200, 0));
        painter.fillRect(barraX, barraY, barraAncho * porcentaje, barraAlto, gradient);

        // Borde
        painter.setPen(QPen(QColor(255, 255, 255), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);

        // Nombre del boss
        painter.setPen(QColor(255, 50, 50));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(barraX, barraY - 8, " ELLOS ");
    }

    // Dibujar textos flotantes
    dibujarTextos(painter);
}

void Xhaal::dibujarAuraFuria(QPainter &painter)
{
    float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
    if(porcentaje >= 0.35f) return; // Solo aparece con poca vida

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float pulso = 0.5f + 0.5f * sin(pulsoNucleo * 1.5f);
    float radioAura = 55.0f + pulso * 15.0f;
    int alphaAura = (int)(90 * (1.0f - porcentaje / 0.35f)) + (int)(30 * pulso);
    if(alphaAura > 160) alphaAura = 160;

    QRadialGradient gradienteAura(centroX, centroY, radioAura);
    gradienteAura.setColorAt(0.0, QColor(255, 30, 30, alphaAura));
    gradienteAura.setColorAt(1.0, QColor(255, 30, 30, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradienteAura);
    painter.drawEllipse(QPointF(centroX, centroY), radioAura, radioAura);
}

void Xhaal::dibujarTextos(QPainter &painter)
{
    painter.save();

    for(auto* texto : textosFlotantes) {
        QColor colorTexto(255, 100, 255, texto->alpha);
        painter.setPen(colorTexto);
        painter.setFont(QFont("Arial", 14, QFont::Bold));

        // Efecto de sombra
        painter.setPen(QColor(0, 0, 0, texto->alpha / 2));
        painter.drawText(texto->posX + 2, texto->posY + 2, texto->texto);

        painter.setPen(colorTexto);
        painter.drawText(texto->posX, texto->posY, texto->texto);
    }

    painter.restore();
}

void Xhaal::actualizar()
{
    if(!vivo) {
        tiempoDesaparecer -= 0.016f;
        actualizarParticulasTeleport();
        return;
    }

    // Animacion general
    if(animacionSubiendo) {
        frameAnimacion += velocidadAnimacion;
        if(frameAnimacion > 3.0f) animacionSubiendo = false;
    } else {
        frameAnimacion -= velocidadAnimacion;
        if(frameAnimacion < 0.0f) animacionSubiendo = true;
    }

    // Animacion de brazos, cada uno con su propia velocidad
    for(int i = 0; i < 6; i++) {
        anguloBrazos[i] += velocidadBrazos[i];
        if(anguloBrazos[i] > 360.0f) anguloBrazos[i] -= 360.0f;
    }

    // Pulso del nucleo, se acelera cuando queda poca vida
    float porcentajeVida = sistemaVida.getPorcentajeVida() / 100.0f;
    pulsoNucleo += 0.05f + (1.0f - porcentajeVida) * 0.15f;

    // Parpadeo de ojos
    parpadeo += 0.016f;
    if(parpadeo > 3.0f) { // Parpadea cada 3 segundos
        pestañasCerradas = true;
        if(parpadeo > 3.2f) { // Cierra por 0.2 segundos
            pestañasCerradas = false;
            parpadeo = 0.0f;
        }
    }

    // Actualizar cadencia de disparo
    if(contadorCadencia > 0) {
        contadorCadencia--;
    }

    // Timer de teletransporte
    timerTeletransporte += 0.016f;
    if(timerTeletransporte >= cooldownTeletransporte) {
        puedeTransportarse = true;
    }

    // Efecto residual tras teletransportarse
    if(timerEfectoTeletransporte > 0.0f) {
        timerEfectoTeletransporte -= 0.016f;
    }

    // Timer de textos
    timerTextos += 0.016f;
    if(timerTextos >= 5.0f) { // Cada 5 segundos genera texto
        generarTextoAleatorio();
        timerTextos = 0.0f;
    }

    // Actualizar textos flotantes y particulas
    actualizarTextos();
    actualizarParticulasTeleport();
}

void Xhaal::generarTextoAleatorio()
{
    TextoFlotante* nuevo = new TextoFlotante();
    nuevo->texto = mensajes[indiceTextoActual];
    nuevo->posX = pos_x + ancho / 2.0f - 80.0f; // Centrar aprox
    nuevo->posY = pos_y - 40.0f;
    nuevo->tiempoVida = 3.0f;
    nuevo->alpha = 255;

    textosFlotantes.append(nuevo);

    indiceTextoActual = (indiceTextoActual + 1) % mensajes.size();
}

void Xhaal::actualizarTextos()
{
    for(int i = textosFlotantes.size() - 1; i >= 0; i--) {
        textosFlotantes[i]->tiempoVida -= 0.016f;
        textosFlotantes[i]->posY -= 0.5f; // Flotar hacia arriba

        // Fade out
        float porcentaje = textosFlotantes[i]->tiempoVida / 3.0f;
        textosFlotantes[i]->alpha = (int)(255 * porcentaje);

        if(textosFlotantes[i]->tiempoVida <= 0) {
            delete textosFlotantes[i];
            textosFlotantes.removeAt(i);
        }
    }
}

void Xhaal::crearParticulasTeleport(float x, float y)
{
    int cantidad = 14;
    for(int i = 0; i < cantidad; i++) {
        float angulo = (rand() % 360) * M_PI / 180.0f;
        float rapidez = 2.0f + (rand() % 40) / 10.0f;

        ParticulaTeleport* p = new ParticulaTeleport();
        p->x = x;
        p->y = y;
        p->vx = cos(angulo) * rapidez;
        p->vy = sin(angulo) * rapidez;
        p->vidaMax = 0.45f + (rand() % 30) / 100.0f;
        p->vida = p->vidaMax;
        p->tam = 2.0f + (rand() % 30) / 10.0f;

        particulasTeleport.append(p);
    }
}

void Xhaal::actualizarParticulasTeleport()
{
    for(int i = particulasTeleport.size() - 1; i >= 0; i--) {
        ParticulaTeleport* p = particulasTeleport[i];
        p->x += p->vx;
        p->y += p->vy;
        p->vx *= 0.92f; // Friccion, la particula frena de a poco
        p->vy *= 0.92f;
        p->vida -= 0.016f;

        if(p->vida <= 0.0f) {
            delete p;
            particulasTeleport.removeAt(i);
        }
    }
}

void Xhaal::dibujarParticulasTeleport(QPainter &painter)
{
    if(particulasTeleport.isEmpty()) return;

    painter.save();
    painter.setPen(Qt::NoPen);

    for(auto* p : particulasTeleport) {
        float progreso = p->vida / p->vidaMax;
        int alpha = (int)(255 * progreso);

        // Nucleo violeta electrico con halo cian, estilo portal alienigena
        QColor nucleo(200, 80, 255, alpha);
        QColor halo(120, 220, 255, alpha / 2);

        painter.setBrush(halo);
        painter.drawEllipse(QPointF(p->x, p->y), p->tam * 1.8f, p->tam * 1.8f);

        painter.setBrush(nucleo);
        painter.drawEllipse(QPointF(p->x, p->y), p->tam * progreso, p->tam * progreso);
    }

    painter.restore();
}

void Xhaal::moverHacia(QPointF objetivo)
{
    objetivoX = objetivo.x();
    objetivoY = objetivo.y();
    tieneObjetivo = true;

    if(!vivo) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia > rangoDisparo * factorMapa * 0.7f) {
        anguloArma = std::atan2(dy, dx) * 180.0f / M_PI;

        // Moverse hacia el jugador
        if(distancia > rangoDisparo * 0.7f) {
            dx = (dx / distancia) * velocidad;
            dy = (dy / distancia) * velocidad;
            mover(dx, dy);
        }
    }
}

void Xhaal::teletransportarse(QPointF jugador)
{
    if(!puedeTransportarse) return;

    // Rafaga de particulas en el punto de partida
    float salidaX = pos_x + ancho / 2.0f;
    float salidaY = pos_y + alto / 2.0f;
    crearParticulasTeleport(salidaX, salidaY);

    // Teletransportarse cerca del jugador (radio de 150-250 pixeles)
    float angulo = (rand() % 360) * M_PI / 180.0f;
    float distancia = (150.0f + (rand() % 100)) * factorMapa; // 150-250

    pos_x = jugador.x() + cos(angulo) * distancia;
    pos_y = jugador.y() + sin(angulo) * distancia;

    // Rafaga de particulas en el punto de llegada, mas el anillo de energia
    crearParticulasTeleport(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);
    timerEfectoTeletransporte = 0.4f;

    // Al teletransportarse, dispara 4 orbes en cruz
    crearProyectilesCruz();

    puedeTransportarse = false;
    timerTeletransporte = 0.0f;
}

void Xhaal::crearProyectilesCruz()
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float angulos[] = {270.0f, 0.0f, 90.0f, 180.0f};

    for(int i = 0; i < 4; i++) {
        // En lugar de: Proyectil* p = new Proyectil(centroX, centroY, angulos[i], 15.0f, 25.0f, false, 0.0f);
        Proyectil* p = new Proyectil();
        p->reiniciar(centroX, centroY, angulos[i], 15.0f, 25.0f, false, 0.0f);

        proyectiles.append(p);
    }
}

void Xhaal::dispararHacia(QPointF objetivo, QPointF velocidadJugador)
{
    objetivoX = objetivo.x();
    objetivoY = objetivo.y();
    tieneObjetivo = true;

    if(contadorCadencia > 0) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia <= rangoDisparo * factorMapa && distancia > 50.0f) {
        // ALTERNANCIA DE ATAQUES
        static int contadorDisparos = 0;
        contadorDisparos++;

        if(contadorDisparos % 3 == 0) {
            // Cada 3 disparos: PROYECTILES BUSCADORES (violetas)
            crearProyectilesBuscadores(objetivo);
        } else {
            // Disparos normales: Orbes azules
            crearProyectil(objetivo);
        }

        contadorCadencia = cadenciaDisparo;
    }
}

void Xhaal::crearProyectil(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    float dx = objetivo.x() - centroX;
    float dy = objetivo.y() - centroY;
    float angulo = std::atan2(dy, dx) * 180.0f / M_PI;

    // En lugar de: Proyectil* p = new Proyectil(centroX, centroY, angulo, 8.0f, 35.0f, false, 0.0f);
    Proyectil* p = new Proyectil();
    p->reiniciar(centroX, centroY, angulo, 8.0f, 35.0f, false, 0.0f);

    proyectiles.append(p);
}

void Xhaal::actualizarProyectiles(QPointF posJugador)
{
    // Actualizar proyectiles normales
    for(int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if(!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }

    // Actualizar proyectiles buscadores
    for(int i = proyectilesBuscadores.size() - 1; i >= 0; i--) {
        proyectilesBuscadores[i]->actualizarHacia(posJugador); // Perseguir al jugador
        proyectilesBuscadores[i]->actualizar();
        if(!proyectilesBuscadores[i]->estaActivo()) {
            delete proyectilesBuscadores[i];
            proyectilesBuscadores.removeAt(i);
        }
    }
}

QList<Proyectil*>& Xhaal::getProyectiles()
{
    return proyectiles;
}

QList<ProyectilBuscador*>& Xhaal::getProyectilesBuscadores()
{
    return proyectilesBuscadores;
}

void Xhaal::crearProyectilesBuscadores(QPointF objetivo)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    // Crear 3 proyectiles buscadores en ángulos ligeramente diferentes
    for(int i = 0; i < 3; i++) {
        float dx = objetivo.x() - centroX;
        float dy = objetivo.y() - centroY;
        float anguloBase = std::atan2(dy, dx) * 180.0f / M_PI;

        // Spread de ±20 grados
        float anguloFinal = anguloBase + (i - 1) * 20.0f;

        // Proyectiles buscadores: velocidad 6.0, daño 30.0
        ProyectilBuscador* pb = new ProyectilBuscador(centroX, centroY, anguloFinal, 6.0f, 30.0f);
        proyectilesBuscadores.append(pb);
    }
}
