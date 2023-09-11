#include <QApplication>

#include "media-player-window.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  MediaPlayerWindow* media = new MediaPlayerWindow();
  media->show();

  return app.exec();
}
