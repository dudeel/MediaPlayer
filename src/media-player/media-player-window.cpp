#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <QUrl>
#include <QtGStreamer/QGst/Init>
#include <QtGStreamer/QGst/Parse>
#include <QtGStreamer/QGst/ElementFactory>

MediaPlayerWindow::MediaPlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaPlayerWindow)
{
    ui->setupUi(this);
    setWindowTitle("Media Player");

    QGst::init(nullptr, nullptr);

    pipeline = QGst::Parse::launch("playbin").dynamicCast<QGst::Pipeline>();
    if (!pipeline) {
        qCritical() << "Failed to create playbin";
        return;
    }

    QGst::Ui::VideoWidget *video_widget = new QGst::Ui::VideoWidget;
    ui->mediaBox->addWidget(video_widget);

    QUrl videoUrl = QUrl::fromLocalFile("/home/user/project/MediaPlayer/data/Monkey.mp4");
    pipeline->setProperty("uri", videoUrl.toString());
    pipeline->setProperty("video-sink", QGst::ElementFactory::make("qwidget5videosink", "video_sink"));
    pipeline->setState(QGst::StatePlaying);

    QGst::ElementPtr video_sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
    video_widget->setVideoSink(video_sink);

    // Слайдер громкости (+Label)
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(ui->volumeSlider, &QSlider::valueChanged, [this](double volume) {
        soundVolume = volume;
        pipeline->setProperty("volume", soundVolume / 100);
        ui->volumeLabel->setText(QString("%1%").arg(soundVolume));
        if (soundStatus == SoundStatus::MUTED) soundStatus = SoundStatus::ENABLED;
    });
    connect(ui->muteButton, &QPushButton::released, [this](){
        switch (soundStatus) {
        case SoundStatus::ENABLED:
            pipeline->setProperty("volume", 0);
            ui->volumeLabel->setText(QString("%1%").arg(soundVolume));
            soundStatus = SoundStatus::MUTED;
            break;
        case SoundStatus::MUTED:
            pipeline->setProperty("volume", soundVolume / 100);
            ui->volumeLabel->setText(QString("%1%").arg(soundVolume));
            soundStatus = SoundStatus::ENABLED;
            break;
        }
    });
}

MediaPlayerWindow::~MediaPlayerWindow()
{
    pipeline->setState(QGst::StateNull);

    delete ui;
}
