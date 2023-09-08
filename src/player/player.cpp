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

Player::Player(QGst::PipelinePtr pipeline, QWidget* parent) : m_pipeline(pipeline)
{
  Q_UNUSED(parent);
  connect(&m_positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));
}

Player::~Player()
{
  if (m_pipeline)
  {
    m_pipeline->setState(QGst::StateNull);
    stopPipelineWatch();
  }
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
