#include "lanzallamas.h"
#include <cstdlib>
#include <ctime>
#include <QPainter>

Lanzallamas::Lanzallamas()
    : disparando(false), anguloDisparo(0.0f), alcanceMaximo(200.0f),
    anchoConoInicial(20.0f), anchoConoFinal(100.0f),
    danioTick(8.0f), // Daño por tick del lanzallamas
    tiempoEntreParticulas(0.02f), contadorParticulas(0.0f)
{
    ancho = 40.0f;
    alto = 12.0f;
    municionMaxima = 100;
    municionMaximaBase = 100;
    municionActual = 100;
    tiempoRecargaMax = 3.0f;
}

Lanzallamas::~Lanzallamas()
{
    qDeleteAll(particulas);
    particulas.clear();
}

void Lanzallamas::dibujar(QPainter &painter)
{
    painter.save();

    // Primero dibujar las llamas (atrás del arma)
    if (!particulas.isEmpty()) {
        dibujarLlamarada(painter);
    }

    painter.translate(pos_x, pos_y);
    painter.rotate(angulo);
    painter.setPen(Qt::NoPen);

    // --- ARMA BASE DETALLADA ---
    // Tanque principal de combustible
    painter.setBrush(QColor(120, 30, 30));
    painter.drawRect(-18, -6, 12, 12);
    // Relieves del tanque (Luz y sombra)
    painter.setBrush(QColor(150, 50, 50)); // Luz
    painter.drawRect(-17, -5, 10, 3);
    painter.setBrush(QColor(80, 20, 20));  // Sombra
    painter.drawRect(-17, 4, 10, 2);

    // Conexión y manguera corrugada
    painter.setBrush(QColor(40, 40, 45));
    painter.drawRect(-6, -3, 6, 6);
    painter.setPen(QPen(QColor(20, 20, 20), 1));
    painter.drawLine(-4, -3, -4, 3);
    painter.drawLine(-2, -3, -2, 3);
    painter.setPen(Qt::NoPen);

    // Cuerpo principal
    painter.setBrush(QColor(70, 75, 80));
    painter.drawRect(0, -5, 16, 10);

    // Empuñaduras
    painter.setBrush(QColor(40, 40, 45));
    painter.drawRect(2, 5, 4, 6);  // Empuñadura gatillo
    painter.drawRect(10, 5, 4, 6); // Agarre frontal

    float extensionBoquilla = nivelAlcanceLanzallamas * 4.0f;
    painter.setBrush(QColor(50, 50, 55));
    painter.drawRect(16, -4, 6 + extensionBoquilla, 8);
    painter.setBrush(QColor(20, 20, 20));
    painter.drawRect(22 + extensionBoquilla, -3, 3, 6);

    // Aros de refuerzo termico visibles a partir del nivel 2 de alcance
    if (nivelAlcanceLanzallamas >= 2) {
        painter.setPen(QPen(QColor(150, 150, 160), 1));
        for (int i = 0; i < nivelAlcanceLanzallamas - 1; i++) {
            painter.drawLine(18 + i * 6, -4, 18 + i * 6, 4);
        }
        painter.setPen(Qt::NoPen);
    }

    // Nucleo de combustion: se pone mas caliente/brillante con cada nivel de danio
    if (nivelDanioLanzallamas > 0) {
        QColor colorCalor(255, std::max(40, 140 - nivelDanioLanzallamas * 25), 0);
        painter.setBrush(colorCalor);
        painter.setPen(QPen(Qt::black, 1));
        painter.drawEllipse(-16, -8, 4, 4);
        painter.drawEllipse(-16, 2, 4, 4);
        painter.setPen(Qt::NoPen);
    }

    // --- MEJORAS VISUALES POR MUNICIÓN (CAPACIDAD DEL TANQUE) ---
    bool luzPilotoAzul = false;

    if (municionMaxima > municionMaximaBase && municionMaxima < 150) {
        // Nivel 1: Medidores de presión y refuerzos
        // Manómetro iluminado
        painter.setBrush(QColor(200, 200, 220)); // Esfera blanca
        painter.drawEllipse(-12, -10, 6, 6);
        painter.setBrush(QColor(255, 50, 50));   // Aguja roja
        painter.drawRect(-10, -9, 1, 3);
        painter.setBrush(QColor(40, 40, 40));    // Borde
        painter.drawEllipse(-13, -11, 8, 8);

        // Placas protectoras
        painter.setBrush(QColor(90, 95, 100));
        painter.drawRect(4, -6, 8, 2);
    }
    else if (municionMaxima >= 150) {
        // Nivel 2: Tanques pesados gemelos, boquilla gruesa, piloto azul
        luzPilotoAzul = true;

        // Tanque extra superior (Amarillo peligroso)
        painter.setBrush(QColor(200, 150, 0));
        painter.drawRect(-18, -14, 12, 8);
        painter.setBrush(QColor(120, 90, 0));
        painter.drawRect(-17, -13, 10, 2); // Detalle del tanque superior

        // Manguera gruesa interconectando tanques
        painter.setBrush(QColor(30, 30, 30));
        painter.drawRect(-10, -6, 4, 4);

        // Armadura térmica en el cuerpo del arma
        painter.setBrush(QColor(50, 55, 60));
        painter.drawPolygon(QPolygonF() << QPointF(0, -8) << QPointF(16, -8) << QPointF(14, -5) << QPointF(0, -5));

        // Boquilla de expansión
        painter.setBrush(QColor(80, 40, 40)); // Aleación resistente al calor extremo
        painter.drawRect(18, -6, 8, 12);
        painter.setBrush(QColor(20, 20, 20));
        painter.drawRect(26, -5, 4, 10);
    }

    // Luz piloto dinámica (si está disparando)
    if (disparando) {
        if (luzPilotoAzul) {
            // Llama de combustión de alta eficiencia
            painter.setBrush(QColor(50, 150, 255));
            painter.drawEllipse(QPointF(28, 0), 3, 3);
            painter.setBrush(QColor(200, 240, 255)); // Brillo interior
            painter.drawEllipse(QPointF(28, 0), 1, 1);
        } else {
            // Llama estándar
            painter.setBrush(QColor(255, 150, 0));
            painter.drawEllipse(QPointF(26, 0), 3, 3);
            painter.setBrush(QColor(255, 255, 150));
            painter.drawEllipse(QPointF(26, 0), 1, 1);
        }
    }

    painter.restore();
}

void Lanzallamas::dibujarLlamarada(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Ordenar partículas de atrás hacia adelante para mejor efecto
    for (auto* p : particulas) {
        float progreso = 1.0f - (p->tiempoVida / p->tiempoVidaMax);
        float alpha = (1.0f - progreso) * 255.0f;

        // Color que cambia de amarillo brillante a rojo oscuro
        QColor color;
        if (progreso < 0.3f) {
            // Amarillo brillante al inicio
            color = QColor(255, 255, 100, alpha);
        } else if (progreso < 0.6f) {
            // Naranja
            color = QColor(255, 150, 0, alpha);
        } else {
            // Rojo oscuro al final
            color = QColor(200, 50, 0, alpha * 0.6f);
        }

        // Dibujar partícula con gradiente radial
        QRadialGradient gradiente(p->x, p->y, p->tamano);
        gradiente.setColorAt(0, color);
        gradiente.setColorAt(0.5, QColor(color.red(), color.green(), color.blue(), alpha * 0.5));
        gradiente.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 0));

        painter.setBrush(gradiente);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(p->x, p->y), p->tamano, p->tamano);

        // Añadir pequeñas chispas ocasionales
        if (rand() % 10 < 3) {
            painter.setBrush(QColor(255, 255, 200, alpha * 0.8));
            painter.drawEllipse(QPointF(p->x + (rand() % 6 - 3),
                                        p->y + (rand() % 6 - 3)),
                                1, 1);
        }
    }

    painter.restore();
}

void Lanzallamas::actualizar(float posX, float posY, QPointF mouseWorld)
{
    pos_x = posX;
    pos_y = posY;

    float dx = mouseWorld.x() - pos_x;
    float dy = mouseWorld.y() - pos_y;
    angulo = std::atan2(dy, dx) * 180.0f / M_PI;
    anguloDisparo = angulo;

    actualizarRecarga();

    if (municionActual <= 0 && !recargando) {
        iniciarRecarga();
        disparando = false;
    }

    actualizarParticulas();
}

void Lanzallamas::disparar()
{
    if (municionActual > 0 && !recargando) {
        disparando = true;

        contadorParticulas += 0.016f;

        if (contadorParticulas >= tiempoEntreParticulas) {
            crearParticulasFuego(pos_x, pos_y);
            municionActual -= 1; // Consume munición continuamente
            contadorParticulas = 0.0f;
        }
    } else {
        disparando = false;
    }
}

void Lanzallamas::crearParticulasFuego(float origenX, float origenY)
{
    // Crear varias partículas por disparo para efecto denso
    int numParticulas = 5 + rand() % 3;

    for (int i = 0; i < numParticulas; i++) {
        // Ángulo de dispersión (forma de cono)
        float anguloRad = anguloDisparo * M_PI / 180.0f;
        float dispersion = (rand() % 60 - 30) * M_PI / 180.0f; // ±30 grados
        float anguloFinal = anguloRad + dispersion;

        // Velocidad variable
        float velocidad = 8.0f + (rand() % 40) / 10.0f;

        float vel_x = cos(anguloFinal) * velocidad;
        float vel_y = sin(anguloFinal) * velocidad;

        // Distancia desde el origen
        float distanciaInicial = 25.0f + (rand() % 10);
        float x = origenX + cos(anguloRad) * distanciaInicial;
        float y = origenY + sin(anguloRad) * distanciaInicial;

        // Tiempo de vida variable
        float factorAlcance = alcanceMaximo / 200.0f; // 200 es el alcance base del constructor
        float vida = (0.3f + (rand() % 20) / 100.0f) * factorAlcance;

        // Tamaño variable
        float tamano = 8.0f + (rand() % 12);

        ParticulaFuego* p = new ParticulaFuego(x, y, vel_x, vel_y, vida, tamano, anguloFinal);
        particulas.append(p);
    }
}

void Lanzallamas::actualizarParticulas()
{
    for (int i = particulas.size() - 1; i >= 0; i--) {
        ParticulaFuego* p = particulas[i];

        // Actualizar posición
        p->x += p->vel_x;
        p->y += p->vel_y;

        // Aplicar ligera desaceleración y efecto de subir (el fuego sube)
        p->vel_x *= 0.95f;
        p->vel_y *= 0.95f;
        p->vel_y -= 0.1f; // Efecto de convección (sube)

        // Aumentar tamaño ligeramente (el fuego se expande)
        p->tamano *= 1.02f;

        // Reducir tiempo de vida
        p->tiempoVida -= 0.016f;

        // Eliminar si expiró
        if (p->tiempoVida <= 0) {
            delete particulas[i];
            particulas.removeAt(i);
        }
    }
}

void Lanzallamas::actualizarProyectiles()
{
    // El lanzallamas no usa proyectiles tradicionales
    actualizarParticulas();
}

QList<Proyectil*>& Lanzallamas::getProyectiles()
{
    return proyectiles; // Lista vacía, pero necesaria por la interfaz
}

QList<QPointF> Lanzallamas::getAreaEfecto() const
{
    QList<QPointF> posiciones;

    // Retornar las posiciones de todas las partículas activas
    for (auto* p : particulas) {
        posiciones.append(QPointF(p->x, p->y));
    }

    return posiciones;
}

float Lanzallamas::get_x() { return pos_x; }
float Lanzallamas::get_y() { return pos_y; }
float Lanzallamas::get_ancho() { return ancho; }
float Lanzallamas::get_alto() { return alto; }
