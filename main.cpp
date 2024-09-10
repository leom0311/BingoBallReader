#include "admincontrol.h"
#include "licencia.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMainWindow>

#define MAXDIAS 15

bool comprobarLicencia()
{
    QString folderPath = QCoreApplication::applicationDirPath() + "/cnfbsp.dll";
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate currentDate = dateTime.date();

    // Obtener el día, el mes y el año como enteros
    int day = currentDate.day();
    int month = currentDate.month();
    int year = currentDate.year();

    string dia, mes, anio;

    int cont=0;

    ifstream flujoLectura(folderPath.toStdString());
    if(flujoLectura.is_open()){
        while(!flujoLectura.eof()){
            getline(flujoLectura, dia, '#');
            if(!flujoLectura.eof()){
                getline(flujoLectura, mes, '#');
                getline(flujoLectura, anio);
                if(stoi(dia)==day && stoi(mes)==month && stoi(anio)==year){
                    cont++;
                }
            }
        }
        flujoLectura.close();
    }
    return cont<MAXDIAS;
}

void escribirApertura(){
    QString folderPath = QCoreApplication::applicationDirPath() + "/cnfbsp.dll";
    ofstream flujoEscritura(folderPath.toStdString(), std::ios::app);
    QDateTime dateTime = QDateTime::currentDateTime();
    QDate currentDate = dateTime.date();

    // Obtener el día, el mes y el año como enteros
    int day = currentDate.day();
    int month = currentDate.month();
    int year = currentDate.year();
    if(flujoEscritura.is_open()){
        flujoEscritura<<day<<"#"<<month<<"#"<<year<<endl;
        flujoEscritura.close();
    }
}

int main(int argc, char *argv[])
{

        QApplication a(argc, argv);
        /*
    if(comprobarLicencia()){
        QDateTime dateTime = QDateTime::currentDateTime();

            // Extraer los componentes de fecha y hora
            int dia = dateTime.date().day();
            int mes = dateTime.date().month();
            int anio = dateTime.date().year();
            int hora = dateTime.time().hour();

        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "BingoBallReader_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }

        bool ok;
        Licencia licencia;
        qDebug()<<licencia.calcularCRC16(dia, mes, anio, hora);
        QString clave = QInputDialog::getText(nullptr, QObject::tr("Ingrese la clave"), QObject::tr("Clave:"), QLineEdit::Password, "", &ok);
        if (!ok || clave != licencia.calcularCRC16(dia, mes, anio, hora)) { // Cambia "clave_correcta" por tu clave real
            // Si el usuario cancela la entrada de la clave o la clave es incorrecta, salimos del programa
            // Creamos un QMessageBox personalizado
                QMessageBox messageBox;
                messageBox.setText("La contraseña es incorrecta, este programa se cerrará ahora");
                messageBox.setIcon(QMessageBox::Warning);
                messageBox.setWindowTitle("Aviso");

                // Creamos un botón de aceptar
                QPushButton *acceptButton = messageBox.addButton("Aceptar", QMessageBox::AcceptRole);

                // Conectamos la señal de hacer clic en el botón a la función de cierre de la ventana
                QObject::connect(acceptButton, &QPushButton::clicked, &messageBox, &QMessageBox::close);

                // Mostramos el QMessageBox
                messageBox.exec();
            return 0; // O cualquier otro valor de retorno que desees
        }
    }
    else{
        // Creamos un QMessageBox personalizado
            QMessageBox messageBox;
            messageBox.setText("Se ha ejecutado demasiadas veces este programa hoy\nPóngase en contacto con BingoSP.es para más información.");
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setWindowTitle("Aviso");

            // Creamos un botón de aceptar
            QPushButton *acceptButton = messageBox.addButton("Aceptar", QMessageBox::AcceptRole);

            // Conectamos la señal de hacer clic en el botón a lfunción de cierre de la ventana
            QObject::connect(acceptButton, &QPushButton::clicked, &messageBox, &QMessageBox::close);

            // Mostramos el QMessageBox
            messageBox.exec();
        return 0;
    }
    */

        AdminControl w;
        w.showMaximized();
        escribirApertura();
        return a.exec();
}
