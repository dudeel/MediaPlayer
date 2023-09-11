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
  m_soundStatus = SoundStatus::ENABLE;
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

    if (m_soundStatus == SoundStatus::DISABLE)
      m_soundStatus = SoundStatus::ENABLE;
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

  QObject::connect(m_muteButton, &QPushButton::released, [this]() { mute(); });

  return true;
}

void Sound::mute()
{
  switch (m_soundStatus)
  {
    case SoundStatus::ENABLE:
      m_pipeline->setProperty("volume", 0);
      m_soundStatus = SoundStatus::DISABLE;

      break;

    case SoundStatus::DISABLE:
      m_pipeline->setProperty("volume", m_soundVolume / 100);
      m_soundStatus = SoundStatus::ENABLE;

      break;
  }

  if (m_volumeLabel)
    m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));
}

void Sound::addVolume()
{
  m_soundVolume += m_percent;
  if (m_soundVolume > 100)
  {
    m_soundVolume = 100;
  }

  m_pipeline->setProperty("volume", m_soundVolume / 100);

  if (m_volumeSlider)
  {
    m_volumeSlider->setValue(static_cast<int>(m_soundVolume));
  }

  if (m_volumeLabel)
  {
    m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));
  }
}

void Sound::removeVolume()
{
  m_soundVolume -= m_percent;
  if (m_soundVolume < 0)
  {
    m_soundVolume = 0;
  }

  m_pipeline->setProperty("volume", m_soundVolume / 100);

  if (m_volumeSlider)
  {
    m_volumeSlider->setValue(static_cast<int>(m_soundVolume));
  }

  if (m_volumeLabel)
  {
    m_volumeLabel->setText(QString("%1%").arg(m_soundVolume));
  }
}

void Sound::fastConnect()
{
  setSettings();

  if (!m_pipeline)
    return;

  connectVolumeSlider();
  connectMuteButton();
}
