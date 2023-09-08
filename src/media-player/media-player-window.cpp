#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <QUrl>
#include <QtGStreamer/QGst/Init>
#include <QtGStreamer/QGst/Parse>
#include <QtGStreamer/QGst/ElementFactory>
#include <sound/sound.h>

MediaPlayerWindow::MediaPlayerWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MediaPlayerWindow)
{
  ui->setupUi(this);
  setWindowTitle("Media Player");

  QGst::init(nullptr, nullptr);

  pipeline = QGst::Parse::launch("playbin").dynamicCast<QGst::Pipeline>();
  if (!pipeline)
  {
    qCritical() << "Failed to create playbin";
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

  // Подключение слайдера громкости
  Sound* sound = new Sound(pipeline, ui->volumeSlider, ui->muteButton, ui->volumeLabel);
  sound->fastConnect();
}

MediaPlayerWindow::~MediaPlayerWindow()
{
  pipeline->setState(QGst::StateNull);

  delete ui;
}
