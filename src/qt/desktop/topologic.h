#ifndef TOPOLOGIC_H
#define TOPOLOGIC_H

#include <QMainWindow>

namespace Ui {
class Topologic;
}

class Topologic : public QMainWindow
{
    Q_OBJECT

public:
    explicit Topologic(QWidget *parent = 0);
    ~Topologic();

private:
    Ui::Topologic *ui;
};

#endif // TOPOLOGIC_H
