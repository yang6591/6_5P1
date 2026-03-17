#ifndef SPSHOW_H
#define SPSHOW_H
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QList>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QtCharts>
#include <algorithm>

class SpShow : public QMainWindow
{
   Q_OBJECT

public:
   //SpShow(QWidget *parent = nullptr);

   //多个构造函数
    SpShow(QWidget *parent, const QString& filenameread);
   // SpShow(QWidget *parent, const QVector<QString>& filenames);

   ~SpShow();

   QVector<QVector<double>> xDataList; // 存储第一列数据的动态数组容器
   QVector<QVector<double>> yDataList;
   QVector<QString> linename;
   QString stringX;
   QString line2;
   void ReadData(const QVector<QString>& filePath);
   QString ExtractAxisX(const QString& line);
   void CreateChart( const QVector<QVector<double>>& xDataList,
                    const QVector<QVector<double>>& yDataList,
                    const QVector<QString>&linename);

   QString GenerateChartTitle(const QString& nodeName);
   QString GenerateYAxisTitle(const QString& nodeName);
   QString currentNodeName;

};

#endif // SPSHOW_H
