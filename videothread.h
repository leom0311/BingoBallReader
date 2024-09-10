#ifndef VideoThread_H
#define VideoThread_H

#include <QThread>
#include <QPixmap>
#include <QImage>
#include "engine.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class VideoThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = nullptr);
    ~VideoThread();

    void initThread(QString modelPath, cv::Size inputSize, QString classesPath, bool onGPU);
    void setVideoPath(QString videoPath);
    void setWebCam(int cameraIdx);
    bool isRunningThread;

protected:
    void run() override;

private:
    cv::Mat preprocessFrame(cv::Mat originalFrame);

    Engine *engine;

    QPixmap resultPixmap;
    QString resultLabel;
    bool    isWebCam;
    string videoFilePath;
    int  nCameraIdx;
signals:
    void sendResult(QPixmap resultPixmap, QString resultLabel);
};

#endif // VideoThread_H
