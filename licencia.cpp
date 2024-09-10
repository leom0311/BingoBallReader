#include "licencia.h"

QString Licencia::calcularCRC16(int num1, int num2, int num3, int num4) {
    // Concatenar los tres números en una cadena
    QString data = QString::number(num1) + QString::number(num2) + QString::number(num3) + QString::number(num4);

    quint16 crc = 0xFFFF; // Valor inicial del CRC16
    quint16 polynomial = 0xA001; // Polinomio utilizado para el cálculo del CRC16

    for (int i = 0; i < data.size(); ++i) {
        crc ^= (quint16)data.at(i).unicode();
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    // Convertir el valor CRC16 a formato hexadecimal y luego a QString
    QString crcString = QString("%1").arg(crc, 4, 16, QChar('0')).toUpper();
    return crcString;
}

Licencia::Licencia()
{

}
