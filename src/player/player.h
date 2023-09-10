#pragma once

#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QSlider>
#include <QTime>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class Player : public QGst::Ui::VideoWidget
{
  Q_OBJECT
public:
  Player(QWidget* parent = nullptr);
  Player(QGst::PipelinePtr pipeline, QSlider* timeSlider, QLabel* currentTimeText = nullptr,
         QLabel* maxTimeText = nullptr, QWidget* parent = nullptr);
  ~Player();

private:
  enum class VideoStatus
  {
    PLAYING = 1,
    PAUSED = 2
  };
  enum class SliderStatus
  {
    PRESSED = 1,
    UNPRESSED = 2
  };

  SliderStatus m_sliderStatus;
  VideoStatus m_videoStatus;

private:
  QTimer m_positionTimer;

private:
  QGst::PipelinePtr m_pipeline;
  QSlider* m_timeSlider = nullptr;
  QLabel* m_currentTimeText = nullptr;
  QLabel* m_maxTimeText = nullptr;

  bool m_isSliderPressed;

private:
  QTime position() const;
  QTime length() const;
  void setPosition(const QTime& pos);

  void loadParameters();
  void createTimer();

public:
  //  bool setSettings();
  bool connectVideoSlider();
  void fastConnect();
};
