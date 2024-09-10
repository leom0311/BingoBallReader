#include "admincontrol.h"
#include "ui_admincontrol.h"

AdminControl::AdminControl(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminControl)
{
    ui->setupUi(this);

    this->reproductor = new QMediaPlayer();
    this->orden=0;
    this->juegoBolas = BINGO90;
    this->ui->resetButton->setStyleSheet("background-color: black; color: white;");
    this->ui->label_estado_lector->setStyleSheet("color: green;");

    /*Ajustes del tamaño inicial de la cartonera*/
    this->ui->cartonera->setLayout(new QHBoxLayout());
    this->ui->cartonera->setGeometry(20, 560, 1880, 181);

    cargaInicial();
    cargarAudios();
    reiniciarBotonera();
    cargarCartones();

    this->cuantosCartonesPintados=6;

    this->votos = 5;
    this->reiniciarVectorReconocimiento();

    cameraIndex = 0;

    pathModel = QDir::currentPath() + "/best.onnx";
    pathClasses = QDir::currentPath() + "/classes.cmake";
    pathModel2 = QDir::currentPath() + "/ball350000.onnx";
    pathClasses2 = QDir::currentPath() + "/extra.cmake";

    threadCamera = new CameraThread();
    threadCamera->initThread(pathModel, cv::Size(320, 320), pathClasses, true);
    threadCamera->initThread2(pathModel2, cv::Size(128, 128), pathClasses2, true);
    connect(threadCamera, &CameraThread::sendResult, this, &AdminControl::showResult);

    getCameraList();

    // Obtener el tamaño actual de la ventana
    QSize currentSize = this->size();

    // Establecer el tamaño fijo de la ventana al tamaño actual
    this->setFixedSize(currentSize);

    this->setWindowTitle("BingoSP Lector Automático");
}

void AdminControl::getCameraList()
{
    int n = 0;
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameras) {
        ui->cbx_cameras->addItem("CAM " + QString::number(n));
        qDebug()<<cameraInfo.deviceName()<<endl;
        n++;
    }
}

void AdminControl::cargaInicial()
{

    //Botonera
    this->botones = this->ui->botonera->findChildren<QPushButton*>();
    qSort(botones.begin(), botones.end(), [](QPushButton *button1, QPushButton *button2) {
        return button1->objectName() < button2->objectName();
    });

    for (int i = 0; i < BINGO90; ++i) {
        //QPushButton *button = qobject_cast<QPushButton*>(this->botones.at(i));
        connect(this->botones.at(i), &QPushButton::clicked, this, &AdminControl::cambiarColor);
        //botones.append(button);
    }

    //Historial
    this->historial = this->ui->historial->findChildren<QPushButton*>();
    qSort(historial.begin(), historial.end(), [](QPushButton *button1, QPushButton *button2) {
        return button1->objectName() < button2->objectName();
    });

}

void AdminControl::reiniciarBotonera()
{
    this->ui->etiquetaHayBingo->hide();
    this->ui->etiquetaHayLinea->hide();
    this->ui->etiquetaAnuncio->setText("");

    foreach(QPushButton *boton, this->botones){
        boton->setStyleSheet("background-color: black; color: white;");
    }

    foreach(QPushButton *boton, this->historial){
        boton->hide();
    }

    this->fase=0;
    this->bolaLinea=0;
    this->bolaBingo=0;
    this->orden=0;
    this->ui->ordenNumeroLabel->setText(QString::number(orden));
    this->ui->restantesNumberLabel->setText(QString::number(this->juegoBolas));
}

void AdminControl::cambiarColor()
{
    QPushButton* botonPresionado = qobject_cast<QPushButton*>(sender());

    if (botonPresionado) {
        // Obtener el color actual del fondo del botón
        QString colorActual = botonPresionado->styleSheet();

        // Determinar si el color actual es negro
        bool esNegro = colorActual.contains("black");

        if(esNegro){
            iluminarNumero(botonPresionado->text().toInt()-1);
        }
        else{
            //TODO: Eliminarlo del historial y reestablecerlo en los cartones.
            botonPresionado->setStyleSheet("background-color: black; color: white;");


            //Eliminarlo del historial
            int eliminadas=0;
            for(int i=0; i<this->orden; i++){
                if(this->historial.at(i)->text()==botonPresionado->text()){
                    eliminadas++;
                }
                else{
                    this->historial.at(i-eliminadas)->setText(this->historial.at(i)->text());
                }
            }
            this->historial.at(this->orden-1)->hide();
            this->orden--;
            this->ui->ordenNumeroLabel->setText(QString::number(orden));
            this->ui->restantesNumberLabel->setText((QString::number(this->juegoBolas - this->orden)));

            //Destacharlo de todos los cartones:
            foreach(Carton *c, this->listaCartonesEnJuego){
                c->destacharNumero(botonPresionado->text().toInt());
            }
            if(this->fase==0){
                Ordenaciones::ordenarLinea(this->listaCartonesEnJuego);
            }
            else{
                Ordenaciones::ordenarBingo(this->listaCartonesEnJuego);
            }
            this->pintarCartones();
            this->pintarTachados();
        }

    }
}

void AdminControl::iluminarNumero(int number)
{
    if(number>=0&&number<90){
        if(this->botones.at(number)->styleSheet().contains("black")){
            this->botones.at(number)->setStyleSheet("background-color: green; color: white;");
            this->reproductor->setMedia(this->rutasSonido.at(number));
            //qDebug()<<this->rutasSonido.at(number)<<endl;
            this->reproductor->play();
            this->orden++;
            this->tacharNumeroCantado(number+1);
            this->ui->ordenNumeroLabel->setText(QString::number(orden));
            this->ui->restantesNumberLabel->setText((QString::number(this->juegoBolas - this->orden)));
            this->historial.at(orden-1)->setText(QString::number(number+1));
            this->historial.at(orden-1)->show();
            if(this->fase==0){
                Ordenaciones::ordenarLinea(this->listaCartonesEnJuego);
            }
            else{
                Ordenaciones::ordenarBingo(this->listaCartonesEnJuego);
            }
            this->pintarCartones();
            this->pintarTachados();
        }
    }
}

void AdminControl::reiniciarVectorReconocimiento()
{
    for (int i=0; i<BINGO90 ; i++) {
        this->reconocimiento[i]=0;
    }
}

void AdminControl::cargarAudios()
{
    // Ruta de la carpeta "audios"
    QString folderPath = QCoreApplication::applicationDirPath() + "/audios/";

    switch(this->ui->cbx_voces->currentIndex()){
        case 0:
            folderPath+="pilar/";
        break;
        case 1:
            folderPath+="coke/";
        break;
    }

    qDebug()<<QCoreApplication::applicationDirPath()<<endl;

    rutasSonido.clear();

    // Generar las rutas de los archivos de sonido del 1 al 90
    for (int i = 1; i <= 90; ++i) {
        QString filePath = folderPath + QString::number(i) + ".mp3";
        rutasSonido.append(QUrl::fromLocalFile(filePath));
    }

    this->rutasSonido.append(QUrl::fromLocalFile(folderPath + "linea.mp3"));
    this->rutasSonido.append(QUrl::fromLocalFile(folderPath + "bingo.mp3"));
}

void AdminControl::cargarCartones()
{
    Carton *c;
    string s[5];
    int m[3][5];
    int cont=1;
    int i=0;
    QString folderPath = QCoreApplication::applicationDirPath() + "/cartones/randoms.txt";
    ifstream flujoLectura(folderPath.toStdString());
    //Limpieza de la lista en caso de que ya tuviera algún cartón cargado.
    while(!this->listaCartonesEnJuego.empty()){
        c = this->listaCartonesEnJuego.first();
        this->listaCartonesEnJuego.removeFirst();
        delete c;
    }

    //Borramos aquellos que pudieran estar en el ui
    while(this->ui->cartonera->layout()->count()){
        this->ui->cartonera->layout()->takeAt(0)->widget()->deleteLater();
    }

    //Leemos los cartones del fichero randoms.txt
    if(flujoLectura.is_open()){
        while(!flujoLectura.eof()){
            getline(flujoLectura, s[0], ' ');
            getline(flujoLectura, s[1], ' ');
            getline(flujoLectura, s[2], ' ');
            getline(flujoLectura, s[3], ' ');
            getline(flujoLectura, s[4]);
            m[i][0]=stoi(s[0]);
            m[i][1]=stoi(s[1]);
            m[i][2]=stoi(s[2]);
            m[i][3]=stoi(s[3]);
            m[i][4]=stoi(s[4]);
            i++;

            if(i==3){
                i=0;
                this->listaCartonesEnJuego.append(new Carton(cont, m));
                cont++;
            }

        }
        flujoLectura.close();
    }

    //Pintamos en pantalla los X primeros cartones
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(0)->pintarCarton());
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(1)->pintarCarton());
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(2)->pintarCarton());
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(3)->pintarCarton());
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(4)->pintarCarton());
    this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(5)->pintarCarton());
}

void AdminControl::tacharNumeroCantado(int n)
{
    if(this->bolaLinea!=0 && this->fase==0){
        this->ui->etiquetaAnuncio->setText("");
    }
    if(this->bolaBingo==0 && this->fase==1){
        this->ui->etiquetaAnuncio->setText("");
    }
    if(this->bolaBingo!=0 && this->fase==1){
        this->ui->etiquetaAnuncio->setText("");
    }
    foreach(Carton *c, this->listaCartonesEnJuego){
        c->tacharNumero(n);
        if(this->fase==0){
            if(c->hayLinea() && (this->bolaLinea==0 || this->bolaLinea<=this->orden)){
                //c->mostrarLinea();
                this->ui->etiquetaHayLinea->show();
                this->ui->etiquetaAnuncio->setText(this->ui->etiquetaAnuncio->text() + QString::number(c->obtenerId()) + " ");
                this->ui->etiquetaAnuncio->show();
                if(this->bolaLinea==0){
                    this->bolaLinea=this->orden;
                    this->ui->botonLinea->setEnabled(true);
                }

            }
        }
        else{
            if(c->hayBingo() && (this->bolaBingo==0 || this->bolaBingo<=this->orden)){
                this->ui->etiquetaHayBingo->show();
                this->ui->etiquetaAnuncio->setText(this->ui->etiquetaAnuncio->text() + QString::number(c->obtenerId()) + " ");
                this->ui->etiquetaAnuncio->show();
                if(this->bolaBingo==0){
                    this->bolaBingo=this->orden;
                    this->ui->botonBingo->setEnabled(true);
                }
            }
        }
    }
}

void AdminControl::pintarCartones()
{
    while(this->ui->cartonera->layout()->count()){
        this->ui->cartonera->layout()->takeAt(0)->widget()->deleteLater();
    }
    for(int i=0; i<this->cuantosCartonesPintados; i++){
        this->ui->cartonera->layout()->addWidget(this->listaCartonesEnJuego.at(i)->pintarCarton());
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void AdminControl::pintarTachados()
{
    for(int i=0; i<this->cuantosCartonesPintados; i++){
        this->listaCartonesEnJuego.at(i)->tacharNumerosEnDibujo();
    }
}

void AdminControl::showResult(QPixmap result_img, QString result_label)
{
    //qDebug()<<result_label<<endl;
    ui->camLabel->setPixmap(result_img.scaledToHeight(ui->camLabel->height()));
    //ui->pushButton->setText(result_label);
    if(!result_label.isEmpty()){
        this->reconocimiento[result_label.toInt()-1]++;
        if(this->reconocimiento[result_label.toInt()-1]>this->votos){
            iluminarNumero(result_label.toInt()-1);
            reiniciarVectorReconocimiento();
        }
    }

    //qDebug()<<result_label<<endl;
}

AdminControl::~AdminControl()
{
    bool isAvailable = true;
   if(threadCamera->isRunning()){
       threadCamera->isRunningThread = false;
       threadCamera->requestInterruption();
       threadCamera->wait();
       isAvailable = false;
   }
   if((!threadCamera->isRunning() && isAvailable == true) || (threadCamera->isFinished() && isAvailable == false)){
       delete threadCamera;
       delete this->reproductor;
       delete ui;
   }
}

void AdminControl::on_cbx_cameras_currentIndexChanged(int index)
{
    cameraIndex = index;
    bool isAvailable = true;
    if(threadCamera->isRunning()){
        threadCamera->isRunningThread = false;
        threadCamera->requestInterruption();
        threadCamera->wait();
        isAvailable = false;
    }
    if((!threadCamera->isRunning() && isAvailable == true) || (threadCamera->isFinished() && isAvailable == false)){
        //digitButtons[currentNumber]->setStyleSheet("background-color: black; color: white;");
        threadCamera->setCameraIndex(cameraIndex);
        threadCamera->setStatus(true);
        threadCamera->start();
    }
}


void AdminControl::on_resetButton_clicked()
{
    this->reiniciarBotonera();
    this->cargarCartones();
}


void AdminControl::on_checkBox90_stateChanged(int arg1)
{
    switch (arg1) {
        case Qt::Unchecked:
            this->ui->checkBox75->setChecked(true);
            break;
        case Qt::PartiallyChecked:
            // Acciones cuando la casilla está parcialmente marcada
            break;
        case Qt::Checked:
            for(int i=75; i<90; i++){
                this->botones.at(i)->show();
            }
            this->ui->checkBox75->setChecked(false);
            this->juegoBolas = BINGO90;
            this->ui->restantesNumberLabel->setText(QString::number(this->juegoBolas - this->orden));
            break;
        default:
            // Manejo para otros casos, si es necesario
            break;
        }
}


void AdminControl::on_checkBox75_stateChanged(int arg1)
{
    switch (arg1) {
        case Qt::Unchecked:
            this->ui->checkBox90->setChecked(true);
            break;
        case Qt::PartiallyChecked:
            // Acciones cuando la casilla está parcialmente marcada
            break;
        case Qt::Checked:
            for(int i=75; i<90; i++){
                this->botones.at(i)->hide();
            }
            this->ui->checkBox90->setChecked(false);
            this->juegoBolas = BINGO75;
            this->ui->restantesNumberLabel->setText(QString::number(this->juegoBolas - this->orden));
            break;
        default:
            // Manejo para otros casos, si es necesario
            break;
    }
}


void AdminControl::on_horizontalSlider_valueChanged(int value)
{
    this->ui->lcdNumber->display(value);
    this->votos=value;
}


void AdminControl::on_cbx_voces_currentIndexChanged(int index)
{
    this->cargarAudios();
}


void AdminControl::on_botonReconocimiento_clicked()
{
    if(this->ui->botonReconocimiento->text()=="Activar"){
        this->ui->botonReconocimiento->setText("Parar");
        this->ui->label_estado_lector->setText("Activo");
        this->ui->label_estado_lector->setStyleSheet("color: green;");
    }
    else{
        this->ui->botonReconocimiento->setText("Activar");
        this->ui->label_estado_lector->setText("Inactivo");
        this->ui->label_estado_lector->setStyleSheet("color: red;");
    }
    threadCamera->setStatus(!threadCamera->getStatus());
}


void AdminControl::on_botonLinea_clicked()
{
    this->fase=1;

    this->ui->etiquetaHayLinea->hide();
    this->ui->etiquetaAnuncio->hide();

    this->ui->botonLinea->setEnabled(false);

    this->reproductor->setMedia(this->rutasSonido.at(this->rutasSonido.size()-2));
    this->reproductor->play();
}


void AdminControl::on_botonBingo_clicked()
{
    this->ui->etiquetaHayBingo->hide();
    this->ui->etiquetaAnuncio->hide();

    this->ui->botonBingo->setEnabled(false);

    this->reproductor->setMedia(this->rutasSonido.at(this->rutasSonido.size()-1));
    this->reproductor->play();
}

