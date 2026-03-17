#include "spshow.h"

// SpShow::SpShow(QWidget *parent) : QMainWindow(parent){
//     setWindowTitle("S-Parameters");//窗口名称
//     setMinimumSize(800, 600);

//     // 获取应用程序可执行文件所在目录
//     QString appDir = QCoreApplication::applicationDirPath();

//     // 构建文件路径
//     // QString spline1 = QDir::cleanPath(appDir + "/s11.txt");
//     // QString spline2 = QDir::cleanPath(appDir + "/s21.txt");
//     // QString spline3 = QDir::cleanPath(appDir + "/s22.txt");
//     // QString spline4 = QDir::cleanPath(appDir + "/s54.txt");
//     QString spline4 = QDir::cleanPath(appDir + "/1-0个探针51-51-67-Ex.txt");


//     // QVector<QString> filenames = {
//     //                               "E:\\qtmesh\\meshnresult\\s11.txt",
//     //                               "E:\\qtmesh\\meshnresult\\s21.txt",
//     //                               "E:\\qtmesh\\meshnresult\\s22.txt",
//     //                               "E:\\qtmesh\\meshnresult\\s54.txt"};

//     QVector<QString> filenames = {spline4};

//     // // 调试输出路径
//     // qDebug() << "Loading mesh files from:" << appDir;
//     // qDebug() << "filenames:" << filenames;


//     ReadData(filenames);
//     CreateChart(xDataList,yDataList,linename);

// }



SpShow::SpShow(QWidget *parent,const QString& filenameread) : QMainWindow(parent){
    //SpShow::SpShow(QWidget *parent) : QMainWindow(parent){
    //setWindowTitle("S-Parameters");//窗口名称
    setWindowTitle(filenameread);//窗口名称
    setMinimumSize(800, 600);

    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 构建文件路径
     // QString spline1 = QDir::cleanPath(appDir + "/s11.txt");
     // QString spline2 = QDir::cleanPath(appDir + "/s21.txt");
     // QString spline3 = QDir::cleanPath(appDir + "/s22.txt");
     // QString spline4 = QDir::cleanPath(appDir + "/s54.txt");
    //QString spline4 = QDir::cleanPath(appDir + "/1-0个探针51-51-67-Ex.txt");
    QString drawlines = QDir::cleanPath(appDir + "/"+filenameread+".txt");
    //QString drawlines = QDir::cleanPath(appDir + "/"+"1-0-probe-73-73-59-Ex"+".txt");

    qDebug() << "正在绘制:" << drawlines;

    // QVector<QString> filenames = {
    //                               //"E:\\qtmesh\\meshnresult\\s11.txt",
    //                               "E:\\qtmesh\\meshnresult\\s21.txt",
    //                               "E:\\qtmesh\\meshnresult\\s22.txt",
    //                               "E:\\qtmesh\\meshnresult\\s54.txt"};

    QVector<QString> filenames = {drawlines};

    // // 调试输出路径
    // qDebug() << "Loading mesh files from:" << appDir;
    // qDebug() << "filenames:" << filenames;


    ReadData(filenames);
    CreateChart(xDataList,yDataList,linename);

}


SpShow::~SpShow()
{

}


void SpShow::ReadData(const QVector<QString>& filenames)
{
    QVector<QFile*> files;
    for (const QString& filename : filenames)
    {
        QFile* file = new QFile(filename);  // 创建文件指针，并动态分配内存
        if (file->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // 读取文件的代码
            QVector<double> columnxArray; // 存储第一列数据的动态数组
            QVector<double> columnyArray; // 存储第二列数据的动态数组

            QTextStream in(file);

            QString line1 = in.readLine(); // 读取第一行
            line2 = in.readLine(); // 读取第二行
            QString line3 = in.readLine(); // 读取第三行

            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringList values = line.split("\t");

                if (values.size() >= 2)
                {
                    bool ok;
                    double valuex = values[0].toDouble(&ok);
                    if (ok)
                    {
                        columnxArray.append(valuex);
                    }

                    double valuey = values[1].toDouble(&ok);
                    if (ok)
                    {
                        columnyArray.append(valuey);
                    }
                }
            }

            file->close();

            xDataList.append(columnxArray); // 将第一列数据数组添加到容器中
            yDataList.append(columnyArray); // 将第二列数据数组添加到容器中

            files.append(file);  // 将文件指针添加到容器中

            // 输出读取到的数据
            /*
             qDebug() << "Data from file" << filename;
                      qDebug() << "X values:";
            for (double value : columnxArray)
            {
                qDebug() << value;
            }

            qDebug() << "Y values:";
            for (double value : columnyArray)
            {
                qDebug() << value;
            }
*/
        }
        else
        {
            qDebug() << "Failed to open file"<< file->fileName();
            delete file;          // 打开失败时释放动态分配的内存
        }
    }

    for (QFile* file : files)// 在不再需要文件时释放动态分配的内存
    {
        file->close();
        delete file;
    }

    for (int i = 0; i < filenames.size(); i++)
    {
        QString filename = filenames[i];
        // 使用QFileInfo提取文件名作为每条曲线图例的名称
        QFileInfo fileInfo(filename);
        QString name = fileInfo.baseName();

        linename.append(name);

        qDebug() << "曲线名称: " << linename[i];
        /*
        qDebug() << "stringName: ";
        for (const QString& legends : linename) {
            qDebug() << legends;
        }
*/
    }





}

/*
 * //读取文件数据与横坐标和文件名
void SpShow::ReadData(const QVector<QString>& filenames)
{
    QVector<QFile*> files;
    for (const QString& filename : filenames)
    {
        QFile* file = new QFile(filename);  // 创建文件指针，并动态分配内存
        if (file->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // 读取文件的代码
            QVector<double> columnxArray; // 存储第一列数据的动态数组
            QVector<double> columnyArray; // 存储第二列数据的动态数组

            // QTextStream in(file);


            // QString line1 = in.readLine(); // 读取第一行
            // QString line2 = in.readLine(); // 读取第二行
            // QString line3 = in.readLine(); // 读取第三行

            QTextStream in(file);

            // 跳过第一行分隔线
            in.readLine();
            // 读取第二行作为标题
            line2 = in.readLine();
            // 跳过第三行分隔线
            in.readLine();

            while (!in.atEnd())
            {
                QString line = in.readLine();
                QStringList values = line.split(" ");

                if (values.size() >= 2)
                {
                    bool ok;
                    double valuex = values[0].toDouble(&ok);
                    if (ok)
                    {
                        columnxArray.append(valuex);
                    }

                    double valuey = values[1].toDouble(&ok);
                    if (ok)
                    {
                        columnyArray.append(valuey);
                    }
                }
            }

            file->close();

            xDataList.append(columnxArray); // 将第一列数据数组添加到容器中
            yDataList.append(columnyArray); // 将第二列数据数组添加到容器中

            files.append(file);  // 将文件指针添加到容器中

            // 输出读取到的数据

            // qDebug() << "Data from file" << filename;
            qDebug() << "X values:";
            for (double value : columnxArray)
            {
                qDebug() << value;
            }

            qDebug() << "Y values:";
            for (double value : columnyArray)
            {
                qDebug() << value;
            }

        }
        else
        {
            qDebug() << "Failed to open file"<< file->fileName();
            delete file;          // 打开失败时释放动态分配的内存
        }
    }

    for (QFile* file : files)// 在不再需要文件时释放动态分配的内存
    {
        file->close();
        delete file;
    }

    for (int i = 0; i < filenames.size(); i++)
    {
        QString filename = filenames[i];
        // 使用QFileInfo提取文件名作为每条曲线图例的名称
        QFileInfo fileInfo(filename);
        QString name = fileInfo.baseName();

        linename.append(name);

        qDebug() << "曲线名称: " << linename[i];

        qDebug() << "stringName: ";
        for (const QString& legends : linename) {
            qDebug() << legends;
        }

}





}

// void SpShow::ReadData(const QVector<QString>& filenames)
// {
//     // 清空之前的数据
//     xDataList.clear();
//     yDataList.clear();
//     linename.clear();

//     for (const QString& filename : filenames)
//     {
//         QFile file(filename);
//         if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//         {
//             qDebug() << "Failed to open file" << filename;
//             continue;
//         }

//         QTextStream in(&file);
//         QVector<double> columnxArray;
//         QVector<double> columnyArray;

//         qDebug() << "开始读取文件:" << filename;

//         // 方法1: 尝试读取前几行查看实际格式
//         QString line;
//         int lineCount = 0;

//         // 读取并打印前5行，了解文件结构
//         while (lineCount < 5 && !in.atEnd()) {
//             line = in.readLine();
//             qDebug() << "第" << lineCount << "行:" << line;
//             lineCount++;
//         }

//         // 重置到文件开头
//         file.seek(0);
//         in.seek(0);

//         // 方法2: 更健壮的读取方式
//         int currentLine = 0;
//         bool dataStarted = false;

//         while (!in.atEnd())
//         {
//             QString line = in.readLine();
//             currentLine++;

//             // 跳过分隔线（包含"="的行）
//             if (line.contains("=")) {
//                 qDebug() << "跳过分隔线（第" << currentLine << "行）";
//                 continue;
//             }

//             // 第二行是标题行，保存横坐标信息
//             if (currentLine == 2) {
//                 line2 = line.trimmed();  // 保存标题行
//                 qDebug() << "标题行:" << line2;
//                 continue;
//             }

//             // 从第四行开始才是数据（前三行：分隔线、标题行、分隔线）
//             if (currentLine <= 3) {
//                 continue;
//             }

//             // 现在开始处理数据行
//             dataStarted = true;

//             // 尝试多种分隔符
//             QStringList values;
//             if (line.contains("\t")) {
//                 values = line.split("\t");
//             } else if (line.contains(" ")) {
//                 // 处理多个空格的情况
//                 values = line.split(" ", Qt::SkipEmptyParts);
//             } else {
//                 qDebug() << "无法识别分隔符（第" << currentLine << "行）:" << line;
//                 continue;
//             }

//             if (values.size() >= 2)
//             {
//                 bool ok1, ok2;
//                 double valuex = values[0].toDouble(&ok1);
//                 double valuey = values[1].toDouble(&ok2);

//                 if (ok1 && ok2)
//                 {
//                     columnxArray.append(valuex);
//                     columnyArray.append(valuey);
//                 }
//                 else
//                 {
//                     qDebug() << "数值转换失败（第" << currentLine << "行）:"
//                              << values[0] << "," << values[1];
//                 }
//             }
//             else
//             {
//                 qDebug() << "列数不足（第" << currentLine << "行）:" << line;
//             }
//         }

//         file.close();

//         // 输出读取统计信息
//         qDebug() << "文件:" << filename;
//         qDebug() << "读取到" << columnxArray.size() << "个数据点";
//         qDebug() << "横坐标范围:"
//                  << (columnxArray.isEmpty() ? "无数据" : QString::number(columnxArray.first()))
//                  << "到"
//                  << (columnxArray.isEmpty() ? "无数据" : QString::number(columnxArray.last()));

//         if (!columnxArray.isEmpty())
//         {
//             xDataList.append(columnxArray);
//             yDataList.append(columnyArray);

//             // 提取文件名作为曲线名称
//             QFileInfo fileInfo(filename);
//             linename.append(fileInfo.baseName());
//             qDebug() << "曲线名称:" << fileInfo.baseName();
//         }
//         else
//         {
//             qDebug() << "警告: 文件" << filename << "没有读取到有效数据";
//         }
//     }

//     qDebug() << "总共读取了" << xDataList.size() << "个文件的数据";
// }
*/



//获取横坐标的名称
QString SpShow::ExtractAxisX(const QString& line)
{
    // QRegularExpression regex("\"(.*?)\"");
    // QRegularExpressionMatch match = regex.match(line);
    // if (match.hasMatch())
    // {
    //     return match.captured(0);
    // }
    // else
    // {
    //     return QString();
    // }

    QStringList parts = line.split("\t");
    if (!parts.isEmpty()) {
        QString firstPart = parts[0].trimmed();
        qDebug() << "提取到横坐标名称:" << firstPart;
        return firstPart;
    }

}

void SpShow::CreateChart( const QVector<QVector<double>>& xDataList,const QVector<QVector<double>>& yDataList,const QVector<QString>&linename)
{

    QChartView *chartView = new QChartView;
    QChart *chart = chartView->chart();
    setCentralWidget(chartView);
    chartView->resize(800, 600);
    //chart->setTitle("S-Parameter");
    QFont titleFont = chart->titleFont();
    titleFont.setPointSize(12); //设置标题字体大小
    chart->setTitleFont(titleFont);
    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    //QList<Qt::PenStyle> lineStyles;

    for (int i = 0; i < xDataList.size(); i++)
    {

        QLineSeries *series = new QLineSeries;

        // 在图表中添加系列
        chart->addSeries(series);
        series->setName(linename[i]);
        //设置图例字体大小
        QLegend* legend = chart->legend();
        QFont font = legend->font();
        font.setPointSize(12);
        legend->setFont(font);
        legend->setAlignment(Qt::AlignRight);

        //获取系列的QPen对象，设置线型粗细
        QPen pen = series->pen();
        pen.setWidth(3);

        //自定义线型，使每一条曲线的线型都不同，且线型根据曲线编号决定
        pen.setStyle(Qt::CustomDashLine);
        pen.setDashPattern(QVector<qreal>() << 5 << i << i << i*2 << i << i*2);

        //将更新后的QPen对象设置给系列
        series->setPen(pen);
            // 将虚线的QPen对象设置给系列
        //lineStyles <<Qt::SolidLine <<Qt::CustomDashLine<< Qt::DashLine << Qt::DotLine << Qt::DashDotLine << Qt::DashDotDotLine;
        //pen.setStyle(lineStyles[i % lineStyles.size()]);

        // 将系列与坐标轴关联
        series->attachAxis(axisX);
        series->attachAxis(axisY);

        // 添加数据到系列
        QVector<double> xData = xDataList[i];
        QVector<double> yData = yDataList[i];

        chart->update();//更新图表显示
        for (int j = 0; j < xData.size(); j++)
        {
            series->append(xData[j], yData[j]);
        }

    }

    double minValuex = std::numeric_limits<double>::max();
    double maxValuex = std::numeric_limits<double>::min();
    double minValuey = std::numeric_limits<double>::max();
    double maxValuey = std::numeric_limits<double>::min();

    for (int i = 0; i < xDataList.size(); i++)
    {
        QVector<double> xData = xDataList[i];
        QVector<double> yData = yDataList[i];
        double minValuexFile = *std::min_element(xData.constBegin(), xData.constEnd());
        double maxValuexFile = *std::max_element(xData.constBegin(), xData.constEnd());
        double minValueyFile = *std::min_element(yData.constBegin(), yData.constEnd());
        double maxValueyFile = *std::max_element(yData.constBegin(), yData.constEnd());
        minValuex = std::min(minValuex, minValuexFile);
        maxValuex = std::max(maxValuex, maxValuexFile);
        minValuey = std::min(minValuey, minValueyFile);
        maxValuey = std::max(maxValuey, maxValueyFile);
    }

    axisX->setRange(minValuex, maxValuex);
    axisY->setRange(minValuey*1.1, maxValuey*1.1);
    stringX = ExtractAxisX(line2);
    axisX->setTitleText(stringX);  // 设置横坐标名字
    //axisY->setTitleText("dB");  // 设置纵坐标名字
    axisY->setTitleText("Ex");  // 设置纵坐标名字
    axisX->setMinorTickCount(1);//X轴的次网格线个数
    axisY->setMinorTickCount(2);//Y轴的次网格线个数

    // 设置横坐标刻度字体大小
    QFont xAxisTickFont;
    xAxisTickFont.setPointSize(10);
    axisX->setLabelsFont(xAxisTickFont);

    // 设置纵坐标刻度字体大小
    QFont yAxisTickFont;
    yAxisTickFont.setPointSize(10);
    axisY->setLabelsFont(yAxisTickFont);

    //设置坐标轴名称字体大小
    QFont xAxisTitleFont = axisX->titleFont();
    QFont yAxisTitleFont = axisY->titleFont();
    xAxisTitleFont.setPointSize(12);
    yAxisTitleFont.setPointSize(12);
    axisX->setTitleFont(xAxisTitleFont);
    axisY->setTitleFont(yAxisTitleFont);


}
