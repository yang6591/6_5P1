#include "simulationmeshstruc.h"
#include "global.h"

#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QCoreApplication>
#include <QDir>





simulationmeshstruc::simulationmeshstruc(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
}

void simulationmeshstruc::setupUI()
{
    setWindowTitle(tr("Mesh Structure"));

    // 创建输入栏并设置更长的宽度
    meshLineEdit1 = new QLineEdit;
    meshLineEdit1->setReadOnly(false);
    meshLineEdit1->setPlaceholderText("connect-structure-distance");
    meshLineEdit1->setMinimumWidth(200);

    meshLineEdit2 = new QLineEdit;
    meshLineEdit2->setReadOnly(false);
    meshLineEdit2->setPlaceholderText("cpml-structure-distance");
    meshLineEdit2->setMinimumWidth(200);

    meshLineEdit3 = new QLineEdit;
    meshLineEdit3->setReadOnly(false);
    meshLineEdit3->setPlaceholderText("X-cpml");
    meshLineEdit3->setMinimumWidth(200);

    meshLineEdit4 = new QLineEdit;
    meshLineEdit4->setReadOnly(false);
    meshLineEdit4->setPlaceholderText("Y-cpml");
    meshLineEdit4->setMinimumWidth(200);

    meshLineEdit5 = new QLineEdit;
    meshLineEdit5->setReadOnly(false);
    meshLineEdit5->setPlaceholderText("Z-cpml");
    meshLineEdit5->setMinimumWidth(200);

    meshLineEdit6 = new QLineEdit;
    meshLineEdit6->setReadOnly(false);
    meshLineEdit6->setPlaceholderText("cpml-extra-distance");
    meshLineEdit6->setMinimumWidth(200);

    meshLineEdit7 = new QLineEdit;
    meshLineEdit7->setReadOnly(false);
    meshLineEdit7->setPlaceholderText("X-start-position");
    meshLineEdit7->setMinimumWidth(200);

    meshLineEdit8 = new QLineEdit;
    meshLineEdit8->setReadOnly(false);
    meshLineEdit8->setPlaceholderText("X-end-position");
    meshLineEdit8->setMinimumWidth(200);

    meshLineEdit9 = new QLineEdit;
    meshLineEdit9->setReadOnly(false);
    meshLineEdit9->setPlaceholderText("Y-start-position");
    meshLineEdit9->setMinimumWidth(200);

    meshLineEdit10 = new QLineEdit;
    meshLineEdit10->setReadOnly(false);
    meshLineEdit10->setPlaceholderText("Y-end-position");
    meshLineEdit10->setMinimumWidth(200);

    meshLineEdit11 = new QLineEdit;
    meshLineEdit11->setReadOnly(false);
    meshLineEdit11->setPlaceholderText("Z-start-position");
    meshLineEdit11->setMinimumWidth(200);

    meshLineEdit12 = new QLineEdit;
    meshLineEdit12->setReadOnly(false);
    meshLineEdit12->setPlaceholderText("Z-end-position");
    meshLineEdit12->setMinimumWidth(200);

    // 创建"边界网格"组框
    QGroupBox* boundaryMeshGroupBox = new QGroupBox("");

    // 创建边界网格的网格布局
    QGridLayout* boundaryGridLayout = new QGridLayout;

    // 将前六个输入框添加到边界网格组框中
    boundaryGridLayout->addWidget(new QLabel("connectDistance:"), 0, 0);
    boundaryGridLayout->addWidget(meshLineEdit1, 0, 1);
    boundaryGridLayout->addWidget(new QLabel("cpmlDistance:"), 1, 0);
    boundaryGridLayout->addWidget(meshLineEdit2, 1, 1);
    boundaryGridLayout->addWidget(new QLabel("cpmlWidth_x:"), 2, 0);
    boundaryGridLayout->addWidget(meshLineEdit3, 2, 1);
    boundaryGridLayout->addWidget(new QLabel("cpmlWidth_y:"), 3, 0);
    boundaryGridLayout->addWidget(meshLineEdit4, 3, 1);
    boundaryGridLayout->addWidget(new QLabel("cpmlWidth_z:"), 4, 0);
    boundaryGridLayout->addWidget(meshLineEdit5, 4, 1);
    boundaryGridLayout->addWidget(new QLabel("extraDistance:"), 5, 0);
    boundaryGridLayout->addWidget(meshLineEdit6, 5, 1);

    // 设置边界网格组框的布局
    boundaryMeshGroupBox->setLayout(boundaryGridLayout);

    // // 创建"建模区域网格"组框
    // QGroupBox* modelingAreaMeshGroupBox = new QGroupBox("建模区域网格");

    // // 创建建模区域网格的网格布局
    // QGridLayout* modelingAreaGridLayout = new QGridLayout;

    // // 将后六个输入框添加到建模区域网格组框中
    // modelingAreaGridLayout->addWidget(new QLabel("Ismin:"), 0, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit7, 0, 1);
    // modelingAreaGridLayout->addWidget(new QLabel("Ismax:"), 1, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit8, 1, 1);
    // modelingAreaGridLayout->addWidget(new QLabel("Jsmin:"), 2, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit9, 2, 1);
    // modelingAreaGridLayout->addWidget(new QLabel("Jsmax:"), 3, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit10, 3, 1);
    // modelingAreaGridLayout->addWidget(new QLabel("Ksmin:"), 4, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit11, 4, 1);
    // modelingAreaGridLayout->addWidget(new QLabel("Ksmax:"), 5, 0);
    // modelingAreaGridLayout->addWidget(meshLineEdit12, 5, 1);

    // 设置建模区域网格组框的布局
    //modelingAreaMeshGroupBox->setLayout(modelingAreaGridLayout);

    // 创建按钮
    meshBtnOk = new QPushButton("OK");
    meshBtnCancel = new QPushButton("Cancel");

    // 创建布局
    mainLayout = new QVBoxLayout(this);
    inputLayout = new QHBoxLayout;
    buttonLayout = new QHBoxLayout;

    // 按钮布局
    buttonLayout->addStretch();
    buttonLayout->addWidget(meshBtnOk);
    buttonLayout->addWidget(meshBtnCancel);

    // 将两个组框添加到输入布局
    inputLayout->addWidget(boundaryMeshGroupBox);
    //inputLayout->addSpacing(30); // 增加间距
    //inputLayout->addWidget(modelingAreaMeshGroupBox);

    // 主布局
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);

    // 设置默认值（可选）
    meshLineEdit1->setText("15");
    meshLineEdit2->setText("30");
    meshLineEdit3->setText("10");
    meshLineEdit4->setText("10");
    meshLineEdit5->setText("10");
    meshLineEdit6->setText("5");
    // meshLineEdit7->setText("0");
    // meshLineEdit8->setText("0");
    // meshLineEdit9->setText("0");
    // meshLineEdit10->setText("0");
    // meshLineEdit11->setText("0");
    // meshLineEdit12->setText("0");
}



void simulationmeshstruc::setupConnections()
{
    connect(meshBtnOk, &QPushButton::clicked, this, [=](){
        // 获取所有输入框的值
        // QString connectDistance = meshLineEdit1->text();
        // QString cpmlDistance = meshLineEdit2->text();
        // QString cpmlWidth_x = meshLineEdit3->text();
        // QString cpmlWidth_y = meshLineEdit4->text();
        // QString cpmlWidth_z = meshLineEdit5->text();
        // QString extraDistance = meshLineEdit6->text();
        // QString Ismin = meshLineEdit7->text();
        // QString Ismax = meshLineEdit8->text();
        // QString Jsmin = meshLineEdit9->text();
        // QString Jsmax = meshLineEdit10->text();
        // QString Ksmin = meshLineEdit11->text();
        // QString Ksmax = meshLineEdit12->text();

        ::connectDistance = meshLineEdit1->text().toInt();
        ::cpmlDistance = meshLineEdit2->text().toInt();
        ::cpmlWidth_x = meshLineEdit3->text().toInt();
        ::cpmlWidth_y = meshLineEdit4->text().toInt();
        ::cpmlWidth_z = meshLineEdit5->text().toInt();
        ::extraDistance = meshLineEdit6->text().toInt();

        // qDebug() << "输入的网格结构信息已保存:";
        // qDebug() << "连接边界距离:" << connectDistance;
        // qDebug() << "吸收边界距离:" << cpmlDistance;
        // qDebug() << "X方向厚度:" << cpmlWidth_x;
        // qDebug() << "Y方向厚度:" << cpmlWidth_y;
        // qDebug() << "Z方向厚度:" << cpmlWidth_z;
        // qDebug() << "外推边界距离:" << extraDistance;
        // qDebug() << "Ismin:" << Ismin;
        // qDebug() << "Ismax:" << Ismax;
        // qDebug() << "Jsmin:" << Jsmin;
        // qDebug() << "Jsmax:" << Jsmax;
        // qDebug() << "Ksmin:" << Ksmin;
        // qDebug() << "Ksmax:" << Ksmax;

        qDebug() << "输入的网格结构信息已保存:";
        qDebug() << "连接边界距离:" << ::connectDistance;
        qDebug() << "吸收边界距离:" << ::cpmlDistance;
        qDebug() << "X方向厚度:" << ::cpmlWidth_x;
        qDebug() << "Y方向厚度:" << ::cpmlWidth_y;
        qDebug() << "Z方向厚度:" << ::cpmlWidth_z;
        qDebug() << "外推边界距离:" << ::extraDistance;

        // 生成meshstructure.txt文件
        // 获取应用程序目录路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString fileName = QDir::cleanPath(appDir + "/meshstructure.txt");

        // 生成meshstructure.txt文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写入文件头
            out << "====================================================================================\n";
            //out << "connectDistance\tcpmlDistance\tcpmlWidth_x\tcpmlWidth_y\tcpmlWidth_z\textraDistance\tIsmin\tIsmax\tJsmin\tJsmax\tKsmin\tKsmax\n";
            out << "connectDistance\tcpmlDistance\tcpmlWidth_x\tcpmlWidth_y\tcpmlWidth_z\textraDistance\n";
            out << "====================================================================================\n";

            // 写入数据行
            out << ::connectDistance << "\t"
                << ::cpmlDistance << "\t"
                << ::cpmlWidth_x << "\t"
                << ::cpmlWidth_y << "\t"
                << ::cpmlWidth_z << "\t"
                << ::extraDistance << "\t";
                // << Ismin << "\t"
                // << Ismax << "\t"
                // << Jsmin << "\t"
                // << Jsmax << "\t"
                // << Ksmin << "\t"
                // << Ksmax << "\n";

            file.close();
            qDebug() << "meshstructure.txt文件已生成";
        } else {
            qDebug() << "无法创建meshstructure.txt文件";
        }

        close();
    });

    connect(meshBtnCancel, &QPushButton::clicked, this, &simulationmeshstruc::close);
}
