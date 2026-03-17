#ifndef DIRSHOW_H
#define DIRSHOW_H
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QRegularExpression>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QtCharts>
#include <QtCharts/QCategoryAxis>
#include <QDir>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>


class DirShow : public QMainWindow
{
    Q_OBJECT

public:
    DirShow(QWidget *parent = nullptr);
    ~DirShow();

    QVector<double> theta;
    QVector<double> phi;
    QVector<double> abs;
    QVector<double> abstheta;
    QVector<double> phasetheta;
    QVector<double> absphi;
    QVector<double> phasephi;
    QVector<double> axratio;
    QString linename;
    QString filename;

    void ReadData(const QString& filePath);
    void DrawPolarDirectionChart(const QVector<double>& a, const QVector<double>& b);


};

#endif // DIRSHOW_H
