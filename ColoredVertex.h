#ifndef COLOREDVERTEX_H
#define COLOREDVERTEX_H

#include <QVector3D>
#include <QVector2D>

struct ColoredVertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
    QVector3D Color;  // 顶点颜色

    ColoredVertex()
        : Position(0,0,0), Normal(0,0,0), TexCoords(0,0), Color(1,1,1)
    {}
};


#endif // COLOREDVERTEX_H
