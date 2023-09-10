#include "player.h"
#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/ClockTime>
#include <QGst/Event>
#include <QGst/StreamVolume>

Player::Player(QWidget* parent) : QGst::Ui::VideoWidget(parent)
{
  Q_UNUSED(parent);
}

Player::Player(QGst::PipelinePtr pipeline, QSlider* timeSlider, QLabel* currentTimeText, QLabel* maxTimeText,
               QWidget* parent)
  : m_pipeline(pipeline), m_timeSlider(timeSlider), m_currentTimeText(currentTimeText), m_maxTimeText(maxTimeText)
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

  connect(timer, &QTimer::timeout, [this]() {
    double currentPositionSlider = static_cast<double>(position().msecsTo(QTime(0, 0))) / length().msecsTo(QTime(0, 0));
    m_timeSlider->setValue(static_cast<int>(currentPositionSlider * length().msecsSinceStartOfDay()));
  });

  timer->start(10);

  m_timeSlider->setTickInterval(10);
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

  connect(m_timeSlider, &QSlider::sliderPressed, [this]() {
    m_isSliderPressed = true;
    qDebug() << "slider Pressed";
  });

  connect(m_timeSlider, &QSlider::sliderReleased, [this]() {
    m_isSliderPressed = false;
    qDebug() << "slider unpressed";

    double value = m_timeSlider->value();
    double duration = length().msecsSinceStartOfDay();

    if (duration != 0.0 && value > 0.0)
    {
      QTime pos(0, 0);
      pos = pos.addMSecs(static_cast<int>((value / static_cast<double>(duration)) * duration));
      this->setPosition(pos);
    }

    m_currentTimeText->setText(position().toString("hh:mm:ss"));
  });

  connect(m_timeSlider, &QSlider::valueChanged, [this]() {
    m_currentTimeText->setText(position().toString("hh:mm:ss"));
    m_maxTimeText->setText(this->length().toString("hh:mm:ss"));
  });

  return true;
}

void Player::fastConnect()
{
  createTimer();
  connectVideoSlider();
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
