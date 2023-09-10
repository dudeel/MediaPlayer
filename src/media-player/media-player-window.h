#include <QMainWindow>

#include <QEventLoop>
#include <QGst/Pipeline>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

using namespace cv;
using namespace dnn;

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

  Net net;
  bool m_yolo_enabled;
  QTimer* m_timer;
  std::vector<std::string> classes;

  void postprocess(Mat& frame, const std::vector<Mat>& output, float threshold_confidence,
                   const std::vector<std::string>& classes);

  QGst::SamplePtr onNewSample(QGst::ElementPtr app_sink);
  void waitForStateChanged(QGst::State state, int timeout_ms);
  void initSoundAndPlayer();
};
