#include "dirshow.h"

DirShow::DirShow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle("Farfields");
    setMinimumSize(800, 600);

    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 构建文件路径
    QString filePath = QDir::cleanPath(appDir + "/phi90.txt");


    //读取数据
    ReadData(filePath);
    //绘制图像
    DrawPolarDirectionChart(theta,abs);

}

DirShow::~DirShow()
{
}

void DirShow::ReadData(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    //读取文件名作为图题
    QFileInfo fileInfo(filePath);
    filename = fileInfo.baseName();
    qDebug() << "文件名称:" << filename;

    //读取文件的第二行作为曲线名称
    file.readLine();
    linename=file.readLine();
    qDebug() << "Line 2:" << linename;
    file.readLine();
    file.readLine();
    while (!file.atEnd()) {
        QString line = file.readLine().trimmed();
        QStringList values = line.split(QRegularExpression("\\s+"));
        if (values.size() >= 8) {
            bool ok;
            double thetaValue = values[0].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert theta value:" << values[0];
                continue;
            }
            double phiValue = values[1].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert phi value:" << values[1];
                continue;
            }
            double absValue = values[2].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert abs value:" << values[2];
                continue;
            }
            double absthetaValue = values[3].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert abstheta value:" << values[3];
                continue;
            }
            double phasethetaValue = values[4].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert phasetheta value:" << values[4];
                continue;
            }
            double absphiValue = values[5].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert absphi value:" << values[5];
                continue;
            }
            double phasephiValue = values[6].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert phasephi value:" << values[6];
                continue;
            }
            double axratioValue = values[7].toDouble(&ok);
            if (!ok) {
                qWarning() << "Failed to convert axratio value:" << values[7];
                continue;
            }
            theta.append(thetaValue);
            phi.append(phiValue);
            abs.append(absValue);
            abstheta.append(absthetaValue);
            phasetheta.append(phasethetaValue);
            absphi.append(absphiValue);
            phasephi.append(phasephiValue);
            axratio.append(axratioValue);
        }
    }

    file.close();

    //打印读取到的数据
    //qDebug() << "Theta:" << theta;
    //qDebug() << "Phi:" << phi;
    //qDebug() << "Abs:" << abs;
    //qDebug() << "AbsTheta:" << abstheta;
    //qDebug() << "PhaseTheta:" << phasetheta;
    //qDebug() << "AbsPhi:" << absphi;
    //qDebug() << "PhasePhi:" << phasephi;
    //qDebug() << "Ax.Ratio:" << axratio;
}


//绘制极坐标方向图，参数为两个数组，在mainwindow中调用
void DirShow::DrawPolarDirectionChart(const QVector<double>& a, const QVector<double>& b)
{
    QChartView *chartView = new QChartView(this); //图表视图
    QPolarChart *chart = new QPolarChart();
    chart->setTitle("Farfield Directivity Abs");//图像标题
    QFont titleFont = chart->titleFont();//设置字体大小
    titleFont.setPointSize(12);
    chart->setTitleFont(titleFont);
    //chart添加到chartView
    chartView->setChart(chart);
    this->setCentralWidget(chartView);
    QLineSeries *series = new QLineSeries();   //折线序列
    series->setName(linename);//曲线名称
    QFont legendFont;// 设置图例字体
    legendFont.setPointSize(12); // 设置字体大小为12
    chart->legend()->setFont(legendFont);

    //QLegend* legend = chart->legend();
    //legend->setAlignment(Qt::AlignRight);

    double minY = *std::min_element(b.constBegin(), b.constEnd());
    double maxY = *std::max_element(b.constBegin(), b.constEnd());
    if (minY>0)
    {
        minY=0;//最小值大于0时坐标轴从0取
    }

    abs.append(abs.first()) ;


    for (int i = 0; i < a.size()+1; i++)
    {

        series->append(i, b[i]);
        //series->append(i*5, b[i]);
    }

    chart->addSeries(series);  //序列添加到图表
    QPen pen = series->pen();
    pen.setWidth(3); // 设置线条宽度
    series->setPen(pen);

    // 创建坐标轴
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    axisX->setTickCount(12); // 角度轴刻度数量
    axisX->setRange(0, 360); // 角度轴范围

    axisY->setRange(minY*1.1, maxY*1.25); // 设置较为合适的坐标轴范围

    chart->addAxis(axisX, QPolarChart::PolarOrientationAngular); // 角度轴添加到极坐标图表
    chart->addAxis(axisY, QPolarChart::PolarOrientationRadial); // 距离轴添加到极坐标图表
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series->setName(linename);

    // 设置横坐标刻度字体大小
    QFont xAxisTickFont;
    xAxisTickFont.setPointSize(12); // 设置字体大小
    axisX->setLabelsFont(xAxisTickFont);

    // 设置纵坐标刻度字体大小
    QFont yAxisTickFont;
    yAxisTickFont.setPointSize(12); // 设置字体大小
    axisY->setLabelsFont(yAxisTickFont);

}

