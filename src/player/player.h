#pragma once

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class Player : public QGst::Ui::VideoWidget
{
  Q_OBJECT
public:
  Player(QWidget* parent = nullptr);
  Player(QGst::PipelinePtr pipeline, QWidget* parent = nullptr);
  ~Player();

  QTime position() const;
  void setPosition(const QTime& pos);
  QTime length() const;

private:
  QGst::PipelinePtr m_pipeline;
  QTimer m_positionTimer;

Q_SIGNALS:
  void positionChanged();
};
