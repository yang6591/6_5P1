#ifndef MESHTRANS_H
#define MESHTRANS_H
#include <QVector>
#include <QPair>
#include <QString>

// 读取网格文件函数
void readGridFile(const QString &filename, QVector<QPair<int, double>> &meshlines);

// 物理坐标转网格标号函数
int xyztomesh(const QVector<QPair<int, double>> &meshlines, double physicalpoi);
#endif // MESHTRANS_H
