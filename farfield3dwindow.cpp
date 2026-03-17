#include "farfield3dwindow.h"
#include "openglwidget.h"
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QMessageBox>


farfield3dwindow::farfield3dwindow(QWidget *parent)
    : QMainWindow(parent)
    , m_glWidget(nullptr)
    , m_modelId(-1)
    , m_modelColor(0.8f, 0.8f, 0.8f)
{
    setupWindow();
    // 窗口创建后立即加载默认模型


    // 使用一个定时器，无论showEvent是否被调用都会执行
    QTimer::singleShot(0, this, [this]() {
        qDebug() << "farfield3dwindow constructor: Timer triggered";
        // 先显示窗口，再延迟加载模型
        this->show();
        QTimer::singleShot(100, this, [this]() {
            qDebug() << "farfield3dwindow constructor: Starting delayed model load";
            this->loadDefaultModel();
        });
    });
}

farfield3dwindow::~farfield3dwindow()
{
    qDebug() << "farfield3dwindow destructor";
}

void farfield3dwindow::setupWindow()
{
    // 设置窗口属性
    setWindowTitle("3D Model Viewer");
    setMinimumSize(800, 600);

    // 设置窗口背景色
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(35, 35, 35));
    setPalette(palette);

    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 创建OpenGLWidget
    m_glWidget = new openglwidget(centralWidget);
    m_glWidget->setMinimumSize(100, 100);

    // 设置焦点策略，确保可以接收键盘事件
    m_glWidget->setFocusPolicy(Qt::StrongFocus);

    // 确保OpenGLWidget可见
    m_glWidget->setVisible(true);

    // 添加到布局
    mainLayout->addWidget(m_glWidget);

    qDebug() << "farfield3dwindow setup complete";
}

bool farfield3dwindow::loadModel(const QString &modelPath, const QVector3D &color)
{
    if (!m_glWidget) {
        qWarning() << "OpenGLWidget is not initialized";
        return false;
    }

    if (modelPath.isEmpty()) {
        qWarning() << "Model path is empty";
        return false;
    }

    // 保存颜色
    m_modelColor = color;

    // 如果之前有加载模型，先删除
    if (m_modelId != -1) {
        m_glWidget->removeModelById(m_modelId);
        m_modelId = -1;
    }

    // 使用openglwidget的loadModel函数加载模型
    m_modelId = m_glWidget->loadModel(modelPath.toStdString(),
                                      openglwidget::MATERIAL,
                                      m_modelColor);

    if (m_modelId != -1) {
        // 更新窗口标题显示模型文件名
        QFileInfo fileInfo(modelPath);
        setWindowTitle(QString("3D Viewer - %1").arg(fileInfo.fileName()));

        qDebug() << "Model loaded successfully:" << modelPath << "ID:" << m_modelId;
        return true;
    } else {
        qWarning() << "Failed to load model:" << modelPath;
        return false;
    }
}

bool farfield3dwindow::loadDefaultModel(const QVector3D &color)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/0227.obj");

    // 获取可执行文件所在目录
    // 检查文件是否存在
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) {
        qWarning() << "Model file does not exist:" << fileName;
        qDebug() << "Application directory:" << appDir;
        qDebug() << "Files in directory:" << QDir(appDir).entryList();
        return false;
    }

    qDebug() << "Model file exists, size:" << fileInfo.size() << "bytes";

    return loadModel(fileName, color);
}



void farfield3dwindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "farfield3dwindow closing, model ID:" << m_modelId;

    // 发出窗口关闭信号
    emit windowClosed();

    // 清理模型资源
    if (m_glWidget && m_modelId != -1) {
        m_glWidget->removeModelById(m_modelId);
        m_modelId = -1;
    }

    event->accept();
}

void farfield3dwindow::keyPressEvent(QKeyEvent *event)
{
    // 处理ESC键关闭窗口
    if (event->key() == Qt::Key_Escape) {
        close();
    }

    // 注意：我们不需要直接调用m_glWidget->keyPressEvent(event)
    // 因为事件会自动传递给焦点所在的子部件
    // 我们只需要确保OpenGLWidget有焦点即可

    // 调用父类处理其他按键
    QMainWindow::keyPressEvent(event);
}


bool farfield3dwindow::testOpenGLWithCube()
{
    if (!m_glWidget) {
        qWarning() << "OpenGLWidget is not initialized";
        return false;
    }

    // 创建一个简单的立方体来测试OpenGL
    qDebug() << "Testing OpenGL by creating a simple cube...";

    m_glWidget->makeCurrent();

    // 创建立方体模型数据
    std::vector<float> vertices = {
        // 前面
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

        // 后面
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        // 左面
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

        // 右面
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

        // 上面
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

        // 下面
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f
    };

    std::vector<unsigned int> indices = {
        // 前面
        0, 1, 2, 2, 3, 0,
        // 后面
        4, 5, 6, 6, 7, 4,
        // 左面
        8, 9, 10, 10, 11, 8,
        // 右面
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };

    // 尝试通过openglwidget的createBox函数创建模型
    int cubeId = m_glWidget->createBox("TestCube",
                                       -0.5f, 0.5f,   // xmin, xmax
                                       -0.5f, 0.5f,   // ymin, ymax
                                       -0.5f, 0.5f,   // zmin, zmax
                                       openglwidget::MATERIAL);

    if (cubeId != -1) {
        qDebug() << "Test cube created successfully, ID:" << cubeId;
        m_modelId = cubeId;  // 保存模型ID
        m_modelLoaded = true;
        return true;
    } else {
        qWarning() << "Failed to create test cube";
        return false;
    }
}

void farfield3dwindow::showEvent(QShowEvent *event)
{

    QMainWindow::showEvent(event);

    qDebug() << "farfield3dwindow shown";

    // 确保OpenGLWidget已经初始化
    if (m_glWidget && !m_modelLoaded) {
        qDebug() << "Window shown, attempting to load model...";

        // 延迟一小段时间确保OpenGL完全初始化
        QTimer::singleShot(100, this, [this]() {
            // 使用默认颜色
            loadDefaultModel();
        });
    }
}
