#include <QMainWindow>

class QCloseEvent;

namespace Ui
{
class MediaPlayerWindow;
}

class MediaPlayerWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MediaPlayerWindow(QWidget* parent = nullptr);

private:
  Ui::MediaPlayerWindow* ui;

private:
  void openFiles();
  QStringList selectedFilesList;

  void enableMenu(const bool& isFileOpen);

  void closeEvent(QCloseEvent* event);
};
