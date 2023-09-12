#ifndef CONVERTATIONWINDOW_H
#define CONVERTATIONWINDOW_H

#include <QWidget>

namespace Ui {
class ConvertationWindow;
}

class ConvertationWindow : public QWidget
{
  Q_OBJECT

public:
  explicit ConvertationWindow(QWidget *parent = nullptr);
  ~ConvertationWindow();

private:
  Ui::ConvertationWindow *ui;
};

#endif // CONVERTATIONWINDOW_H
