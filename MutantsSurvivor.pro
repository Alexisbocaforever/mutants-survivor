QT = core \
    widgets

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Xhaal.cpp \
        aranotek.cpp \
        arma.cpp \
        basemilitar.cpp \
        carnicero.cpp \
        cascarudo.cpp \
        colision.cpp \
        dibujable_abstracto.cpp \
        embistidor.cpp \
        enemigos.cpp \
        escenario.cpp \
        escopeta.cpp \
        explosivo.cpp \
        fal.cpp \
        gamelogic.cpp \
        gamewidget.cpp \
        gestor_hordas.cpp \
        goliat.cpp \
        granada.cpp \
        gravital.cpp \
        invocador.cpp \
        lanzacohetes.cpp \
        lanzallamas.cpp \
        lanzasierras.cpp \
        main.cpp \
        mecanicdemon.cpp \
        murcielago.cpp \
        mutante.cpp \
        ondaexpansiva.cpp \
        pared.cpp \
        pistola.cpp \
        poolentidades.cpp \
        proyectil.cpp \
        proyectilbuscador.cpp \
        proyectilcurvo.cpp \
        puerta.cpp \
        pulsar.cpp \
        rayolaser.cpp \
        sierravoladora.cpp \
        sistema_colisiones.cpp \
        sobreviviente.cpp \
        tienda.cpp \
        vida.cpp \
        zombiearmado.cpp \
        zombiepala.cpp \
        zombietanque.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Xhaal.h \
    aranotek.h \
    arma.h \
    basemilitar.h \
    carnicero.h \
    cascarudo.h \
    colision.h \
    dibujable_abstracto.h \
    embistidor.h \
    enemigos.h \
    escenario.h \
    escopeta.h \
    explosivo.h \
    fal.h \
    gamelogic.h \
    gamewidget.h \
    gestor_hordas.h \
    goliat.h \
    granada.h \
    gravital.h \
    invocador.h \
    lanzacohetes.h \
    lanzallamas.h \
    lanzasierras.h \
    mecanicdemon.h \
    murcielago.h \
    mutante.h \
    ondaexpansiva.h \
    pared.h \
    pistola.h \
    poolentidades.h \
    proyectil.h \
    proyectilbuscador.h \
    proyectilcurvo.h \
    puerta.h \
    pulsar.h \
    rayolaser.h \
    sierravoladora.h \
    sistema_colisiones.h \
    sobreviviente.h \
    tienda.h \
    vida.h \
    zombiearmado.h \
    zombiepala.h \
    zombietanque.h
