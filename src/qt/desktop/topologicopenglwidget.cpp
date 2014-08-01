#include "topologicopenglwidget.h"

#include <QDebug>
#include <QOpenGLContext>

TopologicOpenGLWidget::TopologicOpenGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
}

void TopologicOpenGLWidget::initializeGL()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    qDebug() << Q_FUNC_INFO << "context =" << context;

    enum topologic::outputMode out = topologic::outGL;

    int argc = 1;
    char *argv[] = { "topologic-qt", 0 };

    if (!topologic::parseArguments (state, argc, argv, out))
    {
        std::cerr << "badness: parseArguments call failed.\n";
        // badness
    }
}

void TopologicOpenGLWidget::paintGL()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    qDebug() << Q_FUNC_INFO << "context =" << context;

    if (state.model)
    {
        std::cerr << "have model\n";
        state.model->opengl(true);
    }
    else
    {
        std::cerr << "no model\n";
    }
}

void TopologicOpenGLWidget::resizeGL(int width, int height)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    qDebug() << Q_FUNC_INFO << "context =" << context;

    state.width  = width;
    state.height = height;
}
