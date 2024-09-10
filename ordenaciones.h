#ifndef ORDENACIONES_H
#define ORDENACIONES_H

#include "Carton.h"
#include <QList>

class Ordenaciones
{
public:
    static void ordenarLinea(QList<Carton*> &listaCartonesEnJuego);
    static void ordenarBingo(QList<Carton*> &listaCartonesEnJuego);
};

#endif // ORDENACIONES_H
