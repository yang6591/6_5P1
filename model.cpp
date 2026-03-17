#include "model.h"

// 辅助函数：检查向量是否为0
bool isZeroVector(const QVector3D& vec) {
    const float eps = 1e-6f;
    return (vec.lengthSquared() < eps);
}
void Model::loadFromData(const std::vector<float> &vertices, const std::vector<unsigned int> &indices)
{
    // 清空现有数据
    meshes.clear();

    // 转换顶点数据
    std::vector<Vertex> verts;
    for (size_t i = 0; i < vertices.size(); i += 6) {
        Vertex v;
        v.Position = QVector3D(vertices[i], vertices[i+1], vertices[i+2]);
        v.Normal = QVector3D(vertices[i+3], vertices[i+4], vertices[i+5]);
        v.TexCoords = QVector2D(0.0f, 0.0f);
        verts.push_back(v);
    }

    // 创建网格
    std::vector<Texture> emptyTextures;
    meshes.push_back(Mesh(m_glFuns, verts, indices, emptyTextures));

    // 更新包围盒
    m_minX = m_minY = m_minZ = std::numeric_limits<float>::max();
    m_maxX = m_maxY = m_maxZ = std::numeric_limits<float>::min();

    for (const auto &v : verts) {
        m_minX = qMin(m_minX, v.Position.x());
        m_minY = qMin(m_minY, v.Position.y());
        m_minZ = qMin(m_minZ, v.Position.z());
        m_maxX = qMax(m_maxX, v.Position.x());
        m_maxY = qMax(m_maxY, v.Position.y());
        m_maxZ = qMax(m_maxZ, v.Position.z());
    }
}

void Model::loadModel(string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "ERROR::ASSIMP::" << import.GetErrorString();
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));//取了除名字外的目录路径
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        if(m_maxX<mesh->mVertices[i].x) m_maxX=mesh->mVertices[i].x;
        if(m_maxY<mesh->mVertices[i].y) m_maxY=mesh->mVertices[i].y;
        if(m_maxZ<mesh->mVertices[i].z) m_maxZ=mesh->mVertices[i].z;
        if(m_minX>mesh->mVertices[i].x) m_minX=mesh->mVertices[i].x;
        if(m_minY>mesh->mVertices[i].y) m_minY=mesh->mVertices[i].y;
        if(m_minZ>mesh->mVertices[i].z) m_minZ=mesh->mVertices[i].z;


        Vertex vertex;
        // 处理顶点位置、法线和纹理坐标
        QVector3D vector;
        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;

        vertex.Normal = QVector3D(0, 0, 0);//不接收文件法线，后续重新生成

        if(mesh->mTextureCoords[0])
        {
            QVector2D vec;
            vec.setX(mesh->mTextureCoords[0][i].x);
            vec.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec;
        } else
            vertex.TexCoords = QVector2D(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    //处理所有面，计算法线
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // 确保是三角形面
        if(face.mNumIndices == 3) {
            // 获取三个顶点索引
            unsigned int idx0 = face.mIndices[0];
            unsigned int idx1 = face.mIndices[1];
            unsigned int idx2 = face.mIndices[2];

            // 获取三个顶点位置
            QVector3D v0 = vertices[idx0].Position;
            QVector3D v1 = vertices[idx1].Position;
            QVector3D v2 = vertices[idx2].Position;

            // 计算两个边向量
            QVector3D edge1 = v1 - v0;
            QVector3D edge2 = v2 - v0;

            // 叉积计算面法线（右手法则）
            QVector3D faceNormal = QVector3D::crossProduct(edge1, edge2);
            faceNormal.normalize();

            // 将面法线直接赋给三个顶点（不进行平滑处理）
            vertices[idx0].Normal = faceNormal;
            vertices[idx1].Normal = faceNormal;
            vertices[idx2].Normal = faceNormal;

            // 添加索引
            indices.push_back(idx0);
            indices.push_back(idx1);
            indices.push_back(idx2);
        }
        // 如果不是三角形面，可以修改以上识别文件顶点数分组
    }

    // 处理材质
    if(mesh->mMaterialIndex >= 0) {//一个网格只包含了一个指向材质对象的索引，一个材质对象有多个纹理
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    }
    return Mesh(m_glFuns,vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        //检查是否已经加载过这个纹理，避免重复加载。如果已经加载过，就直接使用现有的，否则加载新的纹理并保存到已加载列表里。
        for(unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                //if(textures_loaded[j].path==str.C_Str()) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip){
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;

}

unsigned int Model::TextureFromFile(const char *path, const string &directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    QOpenGLTexture * texture=new QOpenGLTexture(QImage(filename.c_str()).mirrored());
    if(texture==NULL) qDebug()<<"texture is NULL";
    else qDebug()<<filename.c_str()<<"loaded";

    return texture->textureId();

}
