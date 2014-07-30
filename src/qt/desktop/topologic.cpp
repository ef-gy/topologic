#include "topologic.h"
#include "ui_topologic.h"

Topologic::Topologic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Topologic)
{
    ui->setupUi(this);
}

Topologic::~Topologic()
{
    delete ui;
}
