#include "topologic.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Topologic w;
    w.show();

    return a.exec();
}
