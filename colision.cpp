#include "colision.h"
#include <cmath>
#include <algorithm>

bool Colision::hayColision(Dibujable_abstracto* obj1, Dibujable_abstracto* obj2)
{
    if (!obj1 || !obj2) return false;

    return hayColision(obj1->get_x(), obj1->get_y(), obj1->get_ancho(), obj1->get_alto(),
                       obj2->get_x(), obj2->get_y(), obj2->get_ancho(), obj2->get_alto());
}

bool Colision::hayColision(float x1, float y1, float ancho1, float alto1,
                           float x2, float y2, float ancho2, float alto2)
{
    return (x1 < x2 + ancho2 &&
            x1 + ancho1 > x2 &&
            y1 < y2 + alto2 &&
            y1 + alto1 > y2);
}

bool Colision::hayColisionCircular(float x1, float y1, float radio1,
                                   float x2, float y2, float radio2)
{
    float distancia = calcularDistancia(x1, y1, x2, y2);
    return distancia < (radio1 + radio2);
}

void Colision::resolverColision(Dibujable_abstracto* obj1, Dibujable_abstracto* obj2)
{
    if (!obj1 || !obj2) return;

    float x1 = obj1->get_x();
    float y1 = obj1->get_y();
    float ancho1 = obj1->get_ancho();
    float alto1 = obj1->get_alto();

    float x2 = obj2->get_x();
    float y2 = obj2->get_y();
    float ancho2 = obj2->get_ancho();
    float alto2 = obj2->get_alto();

    float centro1_x = x1 + ancho1 / 2.0f;
    float centro1_y = y1 + alto1 / 2.0f;
    float centro2_x = x2 + ancho2 / 2.0f;
    float centro2_y = y2 + alto2 / 2.0f;

    float dx = centro1_x - centro2_x;
    float dy = centro1_y - centro2_y;

    float overlapX = (ancho1 + ancho2) / 2.0f - std::abs(dx);
    float overlapY = (alto1 + alto2) / 2.0f - std::abs(dy);

    if (overlapX < overlapY) {
        if (dx > 0) {
            x1 += overlapX;
        } else {
            x1 -= overlapX;
        }
    } else {
        if (dy > 0) {
            y1 += overlapY;
        } else {
            y1 -= overlapY;
        }
    }
}

float Colision::calcularDistancia(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}
