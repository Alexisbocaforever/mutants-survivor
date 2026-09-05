#include "gamewidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include "sobreviviente.h"

Gamewidget::Gamewidget(Gamelogic &gl, QWidget *parent)
    : QWidget(parent), gamelogic(gl),enTutorial(true),enSeleccionPersonaje(true),enSeleccionIdioma(true),idiomaSeleccionadoTemp(IDIOMA_ES),
 skinSeleccionada(SKIN_SURVIVOR),
    itemSeleccionadoTienda(0), mousePresionado(false),pausado(false)
{





    Crear_ventana();
    timer_principal = new QTimer(this);
    connect(timer_principal, &QTimer::timeout, this, [this]() {
        if (!enSeleccionPersonaje && !pausado) {
            int dx = 0, dy = 0;
            if (teclasPresionadas.contains(Qt::Key_W)) dy -= 1;
            if (teclasPresionadas.contains(Qt::Key_S)) dy += 1;
            if (teclasPresionadas.contains(Qt::Key_A)) dx -= 1;
            if (teclasPresionadas.contains(Qt::Key_D)) dx += 1;

            this->gamelogic.setMouseWorldPos(this->mouseWorldPos);

            // NUEVO: Disparo continuo
            if (this->mousePresionado) {
                this->gamelogic.disparar();
            }

            this->gamelogic.mover(dx, dy);
            this->gamelogic.actualizar();
        }
        this->update();
    });
    timer_principal->start(16);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    camPos = QPointF(0, 0);
    mouseScreenPos = QPointF(width() / 2.0, height() / 2.0);
}
void Gamewidget::dibujarSeleccionIdioma(QPainter &painter)
{
    painter.fillRect(rect(), QColor(15, 20, 25));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 32, QFont::Bold));
    painter.drawText(0, 60, width(), 60, Qt::AlignHCenter, "MUTANTS SURVIVOR");

    painter.setFont(QFont("Arial", 16));
    painter.drawText(0, height()/2 - 80, width(), 30, Qt::AlignCenter, "Selecciona tu idioma / Select your language");

    int anchoOpcion = 220;
    int altoOpcion = 80;
    int espacio = 40;
    int totalAncho = anchoOpcion * 2 + espacio;
    int xInicio = (width() - totalAncho) / 2;
    int y = height()/2 - altoOpcion/2;

    bool selEs = (idiomaSeleccionadoTemp == IDIOMA_ES);
    painter.setBrush(selEs ? QColor(80, 120, 80, 200) : QColor(50, 60, 70, 150));
    painter.setPen(selEs ? QPen(QColor(150, 255, 150), 3) : QPen(QColor(80, 80, 90), 1));
    painter.drawRect(xInicio, y, anchoOpcion, altoOpcion);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 18, QFont::Bold));
    painter.drawText(xInicio, y, anchoOpcion, altoOpcion, Qt::AlignCenter, "ESPAÑOL");

    bool selEn = (idiomaSeleccionadoTemp == IDIOMA_EN);
    painter.setBrush(selEn ? QColor(80, 120, 80, 200) : QColor(50, 60, 70, 150));
    painter.setPen(selEn ? QPen(QColor(150, 255, 150), 3) : QPen(QColor(80, 80, 90), 1));
    painter.drawRect(xInicio + anchoOpcion + espacio, y, anchoOpcion, altoOpcion);
    painter.setPen(Qt::white);
    painter.drawText(xInicio + anchoOpcion + espacio, y, anchoOpcion, altoOpcion, Qt::AlignCenter, "ENGLISH");

    painter.setFont(QFont("Arial", 12));
    painter.drawText(0, y + altoOpcion + 40, width(), 30, Qt::AlignCenter,
                     "Flechas para elegir, Enter para confirmar / Arrows to choose, Enter to confirm");
}

void Gamewidget::Crear_ventana()
{
    this->resize(1500, 900);
    this->setWindowTitle("El Mutants Survivor - Base Militar Campo de Mayo");
    show();
}
void Gamewidget::reiniciarJuego()
{
    // Reiniciar lógica del juego
    gamelogic.reiniciarJuego();

    // Volver a selección de personaje
    enSeleccionPersonaje = true;
    skinSeleccionada = SKIN_SURVIVOR;
    itemSeleccionadoTienda = 0;
    mousePresionado = false;
    pausado = false;

    // Resetear cámara
    camPos = QPointF(0, 0);
    mouseScreenPos = QPointF(width() / 2.0, height() / 2.0);

    // Limpiar teclas presionadas
    teclasPresionadas.clear();
}
void Gamewidget::dibujarPreviewPersonaje(QPainter &painter, int skin, int x, int y, bool seleccionado, bool bloqueado)
{
    painter.save();

    int anchoBox = 250;
    int altoBox = 400;
    QColor colorFondo = seleccionado ? QColor(100, 150, 100, 220) : QColor(60, 60, 70, 220);
    painter.fillRect(x, y, anchoBox, altoBox, colorFondo);

    if (seleccionado) {
        painter.setPen(QPen(QColor(150, 255, 150), 5));
    } else {
        painter.setPen(QPen(QColor(100, 100, 110), 2));
    }
    painter.drawRect(x, y, anchoBox, altoBox);

    painter.setPen(Qt::NoPen);
    int pixelSize = 6;
    int offsetX = x + (anchoBox - 16 * pixelSize) / 2;
    int offsetY = y + 44; // ajustado para que cabeza y cuerpo queden pegados correctamente

    EstadisticasPersonaje stats; // declarar una sola vez y rellenar según skin

    if (skin == SKIN_SURVIVOR) {
        QColor traje(70, 110, 150);
        QColor mochila(58, 78, 92);
        QColor panuelo(150, 130, 90);
        QColor guantes(35, 35, 38);
        QColor botas(28, 28, 32);

        painter.setBrush(traje);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 10*pixelSize);
        painter.drawRect(offsetX + 3*pixelSize, offsetY + 1*pixelSize, 10*pixelSize, 9*pixelSize);

        painter.setBrush(Qt::black);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 3*pixelSize, 6*pixelSize, 1*pixelSize);

        painter.setBrush(panuelo);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 4*pixelSize, 8*pixelSize, 4*pixelSize);

        painter.setBrush(mochila);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 10*pixelSize, 3*pixelSize, 6*pixelSize);

        painter.setBrush(botas);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);

        // Stats SURVIVOR
        stats.velocidadMovimiento = 1.0f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 120.0f;
        stats.resistenciaDanio = 0.15f;
    }
    else if (skin == SKIN_MILITAR) {
        QColor uniforme(100, 120, 80);
        QColor chaleco(80, 90, 70);
        QColor casco(60, 80, 50);
        QColor piel(200, 150, 120);
        QColor botas(40, 35, 30);

        painter.setBrush(uniforme);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);

        painter.setBrush(botas);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);

        painter.setBrush(uniforme);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 10*pixelSize);

        painter.setBrush(chaleco);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 9*pixelSize, 6*pixelSize, 8*pixelSize);

        painter.setBrush(uniforme);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);

        painter.setBrush(piel);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 16*pixelSize, 2*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 16*pixelSize, 2*pixelSize, 2*pixelSize);

        painter.setBrush(casco);
        painter.drawRect(offsetX + 3*pixelSize, offsetY - 1*pixelSize, 10*pixelSize, 9*pixelSize);

        painter.setBrush(piel);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 2*pixelSize, 6*pixelSize, 4*pixelSize);

        painter.setBrush(Qt::black);
        painter.drawRect(offsetX + 6*pixelSize, offsetY + 3*pixelSize, 1*pixelSize, 1*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 3*pixelSize, 1*pixelSize, 1*pixelSize);

        painter.setBrush(chaleco);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 10*pixelSize, 2*pixelSize, 3*pixelSize);

        painter.setBrush(QColor(60, 70, 50));
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 14*pixelSize, 2*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 14*pixelSize, 2*pixelSize, 2*pixelSize);

        // Stats MILITAR
        stats.velocidadMovimiento = 1.2f;
        stats.velocidadRecarga = 0.75f;
        stats.vidaMaxima = 100.0f;
        stats.resistenciaDanio = 0.0f;
    }
    else if (skin == SKIN_RUNNER) { // remera deportiva, igual a como se ve en el juego
        QColor pielMorena(160, 110, 70);
        QColor cabello(50, 30, 10);
        QColor topBlanco(255, 255, 255);
        QColor topAzul(0, 50, 180);
        QColor shortNegro(20, 20, 20);
        QColor zapatilla(240, 240, 240);
        QColor suela(60, 60, 60);
        QColor vincha(0, 50, 180);

        // 1 piernas con short, zapatillas y suela
        painter.setBrush(shortNegro);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 3*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 3*pixelSize);
        painter.setBrush(pielMorena);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 20*pixelSize, 3*pixelSize, 4*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 20*pixelSize, 3*pixelSize, 4*pixelSize);
        painter.setBrush(zapatilla);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.setBrush(suela);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 26*pixelSize, 3*pixelSize, 1*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 26*pixelSize, 3*pixelSize, 1*pixelSize);

        // 2 brazos
        painter.setBrush(pielMorena);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);

        // 3 remera deportiva bicolor con "Run"
        painter.setBrush(topAzul);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 13*pixelSize, 8*pixelSize, 4*pixelSize);
        painter.setBrush(topBlanco);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 5*pixelSize);

        painter.setPen(QPen(Qt::white, 1));
        QFont f = painter.font(); f.setPointSize(5); f.setBold(true);
        painter.setFont(f);
        painter.drawText(offsetX + 4*pixelSize, offsetY + 13*pixelSize, 8*pixelSize, 4*pixelSize, Qt::AlignCenter, "Run");
        painter.setPen(Qt::NoPen);

        // 4 cara
        painter.setBrush(pielMorena);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 4*pixelSize, 6*pixelSize, 4*pixelSize);
        painter.setBrush(Qt::black);
        painter.drawRect(offsetX + 6*pixelSize, offsetY + 5*pixelSize, 1*pixelSize, 1*pixelSize);
        painter.drawRect(offsetX + 9*pixelSize, offsetY + 5*pixelSize, 1*pixelSize, 1*pixelSize);

        // 5 flequillo, vincha y coleta
        painter.setBrush(cabello);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 2*pixelSize, 8*pixelSize, 3*pixelSize);
        painter.setBrush(vincha);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 4*pixelSize, 8*pixelSize, 1*pixelSize);
        painter.setBrush(cabello);
        painter.drawRect(offsetX + 7*pixelSize, offsetY + 1*pixelSize, 2*pixelSize, 2*pixelSize);

        // Stats RUNNER
        stats.velocidadMovimiento = 1.4f;
        stats.velocidadRecarga = 0.9f;
        stats.vidaMaxima = 90.0f;
        stats.resistenciaDanio = 0.05f;
    }
    else if (skin == SKIN_Argentine) {
        QColor celeste(130, 180, 230);
        QColor blanco(250, 250, 250);
        QColor shortAzul(30, 40, 90);
        QColor piel(200, 160, 120);
        QColor pelo(40, 30, 20);

        // Medias blancas
        painter.setBrush(blanco);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);

        // Short azul
        painter.setBrush(shortAzul);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 3*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 3*pixelSize);

        // Brazos
        painter.setBrush(piel);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);

        // Camiseta a rayas celeste y blanco
        painter.setBrush(celeste);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 10*pixelSize);
        painter.setBrush(blanco);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 11*pixelSize, 8*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 15*pixelSize, 8*pixelSize, 2*pixelSize);

        // Numero 10
        painter.setPen(QPen(Qt::black, 1));
        QFont fArgentine = painter.font();
        fArgentine.setPointSize(9);
        fArgentine.setBold(true);
        painter.setFont(fArgentine);
        painter.drawText(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 10*pixelSize, Qt::AlignCenter, "10");
        painter.setPen(Qt::NoPen);

        // Cabeza y pelo
        painter.setBrush(piel);
        painter.drawRect(offsetX + 3*pixelSize, offsetY + 1*pixelSize, 10*pixelSize, 9*pixelSize);
        painter.setBrush(pelo);
        painter.drawRect(offsetX + 3*pixelSize, offsetY + 1*pixelSize, 10*pixelSize, 3*pixelSize);

        // Botines
        painter.setBrush(Qt::black);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);

        // Stats Argentine
        stats.velocidadMovimiento = 1.5f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 100.0f;
        stats.resistenciaDanio = 0.3f;
    }
    else { // SKIN_Exterminador
        QColor armaduraVerde(50, 70, 45);
        QColor armaduraOscura(35, 40, 38);
        QColor visor(200, 60, 20);
        QColor metal(70, 75, 70);

        // Piernas
        painter.setBrush(armaduraOscura);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 17*pixelSize, 3*pixelSize, 7*pixelSize);

        // Brazos
        painter.setBrush(armaduraVerde);
        painter.drawRect(offsetX + 2*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);
        painter.drawRect(offsetX + 12*pixelSize, offsetY + 9*pixelSize, 2*pixelSize, 7*pixelSize);

        // Peto con emblema
        painter.setBrush(armaduraVerde);
        painter.drawRect(offsetX + 4*pixelSize, offsetY + 8*pixelSize, 8*pixelSize, 10*pixelSize);
        painter.setBrush(metal);
        painter.drawRect(offsetX + 6*pixelSize, offsetY + 11*pixelSize, 4*pixelSize, 4*pixelSize);

        // Casco con visor
        painter.setBrush(armaduraOscura);
        painter.drawRect(offsetX + 3*pixelSize, offsetY + 1*pixelSize, 10*pixelSize, 9*pixelSize);
        painter.setBrush(visor);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 4*pixelSize, 6*pixelSize, 2*pixelSize);

        // Botas
        painter.setBrush(Qt::black);
        painter.drawRect(offsetX + 5*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);
        painter.drawRect(offsetX + 8*pixelSize, offsetY + 24*pixelSize, 3*pixelSize, 2*pixelSize);

        // Stats exterminador
        stats.velocidadMovimiento = 0.75f;
        stats.velocidadRecarga = 1.0f;
        stats.vidaMaxima = 200.0f;
        stats.resistenciaDanio = 0.4f;
    }





    // Nombre del personaje según skin
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    QString nombre;
    if (skin == SKIN_SURVIVOR) nombre = "Survivor class";
    else if (skin == SKIN_MILITAR) nombre = "Soldier class";
    else if (skin == SKIN_RUNNER) nombre = "Runner class";
    else if (skin == SKIN_Argentine) nombre = "Argentine class";
    else nombre = "exterminador class";
    painter.drawText(x, y + 220, anchoBox, 30, Qt::AlignCenter, nombre);

    // Dibujar estadísticas (usando la estructura stats ya inicializada)
    // Dibujar estadisticas (usando la estructura stats ya inicializada)
    int idioma = gamelogic.getIdioma();
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(200, 255, 200));
    int statY = y + 260;
    int lineHeight = 22;

    painter.drawText(x + 10, statY, T(idioma, "♥ VIDA:", "♥ HP:"));
    painter.drawText(x + 180, statY, QString::number((int)stats.vidaMaxima));

    painter.drawText(x + 10, statY + lineHeight, T(idioma, "⚡ VELOCIDAD:", "⚡ SPEED:"));
    QString velTexto = QString::number(stats.velocidadMovimiento * 100, 'f', 0) + "%";
    painter.drawText(x + 180, statY + lineHeight, velTexto);

    painter.drawText(x + 10, statY + lineHeight * 2, T(idioma, "🔄 RECARGA:", "🔄 RELOAD:"));
    QString recTexto = QString::number((1.0f / stats.velocidadRecarga) * 100, 'f', 0) + "%";
    painter.drawText(x + 180, statY + lineHeight * 2, recTexto);

    painter.drawText(x + 10, statY + lineHeight * 3, T(idioma, "🛡 RESIST:", "🛡 RESIST:"));
    QString resTexto = QString::number(stats.resistenciaDanio * 100, 'f', 0) + "%";
    painter.drawText(x + 180, statY + lineHeight * 3, resTexto);

    if (bloqueado) {
        painter.setBrush(QColor(0, 0, 0, 190));
        painter.setPen(Qt::NoPen);
        painter.drawRect(x, y, anchoBox, altoBox);

        painter.setPen(QColor(255, 210, 80));
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        painter.drawText(x, y + altoBox / 2 - 20, anchoBox, 40, Qt::AlignCenter, T(idioma, "BLOQUEADO", "LOCKED"));

        painter.setFont(QFont("Arial", 10));
        painter.drawText(x, y + altoBox / 2 + 15, anchoBox, 40, Qt::AlignCenter,
                         T(idioma, "Gana la partida\npara desbloquear", "Win the game\nto unlock"));
    }
}

void Gamewidget::dibujarSeleccionPersonaje(QPainter &painter)
{
    painter.fillRect(rect(), QColor(20, 30, 40));

    painter.setPen(QColor(255, 255, 100));
    painter.setFont(QFont("Arial", 36, QFont::Bold));
    painter.drawText(rect().adjusted(0, 30, 0, 0), Qt::AlignTop | Qt::AlignHCenter,
                     "SELECCIONA TU PERSONAJE");

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(rect().adjusted(0, 90, 0, 0), Qt::AlignTop | Qt::AlignHCenter,
                     "Usa las flechas ← → y presiona ENTER para comenzar");

    int centerX = width() / 2;
    int centerY = height() / 2 - 20;

    dibujarPreviewPersonaje(painter, SKIN_SURVIVOR, centerX - 705, centerY - 200,
                            skinSeleccionada == SKIN_SURVIVOR);

    dibujarPreviewPersonaje(painter, SKIN_RUNNER, centerX - 415, centerY - 200,
                            skinSeleccionada == SKIN_RUNNER);

    dibujarPreviewPersonaje(painter, SKIN_MILITAR, centerX - 125, centerY - 200,
                            skinSeleccionada == SKIN_MILITAR);

    dibujarPreviewPersonaje(painter, SKIN_Argentine, centerX + 165, centerY - 200,
                            skinSeleccionada == SKIN_Argentine);

    dibujarPreviewPersonaje(painter, SKIN_Exterminador, centerX + 455, centerY - 200,
                            skinSeleccionada == SKIN_Exterminador, !gamelogic.juegoFueGanado());

}

// gamewidget.cpp
void Gamewidget::dibujarItemTienda(QPainter &painter, int x, int y, QString nombre, QString desc,
                                   QString precio, bool seleccionado, bool yaComprado,
                                   int itemAncho, int itemAlto)
{
    QColor colorFondo = seleccionado ? QColor(80, 120, 80, 180) : QColor(50, 60, 70, 150);
    if(yaComprado) colorFondo = QColor(100, 100, 100, 120);

    painter.fillRect(x, y, itemAncho, itemAlto, colorFondo);

    if(seleccionado && !yaComprado) {
        painter.setPen(QPen(QColor(150, 255, 150), 2));
    } else {
        painter.setPen(QPen(QColor(80, 80, 90), 1));
    }
    painter.drawRect(x, y, itemAncho, itemAlto);

    if(yaComprado) {
        painter.setPen(QColor(150, 150, 150));
    } else {
        painter.setPen(QColor(255, 255, 255));
    }

    bool compacto = (itemAlto < 50);

    painter.setFont(QFont("Arial", compacto ? 10 : 14, QFont::Bold));
    painter.drawText(x + 8, y + (compacto ? 17 : 25), nombre);

    if(!compacto && !desc.isEmpty()) {
        painter.setFont(QFont("Arial", 10));
        painter.drawText(x + 10, y + 45, desc);
    }

    // 1 el texto de la derecha siempre es el que se paso por parametro
    if(yaComprado) {
        painter.setPen(QColor(100, 200, 100));
    } else {
        painter.setPen(QColor(255, 200, 100));
    }
    painter.setFont(QFont("Arial", compacto ? 9 : 13, QFont::Bold));
    if(compacto) {
        painter.drawText(x, y + 17, itemAncho - 6, itemAlto, Qt::AlignRight | Qt::AlignVCenter, precio);
    } else {
        painter.drawText(x, y + 35, itemAncho - 10, 20, Qt::AlignRight, precio);
    }
}

void Gamewidget::dibujarMenuTienda(QPainter &painter, Tienda* tienda)
{
    int panelX = width() / 2 - 350;
    int panelY = 25;
    int panelAncho = 700;
    int panelAlto = 560;
    int itemY = panelY + 90;
    int lineHeight = 32; // ajustado para 14 items

    int idioma = gamelogic.getIdioma();

    painter.fillRect(panelX, panelY, panelAncho, panelAlto, QColor(30, 40, 50, 240));
    painter.setPen(QPen(QColor(100, 200, 255), 4));
    painter.drawRect(panelX, panelY, panelAncho, panelAlto);

    painter.setPen(QColor(255, 255, 100));
    painter.setFont(QFont("Arial", 20, QFont::Bold));

    if(tienda->getTipo() == TIENDA_ARMAS) {
        painter.drawText(panelX + 20, panelY + 35, "FERRETERIAS PIPO");
        painter.setPen(QColor(200, 220, 255));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(panelX + 20, panelY + 60, T(idioma, "Joni Pirri te ofrece:", "Joni Pirri offers you:"));

        int itemY = panelY + 90;
        int lineHeight = 68; // separacion suficiente para los 5 items

        // FAL
        bool sel1 = (itemSeleccionadoTienda == 0);
        bool tiene1 = gamelogic.tieneArma(2);
        int precio1 = gamelogic.calcularPrecio(TIENDA_ARMAS, 1);
        dibujarItemTienda(painter, panelX + 20, itemY,
                          "FAL", T(idioma, "Rifle automatico. Dano: 15", "Automatic rifle. Damage: 15"),
                          QString::number(precio1) + " pts",
                          sel1, tiene1);

        // ESCOPETA
        bool sel2 = (itemSeleccionadoTienda == 1);
        bool tiene2 = gamelogic.tieneArma(5);
        int precio2 = gamelogic.calcularPrecio(TIENDA_ARMAS, 2);
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight,
                          T(idioma, "ESCOPETA", "SHOTGUN"), T(idioma, "5 perdigones. Dano total: 40", "5 pellets. Total damage: 40"),
                          QString::number(precio2) + " pts",
                          sel2, tiene2);

        // LANZALLAMAS
        bool sel3 = (itemSeleccionadoTienda == 2);
        bool tiene3 = gamelogic.tieneArma(6);
        int precio3 = gamelogic.calcularPrecio(TIENDA_ARMAS, 3);
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * 2,
                          T(idioma, "LANZALLAMAS", "FLAMETHROWER"), T(idioma, "Dano continuo 8.0/tick", "Continuous damage 8.0/tick"),
                          QString::number(precio3) + " pts",
                          sel3, tiene3);

        // GRANADAS
        bool sel4 = (itemSeleccionadoTienda == 3);
        bool tiene4 = gamelogic.tieneArma(3);
        int precio4 = gamelogic.calcularPrecio(TIENDA_ARMAS, 4);
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * 3,
                          T(idioma, "GRANADAS", "GRENADES"), T(idioma, "Explosivos. Dano: 50", "Explosives. Damage: 50"),
                          QString::number(precio4) + " pts",
                          sel4, tiene4);

        // LANZACOHETES
        bool sel5 = (itemSeleccionadoTienda == 4);
        bool tiene5 = gamelogic.tieneArma(4);
        int precio5 = gamelogic.calcularPrecio(TIENDA_ARMAS, 5);
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * 4,
                          T(idioma, "LANZACOHETES", "ROCKET LAUNCHER"), T(idioma, "Devastador. Dano: 100", "Devastating. Damage: 100"),
                          QString::number(precio5) + " pts",
                          sel5, tiene5);

        // LANZASIERRAS
        bool sel6 = (itemSeleccionadoTienda == 5);
        bool tiene6 = gamelogic.tieneArma(7);
        int precio6 = gamelogic.calcularPrecio(TIENDA_ARMAS, 6);
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * 5,
                          T(idioma, "LANZASIERRAS", "SAW LAUNCHER"), T(idioma, "Perfora, va y vuelve. Dano: 35", "Pierces, goes and returns. Damage: 35"),
                          QString::number(precio6) + " pts",
                          sel6, tiene6);
    }
    // === TIENDA DE VIDA ===
    else if(tienda->getTipo() == TIENDA_VIDA) {
        painter.drawText(panelX + 20, panelY + 35, "CLINICA MALDONADO");
        painter.setPen(QColor(200, 220, 255));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(panelX + 20, panelY + 60, T(idioma, "El medico de la clinica te ofrece:", "The clinic's doctor offers you:"));

        int itemY = panelY + 90;
        int lineHeight = 70;

        // MEJORA DE RESISTENCIA
        bool sel1 = (itemSeleccionadoTienda == 0);
        int precio1 = gamelogic.calcularPrecio(TIENDA_VIDA, 1);
        dibujarItemTienda(painter, panelX + 20, itemY,
                          T(idioma, "MEJORA DE RESISTENCIA", "STAMINA UPGRADE"),
                          T(idioma, "+75 HP maxima permanente", "+75 max HP, permanent"),
                          QString::number(precio1) + " pts",
                          sel1, false);

        // MEJORA DE VELOCIDAD
        bool sel2 = (itemSeleccionadoTienda == 1);
        int precio2 = gamelogic.calcularPrecio(TIENDA_VIDA, 2);
        int nivelVel = gamelogic.getNivelVelocidadExtra();
        bool velocidadMax = (nivelVel >= 15);
        QString textoVel = velocidadMax ? T(idioma, "COMPLETADO", "COMPLETED") : QString::number(precio2) + " pts";
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight,
                          QString("%1 (%2/15)").arg(T(idioma, "MEJORA DE VELOCIDAD", "SPEED UPGRADE")).arg(nivelVel),
                          T(idioma, "+Velocidad de movimiento", "+Movement speed"),
                          textoVel,
                          sel2, velocidadMax);

        // CURACION
        bool sel3 = (itemSeleccionadoTienda == 2);
        int precio3 = gamelogic.calcularPrecio(TIENDA_VIDA, 3);

        bool vidaLlena = false;
        QList<Dibujable_abstracto*> &lista = gamelogic.Obtener_Dibujables();
        for(Dibujable_abstracto* d : lista) {
            if(auto* s = dynamic_cast<Sobreviviente*>(d)) {
                if(s->getVida().getVidaActual() >= s->getVida().getVidaMaxima()) vidaLlena = true;
                break;
            }
        }

        QString textoCuracion = vidaLlena ? T(idioma, "VIDA LLENA", "FULL HEALTH") : QString::number(precio3) + " pts";
        dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * 2,
                          T(idioma, "CURACION", "HEALING"),
                          T(idioma, "Restaura 50% HP", "Restores 50% HP"),
                          textoCuracion,
                          sel3, vidaLlena);
    }
    // === TIENDA DE CHICHES (MEJORAS) ===
    else if(tienda->getTipo() == TIENDA_CHICHES) {
        painter.drawText(panelX + 20, panelY + 35, T(idioma, "MEJORAS DE EQUIPAMIENTO", "EQUIPMENT UPGRADES"));
        painter.setPen(QColor(200, 220, 255));
        painter.setFont(QFont("Arial", 14));
        // en TIENDA_CHICHES
        painter.drawText(panelX + 20, panelY + 60, T(idioma, "El mecanico ofrece tecnologia de punta:", "The mechanic offers cutting-edge tech:"));

        // 1 grilla de 2 columnas para que los 17 items entren sin pisarse
        int itemY = panelY + 95;
        int filas = 9;
        int colAncho = 325;
        int filaAlto = 44;
        int espacioCol = 20;

        auto dibujarMejora = [&](int idx, QString nombre, int nivelActual, bool tieneElArma, int idItem) {
            bool esSeleccionado = (itemSeleccionadoTienda == idx);
            int precio = gamelogic.calcularPrecio(TIENDA_CHICHES, idItem);

            bool estaAlMaximo = (nivelActual >= 5);
            bool estaBloqueado = !tieneElArma;

            QString textoPrecio;
            bool desactivado = false;

            if (estaBloqueado) {
                textoPrecio = T(idioma, "BLOQUEADO", "LOCKED");
                desactivado = true;
            } else if (estaAlMaximo) {
                textoPrecio = T(idioma, "NIVEL MAX", "MAX LEVEL");
                desactivado = true;
            } else {
                textoPrecio = QString::number(precio) + " pts";
                desactivado = false;
            }

            QString nombreConNivel = QString("%1 (%2/5)").arg(nombre).arg(nivelActual);

            int col = idx / filas;
            int fila = idx % filas;
            int x = panelX + 20 + col * (colAncho + espacioCol);
            int y = itemY + fila * filaAlto;

            dibujarItemTienda(painter, x, y, nombreConNivel, "", textoPrecio, esSeleccionado, desactivado,
                              colAncho, filaAlto - 4);
        };

        // 2 renderizado de los 17 items, en 2 columnas de hasta 9 filas
        dibujarMejora(0, T(idioma, "RECARGA PISTOLA", "PISTOL RELOAD"),    gamelogic.getNivelRecargaPistola(), true, 1);
        dibujarMejora(1, T(idioma, "RECARGA FAL", "FAL RELOAD"),        gamelogic.getNivelRecargaFal(), gamelogic.tieneArma(2), 2);
        dibujarMejora(2, T(idioma, "RECARGA ESCOPETA", "SHOTGUN RELOAD"),   gamelogic.getNivelRecargaEscopeta(), gamelogic.tieneArma(5), 3);
        dibujarMejora(3, T(idioma, "RECARGA GRANADAS", "GRENADE RELOAD"),   gamelogic.getNivelRecargaGranadas(), gamelogic.tieneArma(3), 4);
        dibujarMejora(4, T(idioma, "RECARGA COHETES", "ROCKET RELOAD"),    gamelogic.getNivelRecargaCohetes(), gamelogic.tieneArma(4), 5);
        dibujarMejora(5, T(idioma, "CAPACIDAD PISTOLA", "PISTOL CAPACITY"),  gamelogic.getNivelCapacidadPistola(), true, 6);
        dibujarMejora(6, T(idioma, "CAPACIDAD FAL", "FAL CAPACITY"),      gamelogic.getNivelCapacidadFal(), gamelogic.tieneArma(2), 7);
        dibujarMejora(7, T(idioma, "CAPACIDAD ESCOPETA", "SHOTGUN CAPACITY"), gamelogic.getNivelCapacidadEscopeta(), gamelogic.tieneArma(5), 8);
        dibujarMejora(8, T(idioma, "CAPACIDAD GRANADAS", "GRENADE CAPACITY"), gamelogic.getNivelCapacidadGranadas(), gamelogic.tieneArma(3), 9);
        dibujarMejora(9, T(idioma, "CAPACIDAD COHETES", "ROCKET CAPACITY"),  gamelogic.getNivelCapacidadCohetes(), gamelogic.tieneArma(4), 10);
        dibujarMejora(10, T(idioma, "RANGO BALAS", "BULLET RANGE"),        gamelogic.getNivelRangoArmas(), true, 11);
        dibujarMejora(11, T(idioma, "RADIO EXPLOSION", "EXPLOSION RADIUS"),    gamelogic.getNivelRangoExplosion(), (gamelogic.tieneArma(3) || gamelogic.tieneArma(4)), 12);
        dibujarMejora(12, T(idioma, "DANO LLAMARADA", "FLAME DAMAGE"),     gamelogic.getNivelDanioLanzallamas(), gamelogic.tieneArma(6), 13);
        dibujarMejora(13, T(idioma, "ALCANCE LLAMARADA", "FLAME RANGE"),  gamelogic.getNivelAlcanceLanzallamas(), gamelogic.tieneArma(6), 14);
        dibujarMejora(14, T(idioma, "CADENCIA SIERRA", "SAW FIRE RATE"),    gamelogic.getNivelCadenciaSierra(), gamelogic.tieneArma(7), 15);
        dibujarMejora(15, T(idioma, "TAMANO SIERRA", "SAW SIZE"),      gamelogic.getNivelTamanioSierra(), gamelogic.tieneArma(7), 16);
        dibujarMejora(16, T(idioma, "CADENCIA ESCOPETA", "SHOTGUN FIRE RATE"),  gamelogic.getNivelCadenciaEscopeta(), gamelogic.tieneArma(5), 17);
    }
    // === TIENDA DE EXPANSION ===
    else if(tienda->getTipo() == TIENDA_EXPANSION) {
        painter.drawText(panelX + 20, panelY + 35, T(idioma, "INGENIERIA DE CAMPO", "FIELD ENGINEERING"));
        painter.setPen(QColor(200, 220, 255));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(panelX + 20, panelY + 60, T(idioma, "El ingeniero te ofrece:", "The engineer offers you:"));

        int itemY = panelY + 90;
        int lineHeight = 70;

        auto dibujarExp = [&](int idx, QString nombre, int nivelActual, int idItem) {
            bool sel = (itemSeleccionadoTienda == idx);
            bool max = (nivelActual >= EXPANSION_MAX);
            int precio = gamelogic.calcularPrecio(TIENDA_EXPANSION, idItem);

            QString descPrecio = max ? "MAX" : QString::number(precio) + " pts";
            QString descNombre = QString("%1 (Nv %2/%3)").arg(nombre).arg(nivelActual).arg(EXPANSION_MAX);

            dibujarItemTienda(painter, panelX + 20, itemY + lineHeight * idx,
                              descNombre, T(idioma, "+150 Espacio", "+150 Space"), descPrecio, sel, max);
        };

        dibujarExp(0, T(idioma, "EXPANSION DERECHA", "RIGHT EXPANSION"), gamelogic.getExpansionDerecha(), 1);
        dibujarExp(1, T(idioma, "EXPANSION IZQUIERDA", "LEFT EXPANSION"), gamelogic.getExpansionIzquierda(), 2);
        dibujarExp(2, T(idioma, "EXPANSION ARRIBA", "TOP EXPANSION"), gamelogic.getExpansionArriba(), 3);
        dibujarExp(3, T(idioma, "EXPANSION ABAJO", "BOTTOM EXPANSION"), gamelogic.getExpansionAbajo(), 4);
    }

    painter.setPen(QColor(150, 255, 150));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(panelX + 20, panelY + panelAlto - 40,
                     T(idioma, "Flechas Seleccionar    E - Comprar    ESC - Salir", "Arrows Select    E - Buy    ESC - Exit"));
    painter.setPen(QColor(255, 255, 100));
    painter.drawText(panelX + panelAncho - 200, panelY + panelAlto - 40,
                     T(idioma, "Tus puntos: ", "Your points: ") + QString::number(gamelogic.getPuntos()));
}

void Gamewidget::dibujarTutorial(QPainter &painter)
{
    int idioma = gamelogic.getIdioma();

    painter.fillRect(rect(), QColor(15, 20, 25));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 30, QFont::Bold));
    painter.drawText(0, 50, width(), 50, Qt::AlignHCenter, T(idioma, "CÓMO JUGAR", "HOW TO PLAY"));

    int centroX = width() / 2;
    int filaY = 150;

    // 1 teclas WASD dibujadas, para moverse
    int tam = 50;
    int espacio = 6;
    int wasdCentroX = centroX - 220;

    auto dibujarTecla = [&](int x, int y, QString letra) {
        painter.setBrush(QColor(50, 60, 70));
        painter.setPen(QPen(QColor(150, 180, 220), 2));
        painter.drawRoundedRect(x, y, tam, tam, 6, 6);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(x, y, tam, tam, Qt::AlignCenter, letra);
    };

    dibujarTecla(wasdCentroX, filaY, "W");
    dibujarTecla(wasdCentroX - tam - espacio, filaY + tam + espacio, "A");
    dibujarTecla(wasdCentroX, filaY + tam + espacio, "S");
    dibujarTecla(wasdCentroX + tam + espacio, filaY + tam + espacio, "D");

    painter.setPen(QColor(200, 220, 255));
    painter.setFont(QFont("Arial", 13));
    painter.drawText(wasdCentroX - tam - espacio, filaY + (tam + espacio) * 2 + 20, (tam * 3 + espacio * 2), 30,
                     Qt::AlignCenter, T(idioma, "Moverse", "Move"));

    // 2 mouse dibujado, con el boton izquierdo marcado
    int mouseX = centroX + 120;
    int mouseY = filaY;
    int mouseAncho = 70;
    int mouseAlto = 100;

    painter.setBrush(QColor(50, 60, 70));
    painter.setPen(QPen(QColor(150, 180, 220), 2));
    painter.drawRoundedRect(mouseX, mouseY, mouseAncho, mouseAlto, 30, 30);

    painter.setBrush(QColor(255, 210, 80));
    painter.setPen(Qt::NoPen);
    painter.drawPie(mouseX, mouseY, mouseAncho, mouseAlto / 2, 0 * 16, 180 * 16);

    painter.setPen(QPen(QColor(20, 25, 30), 2));
    painter.drawLine(mouseX + mouseAncho / 2, mouseY, mouseX + mouseAncho / 2, mouseY + mouseAlto / 2);

    painter.setPen(QColor(200, 220, 255));
    painter.setFont(QFont("Arial", 13));
    painter.drawText(mouseX - 40, mouseY + mouseAlto + 20, mouseAncho + 80, 60,
                     Qt::AlignCenter | Qt::TextWordWrap,
                     T(idioma, "Mantené click para disparar sin soltar", "Hold click to keep firing"));

    painter.setPen(QColor(255, 220, 80));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(0, filaY + 220, width(), 30, Qt::AlignCenter,
                     T(idioma, "0 al 7 para cambiar de arma", "0 to 7 to switch weapons"));

    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Arial", 14));
    painter.drawText(0, height() - 60, width(), 30, Qt::AlignCenter,
                     T(idioma, "Presioná ENTER para continuar", "Press ENTER to continue"));
}
void Gamewidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (enSeleccionIdioma) {
        dibujarSeleccionIdioma(painter);
        return;
    }
    if (enTutorial) {              // NUEVO
        dibujarTutorial(painter);
        return;
    }


    if (enSeleccionPersonaje) {
        dibujarSeleccionPersonaje(painter);
        return;
    }

    QList<Dibujable_abstracto*> &lista = gamelogic.Obtener_Dibujables();

    Sobreviviente* sobreviviente = nullptr;
    for (Dibujable_abstracto* d : lista) {
        if (auto* s = dynamic_cast<Sobreviviente*>(d)) {
            sobreviviente = s;
            break;
        }
    }

    if (sobreviviente) {
        float targetX = sobreviviente->get_x() + sobreviviente->get_ancho() / 2.0f - width() / 2.0f;
        float targetY = sobreviviente->get_y() + sobreviviente->get_alto() / 2.0f - height() / 2.0f;
        camPos = QPointF(targetX, targetY);
    }

    mouseWorldPos = camPos + mouseScreenPos;

    painter.save();
    painter.translate(-camPos.x(), -camPos.y());

    for (Dibujable_abstracto *d : lista) {
        if (d) d->dibujar(painter);
    }

    if (sobreviviente && sobreviviente->getArma()) {
        auto& proyectiles = sobreviviente->getArma()->getProyectiles();
        for (auto* p : proyectiles) {
            if (p && p->estaActivo()) p->dibujar(painter);
        }
    }
    for(auto* d : lista) {
        if(auto* zombieArmado = dynamic_cast<ZombieArmado*>(d)) {
            if(zombieArmado->estaVivo()) {
                auto& proyectiles = zombieArmado->getProyectiles();
                for(auto* p : proyectiles) {
                    if(p && p->estaActivo()) {
                        p->dibujar(painter);
                    }
                }
            }
        }
        // ---> NUEVO: Renderizado de proyectiles de AranoTek <---
        else if(auto* aranoTek = dynamic_cast<AranoTek*>(d)) {
            if(aranoTek->estaVivo()) {
                auto& proyectiles = aranoTek->getProyectiles();
                for(auto* p : proyectiles) {
                    if(p && p->estaActivo()) {
                        p->dibujar(painter);
                    }
                }
            }
        }

        else if(auto* invocador = dynamic_cast<Invocador*>(d)) {
            if(invocador->estaVivo()) {
                auto& proyectiles = invocador->getProyectiles();
                for (auto& p : proyectiles) {
                    p.dibujar(painter); // O la función de dibujado que uses (usando el punto '.' en vez de '->')
                }

            }
        }
    }
    for(auto* d : lista) {
        if(auto* boss = dynamic_cast<Xhaal*>(d)) {
            if(boss->estaVivo()) {
                // Proyectiles normales
                auto& proyectiles = boss->getProyectiles();
                for(auto* p : proyectiles) {
                    if(p && p->estaActivo()) {
                        p->dibujar(painter);
                    }
                }

                // Proyectiles buscadores (MAGENTA)
                auto& buscadores = boss->getProyectilesBuscadores();
                for(auto* pb : buscadores) {
                    if(pb && pb->estaActivo()) {
                        pb->dibujar(painter);
                    }
                }
            }
        }
    }
    painter.restore();
    // NUEVO: nieve cayendo, en espacio de pantalla
    painter.restore();

    // Nieve cayendo, en espacio de pantalla, con dos capas de profundidad
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(Qt::NoPen);
    for(const ParticulaNieve &copo : gamelogic.getParticulasNieve()) {
        float px = copo.x * width();
        float py = copo.y * height();
        int alfa = (copo.capa == 1) ? 235 : 140;
        painter.setBrush(QColor(255, 255, 255, alfa));
        painter.drawRect(QRectF(px, py, copo.tam, copo.tam));
    }
    painter.restore();
    int idioma = gamelogic.getIdioma();
    // HUD Principal


    // HUD Principal
    painter.fillRect(0, 0, width(), 85, QColor(20, 25, 35, 200));
    painter.setPen(QPen(QColor(100, 150, 200), 2));
    painter.drawLine(0, 85, width(), 85);

    painter.setPen(QColor(200, 220, 255));
    painter.setFont(QFont("Arial", 14, QFont::Bold));

    painter.drawText(20, 22, T(idioma, "PUNTOS: ", "SCORE: ") + QString::number(gamelogic.getPuntos()));

    if(sobreviviente) {
        painter.drawText(20, 45, T(idioma, "VIDA: ", "HEALTH: ") + QString::number((int)sobreviviente->getVida().getVidaActual()) +
                                     "/" + QString::number((int)sobreviviente->getVida().getVidaMaxima()));
    }

    QString armaActual = gamelogic.getNombreArmaActual();
    painter.drawText(200, 22, T(idioma, "ARMA: ", "WEAPON: ") + armaActual);

    if(sobreviviente && sobreviviente->getArma()) {
        Arma* arma = sobreviviente->getArma();

        painter.drawText(200, 45, T(idioma, "MUNICIÓN: ", "AMMO: ") +
                                      QString::number(arma->getMunicionActual()) + "/" +
                                      QString::number(arma->getMunicionMaxima()));

        if(arma->estaRecargando()) {
            int barraX = 200;
            int barraY = 55;
            int barraAncho = 100;
            int barraAlto = 8;

            painter.setBrush(QColor(80, 80, 80));
            painter.drawRect(barraX, barraY, barraAncho, barraAlto);

            float progreso = arma->getProgresoRecarga();
            painter.setBrush(QColor(255, 150, 0));
            painter.drawRect(barraX, barraY, barraAncho * progreso, barraAlto);

            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRect(barraX, barraY, barraAncho, barraAlto);

            painter.setPen(QColor(255, 200, 100));
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(barraX, barraY - 5, T(idioma, "RECARGANDO...", "RELOADING..."));
        }
    }

    // Stats del personaje actual
    if(sobreviviente) {
        EstadisticasPersonaje st = sobreviviente->getStats();
        painter.setFont(QFont("Arial", 10));
        painter.setPen(QColor(180, 220, 180));
        int stX = 430;
        painter.drawText(stX, 22, QString("%1: %2%").arg(T(idioma, "VEL", "SPD")).arg((int)(st.velocidadMovimiento * 100)));
        painter.drawText(stX, 40, QString("%1: %2%").arg(T(idioma, "RECARGA", "RELOAD")).arg((int)((1.0f / st.velocidadRecarga) * 100)));
        painter.drawText(stX, 58, QString("%1: %2%").arg(T(idioma, "RESIST", "RESIST")).arg((int)(st.resistenciaDanio * 100)));
        painter.drawText(stX, 76, QString("%1: %2").arg(T(idioma, "VID MAX", "MAX HP")).arg((int)st.vidaMaxima));
    }

    if (gamelogic.getEstado() == EN_DESCANSO) {
        painter.setPen(QColor(100, 255, 100));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(width() / 2 - 150, 28, T(idioma, "REFUGIO SEGURO", "SAFE SHELTER"));
        painter.setFont(QFont("Arial", 12));
        painter.drawText(width() / 2 - 150, 48, T(idioma, "Próxima horda en: ", "Next horde in: ") +
                                                    QString::number(gamelogic.getTiempoDescanso()) + "s");
    } else {
        if(gamelogic.getNivel() == NIVEL_BOSS_FINAL) { // ANTES: == 10
            painter.setPen(QColor(255, 50, 150));
            painter.setFont(QFont("Arial", 20, QFont::Bold));
            painter.drawText(width() / 2 - 150, 30, "🛸 " + T(idioma, "BOSS FINAL", "FINAL BOSS") + " 🛸");

            painter.setFont(QFont("Arial", 16, QFont::Bold));
            painter.setPen(QColor(150, 100, 255));
            painter.drawText(width() / 2 - 150, 55, "「 XHAAL 」"); // ANTES: 「 ELLOS 」
        } else {
            painter.setPen(QColor(255, 100, 100));
            painter.setFont(QFont("Arial", 16, QFont::Bold));
            painter.drawText(width() / 2 - 100, 28, T(idioma, "¡INVASIÓN - NIVEL ", "INVASION - LEVEL ") +
                                                        QString::number(gamelogic.getNivel()) + "!");
            painter.setFont(QFont("Arial", 12));
            painter.setPen(QColor(255, 200, 100));
            painter.drawText(width() / 2 - 100, 48, T(idioma, "¡Elimina a todos los invasores!", "Eliminate all the invaders!"));
        }
    }

    painter.setPen(QColor(150, 170, 200));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(width() - 220, 18, T(idioma, "WASD - Mover", "WASD - Move"));
    painter.drawText(width() - 220, 33, T(idioma, "Mouse + Click - Disparar", "Mouse + Click - Shoot"));
    painter.drawText(width() - 220, 48, T(idioma, "0 al 7 - Cambiar arma (0=Nada)", "0 to 7 - Switch weapon (0=None)"));
    painter.drawText(width() - 220, 63, T(idioma, "E - Comprar / ↑↓ - Navegar", "E - Buy / ↑↓ - Navigate"));
    painter.setPen(QColor(255, 220, 80));
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.drawText(width() - 220, 80, T(idioma, "P - Pausa", "P - Pause"));

    Tienda* tiendaCercana = gamelogic.getTiendaCercana();
    if(tiendaCercana && gamelogic.getEstado() == EN_DESCANSO) {
        dibujarMenuTienda(painter, tiendaCercana);
    }

    // OVERLAY DE PAUSA
    if (pausado) {
        painter.fillRect(rect(), QColor(0, 0, 0, 160));
        painter.setPen(QColor(255, 220, 80));
        painter.setFont(QFont("Arial", 52, QFont::Bold));
        painter.drawText(rect().adjusted(0, -60, 0, 0), Qt::AlignCenter, T(idioma, "PAUSA", "PAUSED"));
        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 18));
        painter.drawText(rect().adjusted(0, 40, 0, 0), Qt::AlignCenter, T(idioma, "Presioná P para continuar", "Press P to continue"));
    }

    // GAME OVER (prioridad media)
    if (gamelogic.juegoTerminado()) {
        painter.fillRect(rect(), QColor(0, 0, 0, 200));
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 40, QFont::Bold));
        painter.drawText(rect().adjusted(0, -50, 0, 0), Qt::AlignCenter,
                         "GAME OVER");

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24));
        painter.drawText(rect().adjusted(0, 30, 0, 0), Qt::AlignCenter,
                         T(idioma, "TOTAL: ", "TOTAL: ") + QString::number(gamelogic.getPuntos()));

        painter.setPen(QColor(255, 255, 100));
        painter.setFont(QFont("Arial", 18, QFont::Bold));
        painter.drawText(rect().adjusted(0, 100, 0, 0), Qt::AlignCenter,
                         T(idioma, "Presiona 'O' para volver a jugar", "Press 'O' to play again"));
    }

    // VICTORIA NIVEL FINAL (solo si llego al nivel final en descanso - NO es el mensaje final)
    if (gamelogic.getNivel() == NIVEL_BOSS_FINAL && // ANTES: == 10
        gamelogic.getMensajeFinalVisible() &&
        gamelogic.getTiempoMensaje() > 0) {
        painter.fillRect(rect(), QColor(0, 0, 0, 220));

        painter.setPen(QColor(255, 215, 0));
        painter.setFont(QFont("Arial", 48, QFont::Bold));
        painter.drawText(rect().adjusted(0, -100, 0, 0), Qt::AlignCenter, T(idioma, "¡VICTORIA!", "VICTORY!"));

        painter.setPen(QColor(150, 255, 150));
        painter.setFont(QFont("Arial", 24));
        painter.drawText(rect().adjusted(0, 0, 0, 0), Qt::AlignCenter,
                         T(idioma, "Has derrotado a XHAAL", "You have defeated XHAAL")); // ANTES: "Has derrotado a  ELLOS "

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 20));
        painter.drawText(rect().adjusted(0, 80, 0, 0), Qt::AlignCenter,
                         T(idioma, "PUNTUACIÓN FINAL: ", "FINAL SCORE: ") + QString::number(gamelogic.getPuntos()));

        painter.setPen(QColor(255, 200, 100));
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect().adjusted(0, 140, 0, 0), Qt::AlignCenter,
                         T(idioma, "Gracias por jugar - Mutants Survivor", "Thanks for playing - Mutants Survivor"));
        painter.setPen(QColor(255, 255, 100));
        painter.setFont(QFont("Arial", 20, QFont::Bold));

        int alphaParpadeo = 150 + (int)(105 * sin(gamelogic.getTiempoMensaje() * 4.0f));
        painter.setPen(QColor(255, 255, 100, alphaParpadeo));

        painter.drawText(rect().adjusted(0, 260, 0, 0), Qt::AlignCenter,
                         T(idioma, "Presiona 'O' para volver a jugar", "Press 'O' to play again"));


    }

    // MENSAJE FINAL DEL BOSS (MÁXIMA PRIORIDAD - SE DIBUJA ENCIMA DE TODO)
    if (gamelogic.getMensajeFinalVisible()) {
        painter.fillRect(rect(), QColor(0, 0, 0, 240));

        int alpha = 200 + (int)(55 * sin(gamelogic.getTiempoMensaje() * 3.0f));

        painter.setPen(QColor(255, 50, 150, alpha));
        painter.setFont(QFont("Arial", 42, QFont::Bold));
        painter.drawText(rect().adjusted(0, -150, 0, 0), Qt::AlignCenter,
                         T(idioma, "VICTORIA FINAL", "FINAL VICTORY"));

        painter.setPen(QPen(QColor(255, 100, 200, 150), 3));
        painter.drawRect(width()/2 - 350, height()/2 - 60, 700, 120);

        painter.setPen(QColor(255, 200, 255));
        painter.setFont(QFont("Arial", 22, QFont::Bold));
        painter.drawText(rect().adjusted(0, -20, 0, 0), Qt::AlignCenter,
                         QString("\"%1\"").arg(gamelogic.getMensajeFinal()));

        painter.setPen(QColor(255, 255, 100));
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect().adjusted(0, 100, 0, 0), Qt::AlignCenter,
                         T(idioma, "PUNTUACIÓN FINAL: ", "FINAL SCORE: ") + QString::number(gamelogic.getPuntos()));

        painter.setPen(QColor(200, 200, 200));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect().adjusted(0, 180, 0, 0), Qt::AlignCenter,
                         "Mutants Survivor - 2026");
    }

}

void Gamewidget::keyPressEvent(QKeyEvent *event)
{
    if (enSeleccionIdioma) {          // NUEVO
        if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Left) {
            idiomaSeleccionadoTemp = (idiomaSeleccionadoTemp == IDIOMA_ES) ? IDIOMA_EN : IDIOMA_ES;
            update();
            return;
        }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            gamelogic.setIdioma(idiomaSeleccionadoTemp);
            enSeleccionIdioma = false;
            update();
            return;
        }
        return;
    }
    if (enTutorial) {              // NUEVO
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            enTutorial = false;
            update();
            return;
        }
        return;
    }
    if (event->key() == Qt::Key_O) {
        if (gamelogic.juegoTerminado() || gamelogic.getMensajeFinalVisible()) {
            reiniciarJuego();
            return;
        }
    }

    // Tecla P: pausa/continuar
    if (event->key() == Qt::Key_P && !enSeleccionPersonaje) {
        pausado = !pausado;
        return;
    }
    // Logica de seleccion de personaje (orden igual al mostrado en pantalla)
    if (enSeleccionPersonaje) {
        const int cantidadSkins = 5;
        const int ordenSkins[cantidadSkins] = {SKIN_SURVIVOR, SKIN_RUNNER, SKIN_MILITAR, SKIN_Argentine, SKIN_Exterminador};
        int indiceActual = 0;
        for (int i = 0; i < cantidadSkins; i++) {
            if (ordenSkins[i] == skinSeleccionada) {
                indiceActual = i;
                break;
            }
        }
        if (event->key() == Qt::Key_Right) {
            indiceActual = (indiceActual + 1) % cantidadSkins;
            if (ordenSkins[indiceActual] == SKIN_Exterminador && !gamelogic.juegoFueGanado()) {
                indiceActual = (indiceActual + 1) % cantidadSkins;
            }
            skinSeleccionada = ordenSkins[indiceActual];
            update();
            return;
        }
        if (event->key() == Qt::Key_Left) {
            indiceActual = (indiceActual - 1 + cantidadSkins) % cantidadSkins;
            if (ordenSkins[indiceActual] == SKIN_Exterminador && !gamelogic.juegoFueGanado()) {
                indiceActual = (indiceActual - 1 + cantidadSkins) % cantidadSkins;
            }
            skinSeleccionada = ordenSkins[indiceActual];
            update();
            return;
        }
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            // NUEVO: no se puede confirmar un personaje bloqueado
            if (skinSeleccionada == SKIN_Exterminador && !gamelogic.juegoFueGanado()) {
                return;
            }
            enSeleccionPersonaje = false;
            gamelogic.setSkinSobreviviente(skinSeleccionada);
            return;
        }
    }

    // NUEVO: Tecla U para saltar descanso (Skip Wave)
    if (event->key() == Qt::Key_U) {
        if (gamelogic.getEstado() == EN_DESCANSO) {
            // Nota: iniciarNivel debe ser público en Gamelogic
            gamelogic.iniciarNivel(gamelogic.getNivel() + 1);
        }
    }

    // Lógica de Tienda (sin cambios mayores)

    Tienda* tiendaCercana = gamelogic.getTiendaCercana();
    if(tiendaCercana && gamelogic.getEstado() == EN_DESCANSO) {
        int maxItems = 2;  // Por defecto VIDA tiene 3 items (índices 0,1,2)

        if(tiendaCercana->getTipo() == TIENDA_ARMAS) {
            maxItems = 5;  // 6 items (índices 0-5)
        } else if(tiendaCercana->getTipo() == TIENDA_CHICHES) {
            maxItems = 16;   // 17 items (índices 0-16)
        } else if(tiendaCercana->getTipo() == TIENDA_EXPANSION) {
            maxItems = 3;  // 4 items (índices 0-3)
        }

        if(event->key() == Qt::Key_Up) {
            itemSeleccionadoTienda--;
            if(itemSeleccionadoTienda < 0) itemSeleccionadoTienda = maxItems;
            return;
        } else if(event->key() == Qt::Key_Down) {
            itemSeleccionadoTienda++;
            if(itemSeleccionadoTienda > maxItems) itemSeleccionadoTienda = 0;
            return;
        } else if(event->key() == Qt::Key_E) {
            gamelogic.comprarItem(itemSeleccionadoTienda + 1);
            return;
        } else if(event->key() == Qt::Key_Escape) {
            return;
        }
    }

    // Cambio de armas (sin cambios)
    // Cambio de armas
    if (event->key() == Qt::Key_1) {
        gamelogic.cambiarArma(1);
    } else if (event->key() == Qt::Key_2) {
        if(gamelogic.tieneArma(2)) gamelogic.cambiarArma(2);
    } else if (event->key() == Qt::Key_3) {
        if(gamelogic.tieneArma(3)) gamelogic.cambiarArma(3);
    } else if (event->key() == Qt::Key_4) {
        if(gamelogic.tieneArma(4)) gamelogic.cambiarArma(4);
    } else if (event->key() == Qt::Key_5) {
        if(gamelogic.tieneArma(5)) gamelogic.cambiarArma(5);
    } else if (event->key() == Qt::Key_6) {
        if(gamelogic.tieneArma(6)) gamelogic.cambiarArma(6);
    } else if (event->key() == Qt::Key_7) {
        if(gamelogic.tieneArma(7)) gamelogic.cambiarArma(7);
    }
    // ---> NUEVO: Tecla 0 para desequipar arma <---
    else if (event->key() == Qt::Key_0) {
        gamelogic.cambiarArma(0);
    }

    teclasPresionadas.insert(event->key());

    if(tiendaCercana && gamelogic.getEstado() == EN_DESCANSO) {
        int maxItems = 2;  // Por defecto para ARMAS y VIDA
        if(tiendaCercana->getTipo() == TIENDA_CHICHES) {
            maxItems = 16;  // 17 items (0-16)
        } else if(tiendaCercana->getTipo() == TIENDA_EXPANSION) {
            maxItems = 3;  // 4 items (0-3)
        }

        if(event->key() == Qt::Key_Up) {
            itemSeleccionadoTienda--;
            if(itemSeleccionadoTienda < 0) itemSeleccionadoTienda = maxItems;
            return;
        } else if(event->key() == Qt::Key_Down) {
            itemSeleccionadoTienda++;
            if(itemSeleccionadoTienda > maxItems) itemSeleccionadoTienda = 0;
            return;
        } else if(event->key() == Qt::Key_E) {
            gamelogic.comprarItem(itemSeleccionadoTienda + 1);
            return;
        } else if(event->key() == Qt::Key_Escape) {
            return;
        }
    }
}

void Gamewidget::keyReleaseEvent(QKeyEvent *event)
{
    // Quitar tecla del conjunto de teclas presionadas
    teclasPresionadas.remove(event->key());
}

void Gamewidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseScreenPos = QPointF(event->pos().x(), event->pos().y());
}

void Gamewidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !enSeleccionPersonaje) {
        mousePresionado = true;
    }
}
void Gamewidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mousePresionado = false;
    }
}
// gamewidget.cpp

void Gamewidget::dibujarCheckpoint(QPainter &painter)
{
    if(!gamelogic.getCheckpointVisible()) return;

    painter.save();
    int anchoBanner = 500;
    int altoBanner = 60;
    int x = (width() - anchoBanner) / 2;
    int y = 40;

    painter.setBrush(QColor(20, 20, 30, 210));
    painter.setPen(QPen(QColor(255, 215, 0), 2));
    painter.drawRoundedRect(x, y, anchoBanner, altoBanner, 10, 10);

    painter.setPen(QColor(255, 215, 0));
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(QRect(x, y, anchoBanner, altoBanner), Qt::AlignCenter,
                     gamelogic.getCheckpointTexto());
    painter.restore();
}
