#include "meshshow.h"
#include <QTimer>

MeshShow::MeshShow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("mesh");//窗口名称
    setMinimumSize(800, 600);

    // // 改为只安装到当前窗口的事件过滤器
    // this->installEventFilter(this);

    // // 设置光标
    // setCursor(Qt::ArrowCursor);


    setMouseTracking(true);
    mouseEventsEnabled = true;
    isDragging = false;

    scaleFactor=10;
    currentLayerIndex=0;
    locationP=0.0;

    totalOffset={0, 0};
    isDragging=false;
    isViewOffset = false;


    // 创建按钮
    xyButton = new QPushButton("XY Plane", this);
    xzButton = new QPushButton("XZ Plane", this);
    yzButton = new QPushButton("YZ Plane", this);

    xyButton->setCheckable(true);
    xzButton->setCheckable(true);
    yzButton->setCheckable(true);

    // 创建按钮组并设置互斥
    planeGroup = new QButtonGroup(this);
    planeGroup->addButton(xyButton);
    planeGroup->addButton(xzButton);
    planeGroup->addButton(yzButton);
    xyButton->setFixedHeight(40);
    xzButton->setFixedHeight(40);
    yzButton->setFixedHeight(40);

    // 设置默认选中XY平面
    xyButton->setChecked(true);

    layerSpinBox = new QDoubleSpinBox(this);  // 正确初始化
    layerSpinBox->setDecimals(6);            // 允许两位小数
    layerSpinBox->setRange(0.0, 1000.0);     // 设置 double 范围
    layerSpinBox->setValue(locationP);       // locationP 需为 double 类型
    layerSpinBox->setSingleStep(0.1);        // 设置步长为 0.1
    layerSpinBox->setFixedWidth(80);
    layerSpinBox->setFixedHeight(40);

    updateButton = new QPushButton("Update", this);
    updateButton->setFixedWidth(80);
    updateButton->setFixedHeight(40);


    // 缩放因子输入控件
    scaleSpinBox = new QDoubleSpinBox(this);
    scaleSpinBox->setDecimals(1);
    scaleSpinBox->setRange(1.0, 5000.0);
    scaleSpinBox->setValue(scaleFactor);
    scaleSpinBox->setSingleStep(1);
    scaleSpinBox->setFixedWidth(80);
    scaleSpinBox->setFixedHeight(40);



    // 使用工具栏放置按钮
    QToolBar *toolBar = addToolBar("Plane Selection");
    toolBar->addWidget(xyButton);
    toolBar->addWidget(xzButton);
    toolBar->addWidget(yzButton);

    // 设置默认选中XY平面
    xyButton->setChecked(true);

    // 设置按钮选中样式
    this->setStyleSheet(
        "QPushButton:checked {"
        "   background-color: #666666;"
        "   color: white;"
        "}"
        "QPushButton {"
        "   background-color: #E0E0E0;"
        "   border: 1px solid #A0A0A0;"
        "   padding: 5px;"
        "}"
        );




    toolBar->addSeparator();
    toolBar->addWidget(new QLabel("position:", this));
    toolBar->addWidget(layerSpinBox);

    toolBar->addSeparator();
    toolBar->addWidget(new QLabel("Scale:", this));
    toolBar->addWidget(scaleSpinBox);  // 确保这里已添加

    toolBar->addWidget(updateButton);
    addToolBar(toolBar);


    // 连接信号槽
    connect(xyButton, &QPushButton::clicked, this, &MeshShow::onXYButtonClicked);
    connect(xzButton, &QPushButton::clicked, this, &MeshShow::onXZButtonClicked);
    connect(yzButton, &QPushButton::clicked, this, &MeshShow::onYZButtonClicked);
    connect(updateButton, &QPushButton::clicked, this, &MeshShow::onUpdateClicked);

    // 创建尺寸标签
    xSizeLabel = new QLabel("X: -", this);
    ySizeLabel = new QLabel("Y: -", this);
    zSizeLabel = new QLabel("Z: -", this);

    // 设置标签样式
    QFont font("Arial", 12);
    xSizeLabel->setFont(font);
    ySizeLabel->setFont(font);
    zSizeLabel->setFont(font);

    // 添加到状态栏
    statusBar()->addPermanentWidget(xSizeLabel);
    statusBar()->addPermanentWidget(ySizeLabel);
    statusBar()->addPermanentWidget(zSizeLabel);

    // 初始化尺寸显示
    updateSizeLabels();

    currentCoordLabel = new QLabel("current coordinates:-", this);
    currentCoordLabel->setFont(font);
    statusBar()->addPermanentWidget(currentCoordLabel);


    setMouseTracking(true);
    //qApp->installEventFilter(this); // 添加全局事件过滤




    // 添加重置按钮
    resetButton = new QPushButton("Reset", this);
    resetButton->setFixedWidth(80);
    resetButton->setFixedHeight(40);
    toolBar->addWidget(resetButton);
    connect(resetButton, &QPushButton::clicked, this, &MeshShow::onResetClicked);


    // // 读取网格文件E:\qtmesh\mesh1
    // readGridFile("E://qtmesh//meshnresult//structureRegionGridsX.txt", Xlines);
    // readGridFile("E://qtmesh//meshnresult//structureRegionGridsY.txt", Ylines);
    // readGridFile("E://qtmesh//meshnresult//structureRegionGridsZ.txt", Zlines);

    // if(!readGridFile("E://qtmesh//meshnresult//structureRegionGridsX.txt", Xlines) ||
    //     !readGridFile("E://qtmesh//meshnresult//structureRegionGridsY.txt", Ylines) ||
    //     !readGridFile("E://qtmesh//meshnresult//structureRegionGridsZ.txt", Zlines))
    // {
    //     qDebug() << "Failed to load grid files!";
    // }

    // //qDebug()<<"Xlines"<<Xlines;
    // //qDebug()<<"Ylines"<<Ylines;
    // //qDebug()<<"Zlines"<<Zlines;

    // // 读取颜色文件
    // readColorFile("E://qtmesh//meshnresult//structureRegion_parameter.txt", meshcolor);
    // readmaterial("E://qtmesh//meshnresult//material.txt",legendLabels,colors);

    // if(!readColorFile("E://qtmesh//meshnresult//structureRegion_parameter.txt", meshcolor) ||
    //     !readmaterial("E://qtmesh//meshnresult//material.txt",legendLabels,colors))
    // {
    //     qDebug() << "Failed to load color/material files!";
    // }


    // if (toolBar) {
    //     toolBar->setCursor(Qt::ArrowCursor);
    // }

    // // 为所有按钮设置箭头光标
    // QList<QPushButton*> buttons = findChildren<QPushButton*>();
    // foreach (QPushButton* button, buttons) {
    //     button->setCursor(Qt::ArrowCursor);
    // }

    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 构建文件路径
    QString gridXFile = QDir::cleanPath(appDir + "/structureRegionGridsX.txt");
    QString gridYFile = QDir::cleanPath(appDir + "/structureRegionGridsY.txt");
    QString gridZFile = QDir::cleanPath(appDir + "/structureRegionGridsZ.txt");
    QString colorFile = QDir::cleanPath(appDir + "/structureRegion_parameter.txt");
    QString materialFile = QDir::cleanPath(appDir + "/material.txt");
    // QString gridXFile = "E:qt/6_5P1/structureRegionGridsX.txt";
    // QString gridYFile = "E:qt/6_5P1/structureRegionGridsY.txt";
    // QString gridZFile = "E:qt/6_5P1/structureRegionGridsZ.txt";
    // QString colorFile = "E:qt/6_5P1/structureRegion_parameter.txt";
    // QString materialFile = "E:qt/6_5P1/material.txt";

    // // 调试输出路径
    // qDebug() << "Loading mesh files from:" << appDir;
    // qDebug() << "X file:" << gridXFile;
    // qDebug() << "Y file:" << gridYFile;
    // qDebug() << "Z file:" << gridZFile;

    // 读取文件
    readGridFile(gridXFile, Xlines);
    readGridFile(gridYFile, Ylines);
    readGridFile(gridZFile, Zlines);

    if(!readGridFile(gridXFile, Xlines) ||
        !readGridFile(gridYFile, Ylines) ||
        !readGridFile(gridZFile, Zlines))
    {
        qDebug() << "Failed to load grid files!";
    }

    readColorFile(colorFile, meshcolor);
    readmaterial(materialFile, legendLabels, colors);

    if(!readColorFile(colorFile, meshcolor) ||
        !readmaterial(materialFile,legendLabels,colors))
    {
        qDebug() << "Failed to load color/material files!";
    }


    // 添加强制刷新
    this->show(); //确保窗口显示
    //update();    //触发首次绘制

    //qDebug() << "MeshShow initialized and updated";


    // 在构造函数末尾添加
    QTimer::singleShot(10, this, [this]() {
        // 延迟设置，确保所有UI都初始化完成
        setCursor(Qt::ArrowCursor);

        // 遍历所有子部件设置光标
        for (QObject* child : children()) {
            if (QWidget* widget = qobject_cast<QWidget*>(child)) {
                widget->setCursor(Qt::ArrowCursor);
            }
        }


    });

}

MeshShow::~MeshShow()
{
    // 不需要手动删除子部件，Qt 会自动处理
}

bool MeshShow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // // 确保光标是箭头
        // if (cursor().shape() != Qt::ArrowCursor) {
        //     setCursor(Qt::ArrowCursor);
        // }

        mouseMoveEvent(mouseEvent);
        return true;
    }
    return QObject::eventFilter(obj, event);


}

int MeshShow::findIndex(double locationp, const QVector<double>& XYZlines) {
    if (XYZlines.isEmpty()) return -1; // 处理空数组
    double air_offset;
    // 读取网格文件
    if(currentPlane=="yz"){
        air_offset =Xlines[2];
    }
    else if(currentPlane=="xz"){
        air_offset =Ylines[2];
    }
    else {
        air_offset =Zlines[2];
    }
    auto it = std::lower_bound(XYZlines.begin(), XYZlines.end(), locationp+air_offset);
    qDebug()<<"locationp+air_offset="<<locationp+air_offset;
    // 处理浮点数精度容差（例如 1e-6）
    // if (it != XYZlines.begin() && (it == XYZlines.end() || *it - locationp > 1e-6)) {
    //     --it;
    // }
    int index = it - XYZlines.begin() -1;
    return (index >= 0) ? index : 0;
}

bool MeshShow::readGridFile(const QString& filename, QVector<double>& lines)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filename;
        return false;
    }

    lines.clear();
    lines.append(0.0); // 起始位置为0
    QTextStream in(&file);


    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('\t');
        if (parts.size() < 2) continue;

        bool ok;
        double length = parts[1].toDouble(&ok);
        if (ok) {
            lines.append((lines.last() + length));

        }
    }

    file.close();

    // 在文件读取成功后更新尺寸
    if(!lines.isEmpty()){
        updateSizeLabels();
    }

    return true;
}

bool MeshShow::readColorFile(const QString& filename, QVector<QVector<QVector<int>>>& mesh3d)
{

    int gridXCount=Xlines.size();
    int gridYCount=Ylines.size();
    int gridZCount=Zlines.size();
    //qDebug()<<"x方向网格个数："<<gridXCount-1;
    //qDebug()<<"y方向网格个数："<<gridYCount-1;
    //qDebug()<<"z方向网格个数："<<gridZCount-1;

    // 添加数组大小验证
    if(gridXCount <= 0 || gridYCount <= 0 || gridZCount <= 0) {
        qWarning() << "Invalid grid dimensions";
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开颜色文件:" << filename;
        return false;
    }



    mesh3d.resize(gridXCount);
    for (int x = 0; x < gridXCount; ++x) {
        mesh3d[x].resize(gridYCount);
        for (int y = 0; y < gridYCount; ++y) {
            mesh3d[x][y].resize(gridZCount);
            mesh3d[x][y].fill(0); // 默认颜色为0
        }
    }

    // 按文件顺序填充数据（Z→Y→X顺序）
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('\t', Qt::SkipEmptyParts);
        if (parts.size() < 4) continue;

        bool ok;
        int fileX = parts[0].toInt(&ok);
        if (!ok || fileX < 0 || fileX >= gridXCount) continue;
        int fileY = parts[1].toInt(&ok);
        if (!ok || fileY < 0 || fileY >= gridYCount) continue;
        int fileZ = parts[2].toInt(&ok);
        if (!ok || fileZ < 0 || fileZ >= gridZCount) continue;
        int color = parts[3].toInt(&ok);
        if (!ok) continue;

        mesh3d[fileX][fileY][fileZ] = color;
        // if(fileX==5){
        //      qDebug() << "Meshcolor[" << fileX << "][" << fileY << "][" << fileZ << "]:"<< mesh3d[fileX][fileY][fileZ];
        // }


        existingMaterials.insert(color); // 记录存在的材料类型
    }

    //existingMaterials.clear(); // 清空旧数据





    return true;
}

bool MeshShow::readmaterial(const QString& filename, QStringList &materiallabels, QVector<QColor> &materialcolors)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open material file:" << filename;
        return false;
    }

    materiallabels.clear();
    materialcolors.clear();

    // 预定义前3种材料的颜色
    QVector<QColor> predefinedColors = {
        QColor(255, 255, 255),  // Air - 白色
        QColor(100, 214, 214),  // Substrate - 青色
        QColor(255, 255, 0)     // Metal - 黄色
    };

    QTextStream in(&file);
    int maxIndex = -1;



    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('\t', Qt::SkipEmptyParts);
        if (parts.size() < 2) {  // 现在只需要2列：索引和材料名
            qWarning() << "Invalid line format:" << line;
            continue;
        }

        bool ok;
        int index = parts[0].toInt(&ok);
        if (!ok || index < 0) {
            qWarning() << "Invalid material index:" << parts[0];
            continue;
        }

        // 扩展容器以容纳当前索引
        if (index >= materiallabels.size()) {
            materiallabels.resize(index + 1);
            materialcolors.resize(index + 1);
        }

        // 存储材料名称
        materiallabels[index] = parts[1];

        // 设置材料颜色
        if (index < predefinedColors.size()) {
            // 使用预定义颜色
            materialcolors[index] = predefinedColors[index];
        } else {
            // 生成不包含预定义颜色的随机颜色
            QColor randomColor;
            bool isUnique;
            int attempts = 0;
            const int maxAttempts = 100; // 防止无限循环

            do {
                // 生成随机颜色
                randomColor = QColor(
                    rand() % 256,  // R: 0-255
                    rand() % 256,  // G: 0-255
                    rand() % 256   // B: 0-255
                    );

                // 检查是否与预定义颜色相同
                isUnique = true;
                for (const QColor& color : predefinedColors) {
                    if (randomColor == color) {
                        isUnique = false;
                        break;
                    }
                }

                // 检查是否与其他已生成随机颜色相同
                if (isUnique) {
                    for (int i = predefinedColors.size(); i < materialcolors.size(); i++) {
                        if (materialcolors[i].isValid() && randomColor == materialcolors[i]) {
                            isUnique = false;
                            break;
                        }
                    }
                }

                attempts++;
            } while (!isUnique && attempts < maxAttempts);

            // 如果尝试多次仍然重复，使用默认颜色
            if (attempts >= maxAttempts) {
                randomColor = QColor(200, 200, 200); // 默认灰色
            }

            materialcolors[index] = randomColor;
        }

        // 更新最大索引
        if (index > maxIndex) maxIndex = index;
    }

    // 裁剪容器到实际需要的尺寸
    if (maxIndex >= 0) {
        materiallabels.resize(maxIndex + 1);
        materialcolors.resize(maxIndex + 1);
    }



    return true;
}

void MeshShow::onXYButtonClicked()
{
    currentPlane = "xy";
    layerSpinBox->setMaximum(Zlines.last()+1);
    update();
}

void MeshShow::onXZButtonClicked()
{
    currentPlane = "xz";
    layerSpinBox->setMaximum(Ylines.last()+1);
    update();
}

void MeshShow::onYZButtonClicked()
{
    currentPlane = "yz";
    layerSpinBox->setMaximum(Xlines.last()+1);
    update();
}

void MeshShow::updateSizeLabels()
{
    if(!Xlines.isEmpty()) {
        //double xSize = Xlines.last() - Xlines.first();
        xSize = Xlines[Xlines.size()-3] - Xlines[2];
        xSizeLabel->setText(QString("X: %1 mm").arg(xSize, 0, 'f', 2));
    }

    if(!Ylines.isEmpty()) {
        //double ySize = Ylines.last() - Ylines.first();
        ySize = Ylines[Ylines.size()-3] - Ylines[2];
        ySizeLabel->setText(QString("Y: %1 mm").arg(ySize, 0, 'f', 2));
    }

    if(!Zlines.isEmpty()) {
        //double zSize = Zlines.last() - Zlines.first();
        zSize = Zlines[Zlines.size()-3] - Zlines[2];
        zSizeLabel->setText(QString("Z: %1 mm").arg(zSize, 0, 'f', 2));
    }
}

void MeshShow::onUpdateClicked()
{
    locationP= layerSpinBox->value();
    scaleFactor = scaleSpinBox->value();

    // 读取网格文件
    if(currentPlane=="yz"){
        currentLayerIndex =findIndex(locationP,Xlines);
    }
    else if(currentPlane=="xz"){
        currentLayerIndex =findIndex(locationP,Ylines);
    }
    else {
        currentLayerIndex =findIndex(locationP,Zlines);
    }

    update();
}

void MeshShow::enableMouseEvents()
{
    mouseEventsEnabled = true;
    setMouseTracking(true);
}

void MeshShow::disableMouseEvents()
{
    mouseEventsEnabled = false;
    setMouseTracking(false);
    isDragging = false; // 重置拖拽状态
}


void MeshShow::mouseMoveEvent(QMouseEvent *event) {

    //qDebug() << "Mouse move event:" << event->pos();
    if (!mouseEventsEnabled) {
        QMainWindow::mouseMoveEvent(event);
        return;
    }

    if (isDragging) {
        QPoint delta = event->pos() - dragStartPos;
        totalOffset += delta;
        dragStartPos = event->pos();
        isViewOffset = true; // 标记视图已偏移
        update();
        currentCoordLabel->setText("current coordinates：-");
    }
    else{

        if(!isViewOffset){
            QPoint mousePos = event->pos();
            //qDebug()<<"scaleFactor:"<<scaleFactor;

            double mouseX = mousePos.x()/scaleFactor;
            double mouseY = mousePos.y()/scaleFactor;

            // qDebug()<<"mouseX:"<<mouseX;
            // qDebug()<<"mouseY:"<<mouseY;
            // qDebug()<<"--";

            double modelX = mouseX - xOffset / scaleFactor-Xlines[2];
            double modelY = (imageHeight /scaleFactor)-Ylines[1] - (mouseY - yOffset/scaleFactor)  -Ylines[1];
            // qDebug()<<"modelX:"<<modelX;
            // qDebug()<<"modelY:"<<modelY;



            // 根据当前平面调整坐标轴,需要对各个方向尺寸取整，以便显示完整边界
            //qRound（四舍五入）qCeil（向上取整）
            bool withinBounds = false;



            //尺寸很小时不用qRound取整，保证各种尺寸都能正常显示坐标
            if (currentPlane == "xz") {
                if(ySize<1){
                    withinBounds = (modelX >= Xlines[0] && modelX <=xSize) &&
                                   (modelY >= Zlines[0] && modelY <=zSize);
                }
                else{
                    withinBounds = (modelX >= Xlines[0] && modelX <=qRound(xSize)) &&
                                   (modelY >= Zlines[0] && modelY <=qRound(zSize));
                }

            } else if (currentPlane == "yz") {
                if(xSize<1){
                    withinBounds = (modelX >= Ylines[0] && modelX <=ySize) &&
                                   (modelY >= Zlines[0] && modelY <=zSize);
                }
                else{
                    withinBounds = (modelX >= Ylines[0] && modelX <=qRound(ySize)) &&
                                   (modelY >= Zlines[0] && modelY <=qRound(zSize));
                }
            }else{
                if(zSize<1){
                    withinBounds = (modelX >= Xlines[0] && modelX <=xSize) &&
                                   (modelY >= Ylines[0] && modelY <=ySize);
                }
                else{
                    withinBounds = (modelX >= Xlines[0] && modelX <=qRound(xSize)) &&
                                   (modelY >= Ylines[0] && modelY <=qRound(ySize));
                }

            }


            //qDebug()<<"withinBounds:"<<withinBounds;


            // 显示坐标
            if (withinBounds) {
                QString coordText;
                if (currentPlane == "xz") {
                    coordText = QString("current coordinates (X: %1, Z: %2)").arg(modelX, 0, 'f', 2).arg(modelY, 0, 'f', 2);
                } else if (currentPlane == "yz") {
                    coordText = QString("current coordinates (Y: %1, Z: %2)").arg(modelX, 0, 'f', 2).arg(modelY, 0, 'f', 2);
                }else{
                    coordText = QString("current coordinates (X: %1, Y: %2)").arg(modelX, 0, 'f', 2).arg(modelY, 0, 'f', 2);
                }
                currentCoordLabel->setText(coordText);
            } else {
                currentCoordLabel->setText("current coordinates-");
            }
        }
        else{
            currentCoordLabel->setText("current coordinates-");
        }

    }
    QMainWindow::mouseMoveEvent(event);
}

void MeshShow::onResetClicked()
{
    totalOffset = QPointF(0, 0);
    isViewOffset = false;
    //scaleFactor = 10.0;   // 重置缩放因子
    //scaleSpinBox->setValue(10.0);
    update();
}

void MeshShow::mousePressEvent(QMouseEvent* event)
{
    if (!mouseEventsEnabled) {
        QMainWindow::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        dragStartPos = event->pos();
        isDragging = true;
    }
    QMainWindow::mousePressEvent(event);
}

void MeshShow::mouseReleaseEvent(QMouseEvent* event)
{
    if (!mouseEventsEnabled) {
        QMainWindow::mouseMoveEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MeshShow::paintEvent(QPaintEvent *event)
{


    Q_UNUSED(event);
    QPainter painter(this);

    // // 读取网格文件E:\qtmesh\mesh1
    // readGridFile("E://qtmesh//mesh1//structureRegionGridsX.txt", Xlines);
    // readGridFile("E://qtmesh//mesh1//structureRegionGridsY.txt", Ylines);
    // readGridFile("E://qtmesh//mesh1//structureRegionGridsZ.txt", Zlines);

    // if(!readGridFile("E://qtmesh//mesh1//structureRegionGridsX.txt", Xlines) ||
    //     !readGridFile("E://qtmesh//mesh1//structureRegionGridsY.txt", Ylines) ||
    //     !readGridFile("E://qtmesh//mesh1//structureRegionGridsZ.txt", Zlines))
    // {
    //     qDebug() << "Failed to load grid files!";
    // }

    // //qDebug()<<"Xlines"<<Xlines;
    // //qDebug()<<"Ylines"<<Ylines;
    // //qDebug()<<"Zlines"<<Zlines;

    // // 读取颜色文件
    // readColorFile("E://qtmesh//mesh1//structureRegion_parameter.txt", meshcolor);
    // readmaterial("E://qtmesh//mesh1//material.txt",legendLabels,colors);

    // if(!readColorFile("E://qtmesh//mesh1//structureRegion_parameter.txt", meshcolor) ||
    //     !readmaterial("E://qtmesh//mesh1//material.txt",legendLabels,colors))
    // {
    //     qDebug() << "Failed to load color/material files!";
    // }

    // qDebug()<<"legendLabels:"<<legendLabels;

    // 定义竖直线
    QVector<double> verticalLines ;
    // 定义水平线
    QVector<double> horizontalLines ;


    // 读取网格文件
    if(currentPlane=="yz"){
        verticalLines=Ylines;
        horizontalLines=Zlines;
    }
    else if(currentPlane=="xz"){
        verticalLines=Xlines;
        horizontalLines=Zlines;
    }
    else {
        verticalLines=Xlines;
        horizontalLines=Ylines;
    }

    // 检查层索引是否有效
    int maxLayer = 0;
    if (currentPlane == "yz") maxLayer = Xlines.size() - 1;
    else if (currentPlane == "xz") maxLayer = Ylines.size() - 1;
    else maxLayer = Zlines.size() - 1;





    if (currentLayerIndex < 0 || currentLayerIndex > maxLayer-1) {
        currentLayerIndex = qBound(0, currentLayerIndex, maxLayer-1);
        double label_max;
        if(currentPlane=="yz"){
            label_max=Xlines[currentLayerIndex+1];
        }
        else if(currentPlane=="xz"){
            label_max=Ylines[currentLayerIndex+1];
        }
        else {
            label_max=Zlines[currentLayerIndex+1];
        }
        layerSpinBox->setValue(label_max);  // 自动修正输入值
    }





    // 动态缩放后的网格坐标
    QVector<double> scaledX = Xlines;
    QVector<double> scaledY = Ylines;
    QVector<double> scaledZ = Zlines;
    for (auto& val : scaledX) val *= scaleFactor;
    for (auto& val : scaledY) val *= scaleFactor;
    for (auto& val : scaledZ) val *= scaleFactor;


    // 应用缩放
    for(auto& x : verticalLines) x *= scaleFactor;
    for(auto& y : horizontalLines) y *= scaleFactor;

    //定义xy最大最小值
    int xmin=verticalLines[2];
    int xmax=verticalLines[verticalLines.size()-1-2];
    int ymin=horizontalLines[2];
    int ymax=horizontalLines[horizontalLines.size()-1-2];

    //定义原始xy最大最小值
    int xmin_ori=verticalLines[0];
    int xmax_ori=verticalLines[verticalLines.size()-1];
    int ymin_ori=horizontalLines[0];
    int ymax_ori=horizontalLines[horizontalLines.size()-1];

    // 计算图像绘制区域的宽度和高度
    int imageWidth = xmax_ori - xmin_ori;
    int imageHeight = ymax_ori - ymin_ori;

    // 获取窗口的宽度和高度
    double windowWidth = this->width();
    double windowHeight = this->height();

    // 计算水平和垂直方向的偏移量，使图像居中
    int xOffset = (windowWidth - imageWidth) / 2 - xmin_ori;
    int yOffset = (windowHeight - imageHeight) / 2 - ymin_ori;

    // // 计算原始和缩放后的图像尺寸
    // int imageWidth = xmax_ori - xmin_ori;
    // int imageHeight = ymax_ori - ymin_ori;
    // scaledImageWidth = imageWidth * scaleFactor;
    // scaledImageHeight = imageHeight * scaleFactor;

    // // 计算居中偏移量（考虑缩放）
    // double windowWidth = this->width();
    // double windowHeight = this->height();
    // xOffset = (windowWidth - scaledImageWidth) / 2 - xmin_ori * scaleFactor;
    // yOffset = (windowHeight - scaledImageHeight) / 2 - ymin_ori * scaleFactor;


    //添加实时坐标显示
    this->xOffset = (windowWidth - imageWidth) / 2 - xmin_ori;
    this->yOffset = (windowHeight - imageHeight) / 2 - ymin_ori;
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
    this->xmin_ori = xmin_ori;
    this->xmax_ori = xmax_ori;
    this->ymin_ori = ymin_ori;
    this->ymax_ori = ymax_ori;


    //图像移动添加部分
    // 应用总偏移
    double currentXOffset = xOffset + totalOffset.x();
    double currentYOffset = yOffset + totalOffset.y();


    // // 反转Y轴：平移并缩放
    // painter.translate(xOffset, yOffset + imageHeight); // 移动到左下角
    // painter.scale(1, -1); // Y轴向上增长

    // 反转Y轴：平移并缩放
    painter.translate(currentXOffset, currentYOffset + imageHeight); // 移动到左下角
    painter.scale(1, -1); // Y轴向上增长





    for (int i = 2; i < horizontalLines.size()-1-2 ; i++) {
        for (int j = 2; j < verticalLines.size()-1-2 ; j++) {
            int x = verticalLines[j];
            int y = horizontalLines[i];
            int width = verticalLines[j+1] - x;
            int height = horizontalLines[i+1] - y;
            int colorNum;
            if(currentPlane=="yz"){
                colorNum = meshcolor[currentLayerIndex][j][i];
            }
            else if(currentPlane=="xz"){
                colorNum = meshcolor[j][currentLayerIndex][i];
            }
            else {
                colorNum = meshcolor[j][i][currentLayerIndex];
            }


            painter.fillRect(x, y, width, height, colors[colorNum]);
        }
    }


    QPen pen(Qt::black);
    pen.setWidth(0);  // 关键：保持1物理像素宽度
    painter.setPen(pen);

    // 绘制竖直线
    painter.setPen(Qt::black);
    // for (int x : verticalLines) {
    //     painter.drawLine(x, ymin, x, ymax);
    // }
    for(int i=2;i<verticalLines.size()-2;i++){
        painter.drawLine(verticalLines[i], ymin, verticalLines[i], ymax);
    }

    // 绘制水平线
    // for (int y : horizontalLines) {
    //     painter.drawLine(xmin, y, xmax, y);
    // }
    for(int j=2;j<horizontalLines.size()-2;j++){
        painter.drawLine(xmin, horizontalLines[j], xmax, horizontalLines[j]);
    }


    // 添加图例部分
    // 保存绘图状态
    painter.save();
    // 重置所有变换
    painter.resetTransform();





    // 生成有效图例项
    QVector<QColor> legend_Colors;
    QStringList legend_Labels;




    for(int i = 0; i < colors.size(); ++i) {
        if(existingMaterials.contains(i)) { // 只包含存在的材料
            legend_Colors.append(colors[i]);
            legend_Labels.append(legendLabels[i]);
        }
    }
    // 设置图例参数
    const int legendWidth = 120;        // 图例宽度固定
    const int blockSize = 20;           // 颜色方块大小
    const int margin = 10;              // 窗口边距
    const int textOffset = 10;          // 文字偏移量
    const int topBottomMargin = 15;     // 图例上下边距
    const int itemSpacing = 15;         // 图例项间距

    // // 计算动态高度（根据颜色数量）
    // int itemCount = colors.size();
    // int legendHeight = topBottomMargin * 2 +
    //                    (blockSize + itemSpacing) * itemCount -
    //                    itemSpacing;  // 最后一项不需要间距

    // // 计算图例位置（右侧居中）
    // int legendX = width() - legendWidth - margin;
    // int legendY = (height() - legendHeight) / 2;
    // 计算动态高度
    int itemCount = legend_Colors.size();
    int legendHeight = topBottomMargin * 2 +
                       (blockSize + itemSpacing) * itemCount -
                       itemSpacing;

    // 计算图例位置（右侧居中）
    int legendX = width() - legendWidth - margin;
    int legendY = qMax(0, (height() - legendHeight) / 2);

    // 绘制图例背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 200)); // 半透明白色背景
    painter.drawRoundedRect(legendX, legendY, legendWidth, legendHeight, 5, 5);

    // 绘制图例内容
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10));

    // for (int i = 0; i < colors.size(); ++i) {
    //     int itemY = legendY + 15 + i * (blockSize + 15);

    //     // 绘制颜色方块
    //     painter.setBrush(colors[i]);
    //     painter.drawRect(legendX + 15, itemY, blockSize, blockSize);

    //     // 绘制文字
    //     painter.drawText(legendX + 15 + blockSize + textOffset,
    //                      itemY + blockSize/2 + 5,
    //                      legendLabels[i]);
    // }
    for (int i = 0; i < itemCount; ++i) {
        int itemY = legendY + 15 + i * (blockSize + 15);

        // 绘制颜色方块
        painter.setBrush(colors[i]);
        painter.drawRect(legendX + 15, itemY, blockSize, blockSize);

        // 绘制文字
        painter.drawText(legendX + 15 + blockSize + textOffset,
                         itemY + blockSize/2 + 5,
                         legend_Labels[i]);
    }

    // 恢复绘图状态
    painter.restore();


    // 绘制坐标轴
    painter.save();
    painter.resetTransform();

    // 坐标轴参数
    const int axisLength = 40;        // 箭头长度
    const int arrowSize = 4;          // 箭头头部大小
    //const int margin = 20;            // 边距
    QColor axisColor(Qt::black);      // 轴线颜色

    // 确定左下角起始点
    QPoint startPoint(margin, height() - margin);

    // 根据当前平面确定轴标签
    QString axis1Label, axis2Label;
    QPoint axis1End, axis2End;

    if(currentPlane == "yz") {
        axis1Label = "Y";
        axis2Label = "Z";
        axis1End = startPoint + QPoint(axisLength, 0);
        axis2End = startPoint + QPoint(0, -axisLength);
    } else if(currentPlane == "xz") {
        axis1Label = "X";
        axis2Label = "Z";
        axis1End = startPoint + QPoint(axisLength, 0);
        axis2End = startPoint + QPoint(0, -axisLength);
    } else { // yz
        axis1Label = "X";
        axis2Label = "Y";
        axis1End = startPoint + QPoint(axisLength, 0);
        axis2End = startPoint + QPoint(0, -axisLength);
    }

    // 绘制坐标轴
    painter.setPen(QPen(axisColor, 2));

    // 绘制第一个轴（水平）
    painter.drawLine(startPoint, axis1End);
    // 绘制箭头
    QPolygon arrowHead1;
    arrowHead1 << axis1End
               << axis1End + QPoint(-arrowSize, arrowSize/2)
               << axis1End + QPoint(-arrowSize, -arrowSize/2);
    painter.drawPolygon(arrowHead1);

    // 绘制第二个轴（垂直）
    painter.drawLine(startPoint, axis2End);
    QPolygon arrowHead2;
    arrowHead2 << axis2End
               << axis2End + QPoint(-arrowSize/2, arrowSize)
               << axis2End + QPoint(arrowSize/2, arrowSize);
    painter.drawPolygon(arrowHead2);

    // 绘制轴标签
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(axis1End + QPoint(5, 5), axis1Label);
    painter.drawText(axis2End + QPoint(5, -5), axis2Label);

    painter.restore();




}


