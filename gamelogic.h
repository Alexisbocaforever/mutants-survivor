#ifndef GAMELOGIC_H
#define GAMELOGIC_H
#include <QSettings>

#include <QList>
#include <QPointF>
#include "carnicero.h"
#include "dibujable_abstracto.h"
#include "basemilitar.h"
#include "embistidor.h"
#include "gravital.h"
#include "invocador.h"
#include "puerta.h"
#include "pulsar.h"
#include "sobreviviente.h"
#include "cascarudo.h"
#include "goliat.h"
#include "colision.h"
#include "tienda.h"
#include "mutante.h"
#include "explosivo.h"
#include "zombiepala.h"
#include "murcielago.h"
#include "zombiearmado.h"
#include "aranotek.h"
#include "Xhaal.h"
#include "proyectilbuscador.h"  // NUEVO
#include "mecanicdemon.h"
#include "lanzallamas.h"
#include "gestor_hordas.h"
#include "sistema_colisiones.h"
#include "zombietanque.h"
#include "lanzasierras.h"
enum EstadoJuego { EN_HORDA, EN_DESCANSO };
#define EXPANSION_MAX 2
// NUEVO: copo de nieve cayendo, en coordenadas relativas de pantalla (0 a 1)
struct ParticulaNieve {
    float x;
    float y;
    float velocidad;
    float deriva;       // amplitud del balanceo lateral
    float faseDeriva;   // fase del seno que controla el balanceo
    float tam;
    int capa;           // 0 lejos (chico, lento, tenue), 1 cerca (grande, rapido, opaco)
};
#define IDIOMA_ES 0
#define IDIOMA_EN 1

inline QString T(int idioma, const QString &es, const QString &en)
{
    return (idioma == IDIOMA_EN) ? en : es;
}
class Gamelogic
{
public:
    Gamelogic();
    ~Gamelogic();
    void mover(int dx, int dy);
    void actualizar();
    void disparar();
    void cambiarArma(int tipoArma);
    void setMouseWorldPos(QPointF pos);
    QList<Dibujable_abstracto*> &Obtener_Dibujables();
    const QList<ParticulaNieve>& getParticulasNieve() const { return particulasNieve; } // NUEVO
    // Sistema de tiendas
    Tienda* getTiendaCercana();
    void comprarItem(int itemId);
    int calcularPrecio(int tipoTienda, int idItem) const;

    bool tieneArma(int tipo) const;
    QString getNombreArmaActual() const;
    void setSkinSobreviviente(int skin);
    int nivelCadenciaEscopeta;
    // Getters mejoras
    int getNivelRecargaPistola() const { return nivelRecargaPistola; }
    int getNivelRecargaFal() const { return nivelRecargaFal; }
    int getNivelRecargaEscopeta() const { return nivelRecargaEscopeta; }
    int getNivelRecargaGranadas() const { return nivelRecargaGranadas; }
    int getNivelRecargaCohetes() const { return nivelRecargaCohetes; }
    int getNivelCapacidadPistola() const { return nivelCapacidadPistola; }
    int getNivelCapacidadFal() const { return nivelCapacidadFal; }
    int getNivelCapacidadEscopeta() const { return nivelCapacidadEscopeta; }
    int getNivelCapacidadGranadas() const { return nivelCapacidadGranadas; }
    int getNivelCapacidadCohetes() const { return nivelCapacidadCohetes; }
    int getNivelRangoArmas() const { return nivelRangoArmas; }
    int getNivelRangoExplosion() const { return nivelRangoExplosion; }
    int getNivelDanioLanzallamas() const { return nivelDanioLanzallamas; }
    int extracted() const;
    int getNivelAlcanceLanzallamas() const { return nivelAlcanceLanzallamas; }
    int getNivelVelocidadExtra() const { return nivelVelocidadExtra; } // NUEVO
    int getNivel() const { return nivelActual; }
    int getPuntos() const { return puntos; }
    int getTiempoDescanso() const { return (int)timerDescanso; }
    EstadoJuego getEstado() const { return estadoActual; }
    bool juegoTerminado() const;
    int ultimoCheckpoint;
    void iniciarNivel(int nivel);


    int getExpansionDerecha() const { return expansionDerecha; }
    int getExpansionIzquierda() const { return expansionIzquierda; }
    int getExpansionArriba() const { return expansionArriba; }
    int getExpansionAbajo() const { return expansionAbajo; }
    bool getMensajeFinalVisible() const { return mensajeFinalMostrado && timerMensajeFinal > 0; }
    QString getMensajeFinal() const { return mensajeFinal; }
    void verificarColisionesLanzallamas();
    float getTiempoMensaje() const { return timerMensajeFinal; }
    void reiniciarJuego();
    bool juegoFueGanado() const;
    bool getCheckpointVisible() const { return checkpointVisible && timerCheckpoint > 0; }
    QString getCheckpointTexto() const { return checkpointTexto; }
    int getNivelCadenciaSierra() const { return nivelCadenciaSierra; }
    int getNivelTamanioSierra() const { return nivelTamanioSierra; }
    int getNivelCadenciaEscopeta() const { return nivelCadenciaEscopeta; }
    int getIdioma() const { return idioma; }
    void setIdioma(int nuevoIdioma) {         // NUEVO
        idioma = nuevoIdioma;
        QSettings settings("MutantsSurvivor", "SaveData");
        settings.setValue("idioma", idioma);
    }

int idioma; // NUEVO
private:

    BaseMilitar base;
    QList<Dibujable_abstracto*> Dibujables;
    QList<ParticulaNieve> particulasNieve; // NUEVO
    void actualizarNieve();                // NUEVO
    void generarParticulaNieve(ParticulaNieve &part); // NUEVO
    // Listas de enemigos
    QList<Cascarudo*> cascarudos;
    QList<Goliat*> goliats;
    QList<ZombiePala*> zombies;
    QList<Murcielago*> murcielagos;
    QList<Mutante*> mutantes;
    QList<ZombieArmado*> zombiesArmados;
    QList<AranoTek*> aranosTek;
    QList<carnicero*> berserkers;
    QList<Tienda*> tiendas;
    QList<Explosivo*> explosiones;
    QList<MecanicDemon*> bestiasForja;
    QList<Gravital*> gravitals;
    QList<Invocador*> invocadores;
    Sobreviviente sobreviviente;
    QList<Embistidor*> embistidores;
    QList<ZombieTanque*> zombiesTanque;
    QPointF mouseWorldPos;
    QList<Pulsar*> pulsares;
    // NUEVO: Boss Final
    Xhaal* jefeFinal;
    int nivelDanioLanzallamas;
    int nivelAlcanceLanzallamas;
    // Progresión
    int nivelActual;
    int puntos;
    EstadoJuego estadoActual;
    float timerDescanso;
    float tiempoUltimoDanioContacto;

    // Inventario y Mejoras
    bool tieneFal;
    bool tieneEscopeta;
    bool tieneGranadas;
    bool tieneLanzacohetes;
    bool tieneLanzallamas;
    int nivelVidaExtra;
    int nivelVelocidadExtra;
    int expansionDerecha;
    int expansionIzquierda;
    int expansionArriba;
    int expansionAbajo;
    float timerTeletransporteGravital; // NUEVO
    int nivelRecargaPistola;
    int nivelRecargaFal;
    int nivelRecargaEscopeta;
    int nivelRecargaGranadas;
    int nivelRecargaCohetes;
    int nivelCapacidadPistola;
    int nivelCapacidadFal;
    int nivelCapacidadEscopeta;
    int nivelCapacidadGranadas;
    int nivelCapacidadCohetes;
    int nivelRangoArmas;
    int nivelRangoExplosion;



    void iniciarDescanso();


    // Colisiones
    void verificarColisiones();
    void verificarColisionesProyectiles();
    void verificarColisionesProyectilesEnemigos();
    void verificarColisionesProyectilesBoss();  // NUEVO
    void verificarColisionesEnemigos();
    void verificarColisionConSobreviviente();

    void crearExplosion(float x, float y, float radio, float danio);
    void crearExplosionEnemiga(float x, float y, float radio, float danio);
    void actualizarExplosiones();
    void aplicarMejorasArmas();
    void aplicarExpansionEscenario();




    bool mensajeFinalMostrado;
    QString mensajeFinal;
    float timerMensajeFinal;



    GestorHordas gestorHordas;
    SistemaColisiones sistemaColisiones;
    bool victoriaMostrada;
    bool checkpointVisible;
    QString checkpointTexto;
    float timerCheckpoint;
    int puntosPorMuerte(Enemigo* enemigo) const;
    void actualizarBossMedio();
    void actualizarBossFinal();
    bool tieneSierra;

    int nivelCadenciaSierra;
    int nivelTamanioSierra;
    void verificarColisionesSierras();

};

#endif // GAMELOGIC_H
