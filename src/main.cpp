#include <QCoreApplication>
#include <QTimer>
#include <QProcess>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString sourceFilePath = "/home/user/project/MediaPlayer/data/Monkey.mp4";
    QString outputFilePath = "/home/user/project/MediaPlayer/data/result/Monkey.avi";

    QString gstCommand = QString("gst-launch-1.0 playbin uri=file://%1 video-sink=autovideosink").arg(sourceFilePath);
    QString gstRecordCommand = QString("gst-launch-1.0 filesrc location=%1 ! decodebin ! videoconvert ! x264enc ! avimux ! filesink location=%2").arg(sourceFilePath, outputFilePath);

    //MKV: gst-launch-1.0 filesrc location=/home/user/project/MediaPlayer/data/Monkey.mp4 ! decodebin ! videoconvert ! x264enc ! matroskamux ! filesink location=/home/user/project/MediaPlayer/data/Monkey.mkv
    //WebM: gst-launch-1.0 filesrc location=/home/user/project/MediaPlayer/data/Monkey.mp4 ! decodebin ! videoconvert ! vp8enc ! webmmux ! filesink location=/home/user/project/MediaPlayer/data/Monkey.webm
    //AVI: gst-launch-1.0 filesrc location=/home/user/project/MediaPlayer/data/Monkey.mp4 ! decodebin ! videoconvert ! x264enc ! avimux ! filesink location=/home/user/project/MediaPlayer/data/Monkey.avi


    QProcess gstProcess;
    gstProcess.start(gstCommand);

    QTimer::singleShot(10000, [&]() {
        gstProcess.kill();
        gstProcess.waitForFinished();

        QProcess gstRecordProcess;
        gstRecordProcess.start(gstRecordCommand);
        if (!gstRecordProcess.waitForStarted()) {
            qCritical() << "Возникла проблема при запуске gstRecordProcess";
            a.quit();
            return 1;
        }

        if (!gstRecordProcess.waitForFinished()) {
            qCritical() << "Возникла проблема при завершении gstRecordProcess";
            a.quit();
            return 1;
        }

        int exitCode = gstRecordProcess.exitCode();
        if (exitCode != 0) {
            qCritical() << "gstRecordProcess завершился с ошибкой №" << exitCode;
            a.quit();
            return 1;
        }
        // Завершение программы
        a.quit();
        return 0;
    });

    return a.exec();
}
