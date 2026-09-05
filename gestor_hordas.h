// gestor_hordas.h
#ifndef GESTOR_HORDAS_H
#define GESTOR_HORDAS_H

#include <QList>

#define NIVEL_BOSS_MEDIO 5
#define NIVEL_BOSS_FINAL 10
#define INTERVALO_CHECKPOINT 4
#define NIVEL_BOSS_FINAL 15 // ANTES: 10
struct SpawnInfo {
    int tipo; // 1 Cascarudo, 2 Goliat, 3 ZombiePala, 4 Murcielago, 5 ZombieArmado
        // 6 Mutante, 7 AranoTek, 8 BerserkerKhorne, 9 BestiaForja, 10 Embistidor, 11 Invocador
        // 12 ZombieTanque, 13 Pulsar
    float multVel;
    float multVida;
    float multDanio;
};

class GestorHordas
{
public:
    GestorHordas();

    // Llena la cola con la composicion del nivel. No hace nada en niveles boss.
    void iniciarNivel(int nivel);

    // Descuenta el timer. Si toca spawnear, devuelve true y llena siguienteSpawn.
    bool actualizar(float dt, SpawnInfo &siguienteSpawn);

    bool hordaVacia() const { return colaSpawn.isEmpty(); }
    void limpiar();

    static bool esNivelBossMedio(int nivel) { return nivel == NIVEL_BOSS_MEDIO; }
    static bool esNivelBossFinal(int nivel) { return nivel == NIVEL_BOSS_FINAL; }
    static bool esNivelCheckpoint(int nivel) { return nivel > 0 && nivel % INTERVALO_CHECKPOINT == 0; }

private:
    QList<SpawnInfo> colaSpawn;
    float timerSpawn;
    int nivelActual;

    void agregar(int tipo, int cantidad, float mult);
};

#endif // GESTOR_HORDAS_Hc
