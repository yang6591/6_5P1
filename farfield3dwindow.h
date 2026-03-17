#ifndef FARFIELD3DWINDOW_H
#define FARFIELD3DWINDOW_H

#include <QMainWindow>
#include <QVector3D>
#include <QCloseEvent>
#include <QKeyEvent>

// 前向声明
class openglwidget;

class farfield3dwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit farfield3dwindow(QWidget *parent = nullptr);
    ~farfield3dwindow();

    // 加载模型并显示
    bool loadModel(const QString &modelPath, const QVector3D &color = QVector3D(0.8f, 0.8f, 0.8f));

    // 自动加载可执行文件同目录下的0227.obj
    bool loadDefaultModel(const QVector3D &color = QVector3D(0.8f, 0.8f, 0.8f));

    // 获取OpenGLWidget
    openglwidget* getOpenGLWidget() const { return m_glWidget; }

    // 获取当前模型ID
    int getModelId() const { return m_modelId; }

    // 获取模型颜色
    QVector3D getModelColor() const { return m_modelColor; }

    bool testOpenGLWithCube();  // 测试OpenGL是否正常工作


signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event)override;

private:
    void setupWindow();

private:
    openglwidget* m_glWidget;
    int m_modelId;
    QVector3D m_modelColor;
    bool m_modelLoaded;
};

#endif // FARFIELD3DWINDOW_H
