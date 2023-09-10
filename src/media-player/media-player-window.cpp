#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <fstream>

#include <QUrl>
#include <QTimer>

#include <QGst/Init>
#include <QGst/Parse>
#include <QGst/ElementFactory>

#include "sound/sound.h"
#include "player/player.h"

MediaPlayerWindow::MediaPlayerWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MediaPlayerWindow)
{
  ui->setupUi(this);
  setWindowTitle("Media Player");

  QGst::init(nullptr, nullptr);

  pipeline = QGst::Parse::launch("playbin").dynamicCast<QGst::Pipeline>();
  if (!pipeline)
  {
    qCritical() << "Не удалось создать QGst::PipelinePtr";
    return;
  }

  QGst::Ui::VideoWidget* video_widget = new QGst::Ui::VideoWidget;
  ui->mediaBox->addWidget(video_widget);

  QUrl videoUrl = QUrl::fromLocalFile("/home/user/project/MediaPlayer/data/Monkey.mp4");
  pipeline->setProperty("uri", videoUrl.toString());
  pipeline->setProperty("video-sink", QGst::ElementFactory::make("qwidget5videosink", "video_sink"));
  pipeline->setState(QGst::StatePlaying);

  QGst::ElementPtr video_sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
  video_widget->setVideoSink(video_sink);

  QTimer::singleShot(0, this, [this]() {
    waitForStateChanged(QGst::StatePlaying, 5000);
    initAddons();
  });
}

void MediaPlayerWindow::waitForStateChanged(QGst::State state, int timeout_ms)
{
  QEventLoop loop;
  QTimer timer;
  timer.start(timeout_ms);

  QObject::connect(&timer, &QTimer::timeout, [&loop]() { loop.quit(); });

  while (pipeline->currentState() != state && timer.isActive())
  {
    QMetaObject::invokeMethod(&loop, "quit", Qt::QueuedConnection);
    loop.exec();
  }
}

void MediaPlayerWindow::initAddons()
{
  Sound* sound = new Sound(pipeline, ui->volumeSlider, ui->muteButton, ui->volumeLabel);
  sound->fastConnect();

  Player* player = new Player(pipeline, ui->timeSlider, ui->currentTimeText, ui->maxTimeText, ui->stopButton,
                              ui->pauseButton, ui->previewButton, ui->nextButton, this);
  player->fastConnect();

  // Подключение нейросети YOLOv3
  m_yolo_enabled = false;
  if (m_yolo_enabled)
  {
    // Загрузка YOLOv3
    const std::string model_config = "/home/user/project/MediaPlayer/libs/yolov3.cfg";
    const std::string model_weights = "/home/user/project/MediaPlayer/libs/yolov3.weights";
    const std::string model_classes = "/home/user/project/MediaPlayer/libs/coco.names";

    net = cv::dnn::readNetFromDarknet(model_config, model_weights);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    // Загрузка имен классов
    std::ifstream classNames(model_classes);
    if (classNames.is_open())
    {
      std::string className = "";
      while (std::getline(classNames, className))
      {
        classes.push_back(className);
      }
    }
    else
    {
      qCritical() << "Не удалось открыть файл с именами классов";
    }

    // Подготовка
    cv::Mat img = cv::imread("/путь/к/изображению.jpg");
    cv::Mat blob;
    cv::dnn::blobFromImage(img, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(), true, false);
    net.setInput(blob);

    // Прогнозирование
    std::vector<std::string> outputBlobNames = net.getUnconnectedOutLayersNames();
    std::vector<cv::Mat> outputBlobs;
    net.forward(outputBlobs, outputBlobNames);

    float threshold_confidence = 0.5;
    postprocess(img, outputBlobs, threshold_confidence, classes);

    // Отображение результата
    cv::namedWindow("MediaPlayer", cv::WINDOW_NORMAL);
    cv::imshow("MediaPlayer", img);
    cv::waitKey(0);
  }
}

MediaPlayerWindow::~MediaPlayerWindow()
{
  pipeline->setState(QGst::StateNull);

  delete ui;
}

void MediaPlayerWindow::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& output, float threshold_confidence,
                                    const std::vector<std::string>& classes)
{
  std::vector<int> class_ids;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;

  int rows = frame.rows;
  int cols = frame.cols;

  for (const auto& netOutput : output)
  {
    float* data = reinterpret_cast<float*>(netOutput.data);

    for (int i = 0; i < netOutput.rows; ++i, data += netOutput.cols)
    {
      cv::Mat scores = netOutput.row(i).colRange(5, netOutput.cols);
      cv::Point class_id_point;
      double confidence;
      cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &class_id_point);

      if (confidence > static_cast<double>(threshold_confidence))
      {
        int centerX = static_cast<int>(data[0] * cols);
        int centerY = static_cast<int>(data[1] * rows);
        int width = static_cast<int>(data[2] * cols);
        int height = static_cast<int>(data[3] * rows);
        int left = centerX - width / 2;
        int top = centerY - height / 2;

        class_ids.push_back(class_id_point.x);
        confidences.push_back(static_cast<float>(confidence));
        boxes.push_back(cv::Rect(left, top, width, height));
      }
    }
  }

  // NMS
  std::vector<int> indices;
  cv::dnn::NMSBoxes(boxes, confidences, threshold_confidence, 0.4f, indices);
  cv::Scalar color(0, 255, 0);

  for (int idx : indices)
  {
    cv::Rect box = boxes[static_cast<quint64>(idx)];
    int class_id = class_ids[static_cast<quint64>(idx)];
    std::string class_name = classes[static_cast<quint64>(class_id)];
    float conf = confidences[static_cast<quint64>(idx)];

    std::stringstream ss;
    ss << class_name << " " << std::fixed << std::setprecision(2) << conf;
    std::string labelText = ss.str();

    cv::putText(frame, labelText, box.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
    cv::rectangle(frame, box, color, 2);
  }
}
