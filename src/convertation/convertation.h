#pragma once

#include <QObject>

class Convertation : public QObject
{
  Q_OBJECT
public:
  explicit Convertation(QObject* parent = nullptr);

public:
  enum class OutputFormat
  {
    MP4,
    AVI,
    WebM,
    Unsupported
  };

public:
  void convertVideo(const QString& sourceFile, const QString& outputFile, Convertation::OutputFormat outputFormat,
                    bool includeAudio);

private:
  Convertation::OutputFormat getOutputFormatFromExtension(const QString& extension);
  QString outputFormatToString(OutputFormat format);
};
