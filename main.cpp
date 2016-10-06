
#include <QApplication>
#include "QtViz.h"

int main(int argc, char** argv)
{

  //  Q_INIT_RESOURCE(QtViz);
  QApplication app(argc, argv);
  QtViz viz(argc, argv);
  //viz.setWindowIcon(QPixmap(":/images/interview.png"));
  viz.resize(1024, 768);
  viz.show();
  return app.exec();
}


