#ifndef CARTON_H
#define CARTON_H

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QDialog>
#include <algorithm>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

const int FILAS=3;
const int COLUMNAS=9;
const int FILASREALES=3;
const int COLUMNASREALES=5;

struct Numero{
    int n;
    bool tachado;
};

struct OrdenLinea{
    int f1, f2, f3;
};

class Carton : public QWidget
{
private:
    int id;
    Numero matriz[FILAS][COLUMNAS];
    int tachados;
    QGroupBox *groupBox;
    bool estaTachado(int n);
public:
    Carton();
    Carton(int id, int m[FILASREALES][COLUMNASREALES]);
    //Busca el número y devuelve cuántos le quedan para cantar bingo.
    int tacharNumero(int n);
    //Quita a un número de estar tachado
    void destacharNumero(int n);
    //Devuelve la cantidad de números tachados
    int obtenerTachados();
    //Devuelve el dibujo del cartón en un groupbox
    QGroupBox *pintarCarton();
    //Tacha con una x roja los números que ya han salido
    void tacharNumerosEnDibujo();
    int obtenerId();
    bool hayLinea();
    bool hayBingo();
    OrdenLinea obtenerRegistroLinea();

};

#endif // CARTON_H
