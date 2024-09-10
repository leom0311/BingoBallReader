#include "Carton.h"

void Carton::tacharNumerosEnDibujo()
{
    //tachar visualmente.
    // Obtenemos el layout del groupbox
    QLayout *layout = groupBox->layout();

    // Recorremos el layout
    for (int i = 0; i < layout->count(); i++) {
        // Obtenemos el widget actual
        QWidget *widget = layout->itemAt(i)->widget();

        // Si el widget es una QLabel
        if (widget->inherits("QLabel")) {
            QLabel *label = qobject_cast<QLabel *>(widget);
            if(estaTachado(label->text().toInt())){
                label->setMinimumSize(label->size());
                label->setMaximumSize(label->size());

                // Obtén el ancho y alto de la etiqueta
                int ancho = label->width();
                int alto = label->height();

                // Crea una imagen vacía con el mismo tamaño que la etiqueta
                QImage image(ancho, alto, QImage::Format_ARGB32);
                image.fill(Qt::transparent);

                // Crea un QPainter para dibujar en la imagen
                QPainter painter(&image);

                // Dibuja el número en la imagen
                QFont font = label->font();
                font.setBold(true);
                painter.setPen(Qt::white);
                painter.setFont(font);
                painter.drawText(0, 0, ancho, alto, Qt::AlignCenter, label->text());

                // Dibuja la X en la imagen sin borrar el número
                painter.setPen(Qt::red);
                //painter.drawLine(0, 0, ancho, alto);
                painter.drawLine(0, alto, ancho, 0);

                // Establece la imagen en la etiqueta
                label->setPixmap(QPixmap::fromImage(image));
            }
        }
    }
}

int Carton::obtenerId()
{
    return this->id;
}

bool Carton::hayLinea()
{
    int cont=0;
    for(int i=0; i<FILAS; i++){
        for(int j=0; j<COLUMNAS; j++){
            if(this->matriz[i][j].tachado){
                cont++;
                if(cont==5){
                    return true;
                }
            }
        }
        cont=0;
    }
    return false;
}

bool Carton::hayBingo()
{
    return this->obtenerTachados()==FILASREALES * COLUMNASREALES;
}

OrdenLinea Carton::obtenerRegistroLinea()
{
    OrdenLinea l;
    //Partimos de tener 5 números en cada línea.
    l.f1=5;
    l.f2=5;
    l.f3=5;

    //Restamos los que ya estén tachados.
    for(int i=0; i<FILAS; i++){
        for(int j=0; j<COLUMNAS; j++){
            switch (i) {
                case 0:
                    if(this->matriz[i][j].tachado){
                        l.f1--;
                    }
                break;
                case 1:
                    if(this->matriz[i][j].tachado){
                        l.f2--;
                    }
                break;
                case 2:
                    if(this->matriz[i][j].tachado){
                        l.f3--;
                    }
            }
        }
    }

    //Ordeno por valores los números que faltan en cada línea.
    if (l.f1 > l.f2) {
        std::swap(l.f1, l.f2);
    }

    if (l.f2 > l.f3) {
        std::swap(l.f2, l.f3);
    }

    if (l.f1 > l.f2) {
        std::swap(l.f1, l.f2);
    }

    return l;
}

bool Carton::estaTachado(int n)
{
    bool enc=false;
    int i=0, j;
    while(i<FILAS&&!enc){
        j=0;
        while(j<COLUMNAS&&!enc){
            if(this->matriz[i][j].n==n){
                return this->matriz[i][j].tachado;
            }
            j++;
        }
        i++;
    }
    return enc;
}

Carton::Carton()
{
    this->id=-1;
    this->tachados=-1;
}

Carton::Carton(int id, int m[FILASREALES][COLUMNASREALES])
{
    this->id=id;
    //Toda el cartón a -1
    for(int i=0; i<FILAS; i++){
        for(int j=0; j<COLUMNAS; j++){
            this->matriz[i][j].n=-1;
            this->matriz[i][j].tachado=false;
        }
    }
    //Pasamos los números reales a la matriz:
    for(int i=0; i<FILASREALES; i++){
        for(int j=0; j<COLUMNASREALES; j++){
            if(m[i][j]==90){
                this->matriz[i][8].n=90;
            }
            else{
                this->matriz[i][m[i][j]/10].n = m[i][j];
            }
        }
    }
    this->tachados=0;
}

int Carton::tacharNumero(int n)
{
    bool enc=false;
    int i=0, j;
    while(i<FILAS&&!enc){
        j=0;
        while(j<COLUMNAS&&!enc){
            if(this->matriz[i][j].n==n){
                this->matriz[i][j].tachado=true;
                this->tachados++;
                enc=true;
            }
            j++;
        }
        i++;
    }

    return this->tachados;
}

void Carton::destacharNumero(int n)
{
    bool enc=false;
    int i=0, j;
    while(i<FILAS&&!enc){
        j=0;
        while(j<COLUMNAS&&!enc){
            if(this->matriz[i][j].n==n){
                this->matriz[i][j].tachado=false;
                this->tachados--;
                enc=true;
            }
            j++;
        }
        i++;
    }
}

int Carton::obtenerTachados()
{
    return this->tachados;
}

QGroupBox *Carton::pintarCarton()
{

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    this->groupBox = new QGroupBox("Cartón: " + QString::number(this->id));
    groupBox->setStyleSheet("QGroupBox { font-size: 10pt; font-weight: bold; }");
    groupBox->setLayout(gridLayout);

    // Establecemos las dimensiones del QGroupBox
        groupBox->setMinimumSize(291, 151);
        groupBox->setMaximumSize(291, 151);

    // Creamos las labels
    for (int i = 0; i < FILAS * COLUMNAS; i++) {

        QLabel *label ;
        if(this->matriz[i/COLUMNAS][i%COLUMNAS].n==-1){
            label = new QLabel("*", groupBox);
        }
        else{
            label = new QLabel(QString::number(this->matriz[i/COLUMNAS][i%COLUMNAS].n), groupBox);
        }
        label->setStyleSheet("background-color: white; border: 1px solid white; font-size: 16px");
        label->setAlignment(Qt::AlignCenter);
        // Establecemos la posición y el tamaño de la label
        int fila = i / COLUMNAS;
        int columna = i % COLUMNAS;
        gridLayout->addWidget(label, fila, columna);
    }

    return groupBox;
}
