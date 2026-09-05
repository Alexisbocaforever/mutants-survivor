#ifndef ESCENARIO_H
#define ESCENARIO_H
#include "dibujable_abstracto.h"
#include <QList>
#include "pared.h"

class Escenario:public Dibujable_abstracto
{
public:
    Escenario();
    float x_escenario;
    float y_escenario;
    float ancho_escenario;
    float alto_escenario;

    void agregarPared(Pared* pared);
    QList<Pared*>& obtenerParedes();
    void dibujarParedes(QPainter &p);

protected:
     QList<Pared*> paredes;

};

#endif // ESCENARIO_H
