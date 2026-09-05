#ifndef COLISION_H
#define COLISION_H

#include "dibujable_abstracto.h"

class Colision
{
public:
    static bool hayColision(Dibujable_abstracto* obj1, Dibujable_abstracto* obj2);
    static bool hayColision(float x1, float y1, float ancho1, float alto1,
                            float x2, float y2, float ancho2, float alto2);
    static bool hayColisionCircular(float x1, float y1, float radio1,
                                    float x2, float y2, float radio2);
    static void resolverColision(Dibujable_abstracto* obj1, Dibujable_abstracto* obj2);
    static float calcularDistancia(float x1, float y1, float x2, float y2);
};

#endif // COLISION_H
