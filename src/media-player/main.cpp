#include <QApplication>
#include "media-player-window.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  MediaPlayerWindow* media = new MediaPlayerWindow();
  media->show();

  return app.exec();
}

// MKV:  x264enc ! matroskamux
// WebM: vp8enc ! webmmux
// AVI:  x264enc ! avimux
