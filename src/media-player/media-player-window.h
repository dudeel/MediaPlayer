#include <QMainWindow>

#include <QGst/Pipeline>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

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

  cv::dnn::Net net;
  bool m_yolo_enabled;
  QTimer* m_timer;
  std::vector<std::string> classes;

  void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& output, float threshold_confidence,
                   const std::vector<std::string>& classes);

  QGst::SamplePtr onNewSample(QGst::ElementPtr app_sink);
  void waitForStateChanged(QGst::State state, int timeout_ms);
  void initAddons();
};
