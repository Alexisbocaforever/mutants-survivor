// pool_entidades.h
#ifndef POOL_ENTIDADES_H
#define POOL_ENTIDADES_H

#include <QList>

// Pool generico: precrea objetos y los reutiliza en vez de new/delete en el loop.
// T necesita constructor por defecto y su propio metodo reiniciar(...) para
// reconfigurarse cada vez que se reutiliza (ver Proyectil::reiniciar mas abajo).
template <typename T>
class PoolEntidades
{
public:
    explicit PoolEntidades(int capacidad = 20)
    {
        for (int i = 0; i < capacidad; i++) {
            libres.append(new T());
        }
    }

    ~PoolEntidades()
    {
        qDeleteAll(libres);
        qDeleteAll(enUso);
    }

    // Saca una entidad libre. Si el pool se quedo corto, agranda de a 10
    // en vez de devolver nullptr (asi nunca faltan balas en oleadas grandes).
    T* obtener()
    {
        if (libres.isEmpty()) {
            for (int i = 0; i < 10; i++) libres.append(new T());
        }
        T* entidad = libres.takeLast();
        enUso.append(entidad);
        return entidad;
    }

    // Devuelve la entidad al pool en vez de destruirla
    void liberar(T* entidad)
    {
        if (enUso.removeOne(entidad)) {
            libres.append(entidad);
        }
    }

    int getEnUso() const { return enUso.size(); }

private:
    QList<T*> libres;
    QList<T*> enUso;
};

#endif // POOL_ENTIDADES_H
