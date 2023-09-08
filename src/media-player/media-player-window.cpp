#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <QGst/Init>
#include <QGst/Parse>
#include <QGst/ElementFactory>

#include <QUrl>
#include <QTimer>

#include "sound/sound.h"

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

  // Подключение слайдера громкости
  Sound* sound = new Sound(pipeline, ui->volumeSlider, ui->muteButton, ui->volumeLabel);
  sound->fastConnect();

  // Подключение слайдера воспроизведения видео
  m_player = new Player(pipeline, this);
  ui->currentTimeText->setText("00:00:00");
  ui->maxTimeText->setText(m_player->length().toString("hh:mm:ss"));

  QTimer* timer = new QTimer();
  connect(timer, &QTimer::timeout, [this]() {
    if (m_player->position().toString("hh:mm:ss") != "23:34:33")
      ui->currentTimeText->setText(m_player->position().toString("hh:mm:ss"));
  });
  timer->start(1000);

  ui->timeSlider->setTickInterval(10);
  ui->timeSlider->setMaximum(1000);

  connect(ui->timeSlider, &QSlider::valueChanged, [this](double value) {
    double length = -this->m_player->length().msecsTo(QTime(0, 0));
    if (length != 0.0 && value > 0.0)
    {
      QTime pos(0, 0);
      pos = pos.addMSecs(static_cast<int>(length * (value / 1000.0)));
      this->m_player->setPosition(pos);

      if (m_player->position().toString("hh:mm:ss") != "23:34:33")
        ui->currentTimeText->setText(m_player->position().toString("hh:mm:ss"));
      ui->maxTimeText->setText(m_player->length().toString("hh:mm:ss"));
    }
  });

  //
}

MediaPlayerWindow::~MediaPlayerWindow()
{
  pipeline->setState(QGst::StateNull);

  delete ui;
}
