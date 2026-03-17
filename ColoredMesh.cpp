#include "ColoredMesh.h"
#include <QDebug>

ColoredMesh::ColoredMesh(QOpenGLFunctions_3_3_Core *glFunc,
                         const std::vector<ColoredVertex> &vertices,
                         const std::vector<unsigned int> &indices)
    : m_glFunc(glFunc), m_vertices(vertices), m_indices(indices), m_VAO(0), m_VBO(0), m_EBO(0)
{
    setupMesh();
}

ColoredMesh::~ColoredMesh()
{
    if (m_VAO) m_glFunc->glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) m_glFunc->glDeleteBuffers(1, &m_VBO);
    if (m_EBO) m_glFunc->glDeleteBuffers(1, &m_EBO);
}

void ColoredMesh::setupMesh()
{
    m_glFunc->glGenVertexArrays(1, &m_VAO);
    m_glFunc->glGenBuffers(1, &m_VBO);
    m_glFunc->glGenBuffers(1, &m_EBO);

    m_glFunc->glBindVertexArray(m_VAO);
    m_glFunc->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    m_glFunc->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(ColoredVertex),
                           &m_vertices[0], GL_STATIC_DRAW);

    m_glFunc->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    m_glFunc->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
                           &m_indices[0], GL_STATIC_DRAW);

    // 位置属性
    m_glFunc->glEnableVertexAttribArray(0);
    m_glFunc->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex),
                                    (void*)offsetof(ColoredVertex, Position));

    // 法线属性
    m_glFunc->glEnableVertexAttribArray(1);
    m_glFunc->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex),
                                    (void*)offsetof(ColoredVertex, Normal));

    // 纹理坐标（可选，但为了对齐保持）
    m_glFunc->glEnableVertexAttribArray(2);
    m_glFunc->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex),
                                    (void*)offsetof(ColoredVertex, TexCoords));

    // 颜色属性
    m_glFunc->glEnableVertexAttribArray(3);
    m_glFunc->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex),
                                    (void*)offsetof(ColoredVertex, Color));

    m_glFunc->glBindVertexArray(0);
}

void ColoredMesh::Draw(QOpenGLShaderProgram &shader)
{
    shader.bind();
    m_glFunc->glBindVertexArray(m_VAO);
    m_glFunc->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    m_glFunc->glBindVertexArray(0);
}
