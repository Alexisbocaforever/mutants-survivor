#include "gamelogic.h"
#include "pistola.h"
#include "fal.h"
#include "granada.h"
#include "lanzacohetes.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <QHash>
#include "escopeta.h"
#include "Xhaal.h"
#include "carnicero.h"
#include "mecanicdemon.h"
#include "embistidor.h"
#include "invocador.h"
#include "pulsar.h"
#include <QRandomGenerator>

Gamelogic::Gamelogic() : sobreviviente(800, 600)
{
    srand(time(nullptr));
    QSettings settingsIdioma("MutantsSurvivor", "SaveData");
    idioma = settingsIdioma.value("idioma", IDIOMA_ES).toInt();
    nivelActual = 0;
    puntos = 0;
    estadoActual = EN_DESCANSO;
    timerDescanso = 5.0f;
    tiempoUltimoDanioContacto = 0.0f;

    tieneFal = false;
    tieneGranadas = false;
    tieneLanzacohetes = false;
    tieneLanzallamas = false;
    tieneSierra = false;
    nivelCadenciaSierra = 0;
    nivelTamanioSierra = 0;
    nivelCadenciaEscopeta = 0;

    jefeFinal = nullptr;

    nivelVidaExtra = 0;
    nivelVelocidadExtra = 0;

    nivelRecargaPistola = 0;
    nivelRecargaFal = 0;
    nivelRecargaGranadas = 0;
    nivelRecargaCohetes = 0;

    nivelCapacidadPistola = 0;
    nivelCapacidadFal = 0;
    nivelCapacidadGranadas = 0;
    nivelCapacidadCohetes = 0;

    nivelRangoArmas = 0;
    nivelRangoExplosion = 0;

    expansionDerecha = 0;
    expansionIzquierda = 0;
    expansionArriba = 0;
    expansionAbajo = 0;
    nivelDanioLanzallamas = 0;
    nivelAlcanceLanzallamas = 0;

    tieneEscopeta = false;

    nivelRecargaEscopeta = 0;
    nivelCapacidadEscopeta = 0;

    QPointF spawnInicial = base.getPuntoSpawnInterno();
    sobreviviente.setPosicion(spawnInicial.x(), spawnInicial.y());
    sobreviviente.reiniciarArmas();

    Dibujables.append(&base);

    Tienda* tiendaArmas = new Tienda(base.get_x() + 50, base.get_y() + 50, TIENDA_ARMAS);
    tiendas.append(tiendaArmas);
    Dibujables.append(tiendaArmas);

    Tienda* tiendaVida = new Tienda(base.get_x() + base.get_ancho() - 200, base.get_y() + 50, TIENDA_VIDA);
    tiendas.append(tiendaVida);
    Dibujables.append(tiendaVida);

    Tienda* tiendaChiches = new Tienda(base.get_x() + 50, base.get_y() + base.get_alto() - 180, TIENDA_CHICHES);
    tiendas.append(tiendaChiches);
    Dibujables.append(tiendaChiches);

    Tienda* tiendaExpansion = new Tienda(base.get_x() + base.get_ancho() - 200, base.get_y() + base.get_alto() - 180, TIENDA_EXPANSION);
    tiendas.append(tiendaExpansion);
    Dibujables.append(tiendaExpansion);

    Dibujables.append(&sobreviviente);
    timerTeletransporteGravital = 14.0f;
    mensajeFinalMostrado = false;
    mensajeFinal = "";
    timerMensajeFinal = 0.0f;
    ultimoCheckpoint = 0;
}
bool Gamelogic::juegoFueGanado() const
{
    // 1 lee el flag guardado en el sistema, false si nunca se gano
    QSettings settings("MutantsSurvivor", "SaveData");
    return settings.value("juegoGanado", false).toBool();
}
void Gamelogic::reiniciarJuego()
{
    qDeleteAll(cascarudos); cascarudos.clear();
    qDeleteAll(goliats); goliats.clear();
    qDeleteAll(zombies); zombies.clear();
    qDeleteAll(murcielagos); murcielagos.clear();
    qDeleteAll(mutantes); mutantes.clear();
    qDeleteAll(zombiesArmados); zombiesArmados.clear();
    qDeleteAll(aranosTek); aranosTek.clear();
    qDeleteAll(berserkers); berserkers.clear();
    qDeleteAll(bestiasForja); bestiasForja.clear();
    qDeleteAll(embistidores); embistidores.clear(); // <--- NUEVO
    qDeleteAll(invocadores); invocadores.clear();   // <--- NUEVO
    qDeleteAll(explosiones); explosiones.clear();
    qDeleteAll(zombiesTanque);
    zombiesTanque.clear();
    if(jefeFinal) {
        Dibujables.removeOne(jefeFinal);
        delete jefeFinal;
        jefeFinal = nullptr;
    }

    Dibujables.clear();

    Dibujables.clear();

    // --- APLICACIÓN DE CHECKPOINTS ---
    if (ultimoCheckpoint >= 4 && ultimoCheckpoint < 8) {
        nivelActual = 4 - 1;
        puntos = 6500;
    }
    else if (ultimoCheckpoint >= 8 && ultimoCheckpoint < 12) {
        nivelActual = 8 - 1;
        puntos = 16000;
    }
    else if (ultimoCheckpoint >= 12) {
        nivelActual = 12 - 1; // NUEVO: checkpoint 3, justo antes de la recta final
        puntos = 28000;
    }
    else {
        nivelActual = 0;
        puntos = 0;
    }
    // Forzamos el reinicio directo a la tienda
    estadoActual = EN_DESCANSO;
    timerDescanso = 30.0f; // 30 segs de tranquilidad para comprar todo
    tiempoUltimoDanioContacto = 0.0f;

    tieneFal = false;
    tieneGranadas = false;
    tieneLanzacohetes = false;
    tieneEscopeta = false;
    tieneLanzallamas = false;
    tieneSierra = false;
    nivelCadenciaSierra = 0;
    nivelTamanioSierra = 0;
    nivelCadenciaEscopeta = 0;

    nivelVidaExtra = 0;
    nivelVelocidadExtra = 0;

    nivelRecargaPistola = 0;
    nivelRecargaFal = 0;
    nivelRecargaEscopeta = 0;
    nivelRecargaGranadas = 0;
    nivelRecargaCohetes = 0;

    nivelCapacidadPistola = 0;
    nivelCapacidadFal = 0;
    nivelCapacidadEscopeta = 0;
    nivelCapacidadGranadas = 0;
    nivelCapacidadCohetes = 0;
    nivelDanioLanzallamas = 0;
    nivelAlcanceLanzallamas = 0;
    nivelRangoArmas = 0;
    nivelRangoExplosion = 0;

    expansionDerecha = 0;
    expansionIzquierda = 0;
    expansionArriba = 0;
    expansionAbajo = 0;
    timerTeletransporteGravital = 14.0f;
    mensajeFinalMostrado = false;
    mensajeFinal = "";
    timerMensajeFinal = 0.0f;

    base = BaseMilitar();
    base.establecerNivelNieve(nivelActual); // NUEVO

    qDeleteAll(tiendas);
    tiendas.clear();

    Dibujables.append(&base);

    Tienda* tiendaArmas = new Tienda(base.get_x() + 50, base.get_y() + 50, TIENDA_ARMAS);
    tiendas.append(tiendaArmas);
    Dibujables.append(tiendaArmas);

    Tienda* tiendaVida = new Tienda(base.get_x() + base.get_ancho() - 200, base.get_y() + 50, TIENDA_VIDA);
    tiendas.append(tiendaVida);
    Dibujables.append(tiendaVida);

    Tienda* tiendaChiches = new Tienda(base.get_x() + 50, base.get_y() + base.get_alto() - 180, TIENDA_CHICHES);
    tiendas.append(tiendaChiches);
    Dibujables.append(tiendaChiches);

    Tienda* tiendaExpansion = new Tienda(base.get_x() + base.get_ancho() - 200, base.get_y() + base.get_alto() - 180, TIENDA_EXPANSION);
    tiendas.append(tiendaExpansion);
    Dibujables.append(tiendaExpansion);

    QPointF spawnInicial = base.getPuntoSpawnInterno();
    sobreviviente.setPosicion(spawnInicial.x(), spawnInicial.y());
    sobreviviente.cambiarArma(1);
    sobreviviente.getVida().restaurarCompleta();

    Dibujables.append(&sobreviviente);
}

Gamelogic::~Gamelogic() {
    qDeleteAll(cascarudos);
    qDeleteAll(goliats);
    qDeleteAll(tiendas);
    qDeleteAll(explosiones);
    qDeleteAll(zombies);
    qDeleteAll(murcielagos);
    qDeleteAll(mutantes);
    qDeleteAll(zombiesArmados);
    qDeleteAll(aranosTek);
    qDeleteAll(berserkers);
    qDeleteAll(bestiasForja);
    qDeleteAll(embistidores);
    qDeleteAll(invocadores);
    qDeleteAll(zombiesTanque);
    zombiesTanque.clear();
    if(jefeFinal) delete jefeFinal;

}

void Gamelogic::actualizar() {
    if (!sobreviviente.estaVivo()) return;
    sobreviviente.actualizar(mouseWorldPos);
    actualizarNieve(); // NUEVO
    if (estadoActual == EN_DESCANSO) {
        timerDescanso -= 0.016f;
        if (timerDescanso <= 0) iniciarNivel(nivelActual + 1);
    }
    else if (GestorHordas::esNivelBossMedio(nivelActual)) {
        actualizarBossMedio();
    }
    else if (GestorHordas::esNivelBossFinal(nivelActual)) {
        actualizarBossFinal();
    }
    else {
        SpawnInfo info;
        if (gestorHordas.actualizar(0.016f, info)) {
            float margenExtra = 100.0f;
            float minX = base.get_x() - base.getMargenExterno() - margenExtra;
            float maxX = base.get_x() + base.get_ancho() + base.getMargenExterno() + margenExtra;
            float minY = base.get_y() - base.getMargenExterno() - margenExtra;
            float maxY = base.get_y() + base.get_alto() + base.getMargenExterno() + margenExtra;

            float x = minX + (rand() % (int)(maxX - minX));
            float y = minY + (rand() % (int)(maxY - minY));

            Enemigo* nuevo = nullptr;
            switch(info.tipo) {
            case 1: nuevo = new Cascarudo(x, y); break;
            case 2: nuevo = new Goliat(x, y); break;
            case 3: nuevo = new ZombiePala(x, y); break;
            case 4: nuevo = new Murcielago(x, y); break;
            case 5: nuevo = new ZombieArmado(x, y, 1 + rand()%3); break;
            case 6: nuevo = new Mutante(x, y); break;
            case 7: nuevo = new AranoTek(x, y); break;
            case 8: nuevo = new carnicero(x, y, rand() % 3); break;
            case 9: nuevo = new MecanicDemon(x, y); break;
            case 10: nuevo = new Embistidor(x, y, Embistidor::TOXICO); break;
            case 11: nuevo = new Invocador(x, y); break;
            case 12: nuevo = new ZombieTanque(x, y); break;
            case 13: nuevo = new Pulsar(x, y); break;
            default: nuevo = new Cascarudo(x, y); break;
            }

            if(nuevo) {
                nuevo->multiplicarVelocidad(info.multVel);
                nuevo->multiplicarVida(info.multVida);
                nuevo->multiplicarDanio(info.multDanio);

                if(info.tipo == 1) cascarudos.append((Cascarudo*)nuevo);
                else if(info.tipo == 2) goliats.append((Goliat*)nuevo);
                else if(info.tipo == 3) zombies.append((ZombiePala*)nuevo);
                else if(info.tipo == 4) murcielagos.append((Murcielago*)nuevo);
                else if(info.tipo == 5) zombiesArmados.append((ZombieArmado*)nuevo);
                else if(info.tipo == 6) mutantes.append((Mutante*)nuevo);
                else if(info.tipo == 7) aranosTek.append((AranoTek*)nuevo);
                else if(info.tipo == 8) berserkers.append((carnicero*)nuevo);
                else if(info.tipo == 9) bestiasForja.append((MecanicDemon*)nuevo);
                else if(info.tipo == 10) embistidores.append((Embistidor*)nuevo);
                else if(info.tipo == 11) invocadores.append((Invocador*)nuevo);
                else if(info.tipo == 13) pulsares.append((Pulsar*)nuevo);
                if(info.tipo == 12) {
                    zombiesTanque.append((ZombieTanque*)nuevo);
                }

                Dibujables.append(nuevo);
            }
        }

        bool hordaViva = !gestorHordas.hordaVacia();
        if(!hordaViva) {
            auto chequearVivos = [](auto& lista) -> bool {
                for(auto* e : lista) if(e->estaVivo()) return true;
                return false;
            };

            // En gamelogic.cpp - Dentro de Gamelogic::actualizar()
            if(chequearVivos(cascarudos) || chequearVivos(goliats) || chequearVivos(zombies) ||
                chequearVivos(murcielagos) || chequearVivos(mutantes) || chequearVivos(zombiesArmados) ||
                chequearVivos(aranosTek) || chequearVivos(berserkers) || chequearVivos(bestiasForja) ||
                chequearVivos(embistidores) || chequearVivos(invocadores) ||
                chequearVivos(zombiesTanque) || chequearVivos(pulsares)) { // ¡AÑADIDO AQUÍ!
                hordaViva = true;
            }
        }
        if (!hordaViva) iniciarDescanso();
    }

    auto procesarEnemigos = [&](auto& lista) {
        for(int i = lista.size() - 1; i >= 0; i--) {
            auto* e = lista[i];
            e->actualizar();
            e->decaerQuemado(0.016f);
            if (!e->estaVivo()) {
                if (e->debeDesaparecer()) {
                    Dibujables.removeOne(e);
                    delete e;
                    lista.removeAt(i);
                }
                continue;
            }

            QPointF pJugador(sobreviviente.get_x() + sobreviviente.get_ancho()/2,
                             sobreviviente.get_y() + sobreviviente.get_alto()/2);

            // 1 cada enemigo sabe que hacer con el jugador, ya no hay que
            //   acordarse de sumarlo aca cuando se agrega un enemigo nuevo
            e->actualizarConJugador(pJugador);
        }
    };

    procesarEnemigos(cascarudos);
    procesarEnemigos(goliats);
    procesarEnemigos(zombies);
    procesarEnemigos(murcielagos);
    procesarEnemigos(mutantes);
    procesarEnemigos(zombiesArmados);
    procesarEnemigos(aranosTek);
    procesarEnemigos(berserkers);
    procesarEnemigos(bestiasForja);
    procesarEnemigos(embistidores);
    procesarEnemigos(invocadores);
    // 1 cada invocador vivo puede traer 2 berserkers khorne cada 20 segundos
    for (auto* inv : invocadores) {
        if (!inv->estaVivo()) continue;
        if (inv->consumirDeseoDeInvocarBerserkers()) {
            for (int i = 0; i < INVOCADOR_CANT_BERSERKERS_POR_INVOCACION; i++) {
                float offsetX = (i == 0) ? -40.0f : 40.0f;
                carnicero* nuevoBk = new carnicero(inv->get_x() + offsetX, inv->get_y(), rand() % 3);
                berserkers.append(nuevoBk);
                Dibujables.append(nuevoBk);
            }
        }
    }
    procesarEnemigos(zombiesTanque);
    procesarEnemigos(pulsares);
    verificarColisiones();
    actualizarExplosiones();
    tiempoUltimoDanioContacto += 0.016f;

    if(mensajeFinalMostrado) {
        timerMensajeFinal -= 0.016f;
    }
    if(checkpointVisible) {
        timerCheckpoint -= 0.016f;
        if(timerCheckpoint <= 0.0f) checkpointVisible = false;
    }
}
void Gamelogic::generarParticulaNieve(ParticulaNieve &part)
{
    // 1 nace arriba de la pantalla en una x aleatoria
    part.x = (float)(rand() % 1000) / 1000.0f;
    part.y = -0.05f - (float)(rand() % 200) / 1000.0f;

    // 2 capa cercana o lejana para dar sensacion de profundidad
    part.capa = (rand() % 100 < 40) ? 1 : 0;

    if(part.capa == 1) {
        part.velocidad = 0.28f + (float)(rand() % 100) / 1000.0f;
        part.tam = 3.0f + (float)(rand() % 30) / 10.0f;
    } else {
        part.velocidad = 0.12f + (float)(rand() % 80) / 1000.0f;
        part.tam = 1.5f + (float)(rand() % 15) / 10.0f;
    }

    part.deriva = 0.015f + (float)(rand() % 20) / 1000.0f;
    part.faseDeriva = (float)(rand() % 628) / 100.0f;
}

void Gamelogic::actualizarNieve()
{
    if(nivelActual < 1) {
        particulasNieve.clear();
        return;
    }

    int nivelClamp = (nivelActual > NIVEL_BOSS_FINAL) ? NIVEL_BOSS_FINAL : nivelActual; // ANTES: hardcodeado en 10
    int maximoParticulas = 40 + nivelClamp * 25;

    while(particulasNieve.size() < maximoParticulas) {
        ParticulaNieve nueva;
        generarParticulaNieve(nueva);
        particulasNieve.append(nueva);
    }
    while(particulasNieve.size() > maximoParticulas) {
        particulasNieve.removeLast();
    }

    for(int i = 0; i < particulasNieve.size(); i++) {
        ParticulaNieve &part = particulasNieve[i];
        part.faseDeriva += 0.03f;
        part.y += part.velocidad * 0.016f;
        part.x += std::sin(part.faseDeriva) * part.deriva * 0.016f;
        if(part.y > 1.05f || part.x < -0.05f || part.x > 1.05f) {
            generarParticulaNieve(part);
        }
    }
}

void Gamelogic::verificarColisionesProyectilesBoss()
{
    if(jefeFinal && jefeFinal->estaVivo()) {
        QList<Proyectil*>& proyectiles = jefeFinal->getProyectiles();
        for(int i = proyectiles.size() - 1; i >= 0; i--) {
            Proyectil* p = proyectiles[i];
            if(!p->estaActivo()) continue;
            if(Colision::hayColision(p, &sobreviviente)) {
                sobreviviente.recibirDanio(p->getDanio());
                p->desactivar();
            }
        }

        QList<ProyectilBuscador*>& buscadores = jefeFinal->getProyectilesBuscadores();
        for(int i = buscadores.size() - 1; i >= 0; i--) {
            ProyectilBuscador* pb = buscadores[i];
            if(!pb->estaActivo()) continue;
            if(Colision::hayColision(pb, &sobreviviente)) {
                sobreviviente.recibirDanio(pb->getDanio());
                pb->desactivar();
            }
        }
    }
    if (!gravitals.isEmpty() && gravitals.first()->estaVivo()) {
        float jx = sobreviviente.get_x() + sobreviviente.get_ancho() / 2.0f;
        float jy = sobreviviente.get_y() + sobreviviente.get_alto() / 2.0f;
        float radioJugador = sobreviviente.get_ancho() / 2.0f;

        for (RayoLaser* r : gravitals.first()->getRayosLaser()) {
            if (r->estaHaciendoDanio() && r->colisionaCon(jx, jy, radioJugador)) {
                sobreviviente.recibirDanio(r->getDanio());
            }
        }
        for (ProyectilCurvo* c : gravitals.first()->getProyectilesCurvos()) {
            if (c->estaActivo() && Colision::hayColision(c, &sobreviviente)) {
                sobreviviente.recibirDanio(c->getDanio());
                c->desactivar();
            }
        }
    }
    for(auto* zt : zombiesTanque) {
        if(!zt->estaVivo()) continue;
        auto& misiles = zt->getMisiles();
        for(int i = misiles.size() - 1; i >= 0; i--) {
            ProyectilBuscador* pb = misiles[i];
            if(!pb->estaActivo()) continue;
            if(Colision::hayColision(pb, &sobreviviente)) {
                sobreviviente.recibirDanio(pb->getDanio());
                pb->desactivar();
            }
        }
        if (zt->getRayoLaser().estaHaciendoDanio()) {
            float jx = sobreviviente.get_x() + sobreviviente.get_ancho() / 2.0f;
            float jy = sobreviviente.get_y() + sobreviviente.get_alto() / 2.0f;
            float radioJugador = sobreviviente.get_ancho() / 2.0f;
            if (zt->getRayoLaser().colisionaCon(jx, jy, radioJugador)) {
                sobreviviente.recibirDanio(zt->getRayoLaser().getDanio());
            }
        }
        // NUEVO: dano de la llamarada del tanque
        if(zt->estaLanzandoLlamas()) {
            float jx = sobreviviente.get_x() + sobreviviente.get_ancho() / 2.0f;
            float jy = sobreviviente.get_y() + sobreviviente.get_alto() / 2.0f;
            for(const QPointF& part : zt->getAreaLlamarada()) {
                float dx = jx - part.x();
                float dy = jy - part.y();
                if(std::sqrt(dx * dx + dy * dy) < 14.0f + sobreviviente.get_ancho() / 2.0f) {
                    sobreviviente.recibirDanio(zt->getDanioLlamarada());
                    break;
                }
            }
        }

    }
}
Tienda* Gamelogic::getTiendaCercana()
{
    float centroX = sobreviviente.get_x() + sobreviviente.get_ancho() / 2.0f;
    float centroY = sobreviviente.get_y() + sobreviviente.get_alto() / 2.0f;

    for(Tienda* t : tiendas) {
        if(t->estaEnRango(centroX, centroY)) {
            return t;
        }
    }
    return nullptr;
}

void Gamelogic::comprarItem(int itemId)
{
    Tienda* tienda = getTiendaCercana();
    if(!tienda || estadoActual != EN_DESCANSO) return;

    int precio = calcularPrecio(tienda->getTipo(), itemId);
    if(puntos < precio) return;

    bool compraExitosa = false;

    if(tienda->getTipo() == TIENDA_ARMAS) {
        switch(itemId) {
        case 1:
            if(!tieneFal) {
                tieneFal = true;
                sobreviviente.cambiarArma(2);
                compraExitosa = true;
            }
            break;
        case 2:
            if(!tieneEscopeta) {
                tieneEscopeta = true;
                sobreviviente.cambiarArma(5);
                compraExitosa = true;
            }
            break;
        case 3:
            if(!tieneLanzallamas) {
                tieneLanzallamas = true;
                sobreviviente.cambiarArma(6);
                compraExitosa = true;
            }
            break;
        case 4:
            if(!tieneGranadas) {
                tieneGranadas = true;
                sobreviviente.cambiarArma(3);
                compraExitosa = true;
            }
            break;
        case 5:
            if(!tieneLanzacohetes) {
                tieneLanzacohetes = true;
                sobreviviente.cambiarArma(4);
                compraExitosa = true;
            }
            break;
        case 6:
            if(!tieneSierra) {
                tieneSierra = true;
                sobreviviente.cambiarArma(7);
                compraExitosa = true;
            }
        }
    }
    else if(tienda->getTipo() == TIENDA_VIDA) {
        switch(itemId) {
        case 1:
            nivelVidaExtra++;
            {
                float nuevaMax = sobreviviente.getVida().getVidaMaxima() + 75.0f;
                sobreviviente.getVida().setVidaMaxima(nuevaMax);
                sobreviviente.getVida().curar(50.0f);
            }
            compraExitosa = true;
            break;

        case 2:
            if(nivelVelocidadExtra < 15) {
                nivelVelocidadExtra++;
                compraExitosa = true;
            }
            break;

        case 3:
            if(sobreviviente.getVida().getVidaActual() < sobreviviente.getVida().getVidaMaxima()) {
                sobreviviente.getVida().curar(sobreviviente.getVida().getVidaMaxima() * 0.5f);
                compraExitosa = true;
            }
            break;
        }
    }
    else if(tienda->getTipo() == TIENDA_CHICHES) {
        int nivelMaximo = 5;

        switch(itemId) {
        case 1: if(nivelRecargaPistola < nivelMaximo) { nivelRecargaPistola++; compraExitosa = true; } break;
        case 2: if(tieneFal && nivelRecargaFal < nivelMaximo) { nivelRecargaFal++; compraExitosa = true; } break;
        case 3: if(tieneEscopeta && nivelRecargaEscopeta < nivelMaximo) { nivelRecargaEscopeta++; compraExitosa = true; } break;
        case 4: if(tieneGranadas && nivelRecargaGranadas < nivelMaximo) { nivelRecargaGranadas++; compraExitosa = true; } break;
        case 5: if(tieneLanzacohetes && nivelRecargaCohetes < nivelMaximo) { nivelRecargaCohetes++; compraExitosa = true; } break;
        case 6: if(nivelCapacidadPistola < nivelMaximo) { nivelCapacidadPistola++; compraExitosa = true; } break;
        case 7: if(tieneFal && nivelCapacidadFal < nivelMaximo) { nivelCapacidadFal++; compraExitosa = true; } break;
        case 8: if(tieneEscopeta && nivelCapacidadEscopeta < nivelMaximo) { nivelCapacidadEscopeta++; compraExitosa = true; } break;
        case 9: if(tieneGranadas && nivelCapacidadGranadas < nivelMaximo) { nivelCapacidadGranadas++; compraExitosa = true; } break;
        case 10: if(tieneLanzacohetes && nivelCapacidadCohetes < nivelMaximo) { nivelCapacidadCohetes++; compraExitosa = true; } break;
        case 11: if(nivelRangoArmas < nivelMaximo) { nivelRangoArmas++; compraExitosa = true; } break;
        case 12: if((tieneGranadas || tieneLanzacohetes) && nivelRangoExplosion < nivelMaximo) { nivelRangoExplosion++; compraExitosa = true; } break;
        case 13: if(tieneLanzallamas && nivelDanioLanzallamas < nivelMaximo) { nivelDanioLanzallamas++; compraExitosa = true; } break;
        case 14: if(tieneLanzallamas && nivelAlcanceLanzallamas < nivelMaximo) { nivelAlcanceLanzallamas++; compraExitosa = true; } break;
        case 15: if(tieneSierra && nivelCadenciaSierra < nivelMaximo) { nivelCadenciaSierra++; compraExitosa = true; } break;
        case 16: if(tieneSierra && nivelTamanioSierra < nivelMaximo) { nivelTamanioSierra++; compraExitosa = true; } break;
        case 17: if(tieneEscopeta && nivelCadenciaEscopeta < nivelMaximo) { nivelCadenciaEscopeta++; compraExitosa = true; } break;
        }
    }
    else if(tienda->getTipo() == TIENDA_EXPANSION) {
        int expansionMax = EXPANSION_MAX;

        switch(itemId) {
        case 1:
            if(expansionDerecha < expansionMax) {
                expansionDerecha++;
                base.expandirDerecha(150.0f);
                compraExitosa = true;
            }
            break;
        case 2:
            if(expansionIzquierda < expansionMax) {
                expansionIzquierda++;
                base.expandirIzquierda(150.0f);
                compraExitosa = true;
            }
            break;
        case 3:
            if(expansionArriba < expansionMax) {
                expansionArriba++;
                base.expandirArriba(150.0f);
                compraExitosa = true;
            }
            break;
        case 4:
            if(expansionAbajo < expansionMax) {
                expansionAbajo++;
                base.expandirAbajo(150.0f);
                compraExitosa = true;
            }
            break;
        }
    }

    if (compraExitosa) {
        puntos -= precio;
        aplicarMejorasArmas();
    }
}

bool Gamelogic::tieneArma(int tipo) const
{
    switch(tipo) {
    case 2: return tieneFal;
    case 3: return tieneGranadas;
    case 4: return tieneLanzacohetes;
    case 5: return tieneEscopeta;
    case 6: return tieneLanzallamas;
    case 7: return tieneSierra;
    default: return false;
    }
}

QString Gamelogic::getNombreArmaActual() const
{
    if(!sobreviviente.getArma()) return "NINGUNA";

    Arma* arma = sobreviviente.getArma();
    if(dynamic_cast<Pistola*>(arma)) return "PISTOLA";
    if(dynamic_cast<Fal*>(arma)) return "FAL";
    if(dynamic_cast<Escopeta*>(arma)) return "ESCOPETA";
    if(dynamic_cast<Granada*>(arma)) return "GRANADAS";
    if(dynamic_cast<Lanzacohetes*>(arma)) return "LANZACOHETES";
    if(dynamic_cast<Lanzallamas*>(arma)) return "LANZALLAMAS";
    if(dynamic_cast<LanzaSierras*>(arma)) return "LANZASIERRAS";
    return "DESCONOCIDA";
}

void Gamelogic::setSkinSobreviviente(int skin)
{
    sobreviviente.setSkin(skin);
}

void Gamelogic::iniciarNivel(int nivel) {
    nivelActual = nivel;
    estadoActual = EN_HORDA;
    base.establecerNivelNieve(nivel); // NUEVO


    if (nivel == 1) {
        // Aparece a la derecha, fuera de la base
        float colX = base.get_x() + base.get_ancho() + 400.0f;
        float colY = base.get_y() + base.get_alto() / 2.0f - 100.0f;


    }

    if (GestorHordas::esNivelBossMedio(nivel)) {
        qDeleteAll(gravitals);
        gravitals.clear();

        // Spawn en el medio exacto de la base
        float x = base.get_x() + base.get_ancho() / 2.0f - 80.0f;
        float y = base.get_y() + base.get_alto() / 2.0f - 80.0f;
        gravitals.append(new Gravital(x, y));
        Dibujables.append(gravitals.first());

        gestorHordas.limpiar();
    }
    else if (GestorHordas::esNivelBossFinal(nivel)) {
        if(jefeFinal) {
            Dibujables.removeOne(jefeFinal);
            delete jefeFinal;
            jefeFinal = nullptr;
        }

        float baseX = base.get_x();
        float baseY = base.get_y();
        float x = baseX + base.get_ancho() + 200.0f;
        float y = baseY - 200.0f;

        jefeFinal = new Xhaal(x, y);
        Dibujables.append(jefeFinal);
    }
    else {
        gestorHordas.iniciarNivel(nivel);
    }

    QPointF spawnExt = base.getPuntoSpawnExterno();
    sobreviviente.setPosicion(spawnExt.x(), spawnExt.y());
}

void Gamelogic::iniciarDescanso() {
    estadoActual = EN_DESCANSO;
    timerDescanso = 30.0f;

    if (GestorHordas::esNivelCheckpoint(nivelActual)) {
        sobreviviente.getVida().restaurarCompleta();
        puntos += 500 * (nivelActual / 4);
        timerDescanso = 45.0f;
        ultimoCheckpoint = nivelActual;
        checkpointVisible = true;
        checkpointTexto = T(idioma, "PUNTO DE CONTROL - Nivel %1 superado", "CHECKPOINT - Level %1 cleared").arg(nivelActual);
        timerCheckpoint = 4.0f;
    }

    QPointF spawnInt = base.getPuntoSpawnInterno();
    sobreviviente.setPosicion(spawnInt.x(), spawnInt.y());
}
void Gamelogic::aplicarExpansionEscenario() {}

void Gamelogic::verificarColisionesProyectilesEnemigos()
{ // Proyectiles buscadores del Invocador
    for (auto* inv : invocadores) {
        if (!inv->estaVivo()) continue;
        for (auto it = inv->getProyectiles().begin(); it != inv->getProyectiles().end(); ) {
            if (!it->estaActivo()) {
                ++it;
                continue;
            }
            if (Colision::hayColision(&(*it), &sobreviviente)) {
                sobreviviente.recibirDanio(it->getDanio());
                it->desactivar();   // o marcar como inactivo
            }
            ++it;
        }
    }
    for(auto za : zombiesArmados) {
        if(!za->estaVivo()) continue;
        QList<Proyectil*>& proyectiles = za->getProyectiles();

        for(int i = proyectiles.size() - 1; i >= 0; i--) {
            Proyectil* p = proyectiles[i];
            if(!p->estaActivo()) continue;

            if(Colision::hayColision(p, &sobreviviente)) {
                if(p->esExplosivo()) {
                    crearExplosionEnemiga(p->get_x(), p->get_y(),
                                          p->getRadioExplosion(), p->getDanio());
                } else {
                    sobreviviente.recibirDanio(p->getDanio());
                }
                p->desactivar();
            }
        }
    }

    for(auto at : aranosTek) {
        if(!at->estaVivo()) continue;
        QList<Proyectil*>& proyectiles = at->getProyectiles();

        for(int i = proyectiles.size() - 1; i >= 0; i--) {
            Proyectil* p = proyectiles[i];
            if(!p->estaActivo()) continue;

            if(Colision::hayColision(p, &sobreviviente)) {
                if(p->esExplosivo()) {
                    crearExplosionEnemiga(p->get_x(), p->get_y(),
                                          p->getRadioExplosion(), p->getDanio());
                } else {
                    sobreviviente.recibirDanio(p->getDanio());
                }
                p->desactivar();
            }
        }
    }
    for(auto* zt : zombiesTanque) {
        if(!zt->estaVivo()) continue;
        auto& misiles = zt->getMisiles();
        for(int i = misiles.size() - 1; i >= 0; i--) {
            ProyectilBuscador* pb = misiles[i];
            if(!pb->estaActivo()) continue;
            if(Colision::hayColision(pb, &sobreviviente)) {
                sobreviviente.recibirDanio(pb->getDanio());
                pb->desactivar();
            }
        }
    }
}

void Gamelogic::crearExplosionEnemiga(float x, float y, float radio, float danio)
{
    Explosivo* exp = new Explosivo(x, y, radio);
    explosiones.append(exp);
    Dibujables.append(exp);

    float jugadorCentroX = sobreviviente.get_x() + sobreviviente.get_ancho() / 2.0f;
    float jugadorCentroY = sobreviviente.get_y() + sobreviviente.get_alto() / 2.0f;
    float dx = jugadorCentroX - x;
    float dy = jugadorCentroY - y;
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia <= radio) {
        sobreviviente.recibirDanio(danio);
    }
}

void Gamelogic::mover(int dx, int dy) {
    if (!sobreviviente.estaVivo()) return;
    float oldX = sobreviviente.get_x(), oldY = sobreviviente.get_y();

    float velocidadBase = 5.0f * sobreviviente.getVelocidadMovimiento();
    float velocidadTotal = velocidadBase * (1.0f + (nivelVelocidadExtra * 0.12f));
    sobreviviente.setPosicion(oldX + dx * velocidadTotal, oldY + dy * velocidadTotal);

    bool colision = false;

    for(auto p : base.obtenerParedes()) {
        if(Colision::hayColision(&sobreviviente, p)) {
            colision = true;
            break;
        }
    }

    if(colision) sobreviviente.setPosicion(oldX, oldY);
    sobreviviente.actualizarAnimacion(dx!=0||dy!=0, (float)dx, (float)dy);
}

void Gamelogic::verificarColisionesProyectiles() {
    if (!sobreviviente.getArma()) return;

    sistemaColisiones.limpiar();
    auto registrar = [&](auto& lista) {
        for(auto* e : lista) if(e->estaVivo()) sistemaColisiones.insertar(e);
    };
    registrar(cascarudos);
    registrar(goliats);
    registrar(zombies);
    registrar(murcielagos);
    registrar(mutantes);
    registrar(zombiesArmados);
    registrar(aranosTek);
    registrar(berserkers);
    registrar(bestiasForja);
    registrar(embistidores);
    registrar(invocadores);
    registrar(zombiesTanque);
    registrar(pulsares);

    if(jefeFinal && jefeFinal->estaVivo()) sistemaColisiones.insertar(jefeFinal);
    if(!gravitals.isEmpty() && gravitals.first()->estaVivo()) sistemaColisiones.insertar(gravitals.first());

    QList<Proyectil*>& proyectiles = sobreviviente.getArma()->getProyectiles();

    for (Proyectil* p : proyectiles) {
        if (!p->estaActivo()) continue;

        for(Dibujable_abstracto* candidato : sistemaColisiones.obtenerVecinos(p)) {
            if(!Colision::hayColision(p, candidato)) continue;

            Enemigo* enemigo = dynamic_cast<Enemigo*>(candidato);
            if(!enemigo || !enemigo->estaVivo()) continue;

            if(auto* grav = dynamic_cast<Gravital*>(candidato)) {
                if(!grav->impactoEsVulnerable(p->get_x(), p->get_y())) {
                    p->desactivar();
                    break;
                }
            }

            if(p->esExplosivo()) {
                crearExplosion(p->get_x(), p->get_y(), p->getRadioExplosion(), p->getDanio());
            } else {
                enemigo->recibirDanio(p->getDanio());
                if(!enemigo->estaVivo()) puntos += puntosPorMuerte(enemigo);
            }
            p->desactivar();
            break;
        }
    }
}

void Gamelogic::aplicarMejorasArmas()
{
    Arma* armaActual = sobreviviente.getArma();
    if(!armaActual) return;

    if(dynamic_cast<Pistola*>(armaActual)) {
        float mejoraRecarga = 1.0f - (nivelRecargaPistola * 0.12f);
        armaActual->setMultiplicadorRecarga(mejoraRecarga);
        int capacidadBase = 15;
        int capacidadTotal = capacidadBase + (nivelCapacidadPistola * 4);
        armaActual->setMunicionMaxima(capacidadTotal);
    }
    else if(dynamic_cast<Fal*>(armaActual)) {
        float mejoraRecarga = 1.0f - (nivelRecargaFal * 0.12f);
        armaActual->setMultiplicadorRecarga(mejoraRecarga);
        int capacidadBase = 30;
        int capacidadTotal = capacidadBase + (nivelCapacidadFal * 6);
        armaActual->setMunicionMaxima(capacidadTotal);
    }
    else if(auto* escopeta = dynamic_cast<Escopeta*>(armaActual)) {
        float mejoraRecarga = 1.0f - (nivelRecargaEscopeta * 0.12f);
        armaActual->setMultiplicadorRecarga(mejoraRecarga);
        int capacidadBase = 8;
        int capacidadTotal = capacidadBase + (nivelCapacidadEscopeta * 2);
        armaActual->setMunicionMaxima(capacidadTotal);
        escopeta->setNivelCadenciaEscopeta(nivelCadenciaEscopeta); // NUEVO
    }
    else if(dynamic_cast<Granada*>(armaActual)) {
        float mejoraRecarga = 1.0f - (nivelRecargaGranadas * 0.12f);
        armaActual->setMultiplicadorRecarga(mejoraRecarga);
        int capacidadBase = 6;
        int capacidadTotal = capacidadBase + (nivelCapacidadGranadas * 2);
        armaActual->setMunicionMaxima(capacidadTotal);
        float mejoraRadio = 1.0f + (nivelRangoExplosion * 0.20f);
        armaActual->setMultiplicadorRadioExplosion(mejoraRadio);
    }
    else if(dynamic_cast<Lanzacohetes*>(armaActual)) {
        float mejoraRecarga = 1.0f - (nivelRecargaCohetes * 0.12f);
        armaActual->setMultiplicadorRecarga(mejoraRecarga);
        int capacidadBase = 3;
        int capacidadTotal = capacidadBase + (nivelCapacidadCohetes * 2);
        armaActual->setMunicionMaxima(capacidadTotal);
        float mejoraRadio = 1.0f + (nivelRangoExplosion * 0.20f);
        armaActual->setMultiplicadorRadioExplosion(mejoraRadio);
    }
    // gamelogic.cpp, en aplicarMejorasArmas(), rama de Lanzallamas corregida:
    else if(auto* lanza = dynamic_cast<Lanzallamas*>(armaActual)) {
        float danioBase = 8.0f; // antes decia 3.0f, no coincidia con el constructor
        lanza->setDanioTick(danioBase + (nivelDanioLanzallamas * 1.0f));
        lanza->setNivelDanioLanzallamas(nivelDanioLanzallamas); // NUEVO

        float alcanceBase = 200.0f; // antes decia 220.0f, tampoco coincidia
        lanza->setAlcanceMaximo(alcanceBase + (nivelAlcanceLanzallamas * 40.0f));
        lanza->setNivelAlcanceLanzallamas(nivelAlcanceLanzallamas); // NUEVO
    }
    else if(auto* sierra = dynamic_cast<LanzaSierras*>(armaActual)) {
        sierra->setDanio(35.0f);
        sierra->setNivelCadenciaSierra(nivelCadenciaSierra);
        sierra->setNivelTamanioSierra(nivelTamanioSierra);
    }
    float mejoraRango = 1.0f + (nivelRangoArmas * 0.15f);
    armaActual->setMultiplicadorRango(mejoraRango);
}

void Gamelogic::verificarColisionesEnemigos() {
    // 1 acumular el empuje de separacion por enemigo
    QHash<Enemigo*, QPointF> empujeAcumulado;

    auto acumularLista = [&](auto& lista) {
        for (auto* e : lista) {
            if (!e || !e->estaVivo()) continue;

            for (Dibujable_abstracto* candidato : sistemaColisiones.obtenerVecinos(e)) {
                if (candidato == e) continue;

                Enemigo* otro = dynamic_cast<Enemigo*>(candidato);
                if (!otro || !otro->estaVivo()) continue;

                // 2 Evitar doble cálculo verificando los punteros de memoria (usando std::less por seguridad en C++)
                if (std::less<const void*>()(e, otro)) continue;

                if (!Colision::hayColision(e, otro)) continue;

                // 3 Calcular el vector entre los centros
                float cxA = e->get_x() + e->get_ancho() / 2.0f;
                float cyA = e->get_y() + e->get_alto() / 2.0f;
                float cxB = otro->get_x() + otro->get_ancho() / 2.0f;
                float cyB = otro->get_y() + otro->get_alto() / 2.0f;

                float dx = cxA - cxB;
                float dy = cyA - cyB;
                float dist = std::sqrt(dx * dx + dy * dy);

                // Evitar superposición exacta (que daría dist = 0)
                if (dist < 0.01f) {
                    dx = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    dy = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    dist = std::sqrt(dx * dx + dy * dy); // Recalcular la hipotenusa real
                }

                // 4 Empujar solo si están muy cerca
                float solapeMinimo = (e->get_ancho() + otro->get_ancho()) * 0.35f;
                if (dist >= solapeMinimo) continue;

                float empuje = (solapeMinimo - dist) * 0.2f; // Coeficiente elástico
                float nx = dx / dist;
                float ny = dy / dist;

                empujeAcumulado[e] += QPointF(nx * empuje, ny * empuje);
                empujeAcumulado[otro] += QPointF(-nx * empuje, -ny * empuje);
            }
        }
    };

    // Procesar listas de enemigos
    acumularLista(cascarudos);
    acumularLista(goliats);
    acumularLista(zombies);
    acumularLista(murcielagos);
    acumularLista(mutantes);
    acumularLista(zombiesArmados);
    acumularLista(aranosTek);
    acumularLista(berserkers);
    acumularLista(bestiasForja);
    acumularLista(embistidores);
    acumularLista(invocadores);
    acumularLista(zombiesTanque);
    acumularLista(pulsares);

    // 5 Aplicar empuje total con límite de velocidad y PREVENCIÓN DE NaN
    const float empujeMaximoTotal = 1.4f;
    for (auto it = empujeAcumulado.begin(); it != empujeAcumulado.end(); ++it) {
        QPointF total = it.value();
        float mag = std::sqrt(total.x() * total.x() + total.y() * total.y());

        // CORRECCIÓN CRÍTICA: Prevenir división por cero si las fuerzas se cancelan (mag == 0)
        if (mag > 0.0001f) {
            if (mag > empujeMaximoTotal) {
                total *= (empujeMaximoTotal / mag);
            }
            it.key()->mover(total.x(), total.y());
        }
    }
}

void Gamelogic::verificarColisiones() {
    verificarColisionesProyectiles();
    verificarColisionesProyectilesEnemigos();
    verificarColisionesProyectilesBoss();
    verificarColisionesLanzallamas();
    verificarColisionesSierras();
    verificarColisionesEnemigos();
    verificarColisionConSobreviviente();
}

void Gamelogic::verificarColisionConSobreviviente() {
    if (tiempoUltimoDanioContacto < 0.5f) return;

    // <--- NUEVO: Daño por contacto de Embistidor --->
    for(auto emb : embistidores) {
        if(emb->estaVivo() && Colision::hayColision(&sobreviviente, emb)) {
            sobreviviente.recibirDanio(emb->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    // <--- NUEVO: Daño por contacto de Invocador --->
    for(auto inv : invocadores) {
        if(inv->estaVivo() && Colision::hayColision(&sobreviviente, inv)) {
            sobreviviente.recibirDanio(inv->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto c : cascarudos) {
        if(c->estaVivo() && Colision::hayColision(&sobreviviente, c)) {
            sobreviviente.recibirDanio(c->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto bf : bestiasForja) {
        if(bf->estaVivo() && Colision::hayColision(&sobreviviente, bf)) {
            sobreviviente.recibirDanio(bf->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto g : goliats) {
        if(g->estaVivo() && Colision::hayColision(&sobreviviente, g)) {
            sobreviviente.recibirDanio(g->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto z : zombies) {
        if(z->estaVivo() && Colision::hayColision(&sobreviviente, z)) {
            sobreviviente.recibirDanio(z->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto m : murcielagos) {
        if(m->estaVivo() && Colision::hayColision(&sobreviviente, m)) {
            sobreviviente.recibirDanio(m->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto mut : mutantes) {
        if(mut->estaVivo() && Colision::hayColision(&sobreviviente, mut)) {
            sobreviviente.recibirDanio(mut->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto za : zombiesArmados) {
        if(za->estaVivo() && Colision::hayColision(&sobreviviente, za)) {
            sobreviviente.recibirDanio(za->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for(auto at : aranosTek) {
        if(at->estaVivo() && Colision::hayColision(&sobreviviente, at)) {
            sobreviviente.recibirDanio(at->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    for (auto bk : berserkers) {
        if (bk->estaVivo() && Colision::hayColision(&sobreviviente, bk)) {
            sobreviviente.recibirDanio(bk->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }

    if(!gravitals.isEmpty() && gravitals.first()->estaVivo() &&
        Colision::hayColision(&sobreviviente, gravitals.first())) {
        sobreviviente.recibirDanio(gravitals.first()->getDanioContacto());
        tiempoUltimoDanioContacto = 0;
        return;
    }

    if(jefeFinal && jefeFinal->estaVivo() && Colision::hayColision(&sobreviviente, jefeFinal)) {
        sobreviviente.recibirDanio(jefeFinal->getDanioContacto());
        tiempoUltimoDanioContacto = 0;
        return;
    }
    for(auto* zt : zombiesTanque) {
        if(zt->estaVivo() && Colision::hayColision(&sobreviviente, zt)) {
            sobreviviente.recibirDanio(zt->getDanioContacto());
            tiempoUltimoDanioContacto = 0;
            return;
        }
    }
    for (auto* pul : pulsares) {
        if (!pul->estaVivo()) continue;

        // Usamos & para tomar la dirección de la onda
        OndaExpansiva& onda = pul->getOnda();
        if (onda.estaActivo()) {
            if (onda.colisionaCon(sobreviviente.get_x(), sobreviviente.get_y(), 20.0f)) {
                sobreviviente.recibirDanio(15.0f);
            }
        }
    }
    for (auto* g : gravitals) {
        if (!g->estaVivo()) continue;

        for (auto* p : g->getProyectiles()) {
            if (p->estaActivo() && Colision::hayColision(&sobreviviente, p)) {
                sobreviviente.recibirDanio(20.0f); // Modifica el daño a tu gusto
                p->desactivar(); // Hace que la bala desaparezca tras golpearte
            }
        }
        for (auto* r : g->getRayosLaser()) {
            if (r->estaActivo() && Colision::hayColision(&sobreviviente, r)) {
                sobreviviente.recibirDanio(35.0f);
                r->desactivar();
            }
        }
        for (auto* c : g->getProyectilesCurvos()) {
            if (c->estaActivo() && Colision::hayColision(&sobreviviente, c)) {
                sobreviviente.recibirDanio(25.0f);
                c->desactivar();
            }
        }
    }
}

void Gamelogic::crearExplosion(float x, float y, float radio, float danio)
{
    Explosivo* exp = new Explosivo(x, y, radio);
    explosiones.append(exp);
    Dibujables.append(exp);

    for(Dibujable_abstracto* candidato : sistemaColisiones.obtenerEnRadio(x, y, radio)) {
        Enemigo* enemigo = dynamic_cast<Enemigo*>(candidato);
        if(!enemigo || !enemigo->estaVivo()) continue;

        float cx = enemigo->get_x() + enemigo->get_ancho() / 2.0f;
        float cy = enemigo->get_y() + enemigo->get_alto() / 2.0f;
        float dx = cx - x, dy = cy - y;
        if(std::sqrt(dx*dx + dy*dy) > radio) continue;

        if(auto* grav = dynamic_cast<Gravital*>(candidato)) {
            if(!grav->impactoEsVulnerable(x, y)) continue;
        }

        enemigo->recibirDanio(danio);
        if(!enemigo->estaVivo()) puntos += puntosPorMuerte(enemigo);
    }
}

void Gamelogic::actualizarExplosiones()
{
    for(int i = explosiones.size() - 1; i >= 0; i--) {
        explosiones[i]->actualizar();
        if(explosiones[i]->terminada()) {
            Dibujables.removeOne(explosiones[i]);
            delete explosiones[i];
            explosiones.removeAt(i);
        }
    }
}

bool Gamelogic::juegoTerminado() const { return !sobreviviente.estaVivo(); }
void Gamelogic::disparar() { sobreviviente.disparar(); }
void Gamelogic::cambiarArma(int t)
{
    sobreviviente.cambiarArma(t);
    aplicarMejorasArmas();
}
void Gamelogic::setMouseWorldPos(QPointF p) { mouseWorldPos = p; }
QList<Dibujable_abstracto*> &Gamelogic::Obtener_Dibujables() { return Dibujables; }

void Gamelogic::actualizarBossMedio()
{
    if(gravitals.isEmpty()) { iniciarDescanso(); return; }

    // 1 el rango de ataque se adapta al tamanio actual del mapa expandido
    float factorMapa = base.getMargenExterno() / 200.0f;
    if (factorMapa < 1.0f) factorMapa = 1.0f;

    for (int i = gravitals.size() - 1; i >= 0; i--) {
        Gravital* boss = gravitals[i];
        boss->setFactorMapa(factorMapa);
        boss->actualizar();

        if(!boss->estaVivo()) {
            Dibujables.removeOne(boss);
            delete boss;
            gravitals.removeAt(i);

            if (gravitals.isEmpty()) {
                iniciarDescanso();
            }
        }
    }

    // 2 cada tanto se reposiciona cerca del jugador, para que no quede
    //   fijo cerca de la base original cuando el mapa esta expandido
    if (!gravitals.isEmpty() && gravitals.first()->estaVivo()) {
        timerTeletransporteGravital -= 0.016f;
        if (timerTeletransporteGravital <= 0.0f) {
            float margen = base.getMargenExterno();
            float xMinMapa = base.get_x() - margen + 150.0f;
            float xMaxMapa = base.get_x() + base.get_ancho() + margen - 150.0f;
            float yMinMapa = base.get_y() - margen + 150.0f;
            float yMaxMapa = base.get_y() + base.get_alto() + margen - 150.0f;

            float angulo = (rand() % 360) * M_PI / 180.0f;
            float distancia = 250.0f + (rand() % 150);

            float nx = sobreviviente.get_x() + cos(angulo) * distancia;
            float ny = sobreviviente.get_y() + sin(angulo) * distancia;

            if (nx < xMinMapa) nx = xMinMapa;
            if (nx > xMaxMapa) nx = xMaxMapa;
            if (ny < yMinMapa) ny = yMinMapa;
            if (ny > yMaxMapa) ny = yMaxMapa;

            gravitals.first()->teletransportar(nx, ny);
            timerTeletransporteGravital = 14.0f + (rand() % 6);
        }
    }
}
void Gamelogic::actualizarBossFinal()
{
        if(!jefeFinal) { iniciarDescanso(); return; }

        if(!jefeFinal->estaVivo()) {
            if(!victoriaMostrada) {
                victoriaMostrada = true;
                mensajeFinalMostrado = true;
                mensajeFinal = T(idioma, "GANASTE - Xhaal fue derrotado", "YOU WON - Xhaal was defeated");
                timerMensajeFinal = 8.0f;
            }
            return;
        }

    // NUEVO: el rango de disparo y el radio de teletransporte se adaptan al mapa expandido
    float factorMapa = base.getMargenExterno() / 200.0f;
    if (factorMapa < 1.0f) factorMapa = 1.0f;
    jefeFinal->setFactorMapa(factorMapa);

    QPointF pJugador(sobreviviente.get_x() + sobreviviente.get_ancho()/2,
                     sobreviviente.get_y() + sobreviviente.get_alto()/2);

    jefeFinal->moverHacia(pJugador);
    jefeFinal->dispararHacia(pJugador, QPointF(0, 0));
    jefeFinal->teletransportarse(pJugador);
    jefeFinal->actualizar();
    jefeFinal->actualizarProyectiles(pJugador);
}

void Gamelogic::verificarColisionesLanzallamas()
{
    if(!sobreviviente.getArma()) return;

    Lanzallamas* lanzallamas = dynamic_cast<Lanzallamas*>(sobreviviente.getArma());
    if(!lanzallamas || !lanzallamas->estaDisparando()) return;

    QList<QPointF> particulas = lanzallamas->getAreaEfecto();
    float danioBase = lanzallamas->getDanioPorTick();
    float radioParticula = 12.0f;

    for(const QPointF& p : particulas) {
        for(Dibujable_abstracto* candidato : sistemaColisiones.obtenerEnRadio(p.x(), p.y(), radioParticula)) {
            Enemigo* enemigo = dynamic_cast<Enemigo*>(candidato);
            if(!enemigo || !enemigo->estaVivo()) continue;

            float ex = enemigo->get_x() + enemigo->get_ancho() / 2.0f;
            float ey = enemigo->get_y() + enemigo->get_alto() / 2.0f;
            float dx = ex - p.x();
            float dy = ey - p.y();
            if(std::sqrt(dx * dx + dy * dy) > radioParticula + enemigo->get_ancho() / 2.0f) continue;

            if(auto* grav = dynamic_cast<Gravital*>(enemigo)) {
                if(!grav->impactoEsVulnerable(p.x(), p.y())) continue;
            }

            enemigo->incrementarQuemado(0.016f);
            float multiplicador = 1.0f + enemigo->getTiempoQuemandose() * 0.5f;
            multiplicador = std::min(multiplicador, 1.5f); // tope: nunca mas de 1.5x el danio base
            float danioFinal = danioBase * multiplicador;
            enemigo->recibirDanio(danioFinal);
            if(!enemigo->estaVivo()) puntos += puntosPorMuerte(enemigo);
        }
    }
}

void Gamelogic::verificarColisionesSierras()
{
    if (!sobreviviente.getArma()) return;
    LanzaSierras* arma = dynamic_cast<LanzaSierras*>(sobreviviente.getArma());
    if (!arma) return;

    for (SierraVoladora* s : arma->getSierras()) {
        if (!s->estaActivo()) continue;

        float cx = s->get_x() + s->get_ancho() / 2.0f;
        float cy = s->get_y() + s->get_alto() / 2.0f;

        for (Dibujable_abstracto* candidato : sistemaColisiones.obtenerEnRadio(cx, cy, s->getRadio())) {
            Enemigo* enemigo = dynamic_cast<Enemigo*>(candidato);
            if (!enemigo || !enemigo->estaVivo()) continue;
            if (s->yaGolpeo(candidato)) continue;
            if (!Colision::hayColision(s, candidato)) continue;

            if (auto* grav = dynamic_cast<Gravital*>(candidato)) {
                if (!grav->impactoEsVulnerable(cx, cy)) continue;
            }

            enemigo->recibirDanio(s->getDanio());
            s->registrarGolpe(candidato);
            if (!enemigo->estaVivo()) puntos += puntosPorMuerte(enemigo);
        }
    }
}

int Gamelogic::calcularPrecio(int tipoTienda, int idItem) const {
    int precioBase = 0;
    if (tipoTienda == TIENDA_ARMAS) {
        if (idItem == 1) precioBase = 500;
        if (idItem == 2) precioBase = 300;
        if (idItem == 3) precioBase = 4500;
        if (idItem == 4) precioBase = 1200;
        if (idItem == 5) precioBase = 3500;
        if (idItem == 6) precioBase = 2800;
    }
    else if (tipoTienda == TIENDA_VIDA) {
        if (idItem == 1) precioBase = 200 + (nivelVidaExtra * 150);
        if (idItem == 2) precioBase = 300 + (nivelVelocidadExtra * 200);
        if (idItem == 3) precioBase = 50;
    }
    else if (tipoTienda == TIENDA_CHICHES) {
        precioBase = 150;
        if (idItem == 13) precioBase = 250 + (nivelDanioLanzallamas * 180);
        if (idItem == 14) precioBase = 250 + (nivelAlcanceLanzallamas * 180);
        if (idItem == 15) precioBase = 300 + (nivelCadenciaSierra * 200);
        if (idItem == 16) precioBase = 300 + (nivelTamanioSierra * 200);
        if (idItem == 17) precioBase = 200 + (nivelCadenciaEscopeta * 150);
    }
    else if (tipoTienda == TIENDA_EXPANSION) {
        precioBase = 500;
    }
    return precioBase;
}
int Gamelogic::puntosPorMuerte(Enemigo* enemigo) const
{
    if(dynamic_cast<Cascarudo*>(enemigo)) return 20;
    if(dynamic_cast<ZombiePala*>(enemigo)) return 30;
    if(dynamic_cast<Murcielago*>(enemigo)) return 50;
    if(dynamic_cast<ZombieArmado*>(enemigo)) return 80;
    if(dynamic_cast<Goliat*>(enemigo)) return 100;
    if(dynamic_cast<AranoTek*>(enemigo)) return 120;
    if(dynamic_cast<carnicero*>(enemigo)) return 150;
    if(dynamic_cast<Mutante*>(enemigo)) return 200;
    if(dynamic_cast<Embistidor*>(enemigo)) return 250;
    if(dynamic_cast<Invocador*>(enemigo)) return 250;
    if(dynamic_cast<MecanicDemon*>(enemigo)) return 300;
    if(dynamic_cast<Gravital*>(enemigo)) return 3000;
    if(dynamic_cast<Xhaal*>(enemigo)) return 5000;
    if(dynamic_cast<ZombieTanque*>(enemigo)) return 800;
    if(dynamic_cast<Pulsar*>(enemigo)) return 220;
    return 10;
}
