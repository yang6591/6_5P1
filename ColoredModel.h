#ifndef COLOREDMODEL_H
#define COLOREDMODEL_H


#include <QOpenGLFunctions_3_3_Core>
#include <vector>
#include <memory>
#include "ColoredMesh.h"

class ColoredModel {
public:
    ColoredModel(QOpenGLFunctions_3_3_Core *glFunc);
    ~ColoredModel();

    // 从顶点和索引数据加载（顶点格式：x,y,z, nx,ny,nz, r,g,b）
    bool loadFromData(const std::vector<float>& vertices,
                      const std::vector<unsigned int>& indices);

    void Draw(QOpenGLShaderProgram &shader);

    // 获取包围盒
    float minX() const { return m_minX; }
    float minY() const { return m_minY; }
    float minZ() const { return m_minZ; }
    float maxX() const { return m_maxX; }
    float maxY() const { return m_maxY; }
    float maxZ() const { return m_maxZ; }

private:
    QOpenGLFunctions_3_3_Core *m_glFunc;
    std::vector<std::unique_ptr<ColoredMesh>> m_meshes;
    float m_minX, m_minY, m_minZ, m_maxX, m_maxY, m_maxZ;
};

#endif // COLOREDMODEL_H
