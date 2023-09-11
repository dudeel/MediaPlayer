#pragma once

#include <QDebug>

#include <QLabel>
#include <QSlider>
#include <QPushButton>

#include <QTime>
#include <QTimer>

#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class Player : public QGst::Ui::VideoWidget
{
  Q_OBJECT
public:
  Player(QWidget* parent = nullptr);

public:
  Player(QGst::PipelinePtr pipeline, QSlider* timeSlider, QLabel* currentTimeText = nullptr,
         QLabel* maxTimeText = nullptr, QPushButton* stopButton = nullptr, QPushButton* pauseButton = nullptr,
         QPushButton* previewButton = nullptr, QPushButton* nextButton = nullptr, QWidget* parent = nullptr);
  ~Player();

private:
  enum class VideoStatus
  {
    PLAYING = 1,
    PAUSED = 2
  };

  VideoStatus m_videoStatus;

private:
  enum class SliderStatus
  {
    PRESSED = 1,
    UNPRESSED = 2
  };

  SliderStatus m_sliderStatus;

private:
  QTimer m_positionTimer;

private:
  QGst::PipelinePtr m_pipeline;

private:
  QSlider* m_timeSlider = nullptr;

private:
  QLabel* m_currentTimeText = nullptr;
  QLabel* m_maxTimeText = nullptr;

private:
  QPushButton* m_stopButton = nullptr;
  QPushButton* m_pauseButton = nullptr;
  QPushButton* m_previewButton = nullptr;
  QPushButton* m_nextButton = nullptr;

private:
  QTime position() const;
  QTime length() const;

private:
  void setPosition(const QTime& pos);

private:
  void loadParameters();
  void createTimer();

public:
  bool connectVideoSlider();
  bool connectStopButton();
  bool connectPauseButton();
  bool connectPreviewButton();
  bool connectNextButton();

  void fastConnect();

public:
  void stop();
  void pause();
  void preview();
  void next();
  void setSpeed(qreal speed);
};
