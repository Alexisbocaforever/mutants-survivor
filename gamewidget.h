#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSet>
#include <QPointF>
#include "gamelogic.h"
#include "sobreviviente.h"

class Gamewidget : public QWidget
{
    Q_OBJECT
public:
    explicit Gamewidget(Gamelogic &gamelogic, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;
    void Crear_ventana();


private:
    QTimer *timer_principal;
    Gamelogic &gamelogic;
    QSet<int> teclasPresionadas;
    QPointF camPos;
    QPointF mouseScreenPos;
    QPointF mouseWorldPos;
    // gamewidget.h, agregar en private:
    void dibujarCheckpoint(QPainter &painter);

    // Sistema de selección de personaje
    bool enSeleccionPersonaje;
    bool enSeleccionIdioma;          // NUEVO
    int idiomaSeleccionadoTemp;      // NUEVO
    bool enTutorial; // NUEVO
    void dibujarTutorial(QPainter &painter); // NUEVO
    void dibujarSeleccionIdioma(QPainter &painter); // NUEVO
    int skinSeleccionada;
    // Sistema de menú de tienda
    int itemSeleccionadoTienda;

    // Bandera para disparo continuo
    bool mousePresionado;
    // Pausa
    bool pausado;

    void dibujarSeleccionPersonaje(QPainter &painter);
    void dibujarPreviewPersonaje(QPainter &painter, int skin, int x, int y, bool seleccionado, bool bloqueado = false);
    void dibujarMenuTienda(QPainter &painter, Tienda* tienda);

    void dibujarItemTienda(QPainter &painter, int x, int y, QString nombre, QString desc,
                           QString precio, bool seleccionado, bool yaComprado,
                           int itemAncho = 560, int itemAlto = 60);
    void reiniciarJuego();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    // NUEVO: Necesario para saber cuándo dejar de disparar
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // GAMEWIDGET_H
