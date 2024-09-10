#ifndef ADMINCONTROL_H
#define ADMINCONTROL_H

#include <QMainWindow>
#include <QtDebug>
#include <QPushButton>
#include <QtWidgets>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QtMultimedia/QCameraInfo>
#include <QDateTime>
#include <fstream>
#include "EngineThread.h"
#include "camerathread.h"
#include "ordenaciones.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdminControl; }
QT_END_NAMESPACE

const int BINGO90 = 90;
const int BINGO75 = 75;

class AdminControl : public QMainWindow
{
    Q_OBJECT

public:
    AdminControl(QWidget *parent = nullptr);
    void getCameraList();
    ~AdminControl();

private:
    void cargaInicial();
    void reiniciarBotonera();
    void cambiarColor();
    void iluminarNumero(int number);
    void reiniciarVectorReconocimiento();
    void cargarAudios();
    void cargarCartones();
    void tacharNumeroCantado(int n);
    void pintarCartones();
    void pintarTachados();

    Ui::AdminControl *ui;
    /* Lista de botones de la botonera */
    QList<QPushButton*> botones;
    /* Lista de botones del historial */
    QList<QPushButton*> historial;
    /* Motor de lectura de objetos con redes neuronales */
    EngineThread* engineThread;
    /* Reproductor de audio */
    QMediaPlayer *reproductor;
    /* Lista donde están todas las rutas de los archivos de sonido */
    QList<QUrl> rutasSonido;
    /* Orden */
    int orden;
    /* Número de bolas en juego */
    int juegoBolas;
    /* Lista de cartones en juego */
    QList<Carton*> listaCartonesEnJuego;
    /* fase = 0 linea, fase = 1 bingo */
    int fase;
    /* Bola en la que se cantó línea */
    int bolaLinea;
    /* Bola en la que se cantó bingo */
    int bolaBingo;
    /* Vector de Reconocimiento */
    int reconocimiento[BINGO90];
    /* Votos necesarios para reconocer un número */
    int votos;
    /* Número de cartones a pintar */
    int cuantosCartonesPintados;
    /* Parámetros del reconocimiento y la cámara */
    int cameraIndex;
    CameraThread *threadCamera;
    QString pathModel, pathClasses, pathModel2, pathClasses2;



private slots:
    void showResult(QPixmap result_img, QString result_label);
    void on_cbx_cameras_currentIndexChanged(int index);
    void on_resetButton_clicked();
    void on_checkBox90_stateChanged(int arg1);
    void on_checkBox75_stateChanged(int arg1);
    void on_horizontalSlider_valueChanged(int value);
    void on_cbx_voces_currentIndexChanged(int index);
    void on_botonReconocimiento_clicked();
    void on_botonLinea_clicked();
    void on_botonBingo_clicked();
};
#endif // ADMINCONTROL_H
