#include <QMainWindow>
#include <QGst/Pipeline>
#include "player/player.h"

namespace Ui
{
class MediaPlayerWindow;
}

class MediaPlayerWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MediaPlayerWindow(QWidget* parent = nullptr);
  ~MediaPlayerWindow();

private:
  Ui::MediaPlayerWindow* ui;
  QGst::PipelinePtr pipeline;
  Player* m_player;
};
