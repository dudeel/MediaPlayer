#include "convertation.h"

#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QString>

Convertation::Convertation(QObject* parent) : QObject{ parent }
{
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

  QString pipeline = QString(
                         "gst-launch-1.0 filesrc location=%1 ! decodebin name=demux demux. ! "
                         "videoconvert ! x264enc ! %2mux name=mux ")
                         .arg(sourceFile, outputFormatStr);

  if (outputFormat != OutputFormat::Unsupported)
  {
    if (includeAudio)
    {
      pipeline +=
          "demux. ! audioconvert ! audioresample ! audio/x-raw,rate=44100 ! voaacenc "
          "! mux. ";
    }
    else
    {
      pipeline += "demux. ! fakesink ";
    }
  }

  if (outputFormat == OutputFormat::WebM)
  {
    pipeline.replace("x264enc", "vp8enc");
    pipeline.replace("voaacenc", "vorbisenc");
  }

  pipeline += QString("mux. ! filesink location=%1.%2").arg(outputFile, outputFormatStr);

  qDebug() << "Pipeline:" << pipeline;

  int result = system(pipeline.toStdString().c_str());

  qDebug() << "Conversion result:" << result;
}
