#include <QMainWindow>
#include <QtGStreamer/QGst/Pipeline>
#include <QtGStreamer/QGst/Ui/VideoWidget>

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
};
