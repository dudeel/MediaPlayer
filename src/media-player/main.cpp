#include <QApplication>
#include "media-player-window.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <fstream>

using namespace cv;
using namespace dnn;
using namespace std;

// Функция для преобразования вывода в читаемые результаты
void postprocess(Mat& frame, const vector<Mat>& output, float threshold_confidence, const vector<string>& classes);

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  // Загрузка YOLOv3
  String model_cfg = "/home/user/project/MediaPlayer/libs/yolov3.cfg";
  String model_weights = "/home/user/project/MediaPlayer/libs/yolov3.weights";
  String classes_names = "/home/user/project/MediaPlayer/libs/coco.names";

  Net net = readNetFromDarknet(model_cfg, model_weights);
  net.setPreferableBackend(DNN_BACKEND_OPENCV);
  net.setPreferableTarget(DNN_TARGET_CPU);

  vector<string> classes;
  ifstream file(classes_names.c_str());
  string className;
  while (getline(file, className))
  {
    classes.push_back(className);
  }

  // Загрузка изображения
  string img_path = "/home/user/project/MediaPlayer/libs/test7.jpg";  // путь к тестовому изображению
  Mat img = imread(img_path);

  if (img.empty())
  {
    cout << "Невозможно открыть изображение!" << endl;
    return -1;
  }

  // Подготовка
  Mat blob;
  blobFromImage(img, blob, 1 / 255.0, Size(416, 416), Scalar(), true, false);
  net.setInput(blob);

  // Прогнозирование
  vector<String> outputBlobNames = net.getUnconnectedOutLayersNames();
  vector<Mat> outputBlobs;
  net.forward(outputBlobs, outputBlobNames);

  float threshold_confidence = 0.5;
  postprocess(img, outputBlobs, threshold_confidence, classes);

  // Отображение результата
  namedWindow("MediaPlayer", WINDOW_NORMAL);
  imshow("MediaPlayer", img);
  waitKey(0);

  MediaPlayerWindow* media = new MediaPlayerWindow();
  media->show();

  return app.exec();
}

void postprocess(Mat& frame, const vector<Mat>& output, float threshold_confidence, const vector<string>& classes)
{
  vector<int> class_ids;
  vector<float> confidences;
  vector<Rect> boxes;

  int rows = frame.rows;
  int cols = frame.cols;

  for (const auto& netOutput : output)
  {
    float* data = (float*)netOutput.data;
    for (int i = 0; i < netOutput.rows; ++i, data += netOutput.cols)
    {
      Mat scores = netOutput.row(i).colRange(5, netOutput.cols);
      Point class_id_point;
      double confidence;
      minMaxLoc(scores, 0, &confidence, 0, &class_id_point);

      if (confidence > threshold_confidence)
      {
        int centerX = (int)(data[0] * cols);
        int centerY = (int)(data[1] * rows);
        int width = (int)(data[2] * cols);
        int height = (int)(data[3] * rows);
        int left = centerX - width / 2;
        int top = centerY - height / 2;

        class_ids.push_back(class_id_point.x);
        confidences.push_back((float)confidence);
        boxes.push_back(Rect(left, top, width, height));
      }
    }
  }

  // NMS
  vector<int> indices;
  NMSBoxes(boxes, confidences, threshold_confidence, 0.4, indices);
  Scalar color(0, 255, 0);

  for (int idx : indices)
  {
    Rect box = boxes[idx];
    int class_id = class_ids[idx];
    string class_name = classes[class_id];
    float conf = confidences[idx];

    stringstream ss;
    ss << class_name << " " << fixed << setprecision(2) << conf;
    string labelText = ss.str();

    putText(frame, labelText, box.tl(), FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
    rectangle(frame, box, color, 2);
  }
}

// MKV:  x264enc ! matroskamux
// WebM: vp8enc ! webmmux
// AVI:  x264enc ! avimux
