#include "sobreviviente.h"
#include "granada.h"
#include "lanzacohetes.h"
#include "escopeta.h"
#include "lanzallamas.h"
#include "lanzasierras.h"
#include <cmath>

Sobreviviente::Sobreviviente(float x, float y)
    : pos_x(x), pos_y(y), armaActual(1), sistemaVida(100.0f), skinActual(SKIN_SURVIVOR), timerDanio(0)
{
    for (int i = 0; i < 8; i++) armasPoseidas[i] = nullptr;

    armasPoseidas[1] = new Pistola();
    arma = armasPoseidas[1];
    aplicarEstadisticas();
}

Sobreviviente::~Sobreviviente()
{
    for (int i = 0; i < 8; i++) delete armasPoseidas[i];
}

void Sobreviviente::setSkin(int nuevaSkin)
{
    skinActual = nuevaSkin;
    aplicarEstadisticas();
}

void Sobreviviente::aplicarEstadisticas() {
    if (skinActual == SKIN_SURVIVOR) {
        stats.velocidadMovimiento = 1.0f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 120.0f;
        stats.resistenciaDanio = 0.15f;
    } else if (skinActual == SKIN_MILITAR) {
        stats.velocidadMovimiento = 1.2f;
        stats.velocidadRecarga = 0.75f;
        stats.vidaMaxima = 100.0f;
        stats.resistenciaDanio = 0.0f;
    } else if (skinActual == SKIN_RUNNER) {
        // ESTADÍSTICAS DE BRISA: Máxima agilidad
        stats.velocidadMovimiento = 1.6f;  // Muy rápida (+60%)
        stats.velocidadRecarga = 0.5f;     // Recarga en la mitad de tiempo
        stats.vidaMaxima = 100.0f;
        stats.resistenciaDanio = 0.5f;
    } else if (skinActual == SKIN_Argentine) {
        // ESTADISTICAS DE Argentine: velocidad y resistencia
        stats.velocidadMovimiento = 1.5f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 100.0f;
        stats.resistenciaDanio = 0.3f;
    } else if (skinActual == SKIN_Exterminador) {
        // ESTADISTICAS DE DOOM SLAYER: mucha resistencia, poca velocidad
        stats.velocidadMovimiento = 0.75f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 200.0f;
        stats.resistenciaDanio = 0.4f;
    }

    sistemaVida.setVidaMaxima(stats.vidaMaxima);
    sistemaVida.restaurarCompleta();
    if (arma) {
        arma->setMultiplicadorRecarga(stats.velocidadRecarga);
    }
}

int Sobreviviente::getSkin() const
{
    return skinActual;
}

EstadisticasPersonaje Sobreviviente::getStats() const
{
    return stats;
}

float Sobreviviente::getVelocidadMovimiento() const
{
    return stats.velocidadMovimiento;
}

void Sobreviviente::dibujar(QPainter &painter) {
    if (skinActual == SKIN_SURVIVOR) {
        dibujarSurvivor(painter);
    } else if (skinActual == SKIN_MILITAR) {
        dibujarMilitar(painter);
    } else if (skinActual == SKIN_RUNNER) {
        dibujarRunner(painter); // <-- Llamada al nuevo dibujo
    } else if (skinActual == SKIN_Argentine) {
        dibujarArgentine(painter);
    } else if (skinActual == SKIN_Exterminador) {
        dibujarExterminador(painter);
    }
    // --- NUEVO: Efecto de teñido rojo al recibir daño ---
    if (timerDanio > 0) {
        painter.save();
        // Color rojo brillante con opacidad al ~60% (150 de 255)
        painter.setBrush(QColor(255, 50, 50, 150));
        painter.setPen(Qt::NoPen);
        // Cubrimos el área exacta de los pixeles del sprite
        painter.drawRect(pos_x, pos_y - 4, ancho, 56);
        painter.restore();
    }

    if (arma) arma->dibujar(painter);

    // Barra de vida
    if (sistemaVida.estaVivo()) {
        float barraAncho = 30.0f;
        float barraAlto = 4.0f;
        float barraX = pos_x;
        float barraY = pos_y - 10.0f;

        painter.setBrush(QColor(100, 0, 0));
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);

        float porcentaje = sistemaVida.getPorcentajeVida() / 100.0f;
        QColor colorVida = sistemaVida.estaCritico() ? QColor(200, 50, 0) : QColor(0, 200, 0);
        painter.setBrush(colorVida);
        painter.drawRect(barraX, barraY, barraAncho * porcentaje, barraAlto);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawRect(barraX, barraY, barraAncho, barraAlto);
    }

}

// ============================================================
// rediseno pixel art del eternauta, con vista para los 4 lados
// paleta: traje azul grisaceo con sombreado lateral (mas oscuro del
// lado de la sombra, mas claro del lado de la luz), guantes y botas
// casi negros, mochila con dos tanques de oxigeno conectados por
// mangueras a la mascara, capucha con dos lentes redondos y un
// filtro central visto de frente, un solo lente y el filtro de
// perfil visto de costado
// la caminata ya no es el rebote triangular de antes: faseCaminata
// crece de forma continua mientras se mueve y las piernas y brazos
// se balancean con seno, en contrafase entre si, mas un pequeno
// rebote vertical del torso (bob) para que se sienta mas vivo
// ============================================================
void Sobreviviente::dibujarSurvivor(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    switch (direccionMovimiento) {
    case SOBREVIVIENTE_DIR_ARRIBA:
        dibujarSurvivorArriba(painter);
        break;
    case SOBREVIVIENTE_DIR_IZQUIERDA:
        dibujarSurvivorIzquierda(painter);
        break;
    case SOBREVIVIENTE_DIR_DERECHA:
        dibujarSurvivorDerecha(painter);
        break;
    default:
        dibujarSurvivorAbajo(painter);
        break;
    }

    painter.restore();
}

void Sobreviviente::dibujarSurvivorAbajo(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor traje(70, 110, 150);
    QColor trajeSombra = traje.darker(125);
    QColor trajeClaro = traje.lighter(112);
    QColor guantes(35, 35, 38);
    QColor botas(28, 28, 32);
    QColor mochila(58, 78, 92);
    QColor panuelo(150, 130, 90);
    QColor metal(150, 150, 140);

    painter.setPen(Qt::NoPen);

    // 1 piernas, alternan con el ciclo de caminata
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 18*ps + pierna, 3*ps, 6*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 18*ps - pierna, 3*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 5*ps, pos_y + 23*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 23*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 5*ps, pos_y + 25*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 25*ps - pierna, 3*ps, 1*ps);

    // 2 brazos, en contrafase con las piernas
    painter.setBrush(traje);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 8*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 8*ps);
    painter.setBrush(guantes);
    painter.drawRect(pos_x + 2*ps, pos_y + 17*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 17*ps - brazo, 2*ps, 2*ps);

    // 3 torso con sombreado lateral, mas claro del lado izquierdo
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 9*ps - bob, 9*ps, 9*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 9*ps);
    painter.setBrush(trajeClaro);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 3*ps, 9*ps);

    // 4 cinturon y hebilla
    painter.setBrush(metal.darker(120));
    painter.drawRect(pos_x + 4*ps, pos_y + 15*ps - bob, 9*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 7*ps, pos_y + 14*ps - bob, 3*ps, 2*ps);

    // 5 mochila de tela asomando de los costados
    painter.setBrush(mochila);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps - bob, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - bob, 2*ps, 7*ps);

    // 6 capucha
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 9*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 8*ps);

    // 7 rendija de ojos y panuelo tapando nariz y boca
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 1*ps);
    painter.setBrush(panuelo);
    painter.drawRect(pos_x + 4*ps, pos_y + 3*ps - bob, 8*ps, 4*ps);
}

void Sobreviviente::dibujarSurvivorArriba(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor traje(70, 110, 150);
    QColor trajeSombra = traje.darker(125);
    QColor guantes(35, 35, 38);
    QColor botas(28, 28, 32);
    QColor mochila(58, 78, 92);
    QColor mochilaOscura = mochila.darker(130);
    QColor metal(150, 150, 140);

    painter.setPen(Qt::NoPen);

    // 1 piernas
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 18*ps + pierna, 3*ps, 6*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 18*ps - pierna, 3*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 5*ps, pos_y + 23*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 23*ps - pierna, 3*ps, 3*ps);

    // 2 brazos
    painter.setBrush(traje);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 8*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 8*ps);
    painter.setBrush(guantes);
    painter.drawRect(pos_x + 2*ps, pos_y + 17*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 17*ps - brazo, 2*ps, 2*ps);

    // 3 torso, espalda
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 9*ps - bob, 9*ps, 9*ps);

    // 4 mochila de tela, con solapa y una hebilla central
    painter.setBrush(mochilaOscura);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 7*ps, 10*ps);
    painter.setBrush(mochila);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 6*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 7*ps, pos_y + 12*ps - bob, 3*ps, 1*ps);

    // 5 capucha de espaldas, con costura central
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 9*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 8*ps);
    painter.setBrush(traje.darker(115));
    painter.drawRect(pos_x + 7*ps, pos_y - 1*ps - bob, 3*ps, 8*ps);
}

void Sobreviviente::dibujarSurvivorDerecha(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.5f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor traje(70, 110, 150);
    QColor trajeSombra = traje.darker(125);
    QColor guantes(35, 35, 38);
    QColor botas(28, 28, 32);
    QColor mochila(58, 78, 92);
    QColor panuelo(150, 130, 90);
    QColor metal(150, 150, 140);

    painter.setPen(Qt::NoPen);

    // 1 mochila asomando detras del torso
    painter.setBrush(mochila.darker(115));
    painter.drawRect(pos_x + 2*ps, pos_y + 8*ps - bob, 3*ps, 9*ps);

    // 2 pierna trasera y delantera alternando
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 23*ps, 4*ps, 3*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 23*ps, 4*ps, 3*ps);

    // 3 torso con sombra del lado de atras
    painter.setBrush(traje);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 8*ps, 9*ps);
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 3*ps, 9*ps);

    // 4 cinturon
    painter.setBrush(metal.darker(120));
    painter.drawRect(pos_x + 5*ps, pos_y + 15*ps - bob, 8*ps, 1*ps);

    // 5 brazo delantero balanceando
    painter.setBrush(traje);
    painter.drawRect(pos_x + 10*ps + pierna*0.5f, pos_y + 10*ps - bob, 3*ps, 8*ps);
    painter.setBrush(guantes);
    painter.drawRect(pos_x + 10*ps + pierna*0.5f, pos_y + 17*ps - bob, 3*ps, 2*ps);

    // 6 capucha de perfil
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 9*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 5*ps, pos_y - 1*ps - bob, 7*ps, 8*ps);

    // 7 rendija de ojos y panuelo de perfil, tapando nariz y boca
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 9*ps, pos_y + 2*ps - bob, 4*ps, 1*ps);
    painter.setBrush(panuelo);
    painter.drawRect(pos_x + 9*ps, pos_y + 3*ps - bob, 6*ps, 4*ps);
}

void Sobreviviente::dibujarSurvivorIzquierda(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.5f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor traje(70, 110, 150);
    QColor trajeSombra = traje.darker(125);
    QColor guantes(35, 35, 38);
    QColor botas(28, 28, 32);
    QColor mochila(58, 78, 92);
    QColor panuelo(150, 130, 90);
    QColor metal(150, 150, 140);

    painter.setPen(Qt::NoPen);

    // 1 mochila asomando detras del torso, del lado derecho ahora
    painter.setBrush(mochila.darker(115));
    painter.drawRect(pos_x + 11*ps, pos_y + 8*ps - bob, 3*ps, 9*ps);

    // 2 pierna trasera y delantera alternando, en espejo
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 23*ps, 4*ps, 3*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 23*ps, 4*ps, 3*ps);

    // 3 torso con sombra del lado de atras (ahora a la derecha)
    painter.setBrush(traje);
    painter.drawRect(pos_x + 3*ps, pos_y + 9*ps - bob, 8*ps, 9*ps);
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 8*ps, pos_y + 9*ps - bob, 3*ps, 9*ps);

    // 4 cinturon
    painter.setBrush(metal.darker(120));
    painter.drawRect(pos_x + 3*ps, pos_y + 15*ps - bob, 8*ps, 1*ps);

    // 5 brazo delantero balanceando
    painter.setBrush(traje);
    painter.drawRect(pos_x + 3*ps - pierna*0.5f, pos_y + 10*ps - bob, 3*ps, 8*ps);
    painter.setBrush(guantes);
    painter.drawRect(pos_x + 3*ps - pierna*0.5f, pos_y + 17*ps - bob, 3*ps, 2*ps);

    // 6 capucha de perfil
    painter.setBrush(trajeSombra);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 9*ps, 9*ps);
    painter.setBrush(traje);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 7*ps, 8*ps);

    // 7 rendija de ojos y panuelo de perfil, tapando nariz y boca
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 1*ps, pos_y + 2*ps - bob, 4*ps, 1*ps);
    painter.setBrush(panuelo);
    painter.drawRect(pos_x + 0*ps, pos_y + 3*ps - bob, 6*ps, 4*ps);
}

// ============================================================
// MILITAR
// uniforme oliva con chaleco tactico, casco, correa bajo el menton,
// placa identificatoria colgando del cuello y bolsillos porta
// cargadores en el pecho. mismo ciclo de caminata que el eternauta
// ============================================================
void Sobreviviente::dibujarMilitar(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    switch (direccionMovimiento) {
    case SOBREVIVIENTE_DIR_ARRIBA:
        dibujarMilitarArriba(painter);
        break;
    case SOBREVIVIENTE_DIR_IZQUIERDA:
        dibujarMilitarIzquierda(painter);
        break;
    case SOBREVIVIENTE_DIR_DERECHA:
        dibujarMilitarDerecha(painter);
        break;
    default:
        dibujarMilitarAbajo(painter);
        break;
    }

    painter.restore();
}

void Sobreviviente::dibujarMilitarAbajo(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor uniforme(100, 120, 80);
    QColor uniformeSombra = uniforme.darker(125);
    QColor chaleco(70, 78, 60);
    QColor casco(60, 80, 50);
    QColor piel(200, 150, 120);
    QColor botas(40, 35, 30);
    QColor correa(55, 42, 30);
    QColor metal(140, 140, 130);

    painter.setPen(Qt::NoPen);

    // 1 piernas con bota y hebilla
    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 7*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 7*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 5*ps, pos_y + 23*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 23*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 5*ps, pos_y + 22*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 22*ps - pierna, 3*ps, 1*ps);

    // 2 brazos con mano al descubierto
    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 7*ps);
    painter.setBrush(piel);
    painter.drawRect(pos_x + 2*ps, pos_y + 16*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 16*ps - brazo, 2*ps, 2*ps);

    // 3 torso con chaleco tactico encima
    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 9*ps - bob, 9*ps, 9*ps);
    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 9*ps);
    painter.setBrush(chaleco);
    painter.drawRect(pos_x + 5*ps, pos_y + 10*ps - bob, 7*ps, 6*ps);

    // 4 bolsillos porta cargadores y cinturon
    painter.setBrush(chaleco.darker(120));
    painter.drawRect(pos_x + 6*ps, pos_y + 11*ps - bob, 2*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 11*ps - bob, 2*ps, 3*ps);
    painter.setBrush(correa);
    painter.drawRect(pos_x + 4*ps, pos_y + 15*ps - bob, 9*ps, 1*ps);

    // 5 placa identificatoria
    painter.setBrush(metal);
    painter.drawRect(pos_x + 7*ps, pos_y + 9*ps - bob, 2*ps, 2*ps);

    // 6 casco con visera y correa bajo el menton
    painter.setBrush(casco.darker(120));
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 8*ps);
    painter.setBrush(casco);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 6*ps);
    painter.setBrush(casco.darker(140));
    painter.drawRect(pos_x + 3*ps, pos_y + 4*ps - bob, 11*ps, 1*ps);

    // 7 cara, ojos y correa del casco
    painter.setBrush(piel);
    painter.drawRect(pos_x + 5*ps, pos_y + 4*ps - bob, 6*ps, 4*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps, pos_y + 5*ps - bob, 1*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 5*ps - bob, 1*ps, 1*ps);
    painter.setBrush(correa);
    painter.drawRect(pos_x + 5*ps, pos_y + 7*ps - bob, 1*ps, 2*ps);
    painter.drawRect(pos_x + 10*ps, pos_y + 7*ps - bob, 1*ps, 2*ps);
}

void Sobreviviente::dibujarMilitarArriba(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor uniforme(100, 120, 80);
    QColor uniformeSombra = uniforme.darker(125);
    QColor chaleco(70, 78, 60);
    QColor casco(60, 80, 50);
    QColor piel(200, 150, 120);
    QColor botas(40, 35, 30);
    QColor metal(140, 140, 130);
    QColor mochilaMilitar(55, 65, 45);

    painter.setPen(Qt::NoPen);

    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 7*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 7*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 5*ps, pos_y + 23*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 23*ps - pierna, 3*ps, 3*ps);

    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 7*ps);
    painter.setBrush(piel);
    painter.drawRect(pos_x + 2*ps, pos_y + 16*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 16*ps - brazo, 2*ps, 2*ps);

    // 1 mochila de asalto con correas cruzadas en la espalda
    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 9*ps - bob, 9*ps, 9*ps);
    painter.setBrush(mochilaMilitar);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 7*ps);
    painter.setBrush(chaleco.darker(115));
    painter.drawRect(pos_x + 6*ps, pos_y + 10*ps - bob, 2*ps, 5*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 10*ps - bob, 2*ps, 5*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 1*ps);

    // 2 casco de espaldas
    painter.setBrush(casco.darker(120));
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 8*ps);
    painter.setBrush(casco);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 7*ps);
    painter.setBrush(casco.darker(135));
    painter.drawRect(pos_x + 8*ps, pos_y - 1*ps - bob, 1*ps, 7*ps);
}

void Sobreviviente::dibujarMilitarDerecha(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.5f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor uniforme(100, 120, 80);
    QColor uniformeSombra = uniforme.darker(125);
    QColor chaleco(70, 78, 60);
    QColor casco(60, 80, 50);
    QColor piel(200, 150, 120);
    QColor botas(40, 35, 30);
    QColor metal(140, 140, 130);

    painter.setPen(Qt::NoPen);

    painter.setBrush(chaleco.darker(115));
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps - bob, 3*ps, 8*ps);

    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 23*ps, 4*ps, 3*ps);
    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 23*ps, 4*ps, 3*ps);

    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 8*ps, 9*ps);
    painter.setBrush(chaleco);
    painter.drawRect(pos_x + 5*ps, pos_y + 10*ps - bob, 5*ps, 6*ps);
    painter.setBrush(chaleco.darker(120));
    painter.drawRect(pos_x + 6*ps, pos_y + 11*ps - bob, 2*ps, 3*ps);

    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 10*ps + pierna*0.5f, pos_y + 10*ps - bob, 3*ps, 7*ps);
    painter.setBrush(piel);
    painter.drawRect(pos_x + 10*ps + pierna*0.5f, pos_y + 16*ps - bob, 3*ps, 2*ps);

    // 1 casco de perfil con visera adelante
    painter.setBrush(casco.darker(120));
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 8*ps);
    painter.setBrush(casco);
    painter.drawRect(pos_x + 5*ps, pos_y - 1*ps - bob, 7*ps, 6*ps);
    painter.setBrush(casco.darker(150));
    painter.drawRect(pos_x + 10*ps, pos_y + 3*ps - bob, 2*ps, 1*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 9*ps, pos_y + 4*ps - bob, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 12*ps, pos_y + 5*ps - bob, 1*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - bob, 2*ps, 2*ps);
}

void Sobreviviente::dibujarMilitarIzquierda(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.5f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor uniforme(100, 120, 80);
    QColor uniformeSombra = uniforme.darker(125);
    QColor chaleco(70, 78, 60);
    QColor casco(60, 80, 50);
    QColor piel(200, 150, 120);
    QColor botas(40, 35, 30);
    QColor metal(140, 140, 130);

    painter.setPen(Qt::NoPen);

    painter.setBrush(chaleco.darker(115));
    painter.drawRect(pos_x + 11*ps, pos_y + 9*ps - bob, 3*ps, 8*ps);

    painter.setBrush(uniformeSombra);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 23*ps, 4*ps, 3*ps);
    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(botas);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 23*ps, 4*ps, 3*ps);

    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 3*ps, pos_y + 9*ps - bob, 8*ps, 9*ps);
    painter.setBrush(chaleco);
    painter.drawRect(pos_x + 6*ps, pos_y + 10*ps - bob, 5*ps, 6*ps);
    painter.setBrush(chaleco.darker(120));
    painter.drawRect(pos_x + 8*ps, pos_y + 11*ps - bob, 2*ps, 3*ps);

    painter.setBrush(uniforme);
    painter.drawRect(pos_x + 3*ps - pierna*0.5f, pos_y + 10*ps - bob, 3*ps, 7*ps);
    painter.setBrush(piel);
    painter.drawRect(pos_x + 3*ps - pierna*0.5f, pos_y + 16*ps - bob, 3*ps, 2*ps);

    painter.setBrush(casco.darker(120));
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 9*ps, 8*ps);
    painter.setBrush(casco);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 7*ps, 6*ps);
    painter.setBrush(casco.darker(150));
    painter.drawRect(pos_x + 4*ps, pos_y + 3*ps - bob, 2*ps, 1*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 3*ps, pos_y + 4*ps - bob, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 3*ps, pos_y + 5*ps - bob, 1*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 1*ps, pos_y + 9*ps - bob, 2*ps, 2*ps);
}

// ============================================================
// BRISA
// se mantiene su piel morena, su pelo castano oscuro atado y la
// remera deportiva blanca y azul con "Run" al frente, tal cual la
// referencia. lo nuevo es mucho mas detalle: zapatillas de running
// con suela marcada, vincha, mechones de flequillo, sombreado en la
// piel y en la ropa, y las 4 vistas completas
// ============================================================
void Sobreviviente::dibujarRunner(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    switch (direccionMovimiento) {
    case SOBREVIVIENTE_DIR_ARRIBA:
        dibujarRunnerArriba(painter);
        break;
    case SOBREVIVIENTE_DIR_IZQUIERDA:
        dibujarRunnerIzquierda(painter);
        break;
    case SOBREVIVIENTE_DIR_DERECHA:
        dibujarRunnerDerecha(painter);
        break;
    default:
        dibujarRunnerAbajo(painter);
        break;
    }

    painter.restore();
}

void Sobreviviente::dibujarRunnerAbajo(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.2f * ps;
    float brazo = -pierna * 0.7f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.2f * ps;

    // 1 piel y pelo exactamente como la referencia, no se tocan
    QColor pielMorena(160, 110, 70);
    QColor pielSombra = pielMorena.darker(115);
    QColor cabello(50, 30, 10);
    QColor cabelloBrillo = cabello.lighter(140);
    QColor topBlanco(255, 255, 255);
    QColor topAzul(0, 50, 180);
    QColor shortNegro(20, 20, 20);
    QColor zapatilla(240, 240, 240);
    QColor suela(60, 60, 60);
    QColor vincha(0, 50, 180);

    painter.setPen(Qt::NoPen);

    // 2 piernas con zapatillas de running y suela marcada
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 5*ps, pos_y + 20*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 20*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 2*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 5*ps, pos_y + 26*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 26*ps - pierna, 3*ps, 1*ps);

    // 3 brazos
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - brazo, 2*ps, 7*ps);
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 1*ps, 7*ps);
    painter.drawRect(pos_x + 14*ps, pos_y + 9*ps - brazo, 1*ps, 7*ps);

    // 4 remera deportiva bicolor con "Run" al frente, igual que antes
    painter.setBrush(topAzul);
    painter.drawRect(pos_x + 4*ps, pos_y + 13*ps - bob, 8*ps, 4*ps);
    painter.setBrush(topBlanco);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 8*ps, 5*ps);
    painter.setBrush(topAzul.darker(115));
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 1*ps, 9*ps);
    painter.drawRect(pos_x + 11*ps, pos_y + 8*ps - bob, 1*ps, 9*ps);

    painter.setPen(QPen(Qt::white, 1));
    QFont f = painter.font(); f.setPointSize(5); f.setBold(true);
    painter.setFont(f);
    painter.drawText(pos_x + 4*ps, pos_y + 13*ps - bob, 8*ps, 4*ps, Qt::AlignCenter, "Run");
    painter.setPen(Qt::NoPen);

    // 5 cara con cejas y mejillas sombreadas, mas expresiva que antes
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 6*ps);
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 6*ps - bob, 6*ps, 2*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps, pos_y + 4*ps - bob, 1*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 4*ps - bob, 1*ps, 1*ps);
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 6*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.setBrush(pielSombra.darker(110));
    painter.drawRect(pos_x + 7*ps, pos_y + 6*ps - bob, 2*ps, 1*ps);

    // 6 flequillo, vincha y coleta atada arriba
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 5*ps, pos_y + 1*ps - bob, 6*ps, 2*ps);
    painter.setBrush(cabelloBrillo);
    painter.drawRect(pos_x + 6*ps, pos_y + 1*ps - bob, 2*ps, 1*ps);
    painter.setBrush(vincha);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 1*ps);
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 7*ps, pos_y + 0*ps - bob, 2*ps, 2*ps);
}

void Sobreviviente::dibujarRunnerArriba(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.2f * ps;
    float brazo = -pierna * 0.7f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.2f * ps;
    float rebotePonytail = std::sin(faseCaminata * 2.0f) * 1.5f * ps;

    QColor pielMorena(160, 110, 70);
    QColor pielSombra = pielMorena.darker(115);
    QColor cabello(50, 30, 10);
    QColor cabelloBrillo = cabello.lighter(140);
    QColor topBlanco(255, 255, 255);
    QColor topAzul(0, 50, 180);
    QColor shortNegro(20, 20, 20);
    QColor zapatilla(240, 240, 240);
    QColor suela(60, 60, 60);
    QColor liga(200, 40, 90);

    painter.setPen(Qt::NoPen);

    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 5*ps, pos_y + 20*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 20*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 2*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 5*ps, pos_y + 26*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 26*ps - pierna, 3*ps, 1*ps);

    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - brazo, 2*ps, 7*ps);

    // 1 espalda de la remera, sin el texto
    painter.setBrush(topAzul);
    painter.drawRect(pos_x + 4*ps, pos_y + 13*ps - bob, 8*ps, 4*ps);
    painter.setBrush(topBlanco);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 8*ps, 5*ps);

    // 2 nuca y pelo cubriendo toda la cabeza
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 5*ps, pos_y + 1*ps - bob, 6*ps, 7*ps);
    painter.setBrush(cabelloBrillo);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 2*ps, 4*ps);

    // 3 coleta larga que rebota con el movimiento
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 6*ps, pos_y + 7*ps - bob, 4*ps, 2*ps);
    painter.drawRect(pos_x + 6*ps + rebotePonytail, pos_y + 9*ps - bob, 3*ps, 6*ps);
    painter.setBrush(liga);
    painter.drawRect(pos_x + 6*ps, pos_y + 7*ps - bob, 4*ps, 1*ps);
    painter.setBrush(cabelloBrillo);
    painter.drawRect(pos_x + 6*ps + rebotePonytail, pos_y + 10*ps - bob, 1*ps, 4*ps);
}

void Sobreviviente::dibujarRunnerDerecha(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.8f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.2f * ps;
    float rebotePonytail = std::sin(faseCaminata * 2.0f) * 1.5f * ps;

    QColor pielMorena(160, 110, 70);
    QColor pielSombra = pielMorena.darker(115);
    QColor cabello(50, 30, 10);
    QColor topBlanco(255, 255, 255);
    QColor topAzul(0, 50, 180);
    QColor shortNegro(20, 20, 20);
    QColor zapatilla(240, 240, 240);
    QColor suela(60, 60, 60);
    QColor liga(200, 40, 90);
    QColor vincha(0, 50, 180);

    painter.setPen(Qt::NoPen);

    // 1 pierna trasera y delantera, zancada mas amplia por ser corredora
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 2*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 26*ps, 4*ps, 1*ps);

    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 2*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 26*ps, 4*ps, 1*ps);

    // 2 torso con la remera de perfil
    painter.setBrush(topAzul);
    painter.drawRect(pos_x + 5*ps, pos_y + 13*ps - bob, 8*ps, 4*ps);
    painter.setBrush(topBlanco);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 8*ps, 5*ps);
    painter.setBrush(topAzul.darker(115));
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 2*ps, 9*ps);

    // 3 brazo delantero balanceando
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 10*ps + pierna*0.5f, pos_y + 9*ps - bob, 3*ps, 7*ps);

    // 4 coleta rebotando detras de la cabeza
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 2*ps + rebotePonytail, pos_y + 3*ps - bob, 3*ps, 6*ps);
    painter.setBrush(liga);
    painter.drawRect(pos_x + 3*ps, pos_y + 3*ps - bob, 2*ps, 1*ps);

    // 5 cabeza de perfil con flequillo, vincha y ojo
    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 6*ps);
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 5*ps, pos_y + 1*ps - bob, 6*ps, 2*ps);
    painter.setBrush(vincha);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 1*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 9*ps, pos_y + 4*ps - bob, 1*ps, 1*ps);
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 9*ps, pos_y + 6*ps - bob, 2*ps, 1*ps);
}

void Sobreviviente::dibujarRunnerIzquierda(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.8f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.2f * ps;
    float rebotePonytail = std::sin(faseCaminata * 2.0f) * 1.5f * ps;

    QColor pielMorena(160, 110, 70);
    QColor pielSombra = pielMorena.darker(115);
    QColor cabello(50, 30, 10);
    QColor topBlanco(255, 255, 255);
    QColor topAzul(0, 50, 180);
    QColor shortNegro(20, 20, 20);
    QColor zapatilla(240, 240, 240);
    QColor suela(60, 60, 60);
    QColor liga(200, 40, 90);
    QColor vincha(0, 50, 180);

    painter.setPen(Qt::NoPen);

    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 2*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 26*ps, 4*ps, 1*ps);

    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 4*ps);
    painter.setBrush(shortNegro);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 2*ps);
    painter.setBrush(zapatilla);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 2*ps);
    painter.setBrush(suela);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 26*ps, 4*ps, 1*ps);

    painter.setBrush(topAzul);
    painter.drawRect(pos_x + 3*ps, pos_y + 13*ps - bob, 8*ps, 4*ps);
    painter.setBrush(topBlanco);
    painter.drawRect(pos_x + 3*ps, pos_y + 8*ps - bob, 8*ps, 5*ps);
    painter.setBrush(topAzul.darker(115));
    painter.drawRect(pos_x + 9*ps, pos_y + 8*ps - bob, 2*ps, 9*ps);

    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 3*ps - pierna*0.5f, pos_y + 9*ps - bob, 3*ps, 7*ps);

    painter.setBrush(cabello);
    painter.drawRect(pos_x + 11*ps - rebotePonytail, pos_y + 3*ps - bob, 3*ps, 6*ps);
    painter.setBrush(liga);
    painter.drawRect(pos_x + 11*ps, pos_y + 3*ps - bob, 2*ps, 1*ps);

    painter.setBrush(pielMorena);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 6*ps);
    painter.setBrush(cabello);
    painter.drawRect(pos_x + 5*ps, pos_y + 1*ps - bob, 6*ps, 2*ps);
    painter.setBrush(vincha);
    painter.drawRect(pos_x + 5*ps, pos_y + 2*ps - bob, 6*ps, 1*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps, pos_y + 4*ps - bob, 1*ps, 1*ps);
    painter.setBrush(pielSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 6*ps - bob, 2*ps, 1*ps);
}

// ============================================================
// Argentine
// camiseta celeste y blanca a rayas, short y medias, botines. mismo
// tratamiento pixel art que el resto, con mas detalle en el cuello,
// las rayas de las medias y el pelo
// ============================================================
void Sobreviviente::dibujarArgentine(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    switch (direccionMovimiento) {
    case SOBREVIVIENTE_DIR_ARRIBA:
        dibujarArgentineArriba(painter);
        break;
    case SOBREVIVIENTE_DIR_IZQUIERDA:
        dibujarArgentineIzquierda(painter);
        break;
    case SOBREVIVIENTE_DIR_DERECHA:
        dibujarArgentineDerecha(painter);
        break;
    default:
        dibujarArgentineAbajo(painter);
        break;
    }

    painter.restore();
}

void Sobreviviente::dibujarArgentineAbajo(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.2f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor celeste(130, 180, 230);
    QColor celesteSombra = celeste.darker(115);
    QColor blanco(250, 250, 250);
    QColor shortAzul(30, 40, 90);
    QColor piel(200, 160, 120);
    QColor pelo(40, 30, 20);
    QColor medias(245, 245, 245);
    QColor rayaMedia(30, 40, 90);

    painter.setPen(Qt::NoPen);

    // 1 medias con raya y botines con tapones
    painter.setBrush(medias);
    painter.drawRect(pos_x + 5*ps, pos_y + 20*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 20*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 5*ps, pos_y + 22*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 22*ps - pierna, 3*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 2*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 2*ps);
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 5*ps, pos_y + 26*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 26*ps - pierna, 3*ps, 1*ps);

    // 2 brazos
    painter.setBrush(piel);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - brazo, 2*ps, 7*ps);
    painter.setBrush(celeste);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - brazo, 2*ps, 2*ps);

    // 3 camiseta a rayas con cuello y mangas
    painter.setBrush(celesteSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 9*ps, 10*ps);
    painter.setBrush(celeste);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 4*ps, pos_y + 11*ps - bob, 8*ps, 2*ps);
    painter.drawRect(pos_x + 4*ps, pos_y + 15*ps - bob, 8*ps, 2*ps);
    painter.setBrush(Qt::white);
    painter.drawRect(pos_x + 6*ps, pos_y + 8*ps - bob, 4*ps, 1*ps);

    // 4 numero en el pecho
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 6*ps, pos_y + 9*ps - bob, 3*ps, 1*ps);

    // 5 cara y pelo con mas volumen
    painter.setBrush(piel);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 10*ps, 9*ps);
    painter.setBrush(pelo);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 10*ps, 3*ps);
    painter.drawRect(pos_x + 3*ps, pos_y + 1*ps - bob, 2*ps, 2*ps);
    painter.drawRect(pos_x + 11*ps, pos_y + 1*ps - bob, 2*ps, 2*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.setBrush(piel.darker(110));
    painter.drawRect(pos_x + 6*ps, pos_y + 5*ps - bob, 2*ps, 1*ps);
}

void Sobreviviente::dibujarArgentineArriba(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.2f * ps;
    float brazo = -pierna * 0.6f;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor celeste(130, 180, 230);
    QColor celesteSombra = celeste.darker(115);
    QColor blanco(250, 250, 250);
    QColor shortAzul(30, 40, 90);
    QColor piel(200, 160, 120);
    QColor pelo(40, 30, 20);
    QColor medias(245, 245, 245);
    QColor rayaMedia(30, 40, 90);

    painter.setPen(Qt::NoPen);

    painter.setBrush(medias);
    painter.drawRect(pos_x + 5*ps, pos_y + 20*ps + pierna, 3*ps, 4*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 20*ps - pierna, 3*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 5*ps, pos_y + 22*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 22*ps - pierna, 3*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 2*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 2*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 9*ps - brazo, 2*ps, 7*ps);

    // 1 camiseta con numero y nombre en la espalda
    painter.setBrush(celesteSombra);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 9*ps, 10*ps);
    painter.setBrush(celeste);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 4*ps, pos_y + 11*ps - bob, 9*ps, 2*ps);
    painter.drawRect(pos_x + 4*ps, pos_y + 15*ps - bob, 9*ps, 2*ps);
    painter.drawRect(pos_x + 7*ps, pos_y + 10*ps - bob, 3*ps, 4*ps);

    // 2 pelo de espaldas, mas voluminoso arriba
    painter.setBrush(pelo);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 10*ps, 9*ps);
    painter.setBrush(pelo.lighter(120));
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 8*ps, 3*ps);
}

void Sobreviviente::dibujarArgentineDerecha(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.6f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor celeste(130, 180, 230);
    QColor celesteSombra = celeste.darker(115);
    QColor blanco(250, 250, 250);
    QColor shortAzul(30, 40, 90);
    QColor piel(200, 160, 120);
    QColor pelo(40, 30, 20);
    QColor medias(245, 245, 245);
    QColor rayaMedia(30, 40, 90);

    painter.setPen(Qt::NoPen);

    painter.setBrush(medias);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 20*ps, 4*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 22*ps, 4*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 17*ps, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 2*ps);

    painter.setBrush(medias);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 20*ps, 4*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 22*ps, 4*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 17*ps, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 2*ps);

    painter.setBrush(celeste);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 5*ps, pos_y + 11*ps - bob, 8*ps, 2*ps);
    painter.drawRect(pos_x + 5*ps, pos_y + 15*ps - bob, 8*ps, 2*ps);
    painter.setBrush(celesteSombra);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 2*ps, 10*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 10*ps + pierna*0.4f, pos_y + 9*ps - bob, 3*ps, 7*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 5*ps, pos_y - 1*ps - bob, 7*ps, 8*ps);
    painter.setBrush(pelo);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 6*ps, 4*ps);
    painter.drawRect(pos_x + 3*ps, pos_y + 1*ps - bob, 2*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 10*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
}

void Sobreviviente::dibujarArgentineIzquierda(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.6f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 1.0f * ps;

    QColor celeste(130, 180, 230);
    QColor celesteSombra = celeste.darker(115);
    QColor blanco(250, 250, 250);
    QColor shortAzul(30, 40, 90);
    QColor piel(200, 160, 120);
    QColor pelo(40, 30, 20);
    QColor medias(245, 245, 245);
    QColor rayaMedia(30, 40, 90);

    painter.setPen(Qt::NoPen);

    painter.setBrush(medias);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 20*ps, 4*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 22*ps, 4*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 17*ps, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 2*ps);

    painter.setBrush(medias);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 20*ps, 4*ps, 4*ps);
    painter.setBrush(rayaMedia);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 22*ps, 4*ps, 1*ps);
    painter.setBrush(shortAzul);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 17*ps, 4*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 2*ps);

    painter.setBrush(celeste);
    painter.drawRect(pos_x + 3*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(blanco);
    painter.drawRect(pos_x + 3*ps, pos_y + 11*ps - bob, 8*ps, 2*ps);
    painter.drawRect(pos_x + 3*ps, pos_y + 15*ps - bob, 8*ps, 2*ps);
    painter.setBrush(celesteSombra);
    painter.drawRect(pos_x + 9*ps, pos_y + 8*ps - bob, 2*ps, 10*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 3*ps - pierna*0.4f, pos_y + 9*ps - bob, 3*ps, 7*ps);

    painter.setBrush(piel);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 7*ps, 8*ps);
    painter.setBrush(pelo);
    painter.drawRect(pos_x + 6*ps, pos_y - 1*ps - bob, 6*ps, 4*ps);
    painter.drawRect(pos_x + 11*ps, pos_y + 1*ps - bob, 2*ps, 3*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
}

// ============================================================
// DOOM SLAYER
// armadura pesada verde con detalles metalicos oscuros, peto con
// emblema, hombreras, visor naranja que brilla mas fuerte al frente.
// se mantiene la silueta original, ahora con mucho mas relieve y
// las 4 vistas completas
// ============================================================
void Sobreviviente::dibujarExterminador(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    switch (direccionMovimiento) {
    case SOBREVIVIENTE_DIR_ARRIBA:
        dibujarExterminadorArriba(painter);
        break;
    case SOBREVIVIENTE_DIR_IZQUIERDA:
        dibujarExterminadorIzquierda(painter);
        break;
    case SOBREVIVIENTE_DIR_DERECHA:
        dibujarExterminadorDerecha(painter);
        break;
    default:
        dibujarExterminadorAbajo(painter);
        break;
    }

    painter.restore();
}

void Sobreviviente::dibujarExterminadorAbajo(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 1.6f * ps;
    float brazo = -pierna * 0.5f;
    float bob = std::fabs(std::sin(faseCaminata)) * 0.8f * ps;

    QColor armaduraVerde(50, 70, 45);
    QColor armaduraClara = armaduraVerde.lighter(120);
    QColor armaduraOscura(35, 40, 38);
    QColor visor(220, 80, 20);
    QColor metal(80, 85, 80);
    QColor metalOscuro(45, 48, 45);

    painter.setPen(Qt::NoPen);

    // 1 piernas con placas y bota metalica pesada
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 7*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 7*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 5*ps, pos_y + 18*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 18*ps - pierna, 3*ps, 1*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 3*ps);
    painter.setBrush(metalOscuro);
    painter.drawRect(pos_x + 5*ps, pos_y + 26*ps + pierna, 3*ps, 1*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 26*ps - pierna, 3*ps, 1*ps);

    // 2 brazos con guantelete y protector de antebrazo
    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 7*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 2*ps, pos_y + 15*ps + brazo, 2*ps, 2*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 15*ps - brazo, 2*ps, 2*ps);

    // 3 peto con emblema central y placas laterales
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 9*ps, 10*ps);
    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 7*ps, 10*ps);
    painter.setBrush(armaduraClara);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 2*ps, 10*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 6*ps, pos_y + 11*ps - bob, 4*ps, 4*ps);
    painter.setBrush(visor.darker(130));
    painter.drawRect(pos_x + 7*ps, pos_y + 12*ps - bob, 2*ps, 2*ps);

    // 4 hombreras
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 2*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);
    painter.drawRect(pos_x + 12*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 2*ps, pos_y + 8*ps - bob, 3*ps, 1*ps);
    painter.drawRect(pos_x + 12*ps, pos_y + 8*ps - bob, 3*ps, 1*ps);

    // 5 casco con visor naranja brillante
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 9*ps);
    painter.setBrush(metalOscuro);
    painter.drawRect(pos_x + 3*ps, pos_y + 0*ps - bob, 11*ps, 1*ps);
    painter.setBrush(visor);
    painter.drawRect(pos_x + 5*ps, pos_y + 3*ps - bob, 6*ps, 2*ps);
    painter.setBrush(visor.lighter(140));
    painter.drawRect(pos_x + 6*ps, pos_y + 3*ps - bob, 2*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 4*ps, pos_y + 6*ps - bob, 8*ps, 1*ps);
}

void Sobreviviente::dibujarExterminadorArriba(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 1.6f * ps;
    float brazo = -pierna * 0.5f;
    float bob = std::fabs(std::sin(faseCaminata)) * 0.8f * ps;

    QColor armaduraVerde(50, 70, 45);
    QColor armaduraOscura(35, 40, 38);
    QColor metal(80, 85, 80);
    QColor metalOscuro(45, 48, 45);

    painter.setPen(Qt::NoPen);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 5*ps, pos_y + 17*ps + pierna, 3*ps, 7*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 17*ps - pierna, 3*ps, 7*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 5*ps, pos_y + 24*ps + pierna, 3*ps, 3*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 24*ps - pierna, 3*ps, 3*ps);

    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 2*ps, pos_y + 10*ps + brazo, 2*ps, 7*ps);
    painter.drawRect(pos_x + 13*ps, pos_y + 10*ps - brazo, 2*ps, 7*ps);

    // 1 mochila de soporte vital en la espalda de la armadura
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 9*ps, 10*ps);
    painter.setBrush(metalOscuro);
    painter.drawRect(pos_x + 5*ps, pos_y + 9*ps - bob, 7*ps, 7*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 6*ps, pos_y + 9*ps - bob, 2*ps, 6*ps);
    painter.drawRect(pos_x + 9*ps, pos_y + 9*ps - bob, 2*ps, 6*ps);

    // 2 hombreras vistas de atras
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 2*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);
    painter.drawRect(pos_x + 12*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);

    // 3 casco de espaldas, sin visor
    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 11*ps, 9*ps);
    painter.setBrush(metalOscuro);
    painter.drawRect(pos_x + 7*ps, pos_y - 1*ps - bob, 3*ps, 9*ps);
}

void Sobreviviente::dibujarExterminadorDerecha(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 0.8f * ps;

    QColor armaduraVerde(50, 70, 45);
    QColor armaduraClara = armaduraVerde.lighter(120);
    QColor armaduraOscura(35, 40, 38);
    QColor visor(220, 80, 20);
    QColor metal(80, 85, 80);
    QColor metalOscuro(45, 48, 45);

    painter.setPen(Qt::NoPen);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 2*ps, pos_y + 9*ps - bob, 3*ps, 9*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 3*ps);
    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 3*ps);

    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(armaduraClara);
    painter.drawRect(pos_x + 5*ps, pos_y + 8*ps - bob, 2*ps, 10*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 7*ps, pos_y + 11*ps - bob, 3*ps, 4*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 4*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);

    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 10*ps + pierna*0.4f, pos_y + 10*ps - bob, 3*ps, 7*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 10*ps + pierna*0.4f, pos_y + 15*ps - bob, 3*ps, 2*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 4*ps, pos_y - 1*ps - bob, 9*ps, 9*ps);
    painter.setBrush(visor);
    painter.drawRect(pos_x + 9*ps, pos_y + 3*ps - bob, 4*ps, 2*ps);
    painter.setBrush(visor.lighter(140));
    painter.drawRect(pos_x + 10*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 4*ps, pos_y + 6*ps - bob, 8*ps, 1*ps);
}

void Sobreviviente::dibujarExterminadorIzquierda(QPainter &painter)
{
    int ps = 2;
    float pierna = std::sin(faseCaminata) * 2.0f * ps;
    float bob = std::fabs(std::sin(faseCaminata)) * 0.8f * ps;

    QColor armaduraVerde(50, 70, 45);
    QColor armaduraClara = armaduraVerde.lighter(120);
    QColor armaduraOscura(35, 40, 38);
    QColor visor(220, 80, 20);
    QColor metal(80, 85, 80);
    QColor metalOscuro(45, 48, 45);

    painter.setPen(Qt::NoPen);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 11*ps, pos_y + 9*ps - bob, 3*ps, 9*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps + pierna, pos_y + 24*ps, 4*ps, 3*ps);
    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 18*ps, 4*ps, 6*ps);
    painter.setBrush(Qt::black);
    painter.drawRect(pos_x + 6*ps - pierna, pos_y + 24*ps, 4*ps, 3*ps);

    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 3*ps, pos_y + 8*ps - bob, 8*ps, 10*ps);
    painter.setBrush(armaduraClara);
    painter.drawRect(pos_x + 9*ps, pos_y + 8*ps - bob, 2*ps, 10*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 6*ps, pos_y + 11*ps - bob, 3*ps, 4*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 9*ps, pos_y + 8*ps - bob, 3*ps, 3*ps);

    painter.setBrush(armaduraVerde);
    painter.drawRect(pos_x + 3*ps - pierna*0.4f, pos_y + 10*ps - bob, 3*ps, 7*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 3*ps - pierna*0.4f, pos_y + 15*ps - bob, 3*ps, 2*ps);

    painter.setBrush(armaduraOscura);
    painter.drawRect(pos_x + 3*ps, pos_y - 1*ps - bob, 9*ps, 9*ps);
    painter.setBrush(visor);
    painter.drawRect(pos_x + 3*ps, pos_y + 3*ps - bob, 4*ps, 2*ps);
    painter.setBrush(visor.lighter(140));
    painter.drawRect(pos_x + 5*ps, pos_y + 3*ps - bob, 1*ps, 1*ps);
    painter.setBrush(metal);
    painter.drawRect(pos_x + 4*ps, pos_y + 6*ps - bob, 8*ps, 1*ps);
}
void Sobreviviente::setPosicion(float x, float y)
{
    pos_x = x;
    pos_y = y;
}

void Sobreviviente::actualizarAnimacion(bool seMueve, float dx, float dy) {
    if (seMueve) {
        // 1 elige la direccion dominante para saber que vista mostrar,
        //   si esta quieto se queda mirando para el mismo lado de antes
        if (std::fabs(dx) > std::fabs(dy)) {
            direccionMovimiento = (dx < 0) ? SOBREVIVIENTE_DIR_IZQUIERDA : SOBREVIVIENTE_DIR_DERECHA;
        } else if (dy != 0.0f) {
            direccionMovimiento = (dy < 0) ? SOBREVIVIENTE_DIR_ARRIBA : SOBREVIVIENTE_DIR_ABAJO;
        }

        // 3 ciclo de caminata continuo, reemplaza el rebote triangular
        //   de -2 a 2 que tenia antes por un movimiento tipo seno
        faseCaminata += velocidadAnimacion * 2.4f;
        if (faseCaminata > 2.0f * M_PI) faseCaminata -= 2.0f * M_PI;

        if (animacionSubiendo) frameAnimacion += velocidadAnimacion;
        else frameAnimacion -= velocidadAnimacion;
        if (frameAnimacion > 2.0f) animacionSubiendo = false;
        if (frameAnimacion < -2.0f) animacionSubiendo = true;
    } else {
        frameAnimacion = 0;
        // 4 vuelve a la pose neutra suavemente en vez de cortar de golpe
        faseCaminata *= 0.8f;
        if (std::fabs(faseCaminata) < 0.02f) faseCaminata = 0.0f;
    }
}

void Sobreviviente::actualizar(QPointF mouseWorld)
{
    float centroX = pos_x + ancho / 2.0f;
    float centroY = pos_y + alto / 2.0f;

    if (arma) {
        arma->actualizar(centroX, centroY, mouseWorld);
        arma->actualizarProyectiles();
    }

    // NUEVO: Reducir el temporizador de daño si está activo
    if (timerDanio > 0) {
        timerDanio--;
    }
}

void Sobreviviente::disparar()
{
    if (arma) {
        arma->disparar();
    }
}

void Sobreviviente::cambiarArma(int tipoArma)
{
    if (tipoArma == armaActual) return;
    if (tipoArma < 1 || tipoArma > 7) return;

    // 1 la instancia se crea recien la primera vez que se selecciona
    //   en los cambios siguientes se reutiliza la misma, con las balas que tenia
    if (armasPoseidas[tipoArma] == nullptr) {
        switch(tipoArma) {
        case 1: armasPoseidas[tipoArma] = new Pistola(); break;
        case 2: armasPoseidas[tipoArma] = new Fal(); break;
        case 3: armasPoseidas[tipoArma] = new Granada(); break;
        case 4: armasPoseidas[tipoArma] = new Lanzacohetes(); break;
        case 5: armasPoseidas[tipoArma] = new Escopeta(); break;
        case 6: armasPoseidas[tipoArma] = new Lanzallamas(); break;
        case 7: armasPoseidas[tipoArma] = new LanzaSierras(); break;
        }
    }

    arma = armasPoseidas[tipoArma];
    armaActual = tipoArma;

    if (arma) {
        arma->setMultiplicadorRecarga(stats.velocidadRecarga);
    }
}
void Sobreviviente::reiniciarArmas()
{
    // 1 al empezar de nuevo se descartan todas las armas y se arranca
    //   solo con la pistola, a full de balas
    for (int i = 0; i < 8; i++) {
        delete armasPoseidas[i];
        armasPoseidas[i] = nullptr;
    }

    armasPoseidas[1] = new Pistola();
    arma = armasPoseidas[1];
    armaActual = 1;
}
void Sobreviviente::recibirDanio(float danio)
{
    // Aplicar resistencia a daño
    float danioReal = danio * (1.0f - stats.resistenciaDanio);
    sistemaVida.recibirDanio(danioReal);

    // NUEVO: Activar el efecto visual de daño (15 frames)
    timerDanio = 15;
}

bool Sobreviviente::estaVivo() const
{
    return sistemaVida.estaVivo();
}

Vida& Sobreviviente::getVida()
{
    return sistemaVida;
}

Arma* Sobreviviente::getArma() const
{
    return arma;
}

float Sobreviviente::get_x() { return pos_x; }
float Sobreviviente::get_y() { return pos_y; }
float Sobreviviente::get_ancho() { return ancho; }
float Sobreviviente::get_alto() { return alto; }
