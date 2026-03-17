#ifndef COLOREDMESH_H
#define COLOREDMESH_H


#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <vector>
#include "ColoredVertex.h"

class ColoredMesh {
public:
    ColoredMesh(QOpenGLFunctions_3_3_Core *glFunc,
                const std::vector<ColoredVertex> &vertices,
                const std::vector<unsigned int> &indices);
    ~ColoredMesh();

    void Draw(QOpenGLShaderProgram &shader);

private:
    void setupMesh();

    QOpenGLFunctions_3_3_Core *m_glFunc;
    std::vector<ColoredVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    GLuint m_VAO, m_VBO, m_EBO;
};

#endif // COLOREDMESH_H
