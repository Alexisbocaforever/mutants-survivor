// gestor_hordas.cpp
#include "gestor_hordas.h"

GestorHordas::GestorHordas() : timerSpawn(0.0f), nivelActual(0)
{
}

void GestorHordas::limpiar()
{
    colaSpawn.clear();
    timerSpawn = 0.0f;
}

void GestorHordas::agregar(int tipo, int cantidad, float mult)
{
    for (int i = 0; i < cantidad; i++) {
        SpawnInfo info;
        info.tipo = tipo;
        info.multVel = mult;
        info.multVida = mult;
        info.multDanio = mult;
        colaSpawn.append(info);
    }
}

void GestorHordas::iniciarNivel(int nivel)
{
    colaSpawn.clear();
    nivelActual = nivel;

    if (esNivelBossMedio(nivel) || esNivelBossFinal(nivel)) {
        timerSpawn = 0.0f;
        return; // el boss lo spawnea Gamelogic aparte
    }

    float escala = 1.0f + (nivel * 0.08f);
    // 1 Cascarudo, 2 Goliat, 3 ZombiePala, 4 Murcielago, 5 ZombieArmado
    // 6 Mutante, 7 AranoTek, 8 BerserkerKhorne, 9 BestiaForja, 10 Embistidor, 11 Invocador
    // 12 ZombieTanque, 13 Pulsar

    switch (nivel) {
    case 1:
        // Introduccion: solo Cascarudo, para aprender a moverse y disparar
        agregar(1, 8, escala);
        break;

    case 2:
        // Se suma ZombiePala y algun Goliat suelto, siguen entrando Cascarudos
        agregar(1, 8, escala);
        agregar(3, 6, escala);
        agregar(2, 2, escala);
        break;

    case 3:
        // Se suma Murcielago, obliga a apuntar mejor
        agregar(1, 6, escala);
        agregar(3, 6, escala);
        agregar(4, 8, escala);
        agregar(2, 2, escala);
        break;

    case 4:
        // Se suma Mutante. Oleada grande de cierre antes del checkpoint
        agregar(1, 8, escala);
        agregar(3, 8, escala);
        agregar(4, 8, escala);
        agregar(2, 4, escala);
        agregar(6, 5, escala);
        break;

        // nivel 5: boss medio (Gravital)

    case 6:
        // Se suma ZombieArmado, primer enemigo a distancia
        agregar(5, 6, escala);
        agregar(1, 6, escala);
        agregar(3, 6, escala);
        agregar(4, 6, escala);
        agregar(6, 5, escala);
        agregar(2, 3, escala);
        break;

    case 7:
        // Se suma AranoTek, a distancia y mas duro. Tambien aparece Pulsar, presion de area
        agregar(7, 6, escala);
        agregar(5, 6, escala);
        agregar(6, 6, escala);
        agregar(4, 6, escala);
        agregar(3, 4, escala);
        agregar(13, 3, escala);
        break;

    case 8:
        // Se suma BerserkerKhorne y ZombieTanque. Oleada grande antes del segundo checkpoint
        agregar(8, 6, escala);
        agregar(12, 2, escala);
        agregar(7, 5, escala);
        agregar(5, 6, escala);
        agregar(6, 6, escala);
        agregar(13, 3, escala);
        break;

    case 9:
        // Gauntlet final: se suma BestiaForja, Embistidor e Invocador
        // como precalentamiento del boss final
        agregar(9, 4, escala);
        agregar(10, 3, escala);
        agregar(11, 2, escala);
        agregar(8, 6, escala);
        agregar(12, 2, escala);
        agregar(7, 5, escala);
        agregar(6, 6, escala);
        break;

        // nivel 10: boss final (AlienEllos)
    case 10:
        agregar(1, 10, escala);
        agregar(3, 10, escala);
        agregar(4, 10, escala);
        agregar(5, 8, escala);
        agregar(6, 8, escala);
        agregar(2, 5, escala);
        break;

    case 11:
        agregar(9, 6, escala);
        agregar(10, 5, escala);
        agregar(11, 4, escala);
        agregar(8, 9, escala);
        agregar(7, 8, escala);
        agregar(6, 8, escala);
        break;

    case 12:
        agregar(12, 4, escala);
        agregar(13, 5, escala);
        agregar(9, 6, escala);
        agregar(8, 9, escala);
        agregar(5, 8, escala);
        agregar(4, 8, escala);
        break;

    case 13:
        agregar(7, 10, escala);
        agregar(6, 10, escala);
        agregar(11, 4, escala);
        agregar(10, 5, escala);
        agregar(2, 6, escala);
        agregar(1, 8, escala);
        break;

    case 14:
        // ultima horda antes del jefe final, la mas dura de todas
        agregar(9, 7, escala);
        agregar(10, 6, escala);
        agregar(11, 5, escala);
        agregar(8, 10, escala);
        agregar(12, 4, escala);
        agregar(13, 6, escala);
        agregar(7, 8, escala);
        agregar(5, 8, escala);
        break;

        // nivel 15: boss final (Xhaal)
    default:
        // Mas alla del nivel 10 (o algun nivel no contemplado): horda mixta con todo el roster
        agregar(1, 6, escala);
        agregar(3, 6, escala);
        agregar(4, 6, escala);
        agregar(5, 5, escala);
        agregar(6, 5, escala);
        agregar(7, 5, escala);
        agregar(8, 4, escala);
        agregar(2, 3, escala);
        agregar(9, 3, escala);
        agregar(10, 2, escala);
        agregar(11, 1, escala);
        agregar(12, 2, escala);
        agregar(13, 3, escala);
        break;
    }
}

bool GestorHordas::actualizar(float dt, SpawnInfo &siguienteSpawn)
{
    if (colaSpawn.isEmpty()) return false;

    timerSpawn -= dt;
    if (timerSpawn > 0.0f) return false;

    siguienteSpawn = colaSpawn.takeFirst();

    float rapidezSpawn = 1.0f - (nivelActual * 0.05f);
    if (rapidezSpawn < 0.2f) rapidezSpawn = 0.2f;
    timerSpawn = rapidezSpawn;
    return true;
}
