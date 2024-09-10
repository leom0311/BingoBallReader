#include "ordenaciones.h"


void Ordenaciones::ordenarLinea(QList<Carton *> &listaCartonesEnJuego)
{
    qSort(listaCartonesEnJuego.begin(), listaCartonesEnJuego.end(),
             [](Carton* a, Carton* b) {

        OrdenLinea la = a->obtenerRegistroLinea();
        OrdenLinea lb = b->obtenerRegistroLinea();

        if(la.f1==lb.f1){
            if(la.f2==lb.f2){
                if(la.f3==lb.f3){
                    return a->obtenerId() < b->obtenerId();
                }
                else{
                    return la.f3 < lb.f3;
                }
            }
            else{
                return la.f2 < lb.f2;
            }
        }
        else{
            return la.f1 < lb.f1;
        }

             });
}

void Ordenaciones::ordenarBingo(QList<Carton *> &listaCartonesEnJuego)
{
    qSort(listaCartonesEnJuego.begin(), listaCartonesEnJuego.end(),
             [](Carton* a, Carton* b) {
        if (a->obtenerTachados() == b->obtenerTachados()) {
                          // En caso de empate, compara por obtenerId
                          return a->obtenerId() < b->obtenerId();
                      }
                      // Compara por obtenerTachados en otros casos
            return a->obtenerTachados() > b->obtenerTachados();

             });
}
