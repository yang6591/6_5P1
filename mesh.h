#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>
#include <string>
#include <vector>
using namespace std;

struct Vertex {
    QVector3D Position;//顶点位置(x, y, z)
    QVector3D Normal;//顶点法线(nx, ny, nz)
    QVector2D TexCoords;//纹理坐标(u, v)
};
struct Texture {
    unsigned int id;// OpenGL 纹理 ID
    string type;// 纹理类型（如 "texture_diffuse", "texture_specular"）
    string path;// 纹理文件路径（用于去重）
};

class Mesh {
public:
    // mesh data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    void Draw(QOpenGLShaderProgram &shader);
    Mesh(QOpenGLFunctions_3_3_Core *glFuns,
         vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
private:
    // render data
    unsigned int VAO, VBO, EBO;
    void setupMesh();
private:
    QOpenGLFunctions_3_3_Core *m_glFuns;
};
