#include "loncher.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    Loncher w;
    w.hide();

    return a.exec();
}
