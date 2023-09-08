#include "sound.h"
#include <QDebug>

Sound::Sound(QObject* parent) : QObject(parent)
{
  Q_UNUSED(parent);
}

Sound::Sound(QGst::PipelinePtr pipeline, QSlider* volumeSlider, QLabel* volumeLabel)
  : m_pipeline(pipeline), m_volumeSlider(volumeSlider), m_volumeLabel(volumeLabel)
{
  loadParameters();
}

Sound::Sound(QGst::PipelinePtr pipeline, QPushButton* muteButton, QLabel* volumeLabel)
  : m_pipeline(pipeline), m_muteButton(muteButton), m_volumeLabel(volumeLabel)
{
  loadParameters();
}

Sound::Sound(QGst::PipelinePtr pipeline, QSlider* volumeSlider, QPushButton* muteButton, QLabel* volumeLabel)
  : m_pipeline(pipeline), m_volumeSlider(volumeSlider), m_muteButton(muteButton), m_volumeLabel(volumeLabel)
{
  loadParameters();
}

void Sound::loadParameters()
{
  m_soundStatus = SoundStatus::ENABLED;
  m_soundVolume = 100;
  m_pipeline->setProperty("volume", m_soundVolume / 100);

  if (m_volumeSlider)
    m_volumeSlider->setValue(m_volumeSlider->maximum());

  if (m_volumeLabel)
    m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));
}

bool Sound::setSettings()
{
  if (!m_volumeSlider)
  {
    qCritical() << "Вы не передали в конструктор класса: QSlider";
    return false;
  }

  m_volumeSlider->setRange(0, 100);
  m_volumeSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  return true;
}

bool Sound::connectVolumeSlider()
{
  if (!m_pipeline)
  {
    qCritical() << "Вы не передали в конструктор класса: QGst::PipelinePtr";
    return false;
  }
  else if (!m_volumeSlider)
  {
    qCritical() << "Вы не передали в конструктор класса: QSlider";
    return false;
  }
  else if (!m_volumeLabel)
  {
    qInfo() << "Вы не передали в конструктор класса QLabel, отображение текста отключено";
  }

  QObject::connect(m_volumeSlider, &QSlider::valueChanged, [this](double volume) {
    m_soundVolume = volume;

    m_pipeline->setProperty("volume", m_soundVolume / 100);

    if (m_volumeLabel)
      m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));

    if (m_soundStatus == SoundStatus::MUTED)
      m_soundStatus = SoundStatus::ENABLED;
  });

  return true;
}

bool Sound::connectMuteButton()
{
  if (!m_pipeline)
  {
    qCritical() << "Вы не передали в конструктор класса: QGst::PipelinePtr";
    return false;
  }
  else if (!m_muteButton)
  {
    qCritical() << "Вы не передали в конструктор класса: QPushButton";
    return false;
  }
  else if (!m_volumeLabel)
  {
    qInfo() << "Вы не передали в конструктор класса QLabel, отображение текста отключено";
  }

  QObject::connect(m_muteButton, &QPushButton::released, [this]() {
    switch (m_soundStatus)
    {
      case SoundStatus::ENABLED:
        m_pipeline->setProperty("volume", 0);
        m_soundStatus = SoundStatus::MUTED;

        break;

      case SoundStatus::MUTED:
        m_pipeline->setProperty("volume", m_soundVolume / 100);
        m_soundStatus = SoundStatus::ENABLED;

        break;
    }

    if (m_volumeLabel)
      m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));
  });

  return true;
}

void Sound::fastConnect()
{
  setSettings();

  if (!m_pipeline)
    return;

  connectVolumeSlider();
  connectMuteButton();
}
