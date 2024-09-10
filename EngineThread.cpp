#include "EngineThread.h"

#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using namespace cv::dnn;
using namespace std;

Net net;
float confThreshold = 0.7, nmsThreshold = 0.1;
int inpWidth = 320;
int inpHeight = 320;
float scale = 0.00392;
int cntRes = 25;
vector<string> classes = vector<string>{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90" };

QVector <QString> m_result_labels, m_labels;
QString m_final_label;

int init_dnn(String onnx_path)
{

    try {
        net = cv::dnn::readNetFromONNX(onnx_path);
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_CUDA);
    }
    catch (exception e) {
        throw e;
        exit(0);
    }

    return 0;
}


cv::Mat formatToSquare(const cv::Mat& source)
{
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}
//Ah I see Did you setup opencv with GPU_Model?
//no, i dont know howto
//Do  u have opncv intaller?
//i dont know, i not implement this
//Ok, just  aminute.
//OOOook -> I implemented the program logic and interface.
//Everything related to opencv was done by another programmer
//and I have no idea about that part :(
//Don't worry. :)
//Hello, I will give you some source files. Ok?
//Ok
// Go to http://cmake.org/download/
//Oh I'll give you Ok?
//Ok
//Go to https://cmake.org/download/
//And then download cmake-3.29.3-windows-x86_64.msi
//next?
//Run CmakeGUI
//exact it on D:
//i dont understand you

//Can you use skype?
//yes
// Could you please share you r skype i


String runInference(const cv::Mat& input2process, const cv::Mat& input2output, cv::Size modelShape, cv::dnn::Net net)
{
    cv::Mat modelInput = input2process;
    if (true && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0 / 255.0, modelShape, cv::Scalar(), true, false);

    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    //net.forward(outputs);

    int rows = outputs[0].size[2];
    int dimensions = outputs[0].size[1];

    //rows = outputs[0].size[2];
    //dimensions = outputs[0].size[1];

    outputs[0] = outputs[0].reshape(1, dimensions);
    cv::transpose(outputs[0], outputs[0]);

    float* data = (float*)outputs[0].data;

    float x_factor = (float)modelInput.cols / modelShape.width;
    float y_factor = (float)modelInput.rows / modelShape.height;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        float* classes_scores = data + 4;

        cv::Mat scores(1, dimensions - 4, CV_32FC1, classes_scores);
        cv::Point class_id;
        double maxClassScore;

        minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

        if (maxClassScore > confThreshold)
        {
            confidences.push_back(maxClassScore);
            class_ids.push_back(class_id.x);

            float x = data[0];
            float y = data[1];
            float w = data[2];
            float h = data[3];

            int left = int((x - 0.5 * w) * x_factor);
            int top = int((y - 0.5 * h) * y_factor);

            if (left < 0) left = 0;
            if (top < 0) top = 0;

            int width = int(w * x_factor);
            int height = int(h * y_factor);

            int right = left + width;
            int bottom = top + height;
            if (right > input2process.cols) right = input2process.cols - 1;
            if (bottom > input2process.rows) bottom = input2process.rows - 1;

            width = right - left;
            height = bottom - top;

            boxes.push_back(cv::Rect(left, top, width, height));
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, nms_result, 0.5f);
    String result;

    if(nms_result.size() == 0 && m_result_labels.size() != 0)
            m_result_labels.pop_front();

    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        int class_id = class_ids[idx];
        Rect box = boxes[idx];

        QString tmp_result_label = QString::fromUtf8(classes[class_id].c_str());
        if(m_labels.count(tmp_result_label) == 0)
            m_labels.append(tmp_result_label);
        if(m_result_labels.length() <= 5){
            m_result_labels.append(tmp_result_label);
            m_final_label = "";
        }else{
            m_result_labels.append(tmp_result_label);

            if(m_result_labels.length()>30)
                m_result_labels.pop_front();
            QString tmp_label = "";
            int tmp_count = 0;
            for (int i = 0; i < m_labels.length(); ++i) {
                if(m_result_labels.count(m_labels.at(i))>tmp_count){
                    tmp_count = m_result_labels.count(m_labels.at(i));
                    tmp_label = m_labels.at(i);
                }
            }
            m_final_label = tmp_label;
        }

        cv::putText(input2output, m_final_label.toLocal8Bit().constData(), Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
        cv::rectangle(input2output, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar(255, 0, 0), 2);

        result = m_final_label.toStdString();
    }
    return result;
}

EngineThread::EngineThread(QObject* parent) :
    QThread(parent),
    m_cameraPath(0)
{

}

EngineThread::~EngineThread()
{
    //wait();
    quit();

}

void EngineThread::initEngine(int cameraPath, QString modelPath)
{
    m_cameraPath = cameraPath;
    m_modelPath = modelPath.toLocal8Bit().constData();
}

void EngineThread::run()
{
    cv::VideoCapture cap;
    cap.open(m_cameraPath);

    if (!cap.isOpened())
    {
        throw "Error on reading stream...";
    }
    m_isRunning = true;

    init_dnn(m_modelPath);

    while (1)
    {
        cap >> m_frame;
        if (m_frame.empty())
            break;

        Mat originalImage, Mask, Inverted_Mask, output1, output_2, Result;
        Mat MaskImage(m_frame.rows, m_frame.cols, CV_8UC3, Scalar(0, 0, 0));

        m_frame.copyTo(originalImage);
        cvtColor(m_frame, m_frame, COLOR_BGR2HSV);
        inRange(m_frame, Scalar(22, 80, 0), Scalar(47, 255, 255), Mask);
        bitwise_not(Mask, Inverted_Mask);

        bitwise_and(MaskImage, MaskImage, output1, Mask);

        bitwise_and(originalImage, originalImage, output_2, Inverted_Mask);

        add(output1, output_2, Result);
        blur(Result, Result, Size(3, 3));

        String result_label = runInference(Result, originalImage, Size(320, 320), net);

        cvtColor(originalImage, originalImage, COLOR_BGR2RGB);
        m_result = QPixmap::fromImage(QImage((unsigned char*)originalImage.data, originalImage.cols, originalImage.rows, originalImage.step, QImage::Format_RGB888));

        emit inferenceGenerate(m_result, QString::fromUtf8(result_label.c_str()));
    }
}



