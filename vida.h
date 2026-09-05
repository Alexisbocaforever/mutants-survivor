#ifndef VIDA_H
#define VIDA_H

class Vida
{
public:
    Vida(float vidaMaxima = 100.0f);

    void recibirDanio(float cantidad);
    void curar(float cantidad);
    void restaurarCompleta();

    float getVidaActual() const;
    float getVidaMaxima() const;
    float getPorcentajeVida() const;
    bool estaVivo() const;
    bool estaCritico() const;

    void setVidaMaxima(float nuevaVidaMaxima);

private:
    float vidaActual;
    float vidaMaxima;
};

#endif // VIDA_H
