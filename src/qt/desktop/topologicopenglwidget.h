#ifndef TOPOLOGICOPENGLWIDGET_H
#define TOPOLOGICOPENGLWIDGET_H

#include <QGLWidget>
#include <topologic/parse.h>

class TopologicOpenGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit TopologicOpenGLWidget(QWidget *parent = 0);

signals:

public slots:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
};

#endif // TOPOLOGICOPENGLWIDGET_H
