#include <QApplication>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QVideoWidget videoWidget;
    QMediaPlayer mediaPlayer;

    mediaPlayer.setVideoOutput(&videoWidget);
    mediaPlayer.setMedia(QUrl::fromLocalFile("/home/user/project/untitled/data/Monke.mp4"));

    mediaPlayer.play();
    videoWidget.show();

    return app.exec();
}
