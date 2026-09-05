#include "coloso.h"
#include <cmath>
#include <cstdlib>
#include <QPainterPath>

Coloso::Coloso(float x, float y)
    : Enemigo(x, y), estadoActual(COLOSO_PERSIGUIENDO),
    timerEstado(0.0f), frameAnimacion(0.0f), anguloMovimiento(0.0f),
    animHombros(0.0f), animRespiracion(0.0f), // Nuevas variables de inercia
    velocidadNormal(2.8f), velocidadCarga(15.0f),
    danioNormal(20.0f), danioCarga(55.0f), danioGolpe(40.0f), radioGolpe(150.0f),
    cargasRestantes(0), dirCargaX(0.0f), dirCargaY(0.0f), golpeAplicado(false),
    aperturaGarras(1.0f)
{
    this->sistemaVida.setVidaMaxima(1400.0f);
    this->sistemaVida.restaurarCompleta();
    this->ancho = 200.0f;
    this->alto = 170.0f;
    this->danioContacto = danioNormal;
    this->velocidad = velocidadNormal;

    // Paleta rojo, purpura, negro y carmesi. Ojos sin cambios.
    colorPiel = QColor(70, 25, 80);
    colorPielSecundario = QColor(30, 12, 38);
    colorHueso = QColor(35, 30, 38);
    colorOjos = QColor(255, 170, 40); // brillo amarillo anaranjado intenso
    colorBrillo = QColor(220, 20, 50); // rojo carmesi brillante para venas y energia
    colorCarne = QColor(150, 18, 32); // rojo carmesi oscuro, carne expuesta
    colorManto = QColor(50, 12, 60); // purpura oscuro sobre la espalda
}

void Coloso::actualizar() { }

void Coloso::actualizarConJugador(QPointF jugador) { actualizar(jugador.x(), jugador.y()); }

void Coloso::actualizar(float jugadorX, float jugadorY)
{
    if (!estaVivo()) return;

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    float dx = jugadorX - centroX;
    float dy = jugadorY - centroY;
    float distancia = std::sqrt(dx * dx + dy * dy);
    if (distancia < 0.001f) distancia = 0.001f;

    frameAnimacion += 0.15f;
    if (frameAnimacion > M_PI * 2) frameAnimacion -= M_PI * 2;

    float dt = 0.016f;

    switch (estadoActual) {
    case COLOSO_PERSIGUIENDO:
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

        pos_x += (dx / distancia) * velocidadNormal;
        pos_y += (dy / distancia) * velocidadNormal;
        danioContacto = danioNormal;

        // Péndulo agresivo al caminar
        animHombros = std::sin(frameAnimacion * 2.0f) * 15.0f;
        animRespiracion = std::sin(frameAnimacion) * 3.0f;

        if (distancia > 40.0f && distancia < 800.0f) {
            static int contadorAtaques = 0;
            contadorAtaques++;
            if (contadorAtaques % 4 != 0) {
                estadoActual = COLOSO_PREPARANDO_CARGA;
                cargasRestantes = COLOSO_CARGAS_POR_SECUENCIA;
            } else {
                estadoActual = COLOSO_PREPARANDO_GOLPE;
            }
            timerEstado = 0.0f;
        }
        break;

    case COLOSO_PREPARANDO_CARGA:
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;

        aperturaGarras = std::min(1.0f, timerEstado / 0.2f);
        // Anticipación: Los brazos se tiran bruscamente hacia atrás
        animHombros = -45.0f * (timerEstado / 0.35f);
        animRespiracion = -5.0f; // Se agacha

        timerEstado += dt;
        if (timerEstado >= 0.35f) {
            estadoActual = COLOSO_CARGANDO;
            timerEstado = 0.0f;
            dirCargaX = dx / distancia;
            dirCargaY = dy / distancia;
            danioContacto = danioCarga;
        }
        break;

    case COLOSO_CARGANDO:
        pos_x += dirCargaX * velocidadCarga;
        pos_y += dirCargaY * velocidadCarga;

        aperturaGarras = std::max(0.0f, 1.0f - timerEstado / 0.05f);
        // Impulso: Brazos bloqueados hacia adelante por la inercia (Smear pose)
        animHombros = 65.0f;
        animRespiracion = 8.0f; // Pecho inflado hacia adelante

        timerEstado += dt;
        if (timerEstado >= 0.3f) {
            cargasRestantes--;
            danioContacto = danioNormal;
            // ¡Pasa a descanso OBLIGATORIO después de cada carga![cite: 13]
            estadoActual = COLOSO_FATIGADO;
            timerEstado = 0.0f;
        }
        break;

    case COLOSO_PREPARANDO_GOLPE:
        // Lógica de área intacta[cite: 13]
        anguloMovimiento = std::atan2(dy, dx) * 180.0f / M_PI;
        if (anguloMovimiento < 0) anguloMovimiento += 360.0f;
        aperturaGarras = std::min(1.0f, timerEstado / 0.6f);
        animHombros = -30.0f;

        timerEstado += dt;
        if (timerEstado >= 0.8f) {
            estadoActual = COLOSO_GOLPEANDO;
            timerEstado = 0.0f;
            golpeAplicado = false;
            iniciarGolpeArea();
        }
        break;

    case COLOSO_GOLPEANDO:
        aperturaGarras = std::max(0.0f, 1.0f - timerEstado / 0.05f);
        animHombros = 80.0f; // Latigazo hacia abajo

        timerEstado += dt;
        if (timerEstado >= 0.4f) {
            estadoActual = COLOSO_FATIGADO;
            timerEstado = 0.0f;
        }
        break;

    case COLOSO_FATIGADO:
        aperturaGarras += (0.7f - aperturaGarras) * 0.1f;

        // Animación Sub-pixel de respiración pesada de 1 segundo exacto
        animHombros = std::sin(timerEstado * 12.0f) * 8.0f; // Tiembla ligeramente
        animRespiracion = std::sin(timerEstado * 6.0f) * 6.0f; // Sube y baja pesado

        timerEstado += dt;
        // ¡1 SEGUNDO DE DESCANSO EXACTO POR CADA EMBESTIDA![cite: 13]
        if (timerEstado >= 1.0f) {
            if (cargasRestantes > 0) estadoActual = COLOSO_PREPARANDO_CARGA;
            else estadoActual = COLOSO_PERSIGUIENDO;
            timerEstado = 0.0f;
        }
        break;
    }

    actualizarRastrosAire();
}

bool Coloso::consumirGolpeArea() {
    if (estadoActual == COLOSO_GOLPEANDO && !golpeAplicado) {
        golpeAplicado = true;
        return true;
    }
    return false;
}

void Coloso::iniciarGolpeArea() {
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;
    int cantidad = 16;
    for (int i = 0; i < cantidad; i++) {
        RastroAireColoso r;
        r.angulo = (360.0f / cantidad) * i + (std::rand() % 20 - 10);
        r.x = centroX;
        r.y = centroY;
        r.longitud = radioGolpe * (0.8f + (std::rand() % 40) / 100.0f);
        r.vida = 0.4f;
        r.vidaMax = 0.4f;
        rastros.append(r);
    }
}

void Coloso::actualizarRastrosAire() {
    for (int i = rastros.size() - 1; i >= 0; i--) {
        rastros[i].vida -= 0.016f;
        if (rastros[i].vida <= 0.0f) rastros.removeAt(i);
    }
}

void Coloso::dibujarClustersPixeles(QPainter &p, float anchoCluster, float altoCluster, QColor color) {
    // Apagamos el Antialiasing para forzar bordes duros tipo Pixel Art
    p.setRenderHint(QPainter::Antialiasing, false);
    p.setBrush(color);
    p.setPen(Qt::NoPen);

    // Dibuja un patrón de cuadrados rústicos (ruido visual HD)
    p.drawRect(QRectF(-anchoCluster*0.2f, -altoCluster*0.1f, 3, 3));
    p.drawRect(QRectF(anchoCluster*0.1f, altoCluster*0.3f, 4, 4));
    p.drawRect(QRectF(-anchoCluster*0.4f, altoCluster*0.4f, 2, 2));

    p.setRenderHint(QPainter::Antialiasing, true); // Lo devolvemos a la normalidad
}

void Coloso::dibujar(QPainter &p) {
    if (esCadaver()) return;

    dibujarRastrosAire(p);

    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(pos_x + ancho / 2.0f, pos_y + alto / 2.0f);

    dibujarAuraAtaque(p);

    float offsetCaminar = animRespiracion; // Lo atamos al nuevo sistema dinámico

    // Efecto de Squash & Stretch (Pixel art HD)
    if (estadoActual == COLOSO_CARGANDO) {
        p.scale(1.0f, 1.15f); // Se estira por la velocidad
    } else if (estadoActual == COLOSO_FATIGADO) {
        p.scale(1.0f + std::sin(timerEstado * 6.0f) * 0.04f, 1.0f - std::sin(timerEstado * 6.0f) * 0.04f);
    }

    if (anguloMovimiento >= 315 || anguloMovimiento < 45) {
        dibujarDerecha(p, offsetCaminar);
    } else if (anguloMovimiento >= 45 && anguloMovimiento < 135) {
        dibujarAbajo(p, offsetCaminar);
    } else if (anguloMovimiento >= 135 && anguloMovimiento < 225) {
        dibujarIzquierda(p, offsetCaminar);
    } else {
        dibujarArriba(p, offsetCaminar);
    }

    p.restore();
}

void Coloso::dibujarRastrosAire(QPainter &p) {
    if (rastros.isEmpty()) return;

    p.save();
    p.setPen(Qt::NoPen);
    for (const auto &r : rastros) {
        float progreso = 1.0f - (r.vida / r.vidaMax);
        int alfa = static_cast<int>(200 * (r.vida / r.vidaMax));
        QColor color(colorBrillo.red(), colorBrillo.green(), colorBrillo.blue(), alfa);

        p.save();
        p.translate(r.x, r.y);
        p.rotate(r.angulo);
        float largoActual = r.longitud * (0.3f + progreso * 0.7f);
        float grosor = 7.0f * (r.vida / r.vidaMax) + 2.0f;
        p.setBrush(color);
        // Detalles cuadrados para los rastros (Smear frames del golpe)
        p.drawRect(QRectF(largoActual * 0.3f, -grosor / 2.0f, largoActual * 0.7f, grosor));
        p.restore();
    }
    p.restore();
}

void Coloso::dibujarAuraAtaque(QPainter &p) {
    p.save();
    p.setBrush(Qt::NoBrush);

    switch (estadoActual) {
    case COLOSO_PREPARANDO_CARGA: {
        // 1. dibujar aro de advertencia carmesi creciendo antes de la embestida, telegrafo del ataque
        float progreso = std::min(1.0f, timerEstado / 0.35f);
        float pulsoAdvertencia = std::sin(timerEstado * 30.0f) * 0.5f + 0.5f;
        QColor colorAura(colorBrillo.red(), colorBrillo.green(), colorBrillo.blue(),
                         static_cast<int>(70 + progreso * 110 * pulsoAdvertencia));
        QPen penAura(colorAura, 3.0f + progreso * 3.0f);
        p.setPen(penAura);
        float radioAura = 55.0f + progreso * 25.0f;
        p.drawEllipse(QPointF(0.0f, 0.0f), radioAura, radioAura * 0.55f);
        break;
    }
    case COLOSO_CARGANDO: {
        // 2. dibujar estela de movimiento detras del cuerpo mientras arrastra la embestida
        for (int i = 1; i <= 3; i++) {
            QColor colorEstela(colorPiel.red(), colorPiel.green(), colorPiel.blue(), 100 - i * 26);
            p.setBrush(colorEstela);
            p.setPen(Qt::NoPen);
            p.drawRect(QRectF(-dirCargaX * i * 20.0f - 35.0f, -dirCargaY * i * 20.0f - 30.0f, 70.0f, 60.0f));
        }
        break;
    }
    case COLOSO_PREPARANDO_GOLPE: {
        // 3. dibujar circulo de impacto en el suelo, crece hasta marcar el radio real del golpe de area
        float progreso = std::min(1.0f, timerEstado / 0.8f);
        QColor colorCirculo(colorCarne.red(), colorCarne.green(), colorCarne.blue(),
                            static_cast<int>(50 + progreso * 110));
        QPen penCirculo(colorCirculo, 2.5f);
        p.setPen(penCirculo);
        float radioCirculo = radioGolpe * progreso;
        p.drawEllipse(QPointF(0.0f, 0.0f), radioCirculo, radioCirculo * 0.5f);
        break;
    }
    default:
        break;
    }

    p.setPen(Qt::NoPen);
    p.restore();
}

void Coloso::dibujarBrazo(QPainter &p, float hombroX, float hombroY, float anguloHombro,
                          float anguloCodoBase, float largoSuperior, float largoInferior,
                          float grosor, QColor colorBase, float signoCurva)
{
    QColor sombra = colorBase.darker(135);

    p.save();
    p.translate(hombroX, hombroY);
    // ¡Aquí aplicamos la inercia global (animHombros) a cada brazo independientemente!
    p.rotate(anguloHombro + (animHombros * signoCurva));

    // Músculo superior
    p.setBrush(sombra);
    p.drawRect(QRectF(0.0f, -grosor * 0.6f, largoSuperior, grosor * 1.2f)); // Bordes más duros tipo pixel
    p.setBrush(colorBase);
    p.drawRect(QRectF(0.0f, -grosor * 0.4f, largoSuperior * 0.9f, grosor * 0.8f));

    // Agregamos Clusters de texturas para simular High-Bit Pixel Art
    dibujarClustersPixeles(p, largoSuperior, grosor, colorBase.lighter(130));

    // Codo y Púa Gigante
    p.setBrush(colorBase.darker(120));
    p.drawRect(QRectF(largoSuperior - grosor*0.35f, -grosor*0.35f, grosor * 0.7f, grosor * 0.7f)); // Codo cuadrado

    QPolygonF puaCodo;
    puaCodo << QPointF(largoSuperior - grosor * 0.3f, -grosor * 0.5f)
            << QPointF(largoSuperior + grosor * 0.5f, -grosor * 1.8f * signoCurva)
            << QPointF(largoSuperior + grosor * 0.8f, -grosor * 0.2f);
    p.setBrush(colorHueso);
    p.drawPolygon(puaCodo);

    // Venas rojizas expuestas en la articulacion del codo, pulsando junto con las del antebrazo
    float pulsoCodo = std::sin(frameAnimacion * 4.0f) * 0.5f + 0.5f;
    QPen penVenaCodo(QColor(colorCarne.red(), colorCarne.green(), colorCarne.blue(), 130 + int(pulsoCodo * 100)), 1.6f);
    p.setPen(penVenaCodo);
    p.drawLine(QPointF(largoSuperior - grosor * 0.32f, -grosor * 0.22f),
               QPointF(largoSuperior + grosor * 0.15f, grosor * 0.12f));
    p.drawLine(QPointF(largoSuperior - grosor * 0.12f, grosor * 0.3f),
               QPointF(largoSuperior + grosor * 0.28f, -grosor * 0.1f));
    p.setPen(Qt::NoPen);

    p.setBrush(colorCarne);
    p.drawRect(QRectF(largoSuperior - grosor * 0.12f, -grosor * 0.12f, 4, 4));

    // Codo dinámico que reacciona a la tensión de la embestida
    float factorCierreCodo = 1.0f - aperturaGarras;
    float tensionExtra = (estadoActual == COLOSO_PREPARANDO_CARGA) ? -20.0f : 0.0f;
    float anguloCodoAnimado = anguloCodoBase + (factorCierreCodo * 35.0f * signoCurva) + (tensionExtra * signoCurva);

    p.translate(largoSuperior, 0.0f);
    p.rotate(anguloCodoAnimado);

    // Antebrazo blindado
    p.setBrush(sombra);
    p.drawRect(QRectF(0.0f, -grosor * 0.5f, largoInferior, grosor));

    // Detalles pixelados del antebrazo
    dibujarClustersPixeles(p, largoInferior, grosor, colorBase.darker(150));

    // Venas de energía que palpitan
    p.setRenderHint(QPainter::Antialiasing, false); // Vena dentada tipo pixel art
    QPen penVena(colorBrillo, 2.0f);
    float pulso = std::sin(frameAnimacion * 4.0f) * 0.5f + 0.5f;
    penVena.setColor(QColor(colorBrillo.red(), colorBrillo.green(), colorBrillo.blue(), 100 + 155 * pulso));
    p.setPen(penVena);

    // Dibujamos la vena como una serie de rectángulos pequeños
    for(float vx = 0; vx < largoInferior * 0.8f; vx += 4) {
        float vy = std::sin(vx * 0.5f) * grosor * 0.2f;
        p.drawRect(QRectF(vx, vy, 2, 2));
    }
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.translate(largoInferior, 0.0f);
    dibujarGarraCurva(p, grosor, aperturaGarras, signoCurva);

    p.restore();
}

void Coloso::dibujarParBrazos(QPainter &p, float hombroXMag, float hombroY,
                              float anguloHombroDerecha, float anguloCodoDerecha,
                              float largoSuperior, float largoInferior,
                              float grosor, QColor colorBase)
{
    dibujarBrazo(p, hombroXMag, hombroY, anguloHombroDerecha, anguloCodoDerecha,
                 largoSuperior, largoInferior, grosor, colorBase, 1.0f);
    dibujarBrazo(p, -hombroXMag, hombroY, 180.0f - anguloHombroDerecha, -anguloCodoDerecha,
                 largoSuperior, largoInferior, grosor, colorBase, -1.0f);
}

void Coloso::dibujarGarraCurva(QPainter &p, float grosorBase, float apertura, float signoCurva)
{
    p.save();
    // 1. dibujar remache carmesi en la base de la garra, union con el antebrazo
    p.setBrush(colorBrillo);
    p.drawRect(QRectF(-4.0f, -grosorBase * 0.25f, 5, 5));

    // 2. dibujar base curva tipo pinza, menos segmentada y mas alargada que antes
    int segmentos = 4;
    float largoSegmento = grosorBase * 1.9f;
    float grosorActual = grosorBase * 0.75f;

    float anguloCierreMax = 34.0f;
    float anguloAbiertoMin = 5.0f;
    float anguloEntreSegmentos = (anguloAbiertoMin + (1.0f - apertura) * (anguloCierreMax - anguloAbiertoMin)) * signoCurva;

    for (int i = 0; i < segmentos; i++) {
        if (i > 0) p.rotate(anguloEntreSegmentos);

        QPolygonF filo;
        filo << QPointF(0.0f, grosorActual * 0.4f * signoCurva)
             << QPointF(largoSegmento * 0.5f, grosorActual * 0.85f * signoCurva)
             << QPointF(largoSegmento, grosorActual * 0.15f * signoCurva);
        p.setBrush(colorHueso.darker(140));
        p.drawPolygon(filo);

        p.setBrush(colorHueso.darker(95 + i * 12));
        // Rectángulos duros para las falanges de la garra (Look pixelado)
        p.drawRect(QRectF(0.0f, -grosorActual / 2.0f, largoSegmento, grosorActual));

        // Brillo pixelado en la garra
        p.setBrush(colorHueso.lighter(125));
        p.drawRect(QRectF(largoSegmento * 0.2f, -grosorActual / 3.0f, 3, 3));

        p.translate(largoSegmento * 0.82f, 0.0f);
        grosorActual *= 0.74f;
    }

    // 3. dibujar punta final larga tipo cuchilla, sello visual del Ultralisco
    QPolygonF hojaFinal;
    hojaFinal << QPointF(0.0f, grosorActual * 0.45f)
              << QPointF(largoSegmento * 1.1f, grosorActual * 0.12f)
              << QPointF(largoSegmento * 1.9f, 0.0f)
              << QPointF(largoSegmento * 1.1f, -grosorActual * 0.12f)
              << QPointF(0.0f, -grosorActual * 0.45f);
    p.setBrush(colorHueso.lighter(110));
    p.drawPolygon(hojaFinal);

    QPen filoOscuro(colorHueso.darker(160), 1.2f);
    p.setPen(filoOscuro);
    p.drawLine(QPointF(0.0f, 0.0f), QPointF(largoSegmento * 1.9f, 0.0f));
    p.setPen(Qt::NoPen);

    p.restore();
}

void Coloso::dibujarCuernoCurvo(QPainter &p, float baseX, float baseY, float signo, QColor colorBase, float escala)
{
    // 1. dibujar cuerpo del colmillo como curva larga que cae y se enrosca hacia arriba en la punta
    QPainterPath cuerno;
    cuerno.moveTo(baseX, baseY);
    cuerno.cubicTo(baseX + signo * 8.0f * escala, baseY + 22.0f * escala,
                   baseX + signo * 4.0f * escala,  baseY + 55.0f * escala,
                   baseX + signo * 20.0f * escala, baseY + 70.0f * escala);
    cuerno.cubicTo(baseX + signo * 34.0f * escala, baseY + 80.0f * escala,
                   baseX + signo * 46.0f * escala, baseY + 68.0f * escala,
                   baseX + signo * 42.0f * escala, baseY + 46.0f * escala);
    cuerno.cubicTo(baseX + signo * 36.0f * escala, baseY + 52.0f * escala,
                   baseX + signo * 28.0f * escala, baseY + 65.0f * escala,
                   baseX + signo * 14.0f * escala, baseY + 68.0f * escala);
    cuerno.cubicTo(baseX + signo * 1.0f * escala,  baseY + 52.0f * escala,
                   baseX + signo * 3.0f * escala,  baseY + 18.0f * escala,
                   baseX,                          baseY + 6.0f * escala);
    cuerno.closeSubpath();

    p.setBrush(colorBase.darker(108));
    p.drawPath(cuerno);

    // 2. dibujar franja de brillo central para dar volumen curvo
    QPen brillo(colorBase.lighter(140), 2.5f * escala);
    p.setPen(brillo);
    p.drawLine(QPointF(baseX + signo * 8.0f * escala, baseY + 15.0f * escala),
               QPointF(baseX + signo * 30.0f * escala, baseY + 85.0f * escala));
    p.setPen(Qt::NoPen);

    // 3. dibujar muescas segmentadas a lo largo del cuerno, textura de quitina anillada
    QPen penMuesca(colorBase.darker(160), 1.2f * escala);
    p.setPen(penMuesca);
    p.drawLine(QPointF(baseX + signo * 10.0f * escala, baseY + 25.0f * escala),
               QPointF(baseX + signo * 20.0f * escala, baseY + 20.0f * escala));
    p.drawLine(QPointF(baseX + signo * 18.0f * escala, baseY + 45.0f * escala),
               QPointF(baseX + signo * 30.0f * escala, baseY + 40.0f * escala));
    p.drawLine(QPointF(baseX + signo * 28.0f * escala, baseY + 62.0f * escala),
               QPointF(baseX + signo * 40.0f * escala, baseY + 58.0f * escala));
    p.setPen(Qt::NoPen);
}

void Coloso::dibujarPata(QPainter &p, float baseX, float baseY, float altoPata,
                         float anchoPata, QColor colorBase, float faseDesfase)
{
    float balanceo = 0.0f;
    if (estadoActual == COLOSO_PERSIGUIENDO || estadoActual == COLOSO_CARGANDO) {
        float velocidadZancada = (estadoActual == COLOSO_CARGANDO) ? 4.0f : 2.0f;
        balanceo = std::sin(frameAnimacion * velocidadZancada + faseDesfase) * 10.0f;
    }

    // 1. dibujar columna principal de la pata, gruesa y columnaria como de elefante
    p.setBrush(colorBase);
    p.drawRect(QRectF(baseX - anchoPata / 2.0f + balanceo, baseY, anchoPata, altoPata));

    dibujarClustersPixeles(p, anchoPata, altoPata, colorBase.darker(120));

    // 2. dibujar anillos de quitina mas oscuros para textura de exoesqueleto segmentado
    p.setBrush(colorBase.darker(145));
    p.drawRect(QRectF(baseX - anchoPata / 2.0f + balanceo, baseY + altoPata * 0.32f, anchoPata, altoPata * 0.12f));
    p.drawRect(QRectF(baseX - anchoPata / 2.0f + balanceo, baseY + altoPata * 0.66f, anchoPata, altoPata * 0.12f));

    // 3. dibujar tejido zerg expuesto en la articulacion superior, piel estriada y venas
    p.setBrush(colorCarne);
    p.drawRect(QRectF(baseX - anchoPata * 0.35f + balanceo, baseY - 6.0f, anchoPata * 0.7f, 8.0f));
    dibujarClustersPixeles(p, anchoPata * 0.7f, 8.0f, colorCarne.darker(122));

    // 4. dibujar almohadilla ancha y redondeada en la base, tipo pata de elefante
    QPolygonF almohadilla;
    almohadilla << QPointF(baseX - anchoPata * 0.6f + balanceo, baseY + altoPata)
                << QPointF(baseX + anchoPata * 0.6f + balanceo, baseY + altoPata)
                << QPointF(baseX + anchoPata * 0.55f + balanceo, baseY + altoPata + 12.0f)
                << QPointF(baseX - anchoPata * 0.55f + balanceo, baseY + altoPata + 12.0f);
    p.setBrush(colorHueso.darker(130));
    p.drawPolygon(almohadilla);

    // 5. dibujar unas romas y cortas sobre el borde delantero de la almohadilla
    p.setBrush(colorHueso.darker(105));
    for (int i = -1; i <= 1; i++) {
        float unaX = baseX + i * anchoPata * 0.35f + balanceo;
        QPolygonF una;
        una << QPointF(unaX - 4.0f, baseY + altoPata + 10.0f)
            << QPointF(unaX + 4.0f, baseY + altoPata + 10.0f)
            << QPointF(unaX, baseY + altoPata + 16.0f);
        p.drawPolygon(una);
    }

    // 6. dibujar remache oscuro central, placa reforzada de la pata
    p.setBrush(colorPielSecundario.darker(120));
    p.drawRect(QRectF(baseX - 3.0f + balanceo, baseY + altoPata * 0.46f, 6.0f, 6.0f));

    // 7. dibujar franja de brillo lateral para dar volumen a la columna de la pata
    QPen penBrilloPata(colorBase.lighter(140), 1.5f);
    p.setPen(penBrilloPata);
    p.drawLine(QPointF(baseX - anchoPata * 0.3f + balanceo, baseY + 4.0f),
               QPointF(baseX - anchoPata * 0.3f + balanceo, baseY + altoPata - 4.0f));
    p.setPen(Qt::NoPen);
}

void Coloso::dibujarDetallesEspalda(QPainter &p, float dirY)
{
    // 1. dibujar placas grandes del caparazon, mas anchas y repartidas en mas filas para que cubra un tramo mas largo del torso
    p.setBrush(colorPiel.lighter(110));
    for(int i = -48; i <= 48; i += 16) {
        p.drawRect(QRectF(-64, (i * dirY) - 8, 128, 16));
        p.setBrush(colorPiel.darker(115));
        p.drawRect(QRectF(-58, ((i + 4) * dirY) - 6, 116, 13));
        p.setBrush(colorPiel.lighter(110));
    }
    dibujarClustersPixeles(p, 128, 96, colorPiel.darker(135));

    // 2. dibujar costura central del caparazon, division tipo elitros de escarabajo
    QPen penCostura(colorPielSecundario.darker(140), 2.2f);
    p.setPen(penCostura);
    p.drawLine(QPointF(0, -60 * dirY), QPointF(0, 60 * dirY));
    p.setPen(Qt::NoPen);

    // 3. dibujar par de franjas de brillo curvas, efecto caparazon pulido
    QPen penBrilloCaparazon(colorPiel.lighter(160), 3.0f);
    p.setPen(penBrilloCaparazon);
    p.drawLine(QPointF(-30, -50 * dirY), QPointF(-18, 45 * dirY));
    p.drawLine(QPointF(30, -50 * dirY), QPointF(18, 45 * dirY));
    p.setPen(Qt::NoPen);

    // 4. dibujar hilera larga de pinches oseos a lo largo de todo el caparazon
    p.setBrush(colorHueso);
    for(int i = -55; i <= 55; i += 14) {
        QPolygonF puaEspalda;
        puaEspalda << QPointF(-7, i * dirY)
                   << QPointF(7, i * dirY)
                   << QPointF(0, (i - 13) * dirY);
        p.drawPolygon(puaEspalda);
    }

    // 5. dibujar venas de energia carmesi pulsante repartidas en tres alturas del caparazon
    QPen penVena(QColor(colorBrillo.red(), colorBrillo.green(), colorBrillo.blue(), 180), 3);
    float pulso = std::sin(frameAnimacion * 5.0f) * 0.5f + 0.5f;
    penVena.setWidthF(1.5f + 2.0f * pulso);
    p.setPen(penVena);

    p.drawLine(QPointF(-20, -38 * dirY), QPointF(-54, -56 * dirY));
    p.drawLine(QPointF(20, -38 * dirY), QPointF(54, -56 * dirY));
    p.drawLine(QPointF(-15, 0), QPointF(-50, 12 * dirY));
    p.drawLine(QPointF(15, 0), QPointF(50, 12 * dirY));
    p.drawLine(QPointF(-15, 36 * dirY), QPointF(-48, 52 * dirY));
    p.drawLine(QPointF(15, 36 * dirY), QPointF(48, 52 * dirY));
    p.setPen(Qt::NoPen);
}

void Coloso::dibujarAbajo(QPainter &p, float offset)
{
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    dibujarPata(p, -68, 8, 55, 26, colorPielSecundario, 0.0f);
    dibujarPata(p, 68, 8, 55, 26, colorPielSecundario, static_cast<float>(M_PI));

    p.setBrush(colorPiel);
    p.drawRect(QRectF(-60, -40, 120, 75)); // Torso cuadrado base

    // 1. dibujar placas del pecho segmentadas con veta central mas oscura
    p.setBrush(colorPiel.darker(122));
    p.drawRect(QRectF(-8, -38, 16, 73));
    dibujarClustersPixeles(p, 120, 75, colorPiel.darker(130));

    // 2. dibujar grietas de piedra resquebrajada cruzando el torso, textura de idolo antiguo
    QPen penGrietaAbajo(colorPielSecundario.darker(150), 1.4f);
    p.setPen(penGrietaAbajo);
    p.drawLine(QPointF(-42, -12), QPointF(-18, 14));
    p.drawLine(QPointF(18, -28), QPointF(38, 2));
    p.drawLine(QPointF(-15, 22), QPointF(8, 32));
    p.setPen(Qt::NoPen);

    // 3. dibujar remaches metalicos oscuros en los bordes superior e inferior del torso
    p.setBrush(colorPielSecundario.darker(130));
    for (int rx = -45; rx <= 45; rx += 22) {
        p.drawRect(QRectF(rx - 2, -37, 4, 4));
        p.drawRect(QRectF(rx - 2, 31, 4, 4));
    }

    // 4. dibujar contorno de brillo tallado en el borde superior e inferior del torso
    QPen penContornoTorso(colorPiel.lighter(150), 1.6f);
    p.setPen(penContornoTorso);
    p.drawLine(QPointF(-60, -40), QPointF(60, -40));
    p.drawLine(QPointF(-60, 35), QPointF(60, 35));
    p.setPen(Qt::NoPen);

    // 5. dibujar runas talladas a los costados del pecho, patron angular grabado en la piedra
    QPen penRuna(colorPielSecundario.darker(160), 1.3f);
    p.setPen(penRuna);
    p.drawLine(QPointF(-28, -20), QPointF(-20, -8));
    p.drawLine(QPointF(-20, -8), QPointF(-28, 4));
    p.drawLine(QPointF(20, -20), QPointF(28, -8));
    p.drawLine(QPointF(28, -8), QPointF(20, 4));
    p.setPen(Qt::NoPen);

    // 6. dibujar placas de lomo con pinches oseos
    dibujarDetallesEspalda(p, 1.0f);

    // 7. dibujar manto purpura oscuro que cae sobre la espalda y los hombros
    QPolygonF manto;
    manto << QPointF(-40, -60) << QPointF(-55, -20)
          << QPointF(-35, 10) << QPointF(0, 18)
          << QPointF(35, 10) << QPointF(55, -20)
          << QPointF(40, -60) << QPointF(0, -45);
    p.setBrush(colorManto);
    p.drawPolygon(manto);
    dibujarClustersPixeles(p, 100, 60, colorManto.darker(120));

    // 8. dibujar ribete de energia carmesi en el borde del manto
    QPen penRibeteManto(colorBrillo, 1.6f);
    p.setPen(penRibeteManto);
    p.drawLine(QPointF(-40, -60), QPointF(-55, -20));
    p.drawLine(QPointF(40, -60), QPointF(55, -20));
    p.setPen(Qt::NoPen);

    // 9. dibujar placa craneal con forma de escudo de quitina, cuello corto fundido al torso
    QPolygonF placaCraneal;
    placaCraneal << QPointF(-58, -38) << QPointF(58, -38)
                 << QPointF(48, -82) << QPointF(30, -95)
                 << QPointF(-30, -95) << QPointF(-48, -82);
    p.setBrush(colorPielSecundario);
    p.drawPolygon(placaCraneal);
    dibujarClustersPixeles(p, 110, 55, colorPielSecundario.lighter(120));

    // 10. dibujar gema carmesi incrustada en el centro de la placa craneal, con pulso de energia
    float pulsoGemaAbajo = std::sin(frameAnimacion * 4.0f) * 0.5f + 0.5f;
    QPolygonF gemaFrente;
    gemaFrente << QPointF(0, -88) << QPointF(9, -78) << QPointF(0, -68) << QPointF(-9, -78);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaAbajo * 70)));
    p.drawPolygon(gemaFrente);
    p.setBrush(colorBrillo.lighter(160));
    float tamBrilloFrente = 3.0f + pulsoGemaAbajo * 2.0f;
    p.drawRect(QRectF(-tamBrilloFrente / 2.0f, -81, tamBrilloFrente, tamBrilloFrente));

    // 11. dibujar motas de energia orbitando la gema de la frente
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * 1.6f + m * 2.1f;
        float radioMota = 13.0f + std::sin(frameAnimacion * 2.0f + m) * 3.0f;
        QPointF mota(std::cos(anguloMota) * radioMota, -78.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorBrillo.lighter(160);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.0f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 12. dibujar pinzas secundarias pequenas bajo la cabeza, para triturar restos organicos
    p.setBrush(colorHueso.darker(115));
    QPolygonF pinzaIzq, pinzaDer;
    pinzaIzq << QPointF(-16, -40) << QPointF(-4, -40) << QPointF(-10, -26);
    pinzaDer << QPointF(4, -40) << QPointF(16, -40) << QPointF(10, -26);
    p.drawPolygon(pinzaIzq);
    p.drawPolygon(pinzaDer);

    // 13. dibujar cuchillas kaiser, colmillos gigantes que nacen a los lados de la mandibula
    dibujarCuernoCurvo(p, -50, -55, -1.0f, colorHueso, 1.1f);
    dibujarCuernoCurvo(p, 50, -55, 1.0f, colorHueso, 1.1f);

    // 14. dibujar ojos pequenos con brillo amarillo anaranjado intenso
    p.setBrush(colorOjos);
    p.drawRect(QRectF(-33, -60, 8, 6));
    p.drawRect(QRectF(25, -60, 8, 6));
    p.setBrush(Qt::white);
    p.drawRect(QRectF(-31, -59, 2, 2));
    p.drawRect(QRectF(27, -59, 2, 2));

    // 15. dibujar tejido zerg expuesto donde el unico par de brazos se une al torso
    p.setBrush(colorCarne);
    p.drawRect(QRectF(-70, -38, 10, 14));
    p.drawRect(QRectF(60, -38, 10, 14));
    dibujarClustersPixeles(p, 10, 14, colorCarne.darker(118));

    // 16. dibujar gema nucleo en el centro del pecho, brillante como fuente de energia, con pulso
    QPolygonF gemaPecho;
    gemaPecho << QPointF(0, -18) << QPointF(11, -6) << QPointF(0, 6) << QPointF(-11, -6);
    p.setBrush(colorCarne.lighter(115));
    p.drawPolygon(gemaPecho);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaAbajo * 70)));
    float tamBrilloPecho = 5.0f + pulsoGemaAbajo * 2.0f;
    p.drawRect(QRectF(-tamBrilloPecho / 2.0f, -9, tamBrilloPecho, tamBrilloPecho));

    // 17. dibujar motas de energia orbitando la gema del pecho
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * -1.8f + m * 2.4f;
        float radioMota = 15.0f + std::sin(frameAnimacion * 2.2f + m) * 3.0f;
        QPointF mota(std::cos(anguloMota) * radioMota, -6.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorCarne.lighter(140);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.2f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 18. dibujar remaches oscuros en las articulaciones de los hombros
    p.setBrush(colorPielSecundario.darker(140));
    p.drawRect(QRectF(-72, -30, 5, 5));
    p.drawRect(QRectF(67, -30, 5, 5));

    dibujarParBrazos(p, 62, -32, 65.0f, 40.0f, 55, 48, 22, colorPiel.darker(105));

    dibujarPata(p, -34, 14, 50, 30, colorPiel, static_cast<float>(M_PI));
    dibujarPata(p, 34, 14, 50, 30, colorPiel, 0.0f);
}

void Coloso::dibujarArriba(QPainter &p, float offset)
{
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    dibujarPata(p, -68, 8, 55, 26, colorPielSecundario, 0.0f);
    dibujarPata(p, 68, 8, 55, 26, colorPielSecundario, static_cast<float>(M_PI));

    p.setBrush(colorPiel);
    p.drawRect(QRectF(-60, -40, 120, 75));

    // 1. dibujar placas dorsales segmentadas
    dibujarClustersPixeles(p, 120, 75, colorPiel.darker(130));

    // 2. dibujar grietas de piedra resquebrajada en la espalda, misma textura de idolo antiguo que en la vista de abajo
    QPen penGrietaArriba(colorPielSecundario.darker(150), 1.4f);
    p.setPen(penGrietaArriba);
    p.drawLine(QPointF(-38, -15), QPointF(-14, 10));
    p.drawLine(QPointF(20, -25), QPointF(40, 5));
    p.drawLine(QPointF(-10, 20), QPointF(12, 30));
    p.setPen(Qt::NoPen);

    // 3. dibujar remaches metalicos oscuros en los bordes del torso
    p.setBrush(colorPielSecundario.darker(130));
    for (int rx = -45; rx <= 45; rx += 22) {
        p.drawRect(QRectF(rx - 2, -37, 4, 4));
        p.drawRect(QRectF(rx - 2, 31, 4, 4));
    }

    // 4. dibujar contorno de brillo tallado en el borde superior e inferior del torso
    QPen penContornoTorsoArriba(colorPiel.lighter(150), 1.6f);
    p.setPen(penContornoTorsoArriba);
    p.drawLine(QPointF(-60, -40), QPointF(60, -40));
    p.drawLine(QPointF(-60, 35), QPointF(60, 35));
    p.setPen(Qt::NoPen);

    // 5. dibujar runas talladas a los costados de la espalda, patron angular grabado en la piedra
    QPen penRunaArriba(colorPielSecundario.darker(160), 1.3f);
    p.setPen(penRunaArriba);
    p.drawLine(QPointF(-28, -20), QPointF(-20, -8));
    p.drawLine(QPointF(-20, -8), QPointF(-28, 4));
    p.drawLine(QPointF(20, -20), QPointF(28, -8));
    p.drawLine(QPointF(28, -8), QPointF(20, 4));
    p.setPen(Qt::NoPen);

    dibujarDetallesEspalda(p, -1.0f);

    dibujarBrazo(p, 62, -45, 305.0f, -40.0f, 55, 48, 22, colorPiel.darker(112), -1.0f);
    dibujarBrazo(p, -62, -45, 235.0f, 40.0f, 55, 48, 22, colorPiel.darker(112), 1.0f);

    // 6. dibujar manto purpura oscuro que cae sobre la espalda, bien visible desde atras
    QPolygonF manto;
    manto << QPointF(-45, -55) << QPointF(-60, -10)
          << QPointF(-38, 20) << QPointF(0, 28)
          << QPointF(38, 20) << QPointF(60, -10)
          << QPointF(45, -55) << QPointF(0, -40);
    p.setBrush(colorManto);
    p.drawPolygon(manto);
    dibujarClustersPixeles(p, 110, 70, colorManto.darker(115));

    // 7. dibujar ribete de energia carmesi en el borde del manto
    QPen penRibeteMantoArriba(colorBrillo, 1.6f);
    p.setPen(penRibeteMantoArriba);
    p.drawLine(QPointF(-45, -55), QPointF(-60, -10));
    p.drawLine(QPointF(45, -55), QPointF(60, -10));
    p.setPen(Qt::NoPen);

    // 8. dibujar gema carmesi central en la espalda, entre los omoplatos, con pulso de energia
    float pulsoGemaArriba = std::sin(frameAnimacion * 4.0f) * 0.5f + 0.5f;
    QPolygonF gemaEspalda;
    gemaEspalda << QPointF(0, -18) << QPointF(10, -4) << QPointF(0, 10) << QPointF(-10, -4);
    p.setBrush(colorCarne.lighter(115));
    p.drawPolygon(gemaEspalda);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaArriba * 70)));
    float tamBrilloEspalda = 5.0f + pulsoGemaArriba * 2.0f;
    p.drawRect(QRectF(-tamBrilloEspalda / 2.0f, -7, tamBrilloEspalda, tamBrilloEspalda));

    // 9. dibujar motas de energia orbitando la gema de la espalda
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * -1.8f + m * 2.4f;
        float radioMota = 15.0f + std::sin(frameAnimacion * 2.2f + m) * 3.0f;
        QPointF mota(std::cos(anguloMota) * radioMota, -4.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorCarne.lighter(140);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.2f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 10. dibujar placa craneal trasera con la misma silueta vista desde atras
    QPolygonF placaCranealTrasera;
    placaCranealTrasera << QPointF(-58, -38) << QPointF(58, -38)
                        << QPointF(48, -82) << QPointF(30, -95)
                        << QPointF(-30, -95) << QPointF(-48, -82);
    p.setBrush(colorPielSecundario);
    p.drawPolygon(placaCranealTrasera);
    dibujarClustersPixeles(p, 110, 55, colorPielSecundario.darker(110));

    // 11. dibujar gema carmesi incrustada en el centro de la placa craneal trasera, con pulso de energia
    QPolygonF gemaCraneoArriba;
    gemaCraneoArriba << QPointF(0, -88) << QPointF(9, -78) << QPointF(0, -68) << QPointF(-9, -78);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaArriba * 70)));
    p.drawPolygon(gemaCraneoArriba);
    p.setBrush(colorBrillo.lighter(160));
    float tamBrilloCraneoArriba = 3.0f + pulsoGemaArriba * 2.0f;
    p.drawRect(QRectF(-tamBrilloCraneoArriba / 2.0f, -81, tamBrilloCraneoArriba, tamBrilloCraneoArriba));

    // 12. dibujar motas de energia orbitando la gema del craneo
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * 1.6f + m * 2.1f;
        float radioMota = 13.0f + std::sin(frameAnimacion * 2.0f + m) * 3.0f;
        QPointF mota(std::cos(anguloMota) * radioMota, -78.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorBrillo.lighter(160);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.0f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 13. dibujar puntas de las cuchillas kaiser asomando a los costados de la cabeza
    dibujarCuernoCurvo(p, -50, -55, -1.0f, colorHueso, 0.7f);
    dibujarCuernoCurvo(p, 50, -55, 1.0f, colorHueso, 0.7f);
}

void Coloso::dibujarIzquierda(QPainter &p, float offset)
{
    p.translate(0, offset);
    p.setPen(Qt::NoPen);

    dibujarPata(p, 40, 12, 55, 26, colorPielSecundario.darker(112), static_cast<float>(M_PI));
    dibujarPata(p, -35, 12, 55, 26, colorPielSecundario.darker(112), 0.0f);

    p.save();
    // 1. inclinar el torso y la cabeza hacia adelante, centro de gravedad concentrado al frente
    p.shear(0.15f, 0.0f);

    p.setBrush(colorPiel);
    p.drawRect(QRectF(-55, -35, 105, 68));

    p.setBrush(colorPiel.lighter(110));
    p.drawRect(QRectF(-30, -30, 60, 40));
    dibujarClustersPixeles(p, 105, 68, colorPiel.darker(128));

    p.setBrush(colorHueso);
    for(int i = -30; i <= 30; i += 20) {
        QPolygonF puaLomo;
        puaLomo << QPointF(i, -32) << QPointF(i+10, -32) << QPointF(i+5, -50);
        p.drawPolygon(puaLomo);
    }

    // 1. dibujar grietas de piedra resquebrajada en el torso, misma textura de idolo antiguo que en arriba/abajo
    QPen penGrietaIzq(colorPielSecundario.darker(150), 1.4f);
    p.setPen(penGrietaIzq);
    p.drawLine(QPointF(-20, -18), QPointF(0, 5));
    p.drawLine(QPointF(15, -25), QPointF(35, -2));
    p.drawLine(QPointF(-10, 15), QPointF(10, 25));
    p.setPen(Qt::NoPen);

    // 2. dibujar remaches metalicos oscuros en los bordes del torso
    p.setBrush(colorPielSecundario.darker(130));
    for (int rx = -40; rx <= 40; rx += 22) {
        p.drawRect(QRectF(rx - 2, -34, 4, 4));
        p.drawRect(QRectF(rx - 2, 30, 4, 4));
    }

    // 3. dibujar contorno de brillo tallado en el borde superior e inferior del torso
    QPen penContornoTorsoIzq(colorPiel.lighter(150), 1.6f);
    p.setPen(penContornoTorsoIzq);
    p.drawLine(QPointF(-55, -35), QPointF(50, -35));
    p.drawLine(QPointF(-55, 33), QPointF(50, 33));
    p.setPen(Qt::NoPen);

    // 4. dibujar runas talladas en el costado del torso, patron angular grabado en la piedra
    QPen penRunaIzq(colorPielSecundario.darker(160), 1.3f);
    p.setPen(penRunaIzq);
    p.drawLine(QPointF(20, -20), QPointF(28, -8));
    p.drawLine(QPointF(28, -8), QPointF(20, 4));
    p.setPen(Qt::NoPen);

    // 5. dibujar manto purpura oscuro que cae sobre la espalda, hacia atras del cuello
    QPolygonF manto;
    manto << QPointF(-15, -60) << QPointF(15, -50)
          << QPointF(35, -20) << QPointF(30, 15)
          << QPointF(5, 20) << QPointF(-20, 0);
    p.setBrush(colorManto);
    p.drawPolygon(manto);
    dibujarClustersPixeles(p, 50, 60, colorManto.darker(118));

    // 6. dibujar ribete de energia carmesi en el borde del manto
    QPen penRibeteMantoIzq(colorBrillo, 1.6f);
    p.setPen(penRibeteMantoIzq);
    p.drawLine(QPointF(-15, -60), QPointF(15, -50));
    p.drawLine(QPointF(30, 15), QPointF(5, 20));
    p.setPen(Qt::NoPen);

    // 7. dibujar placa craneal lateral, angulosa, cuello corto y grueso fundido al torso
    QPolygonF placaLateral;
    placaLateral << QPointF(-95, -35) << QPointF(-40, -35)
                 << QPointF(-40, -70) << QPointF(-70, -85)
                 << QPointF(-100, -78) << QPointF(-108, -55);
    p.setBrush(colorPielSecundario);
    p.drawPolygon(placaLateral);
    dibujarClustersPixeles(p, 60, 40, colorPielSecundario.lighter(118));

    // 8. dibujar gema carmesi incrustada en la placa craneal lateral, con pulso de energia
    float pulsoGemaIzq = std::sin(frameAnimacion * 4.0f) * 0.5f + 0.5f;
    QPolygonF gemaCraneoIzq;
    gemaCraneoIzq << QPointF(-75, -72) << QPointF(-66, -63) << QPointF(-75, -54) << QPointF(-84, -63);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaIzq * 70)));
    p.drawPolygon(gemaCraneoIzq);
    p.setBrush(colorBrillo.lighter(160));
    float tamBrilloCraneoIzq = 3.0f + pulsoGemaIzq * 2.0f;
    p.drawRect(QRectF(-77 - tamBrilloCraneoIzq / 2.0f + 2.0f, -65 - tamBrilloCraneoIzq / 2.0f + 2.0f, tamBrilloCraneoIzq, tamBrilloCraneoIzq));

    // 9. dibujar motas de energia orbitando la gema del craneo
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * 1.6f + m * 2.1f;
        float radioMota = 12.0f + std::sin(frameAnimacion * 2.0f + m) * 3.0f;
        QPointF mota(-75.0f + std::cos(anguloMota) * radioMota, -63.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorBrillo.lighter(160);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.0f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 10. dibujar cuchilla kaiser, colmillo gigante y curvo que nace del costado de la mandibula
    dibujarCuernoCurvo(p, -95, -50, -1.0f, colorHueso, 1.3f);

    // 11. dibujar pinzas secundarias pequenas bajo la cabeza, para triturar restos organicos
    p.setBrush(colorHueso.darker(115));
    QPolygonF pinza1, pinza2;
    pinza1 << QPointF(-92, -35) << QPointF(-78, -35) << QPointF(-88, -22);
    pinza2 << QPointF(-76, -35) << QPointF(-64, -35) << QPointF(-70, -20);
    p.drawPolygon(pinza1);
    p.drawPolygon(pinza2);

    // 12. dibujar ojo pequeno con brillo amarillo anaranjado intenso
    p.setBrush(colorOjos);
    p.drawRect(QRectF(-85, -55, 8, 6));
    p.setBrush(Qt::white);
    p.drawRect(QRectF(-83, -54, 2, 2));

    // 13. dibujar tejido zerg expuesto donde el par de brazos se une al torso
    p.setBrush(colorCarne);
    p.drawRect(QRectF(-38, -14, 11, 8));
    dibujarClustersPixeles(p, 11, 8, colorCarne.darker(120));
    p.drawRect(QRectF(-52, 6, 14, 10));
    dibujarClustersPixeles(p, 14, 10, colorCarne.darker(120));

    // 14. dibujar gema nucleo en el pecho, brillante como fuente de energia, con pulso
    QPolygonF gemaPechoIzq;
    gemaPechoIzq << QPointF(-15, -8) << QPointF(-6, 2) << QPointF(-15, 12) << QPointF(-24, 2);
    p.setBrush(colorCarne.lighter(115));
    p.drawPolygon(gemaPechoIzq);
    p.setBrush(colorBrillo.lighter(100 + int(pulsoGemaIzq * 70)));
    float tamBrilloPechoIzq = 5.0f + pulsoGemaIzq * 2.0f;
    p.drawRect(QRectF(-18 - tamBrilloPechoIzq / 2.0f + 2.5f, -1 - tamBrilloPechoIzq / 2.0f + 2.5f, tamBrilloPechoIzq, tamBrilloPechoIzq));

    // 15. dibujar motas de energia orbitando la gema del pecho
    for (int m = 0; m < 3; m++) {
        float anguloMota = frameAnimacion * -1.8f + m * 2.4f;
        float radioMota = 13.0f + std::sin(frameAnimacion * 2.2f + m) * 3.0f;
        QPointF mota(-15.0f + std::cos(anguloMota) * radioMota, 2.0f + std::sin(anguloMota) * radioMota * 0.6f);
        QColor colorMota = colorCarne.lighter(140);
        colorMota.setAlpha(90 + int((std::sin(frameAnimacion * 3.2f + m) * 0.5f + 0.5f) * 120));
        p.setBrush(colorMota);
        p.drawEllipse(mota, 2.0f, 2.0f);
    }

    // 16. dibujar brazo trasero, mas chico y mas oscuro por quedar a la sombra del torso, curva en sentido contrario al delantero
    dibujarBrazo(p, -44, -42, -130.0f, -10.0f, 36, 32, 15, colorPiel.darker(125), -1.0f);

    // 17. dibujar brazo delantero, proporcionado al resto del cuerpo sin volver al tamano original de un solo brazo
    dibujarBrazo(p, -60, -25, 130.0f, 10.0f, 50, 44, 20, colorPiel.darker(105), 1.0f);

    p.restore();

    dibujarPata(p, 35, 15, 55, 28, colorPiel, 0.0f);
    dibujarPata(p, -30, 15, 55, 28, colorPiel, static_cast<float>(M_PI));
}

void Coloso::dibujarDerecha(QPainter &p, float offset)
{
    p.scale(-1, 1);
    dibujarIzquierda(p, offset);
    p.scale(-1, 1);
}
