#include "fielddiagramshow.h"

FieldDiagramShow::FieldDiagramShow(QWidget *parent) : QMainWindow(parent){
    setWindowTitle("FieldDiagram");
    setMinimumSize(800, 600);

    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 构建文件路径
    QString filePathX = QDir::cleanPath(appDir + "/x.txt");
    QString filePathY = QDir::cleanPath(appDir + "/y.txt");
    QString filePathAbs = QDir::cleanPath(appDir + "/abs.txt");


    // QString filePathX="E://qtmesh//meshnresult//x.txt";
    // QString filePathY="E://qtmesh//meshnresult//y.txt";
    // QString filePathAbs="E://qtmesh//meshnresult//abs.txt";

    ReadAxis(filePathX);
    X=Axis;
    dX=delta;
    xmax=maxValue;
    xmin=minValue;
    countx=Count;

    ReadAxis(filePathY);
    Y=Axis;
    dY=delta;
    ymax=maxValue;
    ymin=minValue;
    county=Count;

    ReadData(filePathAbs);
    absmax=maxValue;
    absmin=minValue;

    DrawChart(dX,dY,Abs);
    qDebug() << "dx.size" << dX.size();
    qDebug() << "dy.size" << dY.size();


}

FieldDiagramShow::~FieldDiagramShow()
{
}

//读取坐标轴数据
void FieldDiagramShow::ReadAxis(const QString& filePath)
{
    Axis.clear();
    delta.clear();
    QFile file(filePath);
    Count = 0;
    minValue=0;
    maxValue=0;
    //minValue = std::numeric_limits<double>::max();
    //maxValue = std::numeric_limits<double>::lowest();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    while (!file.atEnd())
    {

        QString line = file.readLine().trimmed();
        bool ok;
        double Value = line.toDouble(&ok);
        if (!ok) {
            qWarning() << "Failed to convert theta value:" << line;
            continue;
        }

        Count++;
        Axis.append(Value);

        // 更新最大值和最小值
        if (Value < minValue)
            minValue = Value;
        if (Value > maxValue)
            maxValue = Value;

    }


    for (int i = 1; i < Axis.size(); i++)
    {
        double diff = Axis[i] - Axis[i - 1];
        delta.append(diff);

    }
    // 重复最后一个数值, 使dx和dy数组完整，不丢失数据
    delta.append(delta.last());

    file.close();

    qDebug() << "Count:" << Count;
    //qDebug() << "length:" << length;
    //qDebug() << "最大:" << maxValue;
    //qDebug() << "最小:" << minValue;

}

//读取每个数据点上的值
void FieldDiagramShow::ReadData(const QString& filePath)
{
    QFile file(filePath);
    int Count = 0;//记录点数

    minValue=0;
    maxValue=0;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << file.errorString();
        return;
    }

    while (!file.atEnd())

    {
        QString line = file.readLine().trimmed();
        QStringList values = line.split(QRegularExpression("\\s+"));

        for (const QString& value : values)
        {
            bool ok;
            double Value = value.toDouble(&ok);

            if (!ok)
            {
                qWarning() << "Failed to convert value to double:" << value;
                continue;
            }

            Abs.append(Value);
            Count++;

            // 更新最大值和最小值
            if (Value < minValue)
                minValue = Value;
            if (Value > maxValue)
                maxValue = Value;
        }

    }

    qDebug() << "Count:" << Count;

    file.close();
}


void FieldDiagramShow::DrawChart(const QVector<double>& dx,const QVector<double>& dy, const QVector<double>& values)
{

//原绘制方法
/*
    // 创建绘图设备
    const int pixelsPerMeter = 100;//基础单元大小，100~150都较为合适
    QPixmap pixmap((xmax-xmin)*pixelsPerMeter, (ymax-ymin)*pixelsPerMeter);//设置画布大小，根据数据的取值范围决定
    //pixmap.fill(Qt::white); // 设置背景颜色为白色
    QPainter painter(&pixmap);


    // 创建主窗口部件和布局管理器
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(centralWidget);//创建一个水平布局管理器
    layout->setSpacing(10); // 设置间距为10像素

    double startX = 0;//计数
    double startY = 0;

    // 绘制热图
    for (int j = 0; j <dy.size() ; j++)
    {
        for (int i = 0; i <dx.size() ; i++)
        {
            QColor color = CalculateColorparula(values[j * dx.size() + i]);
            painter.fillRect(startX, startY, dx[i]*pixelsPerMeter*2, dy[j]*pixelsPerMeter*2, color);
            startX += dx[i]*pixelsPerMeter;
        }
        startY += dy[j]*pixelsPerMeter;
        startX = 0;
    }

    // 绘制色标
    int colorBarWidth = 20; // 色标的宽度（可调整）
    int colorBarHeight = pixmap.height(); // 色标的高度，与图像的高度一致

    QPixmap colorBarPixmap(colorBarWidth, colorBarHeight);
    QPainter colorBarPainter(&colorBarPixmap);

    for (int y = 0; y < colorBarHeight; ++y)
    {
        double value = absmax - (y / static_cast<double>(colorBarHeight)) * (absmax - absmin);
        QColor color = CalculateColorparula(value);
        colorBarPainter.fillRect(0, y, colorBarWidth, 1, color);
    }




    //创建y坐标部件
    QVBoxLayout* ylabel = new QVBoxLayout();

    const int YnumLabels = 6;//文本标签个数，实际个数+1
    for (int i = 0; i <= YnumLabels; i++) {
        double value;
        if (i == YnumLabels) {
            value = ymax;
            //ylabel->setAlignment(Qt::AlignTop);
        } else {
            value = i * (ymax - ymin) / YnumLabels + ymin;//根据最大值和最小值等间隔设置文本
        }
        QLabel* label = new QLabel(QString::number(value, 'f', 1), this);//文本数值为浮点型，保留一位小数
        label->setAlignment(Qt::AlignTop);
        label->setFixedHeight(pixmap.height() / (YnumLabels+1));
        ylabel->addWidget(label);
        ylabel->addStretch();
        ylabel->setAlignment(Qt::AlignBottom);
    }


    // 将垂直布局管理器添加到水平布局管理器
    layout->addLayout(ylabel);

    //创建x坐标部件
    QHBoxLayout* xlabel = new QHBoxLayout();
    const int XnumLabels = 6;
    for (int i = 1; i <=XnumLabels; i++) {
        double value;
        if (i == XnumLabels) {
            value = xmax;

        } else {
            value = (i-1) * (xmax - xmin) / XnumLabels + xmin;//i从0开始会导致x坐标所占位置超出范围

        }
        QLabel* label = new QLabel(QString::number(value, 'f', 1), this);
        //label->setAlignment(Qt::AlignRight);
        label->setAlignment(Qt::AlignHCenter);
        label->setFixedHeight(pixmap.height() / (XnumLabels + 1));
        xlabel->addWidget(label);
    }
    xlabel->addStretch();
    xlabel->setSpacing(pixmap.width()/ (XnumLabels+1));
    //xlabel->setContentsMargins(0, 0, 0, 0); // 设置x坐标部件和左上右下的边距为0

    // 创建热图部件
    QLabel* chartLabel = new QLabel(this);
    chartLabel->setPixmap(pixmap);

    //创建x坐标和热图部件部件
    QVBoxLayout* maxandx = new QVBoxLayout();
    // 将热图部件添加到布局管理器
    maxandx->addWidget(chartLabel);
    maxandx->addLayout(xlabel);

    // 创建色标部件
    QLabel* colorBarLabel = new QLabel(this);
    colorBarLabel->setPixmap(colorBarPixmap);
    // 将色标向上对齐
    colorBarLabel->setAlignment(Qt::AlignTop);


    //色标刻度
    QVBoxLayout* barlabel = new QVBoxLayout();

    const int numLabels = 6;
    for (int i = numLabels; i >= 0; i--) {
        double value;
        if (i == numLabels) {
            value = absmax;
        } else {
            value = i * (absmax - absmin) / numLabels + absmin;
        }
        QLabel* label = new QLabel(QString::number(value, 'f', 1), this);
        label->setAlignment(Qt::AlignTop);
        label->setFixedHeight(pixmap.height() / (numLabels + 1));
        barlabel->addWidget(label);
        barlabel->addStretch();
        barlabel->setAlignment(Qt::AlignBottom);
    }


    // 将垂直布局管理器添加到水平布局管理器
    layout->addLayout(maxandx);
    layout->addWidget(colorBarLabel);
    layout->addLayout(barlabel);

    // 设置布局管理器和主窗口部件
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);


    // 调整窗口大小以适应内容
    resize(pixmap.width(), pixmap.height());
*/

    // 创建绘图设备
    pixelsPerMeter = 100;
    int baseWidth = (xmax - xmin) * pixelsPerMeter;
    int baseHeight = (ymax - ymin) * pixelsPerMeter;

    // 创建主容器
    QWidget* container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 创建整体布局
    QHBoxLayout* mainLayout = new QHBoxLayout(container);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Y轴标签 (左侧)
    QVBoxLayout* yAxisLayout = new QVBoxLayout();
    yAxisLayout->setSpacing(0);
    const int yNumLabels = 6;

    for (int i = 0; i <= yNumLabels; ++i) {
       //double value = (i == yNumLabels) ? ymax : ymin + i * (ymax - ymin) / yNumLabels;
       double value = ymax - i * (ymax - ymin) / yNumLabels;
       QLabel* label = new QLabel(QString::number(value, 'f', 1));
       label->setAlignment(Qt::AlignRight | Qt::AlignTop);
       yAxisLayout->addWidget(label);
    }


    // 2. 热图 + X轴 (中部)
    QVBoxLayout* chartLayout = new QVBoxLayout();
    chartLayout->setSpacing(5);

    // 热图显示
    QLabel* chartLabel = new QLabel();
    chartLabel->setAlignment(Qt::AlignCenter);
    chartLayout->addWidget(chartLabel);

    // X轴标签
    QHBoxLayout* xAxisLayout = new QHBoxLayout();
    xAxisLayout->setSpacing(0);
    const int xNumLabels = 6;

    for (int i = 0; i <= xNumLabels; ++i) {
        double value = xmin + i * (xmax - xmin) / xNumLabels;
        QLabel* label = new QLabel(QString::number(value, 'f', 1));
        label->setAlignment(Qt::AlignHCenter);
        xAxisLayout->addWidget(label);
    }
    chartLayout->addLayout(xAxisLayout);

    // 3. 色标 + 刻度 (右侧)
    QHBoxLayout* colorBarLayout = new QHBoxLayout();
    colorBarLayout->setSpacing(5);

    // 色标显示
    QLabel* colorBarLabel = new QLabel();
    colorBarLabel->setAlignment(Qt::AlignTop);
    colorBarLayout->addWidget(colorBarLabel);

    // 色标刻度
    QVBoxLayout* scaleLayout = new QVBoxLayout();
    scaleLayout->setSpacing(0);
    const int scaleNumLabels = 6;

    for (int i = scaleNumLabels; i >= 0; --i) {
        double value = absmin + i * (absmax - absmin) / scaleNumLabels;
        QLabel* label = new QLabel(QString::number(value, 'f', 1));
        label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        scaleLayout->addWidget(label);
    }
    colorBarLayout->addLayout(scaleLayout);

    // 组装主布局
    mainLayout->addLayout(yAxisLayout);
    mainLayout->addLayout(chartLayout, 1);  // 热图区域优先扩展
    mainLayout->addLayout(colorBarLayout);

    // 创建热图
    QPixmap pixmap(baseWidth, baseHeight);
    //pixmap.fill(Qt::white);
    QPainter painter(&pixmap);

    double startX = 0;
    double startY = 0;

    // 绘制热图


    for (int j = 0; j < dy.size(); j++) {
        for (int i = 0; i < dx.size(); i++) {
            QColor color = CalculateColorparula(values[j * dx.size() + i]);
            painter.fillRect(startX, startY, dx[i] * pixelsPerMeter*2, dy[j] * pixelsPerMeter*2, color);
            startX += dx[i] * pixelsPerMeter;
        }
        startY += dy[j] * pixelsPerMeter;
        startX = 0;
    }

    // 创建色标
    int colorBarWidth = 20;
    int colorBarHeight = baseHeight;
    QPixmap colorBarPixmap(colorBarWidth, colorBarHeight);
    QPainter colorBarPainter(&colorBarPixmap);

    for (int y = 0; y < colorBarHeight; ++y) {
        double value = absmax - (y / static_cast<double>(colorBarHeight)) * (absmax - absmin);
        QColor color = CalculateColorparula(value);
        colorBarPainter.fillRect(0, y, colorBarWidth, 1, color);
    }

    // 设置图像
    chartLabel->setPixmap(pixmap);
    colorBarLabel->setPixmap(colorBarPixmap);

    // 设置容器大小
    container->setFixedSize(baseWidth + 150, baseHeight + 50);  // 额外空间用于标签

    // 创建中央容器
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* centralLayout = new QHBoxLayout(centralWidget);
    centralLayout->addStretch();
    centralLayout->addWidget(container);
    centralLayout->addStretch();

    QVBoxLayout* outerLayout = new QVBoxLayout(centralWidget);
    outerLayout->addStretch();
    outerLayout->addLayout(centralLayout);
    outerLayout->addStretch();

    setCentralWidget(centralWidget);



    // 调整窗口大小
    //resize(container->width() + 100, container->height() + 100);
    resize(container->width() , container->height() );



}




QColor FieldDiagramShow::CalculateColorjet(double value)
{
    // "jet"颜色映射的RGB数值表
    QVector<QColor> jetColors = {
        QColor(0, 0, 127),
        QColor(0, 0, 255),
        QColor(0, 127, 255),
        QColor(0, 255, 255),
        QColor(127, 255, 127),
        QColor(255, 255, 0),
        QColor(255, 127, 0),
        QColor(255, 0, 0),
        QColor(127, 0, 0)
    };

    //用最大最小值之差进行映射并做差值
    double position = (value - absmin) / (absmax - absmin) * (jetColors.size() - 1);
    int lowerIndex = floor(position);
    int upperIndex = ceil(position);
    double interpolationFactor = position - lowerIndex;

    QColor lowerColor = jetColors[lowerIndex];
    QColor upperColor = jetColors[upperIndex];

    int red = qRound((1 - interpolationFactor) * lowerColor.red() + interpolationFactor * upperColor.red());
    int green = qRound((1 - interpolationFactor) * lowerColor.green() + interpolationFactor * upperColor.green());
    int blue = qRound((1 - interpolationFactor) * lowerColor.blue() + interpolationFactor * upperColor.blue());

    return QColor(red, green, blue);

}


QColor FieldDiagramShow::CalculateColorparula(double value)
{
    // "parula"颜色映射的RGB数值表
    QVector<QColor> parulaColors = {
        QColor(53, 42, 135),
        QColor(15, 92, 221),
        QColor(18, 125, 216),
        QColor(7, 156, 207),
        QColor(21, 177, 180),
        QColor(89, 189, 140),
        QColor(165, 190, 107),
        QColor(225, 185, 82),
        QColor(252, 206, 46),
        QColor(249, 251, 14)
    };

    double position = (value - absmin) / (absmax - absmin) * (parulaColors.size() - 1);
    int lowerIndex = floor(position);
    int upperIndex = ceil(position);
    double interpolationFactor = position - lowerIndex;

    QColor lowerColor = parulaColors[lowerIndex];
    QColor upperColor = parulaColors[upperIndex];

    int red = qRound((1 - interpolationFactor) * lowerColor.red() + interpolationFactor * upperColor.red());
    int green = qRound((1 - interpolationFactor) * lowerColor.green() + interpolationFactor * upperColor.green());
    int blue = qRound((1 - interpolationFactor) * lowerColor.blue() + interpolationFactor * upperColor.blue());

    return QColor(red, green, blue);
}
