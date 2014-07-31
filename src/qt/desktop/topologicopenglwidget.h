#ifndef TOPOLOGICOPENGLWIDGET_H
#define TOPOLOGICOPENGLWIDGET_H

#include <QGLWidget>

#define NOLIBRARIES
#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

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

    topologic::state<GLfloat,MAXDEPTH> state;
};

#endif // TOPOLOGICOPENGLWIDGET_H
