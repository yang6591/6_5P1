#include "meshtrans.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

void readGridFile(const QString &filename, QVector<QPair<int, double>> &meshlines)
{
    // 清空现有数据
    meshlines.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("无法打开文件：%s", qUtf8Printable(filename));
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('\t');

        if (parts.size() >= 2) {
            bool ok1, ok2;
            int index = parts[0].toInt(&ok1);
            double size = parts[1].toDouble(&ok2);

            if (ok1 && ok2) {
                meshlines.append(qMakePair(index, size));
            } else {
                qWarning("行数据格式错误：%s", qUtf8Printable(line));
            }
        }
    }

    file.close();

    qDebug() << "成功读取" << meshlines.size() << "个网格数据";
}




int xyztomesh(const QVector<QPair<int, double>> &meshlines, double physicalpoi)
{
    if (meshlines.isEmpty()) {
        return -1; // 空数据返回-1
    }

    // 计算网格系统的总长度
    double totalLength = 0.0;
    for (const auto &grid : meshlines) {
        totalLength += grid.second;
    }

    // 将原点移动到网格中心，即向负方向平移总长度的一半
    double centeredPhysicalpoi = physicalpoi + totalLength / 2.0;

    double accumulatedLength = 0.0;

    // 遍历所有网格，累加长度
    for (const auto &grid : meshlines) {
        double gridSize = grid.second;

        // 检查当前物理坐标是否在当前网格范围内
        if (centeredPhysicalpoi >= accumulatedLength && centeredPhysicalpoi < accumulatedLength + gridSize) {
            return grid.first; // 返回当前网格标号
        }

        accumulatedLength += gridSize;
    }

    // 如果坐标超出范围，计算超出部分并按每2mm一个网格计算
    if (centeredPhysicalpoi >= accumulatedLength) {
        double extraLength = centeredPhysicalpoi - accumulatedLength;
        int extraGrids = static_cast<int>(extraLength / 2.0); // 每2mm一个网格
        return meshlines.last().first + extraGrids + 1; // +1 因为从下一个网格开始
    }

    // 如果坐标小于0，计算超出部分并按每2mm一个网格计算
    if (centeredPhysicalpoi < 0) {
        double extraLength = -centeredPhysicalpoi; // 取绝对值
        int extraGrids = static_cast<int>(extraLength / 2.0); // 每2mm一个网格
        return meshlines.first().first - extraGrids - 1; // -1 因为从上一个网格开始
    }

    return -1; // 理论上不会执行到这里
}
