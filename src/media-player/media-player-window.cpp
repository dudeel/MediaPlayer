#include "media-player-window.h"
#include "ui_media-player-window.h"

#include <QFileDialog>
#include <QDebug>
#include <QCloseEvent>

#include <gst/gst.h>

struct CustomData
{
  GstElement* playbin; /* Our one and only element */

  gint n_video; /* Number of embedded video streams */
  gint n_audio; /* Number of embedded audio streams */
  gint n_text;  /* Number of embedded subtitle streams */

  gint current_video; /* Currently playing video stream */
  gint current_audio; /* Currently playing audio stream */
  gint current_text;  /* Currently playing subtitle stream */

  GMainLoop* main_loop; /* GLib's Main Loop */
};

enum GstPlayFlags
{
  GST_PLAY_FLAG_VIDEO = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT = (1 << 2)   /* We want subtitle output */
};

MediaPlayerWindow::MediaPlayerWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MediaPlayerWindow)
{
  ui->setupUi(this);
  enableMenu(false);

  //  CustomData data;
  //  GstBus* bus;
  //  GstStateChangeReturn ret;
  //  gint flags;
  //  GIOChannel* io_stdin;
  //  gst_init(nullptr, nullptr);
  //  data.playbin = gst_element_factory_make("playbin", "playbin");
  //  if (!data.playbin)
  //  {
  //    g_printerr("Not all elements could be created.\n");
  //    return;
  //  }
  //  g_object_set(data.playbin, "uri", "rtsp://admin:Admin12345@reg.fuzzun.ru:50232/ISAPI/Streaming/Channels/101",
  //  NULL); g_object_get(data.playbin, "flags", &flags, NULL); g_object_set(data.playbin, "flags", flags, NULL);
  //  g_object_set(data.playbin, "connection-speed", 56, NULL);
  //  bus = gst_element_get_bus(data.playbin);
  //  ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
  //  if (ret == GST_STATE_CHANGE_FAILURE)
  //  {
  //    g_printerr("Unable to set the pipeline to the playing state.\n");
  //    gst_object_unref(data.playbin);
  //    return;
  //  }
  //  data.main_loop = g_main_loop_new(NULL, FALSE);
  //  g_main_loop_run(data.main_loop);
  //  g_main_loop_unref(data.main_loop);
  //  gst_object_unref(bus);
  //  gst_element_set_state(data.playbin, GST_STATE_NULL);
  //  gst_object_unref(data.playbin);
}

void MediaPlayerWindow::openFiles()
{
  const QString windowTitle = "Выберите видео";
  const QString fileFilter = "Все (*.mp4 *.avi *.webm);;MP4 (*.mp4);;AVI (*.avi);;WebM (*.webm)";
  const QString standartDirectory = "/home/user/project/MediaPlayer/data/";

  QFileDialog fileDialog(this);
  fileDialog.setWindowTitle(windowTitle);
  fileDialog.setNameFilter(fileFilter);
  fileDialog.setDirectory(standartDirectory);
  fileDialog.setFileMode(QFileDialog::ExistingFiles);

  if (!fileDialog.exec())
    return;

  selectedFilesList = fileDialog.selectedFiles();
  if (selectedFilesList.isEmpty())
    return;

  for (const QString& selectedFile : qAsConst(selectedFilesList))
  {
    enableMenu(true);
    qDebug() << selectedFile;
  }
}

void MediaPlayerWindow::enableMenu(const bool& isFileOpen)
{
  if (!isFileOpen)
  {
    // Menu
    ui->addVolume->setEnabled(false);
    ui->removeVolume->setEnabled(false);
    ui->enableVolume->setEnabled(false);

    ui->timePreview->setEnabled(false);
    ui->timeNext->setEnabled(false);
    ui->playVideo->setEnabled(false);
    ui->stopVideo->setEnabled(false);

    ui->convertMenu->setEnabled(false);

    ui->yolov3->setEnabled(false);

    // Buttons
    ui->stopButton->setEnabled(false);
    ui->previewButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->timeSlider->setEnabled(false);

    ui->muteButton->setEnabled(false);
    ui->volumeSlider->setEnabled(false);

    // Text
    ui->currentTimeText->setText("00:00:00");
    ui->maxTimeText->setText("00:00:00");
    ui->volumeLabel->setText("0%");

    // Connects
    QObject::connect(ui->openFile, &QAction::triggered, this, [this]() { openFiles(); });
    QObject::connect(ui->exit, &QAction::triggered, this, [=]() { close(); });
  }
  else
  {
    // Menu
    ui->addVolume->setEnabled(true);
    ui->removeVolume->setEnabled(true);
    ui->enableVolume->setEnabled(true);

    ui->timePreview->setEnabled(true);
    ui->timeNext->setEnabled(true);
    ui->playVideo->setEnabled(true);
    ui->stopVideo->setEnabled(true);

    ui->convertMenu->setEnabled(true);

    ui->yolov3->setEnabled(true);

    // Buttons
    ui->stopButton->setEnabled(true);
    ui->previewButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->nextButton->setEnabled(true);
    ui->timeSlider->setEnabled(true);

    ui->muteButton->setEnabled(true);
    ui->volumeSlider->setEnabled(true);

    // Text
    ui->currentTimeText->setText("00:00:00");
    ui->maxTimeText->setText("00:00:00");
    ui->volumeLabel->setText("100%");

    // Connects
    QObject::connect(ui->addVolume, &QAction::triggered, this, []() {});
    QObject::connect(ui->removeVolume, &QAction::triggered, this, []() {});
    QObject::connect(ui->enableVolume, &QAction::triggered, this, []() {});

    QObject::connect(ui->timePreview, &QAction::triggered, this, []() {});
    QObject::connect(ui->timeNext, &QAction::triggered, this, []() {});
    QObject::connect(ui->playVideo, &QAction::triggered, this, []() {});
    QObject::connect(ui->stopVideo, &QAction::triggered, this, []() {});

    QObject::connect(ui->mp4, &QAction::triggered, this, []() {});
    QObject::connect(ui->avi, &QAction::triggered, this, []() {});
    QObject::connect(ui->webm, &QAction::triggered, this, []() {});

    QObject::connect(ui->yolov3, &QAction::changed, this, []() {});
  }
}

void MediaPlayerWindow::closeEvent(QCloseEvent* event)
{
  event->accept();
}
