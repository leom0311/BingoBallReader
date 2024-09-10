#ifndef LICENCIA_H
#define LICENCIA_H

#include <QString>

class Licencia
{
public:
    Licencia();
    QString calcularCRC16(int num1, int num2, int num3, int num4);
};

#endif // LICENCIA_H
