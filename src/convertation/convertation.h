#pragma once

#include <QObject>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

class Convertation : public QObject
{
  Q_OBJECT
public:
  Convertation();

public:
  enum class OutputFormat
  {
    MP4,
    AVI,
    WebM,
    Unsupported
  };

public:
  void startConvertation(const QString& sourceFile, const QString& outputFile, OutputFormat outputFormat,
                         bool includeAudio);

private:
  void convertVideo(const QString& sourceFile, const QString& outputFile, OutputFormat outputFormat, bool includeAudio);

private:
  OutputFormat getOutputFormatFromExtension(const QString& extension);
  QString outputFormatToString(OutputFormat format);
  QString convertBytes(qint64 bytes);

private:
  QThread convertThread;
  bool conversionRunning = false;

Q_SIGNALS:
  void conversionFinished();
  void conversionProgress(int progress);
};
