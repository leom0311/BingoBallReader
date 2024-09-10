#pragma once
#include <qthread.h>
#include <qpixmap.h>
#include <qimage.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gapi.hpp>
#include <stdio.h>

using namespace cv;
using namespace dnn;
using namespace std;
using namespace std::chrono;

class EngineThread : public QThread
{
    Q_OBJECT

public:
    EngineThread(QObject* parent = nullptr);
    ~EngineThread();

    void initEngine(int cameraPath, QString modelPath);

signals:
    void inferenceGenerate(QPixmap result_img, QString result_label);

protected:
    void run() override;

private:
    int m_cameraPath;
    string m_modelPath;
    bool m_isRunning;

    Mat m_frame;

    QPixmap m_result;


};

