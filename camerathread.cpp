#include "camerathread.h"

CameraThread::CameraThread(QObject *parent) :
    QThread(parent)
{
    isRunningThread = false;
    is_detecting = false;
    isWebCam = false;
    videoPath = "";
    cameraIndex = 0;

    resultLabel = "";
}

CameraThread::~CameraThread()
{

}

// Initialize model
void CameraThread::initThread(QString modelPath, cv::Size inputSize, QString classesPath, bool onGPU)
{
    engine = new Engine(modelPath.toLocal8Bit().constData(),        //Model abs path
                        inputSize,                                  //Inputsize e.g : (320, 320)
                        classesPath.toLocal8Bit().constData(),      //Abs path of file that contains classes
                        onGPU                                       //If true, using GPU, elif false, using CPU
                        );
}

void CameraThread::initThread2(QString modelPath, Size inputSize, QString classesPath, bool onGPU)
{
    engine2 = new Engine(modelPath.toLocal8Bit().constData(),        //Model abs path
                        inputSize,                                  //Inputsize e.g : (128, 128)
                        classesPath.toLocal8Bit().constData(),      //Abs path of file that contains classes
                        onGPU                                       //If true, using GPU, elif false, using CPU
                        );
}

void CameraThread::setCameraIndex(int cameraIdx)
{
    cameraIndex = cameraIdx;
    isWebCam = true;
}

void CameraThread::setVideoPath(QString videoPth)
{
    videoPath = videoPth;
    isWebCam = false;
}

void CameraThread::setStatus(bool newStatus)
{
    is_detecting = newStatus;
}

bool CameraThread::getStatus()
{
    return is_detecting;
}

//Main Process
void CameraThread::run()
{
    cv::VideoCapture camera2Open;
    cv::Mat cameraFrame, outputFrame;

    if(isWebCam)
        camera2Open.open(cameraIndex);
    else
        camera2Open.open(videoPath.toLocal8Bit().constData());

    if(!camera2Open.isOpened())
        throw "Error on loading stream...";

    isRunningThread = true;

    while(1){

        if(!isRunningThread)
            break;
        camera2Open >> cameraFrame;
        if(cameraFrame.empty())
            break;

        cameraFrame.copyTo(outputFrame);
        if(is_detecting == true){


            cameraFrame = preprocessFrame(cameraFrame);

            std::vector<Detection> outputOfDetect = engine->runInference(cameraFrame);

            int detections = outputOfDetect.size();
            int maxIndex = -1;
            float maxConfidence = -1.0;
            resultLabel = "";
            for (int i = 0; i < detections; ++i)
            {
                Detection detection = outputOfDetect[i];

                if(detection.confidence > maxConfidence){
                    maxConfidence = detection.confidence;
                    maxIndex = i;
                }
            }
            if(maxIndex != -1){
                Detection detection = outputOfDetect[maxIndex];
                Detection detection2;

                if(detection.className == "6" || detection.className == "9" || detection.className == "68" || detection.className == "89"){
                    cv::Mat tmpFrame;
                    cameraFrame.copyTo(tmpFrame);
                    if(detection.box.x < 0)
                        detection.box.x = 0;
                    if(detection.box.y < 0)
                        detection.box.y = 0;
                    if(detection.box.width > tmpFrame.size().width)
                        detection.box.width = tmpFrame.size().width - 1;
                    if(detection.box.height > tmpFrame.size().width)
                        detection.box.width = tmpFrame.size().height - 1;

                    //tmpFrame = tmpFrame(detection.box);
                    std::vector<Detection> outputOfDetect2 = engine2->runInference(cameraFrame);

                    int detections2 = outputOfDetect2.size();
                    int maxIndex2 = -1;
                    float maxConfidence2 = -1.0;

                    for (int i = 0; i < detections2; ++i)
                    {
                        Detection detection2 = outputOfDetect2[i];

                        if(detection2.confidence > maxConfidence2){
                            maxConfidence2 = detection2.confidence;
                            maxIndex2 = i;
                        }
                    }
                    if(maxIndex2 != -1){
                        detection2 = outputOfDetect2[maxIndex2];
                    }
                    else{
                        detection2.className = "";
                    }
                }


                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;

                // Draw detection box
                cv::rectangle(outputFrame, box, color, 2);

                // Draw detection class
                if(detection2.className != "")
                    resultLabel = detection2.className.c_str();
                else
                    resultLabel = detection.className.c_str();
            }
        }
        cvtColor(outputFrame, outputFrame, COLOR_BGR2RGB);
        resultPixmap = QPixmap::fromImage(QImage((unsigned char*)outputFrame.data, outputFrame.cols, outputFrame.rows, outputFrame.step, QImage::Format_RGB888));

        emit sendResult(resultPixmap, resultLabel);
    }
    camera2Open.release();
}

// Exchange color of yellow section to black
cv::Mat CameraThread::preprocessFrame(Mat originalFrame)
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
