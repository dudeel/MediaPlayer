#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <QVBoxLayout>
#include <QUrl>
#include <QtGStreamer/QGst/Init>
#include <QtGStreamer/QGst/Parse>
#include <QtGStreamer/QGst/ElementFactory>

MediaPlayerWindow::MediaPlayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaPlayerWindow)
{
    ui->setupUi(this);

    QGst::init(nullptr, nullptr);
    QGst::init();

    pipeline = QGst::Parse::launch("playbin").dynamicCast<QGst::Pipeline>();
    if (!pipeline) {
        qCritical() << "Failed to create playbin";
        return;
    }

    QGst::Ui::VideoWidget *video_widget = new QGst::Ui::VideoWidget;
    QVBoxLayout *layout = new QVBoxLayout(ui->mediaBox);
    layout->addWidget(video_widget);

    QUrl videoUrl = QUrl::fromLocalFile("/home/user/project/MediaPlayer/data/Monkey.mp4");
    pipeline->setProperty("uri", videoUrl.toString());
    pipeline->setProperty("video-sink", QGst::ElementFactory::make("qwidget5videosink", "video_sink"));
    pipeline->setState(QGst::StatePlaying);

    QGst::ElementPtr video_sink = pipeline->property("video-sink").get<QGst::ElementPtr>();
    video_widget->setVideoSink(video_sink);

    this->setWindowTitle("Media Player");
}

MediaPlayerWindow::~MediaPlayerWindow()
{
    pipeline->setState(QGst::StateNull);

    delete ui;
}