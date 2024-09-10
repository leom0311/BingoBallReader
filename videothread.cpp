#include "videothread.h"

VideoThread::VideoThread(QObject *parent) :
    QThread(parent)
{
    isWebCam = false;
    nCameraIdx = 0;
    videoFilePath = "";
}

VideoThread::~VideoThread()
{

}

// Initialize model
void VideoThread::initThread(QString modelPath, cv::Size inputSize, QString classesPath, bool onGPU)
{
    engine = new Engine(modelPath.toLocal8Bit().constData(),        //Model abs path
                        inputSize,                                  //Inputsize e.g : (320, 320)
                        classesPath.toLocal8Bit().constData(),      //Abs path of file that contains classes
                        onGPU                                       //If true, using GPU, elif false, using CPU
                        );
}

void VideoThread::setVideoPath(QString videoPath)
{
    videoFilePath = videoPath.toLocal8Bit().constData();
    isWebCam = false;
}

void VideoThread::setWebCam(int cameraIdx)
{
    nCameraIdx = cameraIdx;
    isWebCam = true;
}

//Main Process
void VideoThread::run()
{
    cv::VideoCapture camera2Open;
    cv::Mat cameraFrame, outputFrame;

    if (isWebCam)
        camera2Open.open(nCameraIdx);
    else
        camera2Open.open(videoFilePath);

    if(!camera2Open.isOpened())
        throw "Error on reading stream";

    isRunningThread = true;

    while(1){

        if(!isRunningThread)
            break;
        camera2Open >> cameraFrame;
        if(cameraFrame.empty())
            break;

        cameraFrame.copyTo(outputFrame);
        cameraFrame = preprocessFrame(cameraFrame);

        std::vector<Detection> outputOfDetect = engine->runInference(cameraFrame);

        int detections = outputOfDetect.size();

        for (int i = 0; i < detections; ++i)
        {
            Detection detection = outputOfDetect[i];

            cv::Rect box = detection.box;
            cv::Scalar color = detection.color;

            // Draw detection box
            cv::rectangle(outputFrame, box, color, 2);

            // Draw detection class
            std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
            cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
            cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

            cv::rectangle(outputFrame, textBox, color, cv::FILLED);
            cv::putText(outputFrame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
        }

        resultLabel = "";
        if(detections != 0)
            resultLabel = outputOfDetect.at(0).className.c_str();

        cvtColor(outputFrame, outputFrame, COLOR_BGR2RGB);
        resultPixmap = QPixmap::fromImage(QImage((unsigned char*)outputFrame.data, outputFrame.cols, outputFrame.rows, outputFrame.step, QImage::Format_RGB888));

        emit sendResult(resultPixmap, resultLabel);
    }
    camera2Open.release();
}

// Exchange color of yellow section to black
cv::Mat VideoThread::preprocessFrame(Mat originalFrame)
{
    Mat originalFrame_src = originalFrame;

    Mat originalImage, Mask, Inverted_Mask, output1, output_2, Result;
    Mat MaskImage(originalFrame_src.rows, originalFrame_src.cols, CV_8UC3, Scalar(0, 0, 0));

    originalFrame_src.copyTo(originalImage);

    cvtColor(originalFrame_src, originalFrame_src, COLOR_BGR2HSV);
    inRange(originalFrame_src, Scalar(22, 80, 0), Scalar(47, 255, 255), Mask);

    bitwise_not(Mask, Inverted_Mask);
    bitwise_and(MaskImage, MaskImage, output1, Mask);
    bitwise_and(originalImage, originalImage, output_2, Inverted_Mask);

    add(output1, output_2, Result);
    blur(Result, Result, Size(3, 3));

    return Result;
}
