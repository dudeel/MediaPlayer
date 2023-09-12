#include "convertation-window.h"
#include "ui_convertation-window.h"

ConvertationWindow::ConvertationWindow(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConvertationWindow)
{
  ui->setupUi(this);
}

ConvertationWindow::~ConvertationWindow()
{
  delete ui;
}
