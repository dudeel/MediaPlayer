#include "convertation.h"

#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QTime>
#include <QtConcurrent/QtConcurrent>

const QString GST_PIPELINE_TEMPLATE =
    "gst-launch-1.0 filesrc location=%1 ! decodebin name=demux demux. ! "
    "videoconvert ! x264enc ! %2mux name=mux %3";

Convertation::Convertation() : QObject(nullptr)
{
  this->moveToThread(&convertThread);
  convertThread.start();
}

Convertation::OutputFormat Convertation::getOutputFormatFromExtension(const QString& extension)
{
  if (extension == "mp4")
    return OutputFormat::MP4;
  if (extension == "avi")
    return OutputFormat::AVI;
  if (extension == "webm")
    return OutputFormat::WebM;
  return OutputFormat::Unsupported;
}

QString Convertation::outputFormatToString(OutputFormat format)
{
  switch (format)
  {
    case OutputFormat::MP4:
      return "mp4";
    case OutputFormat::AVI:
      return "avi";
    case OutputFormat::WebM:
      return "webm";
    default:
      return "";
  }
}

void Convertation::convertVideo(const QString& sourceFile, const QString& outputFile,
                                Convertation::OutputFormat outputFormat, bool includeAudio)
{
  QString outputFormatStr = outputFormatToString(outputFormat);

  QString finalOutputFile = QString("%1.%2").arg(outputFile, outputFormatStr);

  // Проверка наличия файла с таким же названием и форматом
  if (QFile::exists(finalOutputFile))
  {
    qDebug() << "Файл с таким же названием и форматом уже существует.";
    return;
  }

  QString audioPart = includeAudio ? "demux. ! audioconvert ! audioresample ! audio/x-raw,rate=44100 ! voaacenc ! "
                                     "mux. " :
                                     "demux. ! fakesink ";

  QString pipeline = GST_PIPELINE_TEMPLATE.arg(sourceFile, outputFormatStr, audioPart);

  if (outputFormat == OutputFormat::WebM)
  {
    pipeline.replace("x264enc", "vp8enc");
    pipeline.replace("voaacenc", "vorbisenc");
  }

  pipeline += QString("mux. ! filesink location=%1.%2").arg(outputFile, outputFormatStr);

  QFileInfo sourceInfo(sourceFile);
  qint64 sourceSize = sourceInfo.size();
  QTime startTime = QTime::currentTime();
  QString progressInfo;
  QProcess process;
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start(pipeline);

  if (process.waitForStarted())
  {
    while (process.waitForReadyRead(-1))
    {
      progressInfo += QString::fromLocal8Bit(process.readAll());

      QStringList progressMessages = progressInfo.split("\n", Qt::SkipEmptyParts);

      for (const QString& message : progressMessages)
      {
        qDebug() << "Progress:" << message;
      }
    }

    process.waitForFinished();
  }
  else
  {
    qDebug() << "Failed to start the process!";
  }

  QTime endTime = QTime::currentTime();
  int duration = startTime.msecsTo(endTime);

  // Конвертация размера в удобный формат
  QString sourceSizeStr = convertBytes(sourceSize);
  QString outputSizeStr = convertBytes(QFileInfo(outputFile + "." + outputFormatStr).size());

  qDebug() << "Source file size:" << sourceSizeStr;
  qDebug() << "Output file size:" << outputSizeStr;

  // Конвертация времени в формат hh:mm:ss
  QTime convertedDuration(0, 0);
  convertedDuration = convertedDuration.addMSecs(duration);

  qDebug() << "Time taken for conversion:" << convertedDuration.toString("hh:mm:ss");

  conversionRunning = false;
}

QString Convertation::convertBytes(qint64 bytes)
{
  QString sizeString;
  double size = static_cast<double>(bytes);

  if (size < 1024)
  {
    sizeString = QString::number(size, 'f', 2) + " bytes";
  }
  else if (size < 1048576)
  {
    size = size / 1024;
    sizeString = QString::number(size, 'f', 2) + " KB";
  }
  else if (size < 1073741824)
  {
    size = size / 1048576;
    sizeString = QString::number(size, 'f', 2) + " MB";
  }
  else
  {
    size = size / 1073741824;
    sizeString = QString::number(size, 'f', 2) + " GB";
  }

  return sizeString;
}

void Convertation::startConvertation(const QString& sourceFile, const QString& outputFile, OutputFormat outputFormat,
                                     bool includeAudio)
{
  // Проверка на запущенную конвертацию
  if (conversionRunning)
  {
    qDebug() << "Конвертация уже запущена.";
    return;
  }

  conversionRunning = true;

  // Создаем лямбда-функцию для запуска convertVideo внутри потока
  auto converterLambda = [this, sourceFile, outputFile, outputFormat, includeAudio]() {
    this->convertVideo(sourceFile, outputFile, outputFormat, includeAudio);
    this->conversionFinished();
    conversionRunning = false;
  };

  // Запускаем лямбда-функцию в отдельном потоке
  QtConcurrent::run(converterLambda);
}
