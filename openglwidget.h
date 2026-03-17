#ifndef AXBOPENGLWIDGET_H
#define AXBOPENGLWIDGET_H

#include <QOpenGLWidget>
#include<QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>
#include<QOpenGLShaderProgram>
#include<QElapsedTimer>
#include<QTimer>
#include"Camera.h"
#include<QWheelEvent>
#include<QOpenGLTexture>
#include<Qpainter>
#include <QMap>
#include"model.h"
#include "drawmodel.h"


class openglwidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    enum MaterialType {
        MATERIAL  // 统一材料类型
    };
    explicit openglwidget(QWidget *parent = nullptr);
    ~openglwidget();

    //void loadModel(string path, MaterialType type = DIELECTRIC1);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QElapsedTimer m_time;
    QTimer m_timer;
    QOpenGLShaderProgram m_ShaderProgram;
    QOpenGLShaderProgram m_GridShaderProgram; // 新增：网格着色器程序
    QOpenGLShaderProgram m_LightShaderProgram;
    Camera m_camera;
    QOpenGLTexture * m_diffuseTex;
    QOpenGLTexture * m_specularTex;
    QVector3D cameraPosInit(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

    //位移
    QVector3D m_modelPosition; // 新增：模型位移量
    bool m_leftMousePressed;   // 新增：左键按下标志
    QPoint m_lastMousePos;     // 新增：记录上次鼠标位置

    //旋转
    bool m_rotating;
    QVector3D m_rotationCenter;
    QQuaternion m_rotation;
    QPoint m_lastRotatePos;

    //duomoxing
    struct ModelData {
        Model* model;
        QVector3D position;
        QQuaternion rotation;
        MaterialType type;
        QVector3D color;  // 存储材料颜色
    };

    void updateModelBounds();
    QVector3D m_minBound;
    QVector3D m_maxBound;
    void setMaterialColor(const ModelData& modelData);

    // 网格坐标系相关
    unsigned int m_gridVAO, m_gridVBO;
    void setupGrid();
    void drawGrid();
    int m_gridDivisions; // 新增：网格分割数
    float m_gridSize; // 网格总大小
    QVector3D m_defaultMinBound; // 默认边界最小值
    QVector3D m_defaultMaxBound; // 默认边界最大值
    QQuaternion m_gridRotation; // 网格的旋转状态
    QVector3D m_gridPosition; // 网格的位置偏移
    // 方向指示器相关
    unsigned int m_axisIndicatorVAO, m_axisIndicatorVBO;
    void setupAxisIndicator();
    void drawAxisIndicator();

public:
    // 添加模型ID管理
    int loadModel(string path, MaterialType type, QVector3D color); // 返回模型ID
    void removeModelById(int id);

    //void highlightModel(int modelId); // 设置高亮模型
    void resetHighlight();           // 重置高亮状态

    void highlightModels(const QList<int>& modelIds);

private:
    QMap<int, ModelData> m_modelsMap; // ID到模型的映射
    int m_nextModelId = 1; // 下一个模型ID
    //int m_highlightModelId = -1;  // 当前高亮显示的模型ID
    bool m_isolateMode = false;   // 是否处于选中模式
    float m_highlightAlpha = 1.0f; // 选中模型透明度
    float m_otherAlpha = 0.05f;     // 虚化模型透明度
    QList<int> m_highlightModelIds;  // 存储多个高亮模型ID

    QList<int> m_drawOrder; // 存储模型绘制顺序（最后绘制的显示在最上层）
    QList<int> m_selectionOrder; // 存储模型的选择顺序（最后选择的在列表末尾）
    QSet<int> m_selectedModels; // 存储当前选中的模型ID
    void rebuildDrawOrder();

public:
    int createBox(const QString &name,
                  float xmin, float xmax,
                  float ymin, float ymax,
                  float zmin, float zmax,
                  MaterialType materialType);
    QMap<int, QString> m_modelNames;  // 存储模型ID和名称的映射
signals:
public slots:
    void on_timeout();
};

#endif // AXBOPENGLWIDGET_H
