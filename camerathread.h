#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>
#include "engine.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr);
    ~CameraThread();

    void initThread(QString modelPath, cv::Size inputSize, QString classesPath, bool onGPU);
    void initThread2(QString modelPath, cv::Size inputSize, QString classesPath, bool onGPU);
    void setCameraIndex(int cameraIdx);
    void setVideoPath(QString videoPth);
    void setStatus(bool newStatus);
    bool getStatus();

    bool isRunningThread;
    bool is_detecting;
    bool isWebCam;

    QString videoPath;
    int cameraIndex;
protected:
    void run() override;

private:
    cv::Mat preprocessFrame(cv::Mat originalFrame);

    Engine *engine;
    Engine *engine2;

    QPixmap resultPixmap;
    QString resultLabel;

signals:
    void sendResult(QPixmap resultPixmap, QString resultLabel);
};

#endif // CAMERATHREAD_H
