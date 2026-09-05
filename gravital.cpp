// gravital.cpp
#include "gravital.h"
#include <cmath>
#include <algorithm>
#include <QRadialGradient>

Gravital::Gravital(float x, float y)
    : Enemigo(x, y),
    timerCurvo(1.5f),
    enEscudoTotal(false), timerEscudoTotal(0.0f), timerParaEscudo(10.0f),
    faseActual(GRAVITAL_FASE_1_ESPIRAL), direccionActual(GRAVITAL_DIR_FRENTE),
    cuartosPerdidos(0), aperturaGajos(0.0f), velocidadApertura(0.02f),
    anguloEspiral(0.0f), timerDisparo(0), cadenciaActual(8),
    frameAnimacion(0.0f), flotacionY(0.0f)
{
    this->ancho = 160.0f;
    this->alto = 160.0f;
    this->velocidad = 0.0f;
    this->danioContacto = 50.0f;

    this->sistemaVida.setVidaMaxima(4000.0f);
    this->sistemaVida.restaurarCompleta();
}
Gravital::~Gravital()
{
    qDeleteAll(proyectiles);
    proyectiles.clear();
    qDeleteAll(rayosLaser);
    rayosLaser.clear();
    qDeleteAll(proyectilesCurvos);
    proyectilesCurvos.clear();
}

void Gravital::actualizar()
{
    if(!estaVivo()) {
        // 1 al morir se destruyen de inmediato todos los ataques activos
        qDeleteAll(proyectiles); proyectiles.clear();
        qDeleteAll(rayosLaser); rayosLaser.clear();
        qDeleteAll(proyectilesCurvos); proyectilesCurvos.clear();
        return;
    }

    frameAnimacion += 0.05f;
    flotacionY = std::sin(frameAnimacion) * 8.0f;

    evaluarFase();

    // 2 control de la fase de escudo total, igual que antes
    timerParaEscudo -= 0.016f;
    if (!enEscudoTotal && timerParaEscudo <= 0.0f) {
        enEscudoTotal = true;
        timerEscudoTotal = 3.0f;
    }
    if (enEscudoTotal) {
        timerEscudoTotal -= 0.016f;
        if (timerEscudoTotal <= 0.0f) {
            enEscudoTotal = false;
            timerParaEscudo = 12.0f;
        }
    }

    // 3 mientras esta en escudo no dispara nada
    if (!enEscudoTotal) {
        if (timerDisparo > 0) {
            timerDisparo--;
        } else {
            ejecutarAtaque();
        }

        timerCurvo -= 0.016f;
        if (timerCurvo <= 0.0f) {
            dispararCurvo();
            timerCurvo = 3.0f;
        }
    }

    for(int i = proyectiles.size() - 1; i >= 0; i--) {
        proyectiles[i]->actualizar();
        if(!proyectiles[i]->estaActivo()) {
            delete proyectiles[i];
            proyectiles.removeAt(i);
        }
    }
    for(int i = rayosLaser.size() - 1; i >= 0; i--) {
        rayosLaser[i]->actualizar();
        if(!rayosLaser[i]->estaActivo()) {
            delete rayosLaser[i];
            rayosLaser.removeAt(i);
        }
    }
    for(int i = proyectilesCurvos.size() - 1; i >= 0; i--) {
        proyectilesCurvos[i]->actualizar();
        if(!proyectilesCurvos[i]->estaActivo()) {
            delete proyectilesCurvos[i];
            proyectilesCurvos.removeAt(i);
        }
    }
}

void Gravital::evaluarFase()
{
    float porcentaje = sistemaVida.getPorcentajeVida();

    // 1 fase 1, 100 a 75 por ciento, mira de frente, dispara espiral
    if (porcentaje > 75.0f) {
        faseActual = GRAVITAL_FASE_1_ESPIRAL;
        direccionActual = GRAVITAL_DIR_FRENTE;
        cadenciaActual = 8;
        cuartosPerdidos = 0;
    }
    // 2 fase 2, 75 a 50 por ciento, mira a la izquierda, dispara en cruz, cadencia mas rapida que antes
    else if (porcentaje > 50.0f) {
        faseActual = GRAVITAL_FASE_2_CRUZ;
        direccionActual = GRAVITAL_DIR_IZQUIERDA;
        cadenciaActual = 30;
        cuartosPerdidos = 1;
    }
    // 3 fase 3, 50 a 25 por ciento, mira a la derecha, dispara en Y con rayos que ahora barren
    else if (porcentaje > 25.0f) {
        faseActual = GRAVITAL_FASE_3_Y;
        direccionActual = GRAVITAL_DIR_DERECHA;
        cadenciaActual = 20;
        cuartosPerdidos = 2;
    }
    // 4 fase 4, menos de 25 por ciento, de espaldas, furia total con doble espiral
    else {
        faseActual = GRAVITAL_FASE_4_LOCURA;
        direccionActual = GRAVITAL_DIR_ESPALDA;
        cadenciaActual = 3;
        cuartosPerdidos = 3;
    }

    // 5 interpolar la apertura de los gajos hacia el objetivo de la fase actual
    // asi el quiebre se ve progresivo en vez de saltar de golpe apenas cambia de fase
    float aperturaObjetivo = static_cast<float>(cuartosPerdidos) / static_cast<float>(GRAVITAL_CANT_GAJOS - 1);
    if (aperturaGajos < aperturaObjetivo) {
        aperturaGajos = std::min(aperturaObjetivo, aperturaGajos + velocidadApertura);
    } else if (aperturaGajos > aperturaObjetivo) {
        aperturaGajos = std::max(aperturaObjetivo, aperturaGajos - velocidadApertura);
    }
}
bool Gravital::impactoEsVulnerable(float proyX, float proyY)
{
    if (enEscudoTotal) return false; // 1 invulnerable sin importar el angulo mientras dura el escudo

    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    if (direccionActual == GRAVITAL_DIR_ESPALDA) return (proyY < centroY);
    if (direccionActual == GRAVITAL_DIR_FRENTE) return (proyY > centroY);
    if (direccionActual == GRAVITAL_DIR_IZQUIERDA) return (proyX < centroX);
    if (direccionActual == GRAVITAL_DIR_DERECHA) return (proyX > centroX);

    return true;
}

void Gravital::ejecutarAtaque()
{
    switch (faseActual) {
    case GRAVITAL_FASE_1_ESPIRAL:
        dispararEspiral();
        break;
    case GRAVITAL_FASE_2_CRUZ:
        dispararCruz();
        break;
    case GRAVITAL_FASE_3_Y:
        dispararY();
        break;
    case GRAVITAL_FASE_4_LOCURA:
        // 1 en la fase final dispara dos rafadas de espiral seguidas para intensificar el ataque
        dispararEspiral();
        anguloEspiral += 25.0f;
        dispararEspiral();
        break;
    }
    timerDisparo = cadenciaActual;
}

void Gravital::dispararEspiral()
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    // 1 la espiral gira mas rapido cuanto mas cuartos de vida perdio
    float incrementoAngulo = 15.0f + cuartosPerdidos * 3.0f;
    anguloEspiral += incrementoAngulo;
    if(anguloEspiral >= 360.0f) anguloEspiral -= 360.0f;

    // 2 mas velocidad y mas danio por proyectil segun la fase
    float velocidadProyectil = 6.0f + cuartosPerdidos * 1.0f;
    float danioProyectil = 20.0f + cuartosPerdidos * 5.0f;

    Proyectil* p = new Proyectil();
    p->reiniciar(centroX, centroY, anguloEspiral, velocidadProyectil, danioProyectil, false, 0.0f);
    proyectiles.append(p);
}

void Gravital::dispararCruz()
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    anguloEspiral += 5.0f;
    float angulos[4] = {0.0f, 90.0f, 180.0f, 270.0f};

    // 1 mas velocidad y danio que la version anterior para que la cruz pegue mas fuerte
    float velocidadProyectil = 7.0f;
    float danioProyectil = 24.0f;

    for(int i = 0; i < 4; i++) {
        Proyectil* p = new Proyectil();
        p->reiniciar(centroX, centroY, angulos[i] + anguloEspiral, velocidadProyectil, danioProyectil, false, 0.0f);
        proyectiles.append(p);
    }
}

void Gravital::dispararY()
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    float angulos[3] = {90.0f, 225.0f, 315.0f};

    // 1 mas rango de rayo cuanta menos vida le queda, ahora bastante mas largo que antes
    float longitudRayo = (800.0f + cuartosPerdidos * 60.0f) * factorMapa; // NUEVO: * factorMapa
    // 2 arco de barrido, arranca en 90 grados y suma 45 grados por cada 25 por ciento de vida menos
    float rangoOscilacion = 90.0f + cuartosPerdidos * 45.0f;
    float velocidadOscilacion = 0.6f + cuartosPerdidos * 0.15f;
    float danioRayo = 25.0f + cuartosPerdidos * 5.0f;

    for(int i = 0; i < 3; i++) {
        RayoLaser* r = new RayoLaser();
        r->reiniciar(centroX, centroY, angulos[i], longitudRayo, danioRayo,
                     0.4f, 0.5f, 18.0f, rangoOscilacion, velocidadOscilacion);
        rayosLaser.append(r);
    }
}

QList<Proyectil*>& Gravital::getProyectiles()
{
    return proyectiles;
}

void Gravital::dibujar(QPainter &painter)
{
    if(!estaVivo()) return;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1 dibujar proyectiles por debajo del boss
    for (auto* p : proyectiles) p->dibujar(painter);
    for (auto* r : rayosLaser) r->dibujar(painter);
    for (auto* c : proyectilesCurvos) c->dibujar(painter);


    // 2 dibujar la esfera pixel art, ya partida en gajos si perdio vida
    // apagar el antialiasing solo para los gajos pixel art, asi los bloques
    // quedan nitidos en vez de suavizados
    painter.setRenderHint(QPainter::Antialiasing, false);
    dibujarEsferaMetalica(painter);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 3 dibujar el ojo segun la direccion actual
    dibujarOjo(painter);

    // 4 dibujar el escudo visual que bloquea el danio
    dibujarEscudoInvulnerabilidad(painter);

    // 5 barra de vida estilo boss
    float barraAncho = 140.0f;
    float barraAlto = 10.0f;
    float barraX = pos_x + (ancho - barraAncho) / 2.0f;
    float barraY = pos_y - 25.0f + flotacionY;

    painter.fillRect(barraX, barraY, barraAncho, barraAlto, QColor(0, 0, 0, 200));
    float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
    painter.fillRect(barraX, barraY, barraAncho * porcentaje, barraAlto, QColor(200, 50, 0));
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(barraX, barraY, barraAncho, barraAlto);

    painter.restore();
}

// ============================================================
// 1 diseno pixel art del gravital
// la esfera se arma con 4 gajos identicos (uno por cuadrante de 90 grados)
// cada gajo es una grilla logica de 10 filas por 10 columnas, recortada en
// forma de cuarto de circulo con distancia euclidiana desde el centro
// escalaPixel define el tamano de cada pixel logico en pantalla, se calcula
// como radio de la esfera dividido GRAVITAL_GAJO_FILAS para que siempre
// ocupe el mismo espacio sin importar el tamano final del boss
// paleta usada por gajo:
//   gris claro 150,165,165  reflejo metalico superior
//   gris medio 65,78,78     panel base
//   gris oscuro 30,40,40    sombra del borde exterior curvo
//   marron oxido 110,55,35  chorreado vertical de oxido
//   casi negro 20,24,24     remaches
//   naranja brillante       grieta del borde recto, mas intensa cuanto mas
//                           abierto esta el gajo (aperturaGajos)
// cuando aperturaGajos es mayor a cero, cada gajo se desplaza hacia afuera
// sobre su propia bisectriz de 45 grados, como si la naranja se pelara, y
// se ve un nucleo de energia (dibujarNucleoEnergia) por la abertura central
// ============================================================
void Gravital::dibujarEsferaMetalica(QPainter &painter)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;
    float escalaPixel = (ancho / 2.0f) / static_cast<float>(GRAVITAL_GAJO_FILAS);

    if (aperturaGajos > 0.02f) {
        dibujarNucleoEnergia(painter, centroX, centroY);
    }

    for (int gajo = 0; gajo < GRAVITAL_CANT_GAJOS; gajo++) {
        dibujarGajoPixelArt(painter, centroX, centroY, gajo, escalaPixel);
    }
}

void Gravital::dibujarGajoPixelArt(QPainter &painter, float centroX, float centroY, int indiceGajo, float escalaPixel)
{
    painter.save();
    painter.translate(centroX, centroY);
    painter.rotate(indiceGajo * 90.0f);

    // 1 empuja el gajo hacia afuera sobre su bisectriz de 45 grados segun cuanto se abrio
    float desplazamiento = aperturaGajos * 42.0f;
    float bisectrizRad = 45.0f * static_cast<float>(M_PI) / 180.0f;
    painter.translate(std::cos(bisectrizRad) * desplazamiento, -std::sin(bisectrizRad) * desplazamiento);

    // 2 la grieta del borde recto se ilumina mas cuanto mas abierto esta el gajo
    int brilloGrieta = static_cast<int>(70 + aperturaGajos * 175.0f);

    for (int fila = 0; fila < GRAVITAL_GAJO_FILAS; fila++) {
        for (int columna = 0; columna < GRAVITAL_GAJO_COLUMNAS; columna++) {
            float distancia = std::sqrt(static_cast<float>(fila * fila + columna * columna));
            if (distancia > 9.5f) continue; // 3 recorta la grilla para que forme un cuarto de circulo

            float px = columna * escalaPixel;
            float py = -fila * escalaPixel;

            bool esBordeRecto = (fila == 0 || columna == 0);
            bool esRemache = ((fila == 2 && columna == 2) || (fila == 2 && columna == 7) || (fila == 7 && columna == 2));
            bool esOxido = (columna == 4 && fila >= 3 && fila <= 8 && (fila % 2 == 0));
            bool esBordeExterior = (distancia > 8.3f);

            QColor colorPixel;
            if (esBordeRecto) {
                colorPixel = QColor(255, 140, 40, brilloGrieta);
            } else if (esRemache) {
                colorPixel = QColor(20, 24, 24);
            } else if (esOxido) {
                colorPixel = QColor(110, 55, 35);
            } else if (esBordeExterior) {
                colorPixel = QColor(30, 40, 40);
            } else if ((fila + columna) % 4 == 0) {
                colorPixel = QColor(150, 165, 165);
            } else {
                colorPixel = QColor(65, 78, 78);
            }

            painter.fillRect(QRectF(px, py, escalaPixel + 0.5f, escalaPixel + 0.5f), colorPixel);
        }
    }

    painter.restore();
}

void Gravital::dibujarNucleoEnergia(QPainter &painter, float centroX, float centroY)
{
    // 1 nucleo que se asoma por las grietas cuando los gajos se separan
    // cuanto mas abierto esta el gravital, mas grande y mas opaco se ve
    float radioNucleo = 18.0f + aperturaGajos * 32.0f;
    int alfaNucleo = static_cast<int>(110 + aperturaGajos * 140.0f);

    QRadialGradient nucleo(centroX, centroY, radioNucleo);
    nucleo.setColorAt(0.0, QColor(255, 230, 180, alfaNucleo));
    nucleo.setColorAt(0.5, QColor(255, 90, 20, alfaNucleo));
    nucleo.setColorAt(1.0, QColor(255, 40, 10, 0));

    painter.setBrush(nucleo);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(centroX, centroY), radioNucleo, radioNucleo);
}

void Gravital::dibujarOjo(QPainter &painter)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    float ojoX = centroX;
    float ojoY = centroY;

    if (direccionActual == GRAVITAL_DIR_FRENTE) {
        ojoY += 10.0f;
    } else if (direccionActual == GRAVITAL_DIR_IZQUIERDA) {
        ojoX -= 50.0f;
    } else if (direccionActual == GRAVITAL_DIR_DERECHA) {
        ojoX += 50.0f;
    } else if (direccionActual == GRAVITAL_DIR_ESPALDA) {
        return;
    }

    float pulso = std::sin(frameAnimacion * 3.0f) * 3.0f;
    float radioOjo = 15.0f;

    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(ojoX, ojoY), radioOjo + 5.0f, radioOjo + 5.0f);

    // 1 azul mientras esta en escudo, rojo el resto del tiempo
    QColor colorOjo = enEscudoTotal ? QColor(60, 160, 255) : QColor(255, 50, 50);

    QRadialGradient halo(ojoX, ojoY, radioOjo + pulso);
    halo.setColorAt(0.0, colorOjo);
    halo.setColorAt(1.0, QColor(colorOjo.red(), colorOjo.green(), colorOjo.blue(), 0));
    painter.setBrush(halo);
    painter.drawEllipse(QPointF(ojoX, ojoY), radioOjo + pulso, radioOjo + pulso);

    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(ojoX, ojoY), 4.0f, 4.0f);
}

void Gravital::dibujarEscudoInvulnerabilidad(QPainter &painter)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;
    float radio = ancho / 2.0f + 15.0f;

    // 1 durante el escudo total se ve un circulo completo, no solo el arco
    if (enEscudoTotal) {
        painter.setPen(QPen(QColor(60, 160, 255, 200), 6));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(QPointF(centroX, centroY), radio, radio);
        return;
    }

    painter.setPen(QPen(QColor(0, 255, 100, 150), 6));
    painter.setBrush(Qt::NoBrush);

    int spanAngle = 180 * 16;
    int startAngle = 0;

    if (direccionActual == GRAVITAL_DIR_FRENTE) {
        startAngle = 0;
    } else if (direccionActual == GRAVITAL_DIR_IZQUIERDA) {
        startAngle = 270 * 16;
    } else if (direccionActual == GRAVITAL_DIR_DERECHA) {
        startAngle = 90 * 16;
    } else if (direccionActual == GRAVITAL_DIR_ESPALDA) {
        startAngle = 180 * 16;
    }

    painter.drawArc(pos_x - 15, pos_y - 15 + flotacionY, ancho + 30, alto + 30, startAngle, spanAngle);
}

void Gravital::dispararCurvo()
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f + flotacionY;

    ProyectilCurvo* p1 = new ProyectilCurvo();
    p1->reiniciar(centroX, centroY, anguloEspiral, 7.0f, 18.0f, 0.8f, 6.0f, 400);
    proyectilesCurvos.append(p1);

    ProyectilCurvo* p2 = new ProyectilCurvo();
    p2->reiniciar(centroX, centroY, anguloEspiral + 180.0f, 7.0f, 18.0f, -0.8f, 6.0f, 400);
    proyectilesCurvos.append(p2);
}
