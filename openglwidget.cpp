#include "openglwidget.h"

//
const unsigned int timeOutmSec=50;
unsigned int VBO,VAO,lightVAO;
QVector3D lightPos(1.2f,1.0f,2.0f);
QVector3D lightColor(1.0f,1.0f,1.0f);
//QVector3D objectColor(1.0f,0.5f,0.31f);
QVector3D objectColor(0.4f,0.4f,0.4f);

QVector3D viewInitPos(0.0,0.0,5.0);

openglwidget::openglwidget(QWidget *parent) : QOpenGLWidget{parent}
{
    //connect(&m_timer,&QTimer::timeout,this,&AXBOpenglWidget::on_timeout);
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
    m_timer.start(timeOutmSec);
    m_time.start();
    m_camera.Position=viewInitPos;
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_modelPosition = QVector3D(0.0f, 0.0f, 0.0f); // 初始位移为0
    m_leftMousePressed = false;
    m_rotating = false;
    // 初始化网格VAO/VBO为0
    m_gridVAO = 0;
    m_gridVBO = 0;
    m_gridDivisions = 24; // 初始化网格分割数（网格数量）
    m_gridSize = 240.0f; // 初始化网格大小(导入模型之前）
    m_defaultMinBound = QVector3D(-m_gridSize, -m_gridSize, -m_gridSize);
    m_defaultMaxBound = QVector3D(m_gridSize, m_gridSize, m_gridSize);
    m_gridRotation = QQuaternion(); // 初始化为无旋转
    m_gridPosition = QVector3D(0.0f, 0.0f, 0.0f); // 初始位置为原点
    // 初始化方向指示器VAO/VBO为0
    m_axisIndicatorVAO = 0;
    m_axisIndicatorVBO = 0;
}

openglwidget::~openglwidget()
{
    makeCurrent();
    // 释放网格资源
    if (m_gridVAO) glDeleteVertexArrays(1, &m_gridVAO);
    if (m_gridVBO) glDeleteBuffers(1, &m_gridVBO);
    // 释放方向指示器资源
    if (m_axisIndicatorVAO) glDeleteVertexArrays(1, &m_axisIndicatorVAO);
    if (m_axisIndicatorVBO) glDeleteBuffers(1, &m_axisIndicatorVBO);
    // 原有资源释放代码...
    doneCurrent();
}

int openglwidget::loadModel(string path, MaterialType type, QVector3D color)
{
    makeCurrent();
    ModelData newModel;
    newModel.type = type;

    // 根据导入顺序自动设置颜色
    static int modelCount = 0;
    if (modelCount == 0) {
        // 第一个模型使用灰绿色
        newModel.color = QVector3D(0.392f, 0.839f, 0.839f); // 灰绿色
        //QColor(100, 214, 214),
    } else if (modelCount == 1) {
        // 第二个模型使用黄色
        newModel.color = QVector3D(0.9f, 0.9f, 0.2f); // 黄色
    } else {
        // 其他模型使用传入的颜色
        newModel.color = color;
    }
    modelCount++;
    //newModel.color = color; // 存储材料颜色


    newModel.position = QVector3D(0.0f, 0.0f, 0.0f);
    newModel.rotation = QQuaternion();

    auto *glmodelfun = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>();

    if (glmodelfun) {
        newModel.model = new Model(glmodelfun, path.c_str());

        // 分配唯一ID
        int modelId = m_nextModelId++;
        m_modelsMap[modelId] = newModel;

        // 添加到绘制顺序列表的末尾
        m_drawOrder.append(modelId);

        // 更新整体包围盒
        updateModelBounds();

        // 导入模型后根据模型大小动态调整坐标系网格
        float m_maxBoundx_y = qMax(m_maxBound.x()-m_minBound.x(), m_maxBound.y()-m_minBound.y());
        m_gridSize = 3 * m_maxBoundx_y;
        setupGrid();
        drawGrid();

        // 计算相机初始位置
        m_camera.Position = cameraPosInit(m_minBound.x(), m_maxBound.x(),
                                          m_minBound.y(), m_maxBound.y(),
                                          m_minBound.z(), m_maxBound.z());

        // 计算并存储初始距离
        QVector3D modelCenter = (m_minBound + m_maxBound) / 2.0f;
        float initialDistance = (m_camera.Position - modelCenter).length();
        m_camera.SetInitialDistance(initialDistance);

        update();
        return modelId; // 返回模型ID
    } else {
        qDebug() << "Failed to get OpenGL functions";
        return -1; // 加载失败
    }
    doneCurrent();
}

void openglwidget::removeModelById(int id)
{
    makeCurrent();
    if (m_modelsMap.contains(id)) {
        // 从绘制顺序列表中移除
        m_drawOrder.removeAll(id);
        m_selectedModels.remove(id);
        m_highlightModelIds.removeAll(id);

        // 释放模型资源
        delete m_modelsMap[id].model;
        m_modelsMap.remove(id);

        // 更新整体包围盒
        updateModelBounds();

        // 导入模型后根据模型大小动态调整坐标系网格
        float m_maxBoundx_y = qMax(m_maxBound.x()-m_minBound.x(), m_maxBound.y()-m_minBound.y());
        m_gridSize = 3 * m_maxBoundx_y;

        if (m_modelsMap.isEmpty()) {
            m_gridSize = 240.0f; // 恢复初始网格大小
            m_gridPosition = QVector3D(0.0f, 0.0f, 0.0f); // 重置网格位置
            m_gridRotation = QQuaternion(); // 重置网格旋转
            m_minBound = m_defaultMinBound; // 恢复默认包围盒
            m_maxBound = m_defaultMaxBound; // 恢复默认包围盒
            m_rotationCenter = QVector3D(0.0f, 0.0f, 0.0f); // 旋转中心设为原点
        }

        setupGrid();
        drawGrid();

        // 重置相机位置
        if (m_modelsMap.isEmpty()) {
            m_camera.Position = QVector3D(0.0f, 0.0f, m_gridSize * 2.0f);
            m_camera.SetInitialDistance(m_gridSize * 2.0f);
        } else {
            m_camera.Position = cameraPosInit(m_minBound.x(), m_maxBound.x(),
                                              m_minBound.y(), m_maxBound.y(),
                                              m_minBound.z(), m_maxBound.z());
        }

        update();
    }
    doneCurrent();
}

// void openglwidget::highlightModel(int modelId)
// {
//     m_highlightModelId = modelId;
//     m_isolateMode = true;
//     update();
// }

void openglwidget::resetHighlight()
{
    m_highlightModelIds.clear();
    m_selectionOrder.clear();
    m_isolateMode = false;
    rebuildDrawOrder();
    update();
}

void openglwidget::highlightModels(const QList<int> &modelIds)
{
    // 更新选择顺序
    for (int id : modelIds) {
        if (!m_selectionOrder.contains(id)) {
            m_selectionOrder.append(id);
        }
    }

    // 从选择顺序中移除未选中的模型
    QList<int> toRemove;
    for (int id : m_selectionOrder) {
        if (!modelIds.contains(id)) {
            toRemove.append(id);
        }
    }
    for (int id : toRemove) {
        m_selectionOrder.removeAll(id);
    }

    // 更新高亮模型列表
    m_highlightModelIds = modelIds;
    m_isolateMode = !modelIds.isEmpty();

    // 重新构建绘制顺序
    rebuildDrawOrder();

    update();
}

void openglwidget::rebuildDrawOrder()
{
    m_drawOrder.clear();

    // 1. 先绘制未选中的模型
    for (auto it = m_modelsMap.begin(); it != m_modelsMap.end(); ++it) {
        if (!m_highlightModelIds.contains(it.key())) {
            m_drawOrder.append(it.key());
        }
    }

    // 2. 然后按选择顺序绘制选中的模型（最后选择的最后绘制）
    for (int id : m_selectionOrder) {
        if (m_highlightModelIds.contains(id)) {
            m_drawOrder.append(id);
        }
    }
}

void openglwidget::initializeGL()
{
    initializeOpenGLFunctions();
    while(glGetError() != GL_NO_ERROR);

    bool success;
    m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/shapes.vert");
    m_ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/shapes.frag");
    success = m_ShaderProgram.link();
    if(!success) {
        qDebug() << "Shader Program Link Error:" << m_ShaderProgram.log();
    }

    // 初始化OpenGL状态
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // 初始化网格着色器
    m_GridShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/grid.vert");
    m_GridShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/grid.frag");
    if (!m_GridShaderProgram.link()) {
        qDebug() << "Grid Shader Program Link Error:" << m_GridShaderProgram.log();
    }

    // 设置默认相机位置以显示整个网格
    m_camera.Position = QVector3D(0.0f, 0.0f, m_gridSize * 4.0f);
    m_camera.SetInitialDistance(m_gridSize * 4.0f);
    viewInitPos = m_camera.Position;

    // 设置网格
    setupGrid();
    // 初始化方向指示器
    setupAxisIndicator();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void openglwidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void openglwidget::paintGL()
{
    //glClearColor(0.93f,0.93f,0.93f,1.0f);
    //glClearColor(0.96f,0.94f,0.90f,1.0f);
    //glClearColor(0.92f,0.94f,0.96f,1.0f);
    glClearColor(0.7f,0.7f,0.7f,1.0f);

    glEnable(GL_DEPTH_TEST);  // 确保深度测试启用(是否显示内部构造-开启后，遮挡不显示）
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawGrid();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 绘制模型
    if (!m_modelsMap.isEmpty()) {
        // 设置视图和投影矩阵
        QMatrix4x4 view = m_camera.GetViewMatrix();
        QMatrix4x4 projection;
        float farPlane = qMax(1000.0f, 10.0f * (m_camera.Position - m_rotationCenter).length());
        projection.perspective(m_camera.Zoom, (float)width()/height(), 0.1f, farPlane);

        if (!m_ShaderProgram.bind()) {
            qDebug() << "Failed to bind shader program";
            return;
        }

        m_ShaderProgram.setUniformValue("projection", projection);
        m_ShaderProgram.setUniformValue("view", view);
        m_ShaderProgram.setUniformValue("viewPos", m_camera.Position);

        // 设置光照参数
        m_ShaderProgram.setUniformValue("light.ambient", 0.5f, 0.5f, 0.5f);
        m_ShaderProgram.setUniformValue("light.diffuse", 0.9f, 0.9f, 0.9f);
        m_ShaderProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
        m_ShaderProgram.setUniformValue("material.shininess", 32.0f);
        //m_ShaderProgram.setUniformValue("light.direction", 1.0f, -1.0f, -1.0f);
        m_ShaderProgram.setUniformValue("light.direction", 0.5f, -0.7f, -1.0f);//模型右前上方

        if (m_isolateMode) {
            // 特殊绘制模式（多选）
            GLboolean depthTestEnabled;
            glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);

            // 第一阶段：关闭深度测试，绘制未被选中的模型（半透明）
            glDisable(GL_DEPTH_TEST);
            for (int modelId : m_drawOrder) {
                if (!m_highlightModelIds.contains(modelId)) {
                    ModelData &modelData = m_modelsMap[modelId];
                    m_ShaderProgram.setUniformValue("alpha", m_otherAlpha);
                    QMatrix4x4 model;
                    model.translate(modelData.position);
                    model.translate(m_rotationCenter);
                    model.rotate(modelData.rotation);
                    model.translate(-m_rotationCenter);
                    m_ShaderProgram.setUniformValue("model", model);
                    setMaterialColor(modelData);
                    modelData.model->Draw(m_ShaderProgram);
                }
            }

            // 第二阶段：开启深度测试，绘制被选中的模型（不透明）
            glEnable(GL_DEPTH_TEST);
            for (int modelId : m_drawOrder) {
                if (m_highlightModelIds.contains(modelId)) {
                    ModelData &modelData = m_modelsMap[modelId];
                    m_ShaderProgram.setUniformValue("alpha", m_highlightAlpha);
                    QMatrix4x4 model;
                    model.translate(modelData.position);
                    model.translate(m_rotationCenter);
                    model.rotate(modelData.rotation);
                    model.translate(-m_rotationCenter);
                    m_ShaderProgram.setUniformValue("model", model);
                    setMaterialColor(modelData);
                    modelData.model->Draw(m_ShaderProgram);
                }
            }

            if (depthTestEnabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
        } else {
            // 默认绘制模式 - 按导入顺序绘制
            for (auto it = m_modelsMap.begin(); it != m_modelsMap.end(); ++it) {
                ModelData &modelData = it.value();
                m_ShaderProgram.setUniformValue("alpha", 1.0f);
                QMatrix4x4 model;
                model.translate(modelData.position);
                model.translate(m_rotationCenter);
                model.rotate(modelData.rotation);
                model.translate(-m_rotationCenter);
                m_ShaderProgram.setUniformValue("model", model);
                setMaterialColor(modelData);
                modelData.model->Draw(m_ShaderProgram);
            }
        }
    }
    // 最后绘制方向指示器（在屏幕最上层）
    drawAxisIndicator();
}

void openglwidget::wheelEvent(QWheelEvent *event)
{
    // //1视角宽窄
    // m_camera.ProcessMouseScroll(event->angleDelta().y()/120);
    // update();

    //2与模型距离
    if (m_modelsMap.isEmpty()) {
        // 没有模型时直接缩放视图
        m_camera.ProcessMouseScroll(event->angleDelta().y()/120);//?????????????????
    } else {
        // 有模型时处理模型距离
        QVector3D modelCenter = (m_minBound + m_maxBound) / 2.0f;
        m_camera.ProcessDistanceChange(event->angleDelta().y()/120.0f, modelCenter);
    }
    update();
}

void openglwidget::keyPressEvent(QKeyEvent *event)
{
    float deltaTime=timeOutmSec/50.0;
    QPoint center = QPoint(width() / 2, height() / 2);
    switch (event->key())
    {
    case Qt::Key_W: m_camera.ProcessKeyboard(FORWARD,deltaTime);break;
    case Qt::Key_S: m_camera.ProcessKeyboard(BACKWARD,deltaTime);break;
    case Qt::Key_D: m_camera.ProcessKeyboard(RIGHT,deltaTime);break;
    case Qt::Key_A: m_camera.ProcessKeyboard(LEFT,deltaTime);break;
    case Qt::Key_Q: m_camera.ProcessKeyboard(DOWN,deltaTime);break;
    case Qt::Key_E: m_camera.ProcessKeyboard(UP,deltaTime);break;
    case Qt::Key_Space:
        // 重置所有模型的位置和旋转
        for (auto &modelData : m_modelsMap) {
            modelData.position = QVector3D(0.0f, 0.0f, 0.0f);
            modelData.rotation = QQuaternion();
        }
        // 重置相机位置
        if (m_modelsMap.isEmpty()) {
            m_camera.Position = QVector3D(0.0f, 0.0f, m_gridSize * 2.0f);
            // 重置网格位置和旋转
            m_gridPosition = QVector3D(0.0f, 0.0f, 0.0f);
            m_gridRotation = QQuaternion();
        } else {
            m_camera.Position = viewInitPos;
        }
        m_camera.Zoom=45;

        //视角复位，但在只对模型操作时，视角实际上一直没动
        m_camera.Front = QVector3D(0.0,0.0,-1.0);
        m_camera.Right = QVector3D::crossProduct(m_camera.Front, m_camera.WorldUp);
        m_camera.Right.normalize();
        m_camera.Up = QVector3D::crossProduct(m_camera.Right, m_camera.Front);
        m_camera.Up.normalize();

        //鼠标复位
        QCursor::setPos(mapToGlobal(center));break;


    default:break;
    }
    update();
}
//************************************************************
void openglwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
        (event->modifiers() & Qt::ControlModifier)) {
        m_rotating = true;
        m_lastRotatePos = event->pos();
        m_rotationCenter = (m_minBound + m_maxBound) / 2.0f;
    }
    else if (event->button() == Qt::LeftButton) {
        m_leftMousePressed = true;
        m_lastMousePos = event->pos();
    }
    QOpenGLWidget::mousePressEvent(event);
}
void openglwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_rotating = false;
        m_leftMousePressed = false;
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}
void openglwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_rotating) {
        QPoint delta = event->pos() - m_lastRotatePos;
        m_lastRotatePos = event->pos();

        float angleY = delta.x() * 0.5f;
        float angleX = delta.y() * 0.5f;

        QQuaternion yRot = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), angleY);
        QQuaternion xRot = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), angleX);

        if (m_modelsMap.isEmpty()) {
            // 旋转网格
            m_gridRotation = yRot * xRot * m_gridRotation;
        } else {
            // 旋转模型
            for (auto &modelData : m_modelsMap) {
                modelData.rotation = yRot * xRot * modelData.rotation;
            }
        }

        update(); // 确保重绘
    }
    else if (m_leftMousePressed) {
        QPoint currentPos = event->pos();
        QPoint delta = currentPos - m_lastMousePos;
        m_lastMousePos = currentPos;

        if (m_modelsMap.isEmpty()) {
            // 没有模型时，拖动整个网格坐标系
            // 计算相机到网格中心的距离
            QVector3D gridCenter(0.0f, 0.0f, 0.0f); // 网格坐标系原点为中心
            float distance = (m_camera.Position - gridCenter).length();

            // 动态灵敏度计算（与模型拖动使用相同逻辑）
            float baseSensitivity = 0.015f;
            float distanceFactor = distance * 0.1f;
            float dynamicSensitivity = baseSensitivity * distanceFactor;

            // 更新网格位置
            m_gridPosition += QVector3D(delta.x() * dynamicSensitivity,
                                        -delta.y() * dynamicSensitivity,
                                        0.0f);
        } else {
            // 有模型时，拖动模型（保持原有逻辑）
            QVector3D modelCenter = (m_minBound + m_maxBound) / 2.0f;
            float distance = (m_camera.Position - modelCenter).length();

            float baseSensitivity = 0.015f;
            float distanceFactor = distance * 0.1f;
            float dynamicSensitivity = baseSensitivity * distanceFactor;

            for (auto &modelData : m_modelsMap) {
                modelData.position += QVector3D(delta.x() * dynamicSensitivity,
                                                -delta.y() * dynamicSensitivity,
                                                0.0f);
            }
        }
        update();
    }
    else if (event->buttons() & Qt::RightButton) {
        static QPoint lastPos(width()/2, height()/2);
        auto currentPos = event->pos();
        QPoint deltaPos = currentPos - lastPos;
        lastPos = currentPos;
        m_camera.ProcessMouseMovement(deltaPos.x(), -deltaPos.y());
    }
}
//**************************************

// 传入模型完整的3D包围盒参数（X/Y/Z轴）
QVector3D openglwidget::cameraPosInit(float minX, float maxX,float minY, float maxY,float minZ, float maxZ) {
    QVector3D cameraPosition;

    // 1. 计算模型中心点（三维中心）
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;

    // 2. 计算模型各轴跨度
    float width = maxX - minX;    // X轴跨度
    float height = maxY - minY;   // Y轴跨度
    float depth = maxZ - minZ;    // Z轴跨度

    // 3. 动态调整相机距离（基于最大维度）
    float maxDimension = qMax(qMax(width, height), depth);
    float distance = 1.6f * maxDimension;  // 缩放系数，可根据需求调整

    // 4. 设置最小观察距离（防止模型过小时相机太近）
    const float MIN_DISTANCE = 2.0f;        // 最小距离
    distance = qMax(distance, MIN_DISTANCE);

    // 5. 设置相机位置
    cameraPosition.setX(centerX);   // 对准模型X轴中心
    cameraPosition.setY(centerY);   // 对准模型Y轴中心
    cameraPosition.setZ(centerZ + distance);  // 沿Z轴正方向后退，看向模型中心

    // 6. 保存并返回相机初始位置
    viewInitPos = cameraPosition;
    return cameraPosition;
}

void openglwidget::updateModelBounds()
{
    if (m_modelsMap.isEmpty()) {
        m_minBound = QVector3D(0, 0, 0);
        m_maxBound = QVector3D(0, 0, 0);
        return;
    }

    // 初始化边界值为第一个模型的边界
    // 初始化边界
    auto firstModel = m_modelsMap.first();
    m_minBound = QVector3D(firstModel.model->m_minX,
                           firstModel.model->m_minY,
                           firstModel.model->m_minZ);
    m_maxBound = QVector3D(firstModel.model->m_maxX,
                           firstModel.model->m_maxY,
                           firstModel.model->m_maxZ);

    // 遍历所有模型，更新边界
    for (const auto &modelData : m_modelsMap) {
        Model *model = modelData.model;
        m_minBound.setX(qMin(m_minBound.x(), model->m_minX));
        m_minBound.setY(qMin(m_minBound.y(), model->m_minY));
        m_minBound.setZ(qMin(m_minBound.z(), model->m_minZ));
        m_maxBound.setX(qMax(m_maxBound.x(), model->m_maxX));
        m_maxBound.setY(qMax(m_maxBound.y(), model->m_maxY));
        m_maxBound.setZ(qMax(m_maxBound.z(), model->m_maxZ));
    }

    // 更新旋转中心
    m_rotationCenter = (m_minBound + m_maxBound) / 2.0f;
}

void openglwidget::setMaterialColor(const ModelData& modelData)
{
    // 所有材料都使用存储的颜色
    m_ShaderProgram.setUniformValue("objectColor", modelData.color);
}

void openglwidget::setupGrid()
{
    // 删除旧的VAO/VBO
    if (m_gridVAO) {
        glDeleteVertexArrays(1, &m_gridVAO);
        m_gridVAO = 0;
    }
    if (m_gridVBO) {
        glDeleteBuffers(1, &m_gridVBO);
        m_gridVBO = 0;
    }

    const float step = m_gridSize / m_gridDivisions;
    std::vector<float> gridVertices;

    // 添加网格线
    for (int i = -m_gridDivisions; i <= m_gridDivisions; ++i) {
        float pos = i * step;

        // 主轴线
        //if (i == 0) {
            // X轴主线 (红色 - 水平线)
            gridVertices.push_back(-m_gridSize); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);
            gridVertices.push_back(1.0f); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);
            gridVertices.push_back(m_gridSize); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);
            gridVertices.push_back(1.0f); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);

            // Y轴主线 (绿色 - 垂直线)
            gridVertices.push_back(0.0f); gridVertices.push_back(-m_gridSize); gridVertices.push_back(0.0f);
            gridVertices.push_back(0.0f); gridVertices.push_back(1.0f); gridVertices.push_back(0.0f);
            gridVertices.push_back(0.0f); gridVertices.push_back(m_gridSize); gridVertices.push_back(0.0f);
            gridVertices.push_back(0.0f); gridVertices.push_back(1.0f); gridVertices.push_back(0.0f);

            // Z轴主线 (蓝色)
            gridVertices.push_back(0.0f); gridVertices.push_back(0.0f); gridVertices.push_back(-m_gridSize);
            gridVertices.push_back(0.0f); gridVertices.push_back(0.0f); gridVertices.push_back(1.0f);
            gridVertices.push_back(0.0f); gridVertices.push_back(0.0f); gridVertices.push_back(m_gridSize);
            gridVertices.push_back(0.0f); gridVertices.push_back(0.0f); gridVertices.push_back(1.0f);

        //} else {
            // X方向网格线 (水平线 - 暗红色)
            // gridVertices.push_back(-m_gridSize); gridVertices.push_back(pos); gridVertices.push_back(0.0f);
            // gridVertices.push_back(0.5f); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);
            // gridVertices.push_back(m_gridSize); gridVertices.push_back(pos); gridVertices.push_back(0.0f);
            // gridVertices.push_back(0.5f); gridVertices.push_back(0.0f); gridVertices.push_back(0.0f);

            // // Y方向网格线 (垂直线 - 暗绿色)
            // gridVertices.push_back(pos); gridVertices.push_back(-m_gridSize); gridVertices.push_back(0.0f);
            // gridVertices.push_back(0.0f); gridVertices.push_back(0.5f); gridVertices.push_back(0.0f);
            // gridVertices.push_back(pos); gridVertices.push_back(m_gridSize); gridVertices.push_back(0.0f);
            // gridVertices.push_back(0.0f); gridVertices.push_back(0.5f); gridVertices.push_back(0.0f);
        //}
    }

    // 创建并绑定网格VAO/VBO
    if (m_gridVAO == 0) glGenVertexArrays(1, &m_gridVAO);
    if (m_gridVBO == 0) glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), &gridVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void openglwidget::drawGrid()
{
    if (!m_GridShaderProgram.bind()) {
        qDebug() << "Failed to bind grid shader program";
        return;
    }

    QMatrix4x4 view = m_camera.GetViewMatrix();
    QMatrix4x4 projection;
    float farPlane = qMax(10000.0f, 20.0f * m_gridSize);
    projection.perspective(m_camera.Zoom, (float)width()/height(), 0.1f, farPlane);

    QMatrix4x4 model;
    if (m_modelsMap.isEmpty()) {
        // 没有模型时，应用网格的位置和旋转
        model.translate(m_gridPosition);
        model.rotate(m_gridRotation);
    } else {
        // 有模型时，使用第一个模型的变换（通过 key 排序获取第一个）
        int firstModelId = m_modelsMap.firstKey();
        const ModelData& firstModel = m_modelsMap[firstModelId];

        model.translate(firstModel.position);
        model.translate(m_rotationCenter);
        model.rotate(firstModel.rotation);
        model.translate(-m_rotationCenter);
    }

    m_GridShaderProgram.setUniformValue("projection", projection);
    m_GridShaderProgram.setUniformValue("view", view);
    m_GridShaderProgram.setUniformValue("model", model);

    // glBindVertexArray(m_gridVAO);
    // glLineWidth(1.0f);
    // glDrawArrays(GL_LINES, 4, (m_gridDivisions*4 + 2)*2*2);
    // glLineWidth(3.0f);
    // glDrawArrays(GL_LINES, 0, 4);
    // glBindVertexArray(0);

    glBindVertexArray(m_gridVAO);
    glLineWidth(1.0f); // 设置坐标轴线宽
    //glDrawArrays(GL_LINES, 0, 6); // 现在只有4个顶点（2条线）
    glBindVertexArray(0);
}

void openglwidget::setupAxisIndicator()
{
    // 箭头数据 + 文字标签位置
    std::vector<float> axisVertices = {
        // X轴 (红色)
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 起点
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 终点
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 箭头部分1起点
        0.4f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f,  // 箭头部分1终点
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // 箭头部分2起点
        0.4f, -0.1f, 0.0f, 1.0f, 0.0f, 0.0f, // 箭头部分2终点

        // Y轴 (绿色)
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.1f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.1f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f,

        // Z轴 (蓝色)
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.1f, 0.4f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, -0.1f, 0.4f, 0.0f, 0.0f, 1.0f,

        // 原点球体标记
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        // X标签位置 (稍微超出箭头末端)
        0.55f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Y标签位置
        0.0f, 0.55f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Z标签位置
        0.0f, 0.0f, 0.55f, 0.0f, 0.0f, 1.0f
    };

    if (m_axisIndicatorVAO == 0) glGenVertexArrays(1, &m_axisIndicatorVAO);
    if (m_axisIndicatorVBO == 0) glGenBuffers(1, &m_axisIndicatorVBO);

    glBindVertexArray(m_axisIndicatorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_axisIndicatorVBO);
    glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(float), axisVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void openglwidget::drawAxisIndicator()
{
    if (!m_GridShaderProgram.bind()) {
        qDebug() << "Failed to bind grid shader program for axis indicator";
        return;
    }

    // 保存当前OpenGL状态
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glDisable(GL_DEPTH_TEST);

    // 设置正交投影矩阵（固定在屏幕空间）
    QMatrix4x4 projection;
    projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // 模型矩阵
    QMatrix4x4 model;

    // 1. 先平移到屏幕右下角
    model.translate(0.8f, -0.8f, 0.0f);

    // 2. 然后缩放
    model.scale(0.2f);

    // 3. 最后在本地坐标系中旋转
    if (m_modelsMap.isEmpty()) {
        model.rotate(m_gridRotation);
    } else {
        // 获取第一个模型的旋转（按插入顺序）
        const ModelData& firstModel = m_modelsMap.first();
        model.rotate(firstModel.rotation);
    }

    m_GridShaderProgram.setUniformValue("projection", projection);
    m_GridShaderProgram.setUniformValue("view", QMatrix4x4());
    m_GridShaderProgram.setUniformValue("model", model);

    // 绘制指示器
    glBindVertexArray(m_axisIndicatorVAO);
    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, 18);  // 绘制箭头线
    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 18, 1); // 绘制原点

    glBindVertexArray(0);

    // 使用QPainter绘制文字标签
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont font = painter.font();
    font.setPointSize(12);
    painter.setFont(font);

    // 获取标签顶点位置（顶点19:X, 20:Y, 21:Z）
    float labelVertices[3*6]; // 3个标签，每个6个float
    glBindBuffer(GL_ARRAY_BUFFER, m_axisIndicatorVBO);
    glGetBufferSubData(GL_ARRAY_BUFFER, 19*6*sizeof(float), 3*6*sizeof(float), labelVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 将标签位置从模型空间转换到屏幕空间
    QMatrix4x4 mvp = projection * model;
    const char* labels[] = {"X", "Y", "Z"};
    const QColor colors[] = {Qt::red, Qt::green, Qt::blue};

    for (int i = 0; i < 3; ++i) {
        // 获取标签位置和颜色
        QVector4D pos(labelVertices[i*6], labelVertices[i*6+1], labelVertices[i*6+2], 1.0f);
        QVector3D color(labelVertices[i*6+3], labelVertices[i*6+4], labelVertices[i*6+5]);

        // 转换到裁剪空间
        QVector4D clipPos = mvp * pos;

        // 转换到标准化设备坐标
        QVector3D ndcPos(clipPos.x()/clipPos.w(), clipPos.y()/clipPos.w(), clipPos.z()/clipPos.w());

        // 转换到屏幕坐标
        int screenX = (ndcPos.x() + 1.0f) * 0.5f * width();
        int screenY = (1.0f - (ndcPos.y() + 1.0f) * 0.5f) * height();

        // 绘制标签
        painter.setPen(colors[i]);
        painter.drawText(screenX, screenY, labels[i]);
    }

    painter.end();


    // 恢复OpenGL状态
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

int openglwidget::createBox(const QString &name, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, MaterialType materialType)
{
    makeCurrent();

    // 生成长方体顶点数据
    std::vector<float> vertices = {
        // 前面
        xmin, ymin, zmax, 0.0f, 0.0f, 1.0f,
        xmax, ymin, zmax, 0.0f, 0.0f, 1.0f,
        xmax, ymax, zmax, 0.0f, 0.0f, 1.0f,
        xmin, ymax, zmax, 0.0f, 0.0f, 1.0f,

        // 后面
        xmin, ymin, zmin, 0.0f, 0.0f, -1.0f,
        xmin, ymax, zmin, 0.0f, 0.0f, -1.0f,
        xmax, ymax, zmin, 0.0f, 0.0f, -1.0f,
        xmax, ymin, zmin, 0.0f, 0.0f, -1.0f,

        // 左面
        xmin, ymin, zmin, -1.0f, 0.0f, 0.0f,
        xmin, ymin, zmax, -1.0f, 0.0f, 0.0f,
        xmin, ymax, zmax, -1.0f, 0.0f, 0.0f,
        xmin, ymax, zmin, -1.0f, 0.0f, 0.0f,

        // 右面
        xmax, ymin, zmin, 1.0f, 0.0f, 0.0f,
        xmax, ymax, zmin, 1.0f, 0.0f, 0.0f,
        xmax, ymax, zmax, 1.0f, 0.0f, 0.0f,
        xmax, ymin, zmax, 1.0f, 0.0f, 0.0f,

        // 上面
        xmin, ymax, zmin, 0.0f, 1.0f, 0.0f,
        xmin, ymax, zmax, 0.0f, 1.0f, 0.0f,
        xmax, ymax, zmax, 0.0f, 1.0f, 0.0f,
        xmax, ymax, zmin, 0.0f, 1.0f, 0.0f,

        // 下面
        xmin, ymin, zmin, 0.0f, -1.0f, 0.0f,
        xmax, ymin, zmin, 0.0f, -1.0f, 0.0f,
        xmax, ymin, zmax, 0.0f, -1.0f, 0.0f,
        xmin, ymin, zmax, 0.0f, -1.0f, 0.0f
    };

    // 索引数据
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

    // 创建模型数据
    ModelData newModel;
    newModel.type = materialType;
    newModel.position = QVector3D(0.0f, 0.0f, 0.0f);
    newModel.rotation = QQuaternion();

    // 创建模型对象
    auto *glmodelfun = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>();
    if (glmodelfun) {
        newModel.model = new Model(glmodelfun);
        newModel.model->loadFromData(vertices, indices);

        // 分配唯一ID
        int modelId = m_nextModelId++;
        m_modelsMap[modelId] = newModel;
        m_drawOrder.append(modelId);

        // 更新模型名称映射
        m_modelNames[modelId] = name;

        // 更新整体包围盒
        updateModelBounds();

        // 更新网格大小
        float m_maxBoundx_y = qMax(m_maxBound.x()-m_minBound.x(),
                                   m_maxBound.y()-m_minBound.y());
        m_gridSize = 3 * m_maxBoundx_y;
        setupGrid();

        update();
        return modelId;
    }

    return -1; // 创建失败

}

void openglwidget::on_timeout()
{
    update();
}
