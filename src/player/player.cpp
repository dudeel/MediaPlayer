#include "player.h"

#include <QGst/Query>
#include <QGst/Event>

Player::Player(QWidget* parent) : QGst::Ui::VideoWidget(parent)
{
  Q_UNUSED(parent);
}

Player::Player(QGst::PipelinePtr pipeline, QSlider* timeSlider, QLabel* currentTimeText, QLabel* maxTimeText,
               QPushButton* stopButton, QPushButton* pauseButton, QPushButton* previewButton, QPushButton* nextButton,
               QWidget* parent)
  : m_pipeline(pipeline)
  , m_timeSlider(timeSlider)
  , m_currentTimeText(currentTimeText)
  , m_maxTimeText(maxTimeText)
  , m_stopButton(stopButton)
  , m_pauseButton(pauseButton)
  , m_previewButton(previewButton)
  , m_nextButton(nextButton)
{
  Q_UNUSED(parent);
  loadParameters();
}

void Player::loadParameters()
{
  m_videoStatus = VideoStatus::PLAYING;
  m_sliderStatus = SliderStatus::UNPRESSED;

  m_currentTimeText->setText(position().toString("hh:mm:ss"));
  m_maxTimeText->setText(length().toString("hh:mm:ss"));
}

void Player::createTimer()
{
  QTimer* timer = new QTimer();
  const quint8 sliderUpdateInterval = 10;

  QObject::connect(timer, &QTimer::timeout, [this]() {
    if (m_sliderStatus == SliderStatus::PRESSED)
      return;

    double currentPositionSlider = static_cast<double>(position().msecsTo(QTime(0, 0))) / length().msecsTo(QTime(0, 0));
    m_timeSlider->setValue(static_cast<int>(currentPositionSlider * length().msecsSinceStartOfDay()));
  });

  timer->start(sliderUpdateInterval);

  m_timeSlider->setTickInterval(sliderUpdateInterval);
  m_timeSlider->setMaximum(length().msecsSinceStartOfDay());
}

bool Player::connectVideoSlider()
{
  if (!m_pipeline)
  {
    qCritical() << "Вы не передали в конструктор класса: QGst::PipelinePtr";
    return false;
  }
  else if (!m_timeSlider)
  {
    qCritical() << "Вы не передали в конструктор класса: QSlider";
    return false;
  }
  else if (!m_currentTimeText)
  {
    qInfo() << "Вы не передали в конструктор класса QLabel, отображение текущего времени видео отключено";
  }
  else if (!m_maxTimeText)
  {
    qInfo() << "Вы не передали в конструктор класса QLabel, отображение общего времени видео отключено";
  }

  QObject::connect(m_timeSlider, &QSlider::sliderPressed, [this]() { m_sliderStatus = SliderStatus::PRESSED; });

  QObject::connect(m_timeSlider, &QSlider::sliderReleased, [this]() {
    m_sliderStatus = SliderStatus::UNPRESSED;

    double value = m_timeSlider->value();
    double duration = length().msecsSinceStartOfDay();

    if (duration != 0.0 && value > 0.0)
    {
      QTime pos(0, 0);
      pos = pos.addMSecs(static_cast<int>((value / static_cast<double>(duration)) * duration));
      this->setPosition(pos);
    }
  });

  QObject::connect(m_timeSlider, &QSlider::valueChanged, [this]() {
    if (m_sliderStatus == SliderStatus::PRESSED)
    {
      double value = m_timeSlider->value();
      double duration = length().msecsSinceStartOfDay();

      if (duration != 0.0 && value > 0.0)
      {
        QTime currentTime(0, 0);
        currentTime = currentTime.addMSecs(static_cast<int>((value / static_cast<double>(duration)) * duration));
        m_currentTimeText->setText(currentTime.toString("hh:mm:ss"));
      }
    }
    else
    {
      m_currentTimeText->setText(position().toString("hh:mm:ss"));
    }
  });

  return true;
}

bool Player::connectStopButton()
{
  if (!m_stopButton)
  {
    qCritical() << Q_FUNC_INFO << "Вы не передали в конструктор класса: QPushButton";
    return false;
  }

  QObject::connect(m_stopButton, &QPushButton::clicked, [this]() { stop(); });

  return true;
}

bool Player::connectPauseButton()
{
  if (!m_pauseButton)
  {
    qCritical() << Q_FUNC_INFO << "Вы не передали в конструктор класса: QPushButton";
    return false;
  }

  QObject::connect(m_pauseButton, &QPushButton::clicked, [this]() { pause(); });

  return true;
}
void Player::pause()
{
  if (m_videoStatus == VideoStatus::PLAYING)
  {
    m_pipeline->setState(QGst::StatePaused);
    m_videoStatus = VideoStatus::PAUSED;
  }
  else
  {
    m_pipeline->setState(QGst::StatePlaying);
    m_videoStatus = VideoStatus::PLAYING;
  }
}

void Player::stop()
{
  setPosition(QTime(0, 0));
  if (m_videoStatus == VideoStatus::PLAYING)
  {
    m_pipeline->setState(QGst::StatePaused);
    m_videoStatus = VideoStatus::PAUSED;
  }
}

bool Player::connectPreviewButton()
{
  if (!m_previewButton)
  {
    qCritical() << Q_FUNC_INFO << "Вы не передали в конструктор класса: QPushButton";
    return false;
  }

  connect(m_previewButton, &QPushButton::clicked, [this]() { preview(); });

  return true;
}

bool Player::connectNextButton()
{
  if (!m_nextButton)
  {
    qCritical() << Q_FUNC_INFO << "Вы не передали в конструктор класса: QPushButton";
    return false;
  }

  connect(m_nextButton, &QPushButton::clicked, [this]() { next(); });

  return true;
}

void Player::fastConnect()
{
  createTimer();
  connectVideoSlider();
  connectStopButton();
  connectPauseButton();
  connectPreviewButton();
  connectNextButton();
}

void Player::preview()
{
  QTime currentPos = position();
  int newPositionInMilliseconds = std::max(currentPos.msecsSinceStartOfDay() - 10000, 0);
  QTime newPosition = QTime(0, 0).addMSecs(newPositionInMilliseconds);
  setPosition(newPosition);
}

void Player::next()
{
  QTime currentPos = position();
  int newPositionInMilliseconds = std::min(currentPos.msecsSinceStartOfDay() + 10000, length().msecsSinceStartOfDay());
  QTime newPosition = QTime(0, 0).addMSecs(newPositionInMilliseconds);
  setPosition(newPosition);
}

QTime Player::position() const
{
  if (m_pipeline)
  {
    QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
    m_pipeline->query(query);
    return QGst::ClockTime(static_cast<quint64>(query->position())).toTime();
  }
  else
  {
    return QTime(0, 0);
  }
}

void Player::setPosition(const QTime& pos)
{
  QGst::SeekEventPtr evt =
      QGst::SeekEvent::create(1.0, QGst::FormatTime, QGst::SeekFlagFlush, QGst::SeekTypeSet,
                              QGst::ClockTime::fromTime(pos), QGst::SeekTypeNone, QGst::ClockTime::None);

  m_pipeline->sendEvent(evt);
}

QTime Player::length() const
{
  if (m_pipeline)
  {
    QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
    m_pipeline->query(query);
    return QGst::ClockTime(static_cast<quint64>(query->duration())).toTime();
  }
  else
  {
    return QTime(0, 0);
  }
}

Player::~Player()
{
  if (m_pipeline)
  {
    m_pipeline->setState(QGst::StateNull);
    stopPipelineWatch();
  }
}
