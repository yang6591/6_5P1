#include "ColoredModel.h"
#include <QDebug>
#include <limits>

ColoredModel::ColoredModel(QOpenGLFunctions_3_3_Core *glFunc)
    : m_glFunc(glFunc)
{
    m_minX = m_minY = m_minZ = std::numeric_limits<float>::max();
    m_maxX = m_maxY = m_maxZ = std::numeric_limits<float>::min();
}

ColoredModel::~ColoredModel() = default;

bool ColoredModel::loadFromData(const std::vector<float>& vertices,
                                const std::vector<unsigned int>& indices)
{
    m_meshes.clear();

    // 转换为 ColoredVertex 数组
    std::vector<ColoredVertex> verts;
    for (size_t i = 0; i < vertices.size(); i += 9) {
        ColoredVertex v;
        v.Position = QVector3D(vertices[i], vertices[i+1], vertices[i+2]);
        v.Normal   = QVector3D(vertices[i+3], vertices[i+4], vertices[i+5]);
        v.Color    = QVector3D(vertices[i+6], vertices[i+7], vertices[i+8]);
        v.TexCoords = QVector2D(0.0f, 0.0f);
        verts.push_back(v);

        // 更新包围盒
        float x = v.Position.x(), y = v.Position.y(), z = v.Position.z();
        if (x < m_minX) m_minX = x;
        if (y < m_minY) m_minY = y;
        if (z < m_minZ) m_minZ = z;
        if (x > m_maxX) m_maxX = x;
        if (y > m_maxY) m_maxY = y;
        if (z > m_maxZ) m_maxZ = z;
    }

    auto mesh = std::make_unique<ColoredMesh>(m_glFunc, verts, indices);
    m_meshes.push_back(std::move(mesh));

    qDebug() << "ColoredModel loaded with" << verts.size() << "vertices,"
             << indices.size() << "indices, bounding box:"
             << m_minX << m_minY << m_minZ << "->" << m_maxX << m_maxY << m_maxZ;
    return true;
}

void ColoredModel::Draw(QOpenGLShaderProgram &shader)
{
    for (auto &mesh : m_meshes) {
        mesh->Draw(shader);
    }
}
