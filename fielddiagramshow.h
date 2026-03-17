#ifndef FIELDDIAGRAMSHOW_H
#define FIELDDIAGRAMSHOW_H
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QRegularExpression>
#include <QPixmap>
#include <QLabel>
#include <QPainter>
#include <QLinearGradient>
#include <QImage>
#include <QPainter>
#include <QColor>
#include <QGradient>
#include <QSize>
#include <QGradientStop>
#include <QBrush>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<QWheelEvent>


class FieldDiagramShow : public QMainWindow
{
    Q_OBJECT

public:
    FieldDiagramShow(QWidget *parent = nullptr);
    ~FieldDiagramShow();

    QVector<double>Axis;
    QVector<double>delta;
    QVector<double>X;
    QVector<double>Y;
    QVector<double>dX;
    QVector<double>dY;
    QVector<double>Abs;
    double minValue;
    double maxValue;
    double xmax;
    double xmin;
    double ymax;
    double ymin;
    double absmax;
    double absmin;
    double stx;
    int Count;
    int countx;
    int county;
    int pixelsPerMeter;
    void ReadAxis(const QString& filePath);
    void ReadData(const QString& filePath);
    void DrawChart(const QVector<double>& dx,const QVector<double>& dy, const QVector<double>& values);

    QColor CalculateColorjet(double value);
    QColor CalculateColorparula(double value);



};

#endif // FIELDDIAGRAMSHOW_H
