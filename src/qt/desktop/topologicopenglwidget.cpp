#include "topologicopenglwidget.h"

#include <QDebug>
#include <QOpenGLContext>

TopologicOpenGLWidget::TopologicOpenGLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    /*
    QGLFormat fmt = format();
    fmt.setProfile(QGLFormat::CoreProfile);
    fmt.setVersion(3, 2);
    setFormat(fmt);
    makeCurrent();
    */
}

void TopologicOpenGLWidget::initializeGL()
{
    qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();

    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    qDebug() << "Driver Version String:" << versionString;
    qDebug() << "Current Context:" << format();

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
    if (state.model)
    {
        std::cerr << "have model\n";
        state.model->opengl(true);
    }
}

void TopologicOpenGLWidget::resizeGL(int width, int height)
{
    state.width  = width;
    state.height = height;
}
