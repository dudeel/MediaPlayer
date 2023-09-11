#pragma once

#include <QObject>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

#include <QGst/Pipeline>

class Sound : public QObject
{
  Q_OBJECT

public:
  explicit Sound(QObject* parent = nullptr);

public:
  Sound(QGst::PipelinePtr pipeline, QSlider* volumeSlider, QLabel* volumeLabel = nullptr);
  Sound(QGst::PipelinePtr pipeline, QPushButton* muteButton, QLabel* volumeLabel = nullptr);
  Sound(QGst::PipelinePtr pipeline, QSlider* volumeSlider, QPushButton* muteButton, QLabel* volumeLabel = nullptr);

private:
  enum class SoundStatus
  {
    ENABLE = 1,  // Звук включен
    DISABLE = 2  // Звук отключен
  };
  SoundStatus m_soundStatus;

private:
  double m_soundVolume;
  void loadParameters();

private:
  QGst::PipelinePtr m_pipeline;
  const int m_percent = 15;

private:
  QSlider* m_volumeSlider = nullptr;

private:
  QPushButton* m_muteButton = nullptr;

private:
  QLabel* m_volumeLabel = nullptr;

public:
  bool setSettings();
  bool connectVolumeSlider();
  bool connectMuteButton();

  void fastConnect();

public:
  void mute();
  void addVolume();
  void removeVolume();
};
