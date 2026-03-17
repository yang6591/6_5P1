#include "simulationprobe.h"
#include "mainwindow.h"
#include "global.h"
#include "meshtrans.h"

#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QPair>

QString nearfieldprobe_EH = "E";  // 默认值
QString nearfieldprobe_direction = "x";  // 默认值
int nearfieldprobe_x = 0;
int nearfieldprobe_y = 0;
int nearfieldprobe_z = 0;



simulationprobe::simulationprobe(Probe_data* Probe_data1, QWidget* parent)
    : QDialog(parent)
{
    // 准备默认值
    Probe_data& Probe_default = Probe_data1 == nullptr ? *create_Prodefault() : *Probe_data1;
    // 命名
    setWindowTitle(tr("Probe"));

    // 创建对话框
    // 创建name项
    proLabel_0 = new QLabel("name:");
    proLineEdit_0 = new QLineEdit;
    // 将结构体内的probe_name数组转化为QString类型，并显示出来
    QString Show_proname = QString(Probe_default.probe_name.data(), Probe_default.probe_name.size());
    qDebug() << Show_proname;
    proLineEdit_0->setText(Show_proname);
    // 对name布局  QVBoxLayout????
    QVBoxLayout* Pro_name_Layout = new QVBoxLayout;
    Pro_name_Layout->addWidget(proLabel_0);
    Pro_name_Layout->addWidget(proLineEdit_0);

    // 创建field项
    QLabel* proLabel_1 = new QLabel("Field: ");
    Field_comBox_0 = new QComboBox;
    QString Field_Optiong_0 = "E-field";
    QString Field_Optiong_1 = "H-field";
    QString Field_Optiong_2 = "E-field(Farfield)";
    QString Field_Optiong_3 = "H-field(Farfield)";
    QString Field_Optiong_4 = "RCS";
    Field_comBox_0->addItem(Field_Optiong_0);
    Field_comBox_0->addItem(Field_Optiong_1);
    Field_comBox_0->addItem(Field_Optiong_2);
    Field_comBox_0->addItem(Field_Optiong_3);
    Field_comBox_0->addItem(Field_Optiong_4);
    // 设置当前选项
    Field_comBox_0->setCurrentIndex(Probe_default.field_index);
    // 布局下拉菜单
    QVBoxLayout* Pro_Fild_Layout = new QVBoxLayout;
    Pro_Fild_Layout->addWidget(proLabel_1);
    Pro_Fild_Layout->addWidget(Field_comBox_0);
    // 信号：点击菜单选项，判断是否调用coor-system，并且获得field的当前序号
    // connect(Field_comBox_0,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&simulationprobe::Fiel_stateChanged);//子项发生变化才会触发函数，所以使用activated

    connect(Field_comBox_0, QOverload<int>::of(&QComboBox::activated), this, &simulationprobe::Fiel_stateChanged);
    // 将name和Field放在一个组内
    QGridLayout* Pro_Layout_0 = new QGridLayout;
    Pro_Layout_0->addLayout(Pro_name_Layout, 0, 0);
    Pro_Layout_0->addLayout(Pro_Fild_Layout, 1, 0);

    // 设置Coordinate system
    QLabel* proLabel_2 = new QLabel("Coordinate system: ");
    Co_Stm_0 = new QComboBox;
    QString CoStm_Option_0 = "Cartesian";
    QString CoStm_Option_1 = "Spherical";
    Co_Stm_0->addItem(CoStm_Option_0);
    Co_Stm_0->addItem(CoStm_Option_1);
    if (Probe_default.field_index == 0 || Probe_default.field_index == 1) {
        Co_Stm_0->setEnabled(false);
    } else {
        Co_Stm_0->setEnabled(true);
    }

    // 设置Coordinate system当前显示
    Co_Stm_0->setCurrentIndex(Probe_default.system_index);
    // 信号：点击coor-system，获取当前的system序号
    connect(Co_Stm_0, QOverload<int>::of(&QComboBox::activated), this, &simulationprobe::Coorstm_state_index);
    //    //获取当前coordinate system的索引号
    //    int coor_name_index = Co_Stm_0 ->currentIndex();

    // 创建空白项
    QLabel* system_void_label = new QLabel;
    QLabel* system_void_label_1 = new QLabel;
    QVBoxLayout* Pro_system_void = new QVBoxLayout;
    Pro_system_void->addWidget(system_void_label);
    Pro_system_void->addWidget(system_void_label_1);

    QVBoxLayout* Pro_Cost_Layout = new QVBoxLayout;
    Pro_Cost_Layout->addWidget(proLabel_2);
    Pro_Cost_Layout->addWidget(Co_Stm_0);

    QGridLayout* Pro_Layout_1 = new QGridLayout;
    Pro_Layout_1->addLayout(Pro_system_void, 0, 0);
    Pro_Layout_1->addLayout(Pro_Cost_Layout, 1, 0);

    // orientation框
    QGroupBox* Orien_grpBox = new QGroupBox("Orientation");

    // QStackedWidget
    QStackedWidget* Orien_stackWdg = new QStackedWidget;
    // 创建第一个：cart窗口
    QWidget* Widget_cart = new QWidget;
    QRadioButton* Orien_RdiBtn_X = new QRadioButton("X");
    QRadioButton* Orien_RdiBtn_Y = new QRadioButton("Y");
    QRadioButton* Orien_RdiBtn_Z = new QRadioButton("Z");
    QRadioButton* Orien_RdiBtn_All = new QRadioButton("All(X,Y,Z,Abs)");
    // Orien_RdiBtn_X -> setChecked(true);
    Car_Btngroup = new QButtonGroup;
    Car_Btngroup->addButton(Orien_RdiBtn_X, 0);
    Car_Btngroup->addButton(Orien_RdiBtn_Y, 1);
    Car_Btngroup->addButton(Orien_RdiBtn_Z, 2);
    Car_Btngroup->addButton(Orien_RdiBtn_All, 3);
   //connect(Car_Btngroup, &QButtonGroup::buttonClicked, this, &simulationprobe::Postion_state_index);
   connect(Car_Btngroup, SIGNAL(buttonClicked(int)), Car_Btngroup , SLOT(Postion_state_index(int))); //台式机
    qDebug() << "Ori_ca" << Probe_default.Ori_ca;
    if (Probe_default.Ori_ca == 0) {
        Orien_RdiBtn_X->setChecked(true);
        qDebug() << "运行到这了";
    }
    switch (Probe_default.Ori_ca) {
    case 0:
        Orien_RdiBtn_X->setChecked(true);
        break;
    case 1:
        Orien_RdiBtn_Y->setChecked(true);
        break;
    case 2:
        Orien_RdiBtn_Z->setChecked(true);
        break;
    case 3:
        Orien_RdiBtn_All->setChecked(true);
        break;
    default:
        break;
    }

    QGridLayout* Pro_Orint_Layout_car = new QGridLayout;
    Pro_Orint_Layout_car->addWidget(Orien_RdiBtn_X);
    Pro_Orint_Layout_car->addWidget(Orien_RdiBtn_Y);
    Pro_Orint_Layout_car->addWidget(Orien_RdiBtn_Z);
    Pro_Orint_Layout_car->addWidget(Orien_RdiBtn_All);
    Widget_cart->setLayout(Pro_Orint_Layout_car);

    // 创建第2个 Spher窗口
    QWidget* Widget_spher = new QWidget;
    QRadioButton* Orien_RdiBtn_Th = new QRadioButton("Theta");
    QRadioButton* Orien_RdiBtn_Ph = new QRadioButton("Phi");
    // QRadioButton* Orien_RdiBtn_Radio = new QRadioButton("Radio");
    QRadioButton* Orien_RdiBtn_AEA = new QRadioButton("All(Alpha,Eps.,Abs)");
    Sph_Btngroup = new QButtonGroup;
    Sph_Btngroup->addButton(Orien_RdiBtn_Th, 0);
    Sph_Btngroup->addButton(Orien_RdiBtn_Ph, 1);
    Sph_Btngroup->addButton(Orien_RdiBtn_AEA, 2);
    // connect(Sph_Btngroup, &QButtonGroup::buttonClicked, this, &simulationprobe::Postion_state_index);//笔记本
     connect(Sph_Btngroup, SIGNAL(buttonClicked(int)), Sph_Btngroup, SLOT(Postion_state_index(int))); //台式机
    switch (Probe_default.Ori_sp) {
    case 0:
        Orien_RdiBtn_Th->setChecked(true);
        break;
    case 1:
        Orien_RdiBtn_Ph->setChecked(true);
        break;
    case 2:
        Orien_RdiBtn_AEA->setChecked(true);
        break;
    default:
        break;
    }

    QGridLayout* Pro_Orint_Layout_sph = new QGridLayout;
    Pro_Orint_Layout_sph->addWidget(Orien_RdiBtn_Th);
    Pro_Orint_Layout_sph->addWidget(Orien_RdiBtn_Ph);
    // Pro_Orint_Layout_sph -> addWidget(Orien_RdiBtn_Radio);
    Pro_Orint_Layout_sph->addWidget(Orien_RdiBtn_AEA);
    Widget_spher->setLayout(Pro_Orint_Layout_sph);

    // 将car和sph窗口放到StackedWidget中
    Orien_stackWdg->addWidget(Widget_cart);
    Orien_stackWdg->addWidget(Widget_spher);

    QVBoxLayout* Orien_Vlayout = new QVBoxLayout;
    Orien_Vlayout->addWidget(Orien_stackWdg);
    // Orien_Vlayout -> addWidget(Btn_prc_0);
    // Orien_Vlayout -> addWidget(Btn_prc_1);
    Orien_grpBox->setLayout(Orien_Vlayout);

    // Position
    QGroupBox* Pos_grpBox = new QGroupBox("Position");//定义一个框

    // QStackedWidget
    QStackedWidget* Orien_posWdg = new QStackedWidget;//定义一个折叠窗口

    // 创建第1个 Pos-car窗口
    QWidget* Widget_pos_car = new QWidget;
    QLabel* proLabel_3 = new QLabel("X:     ");
    QLabel* proLabel_4 = new QLabel("Y:     ");
    QLabel* proLabel_5 = new QLabel("Z:     ");
    proLineEdit_1 = new QLineEdit;
    proLineEdit_2 = new QLineEdit;
    proLineEdit_3 = new QLineEdit;
    proLineEdit_1->setText(QString("%1").arg(Probe_default.Pos_car[0]));
    proLineEdit_2->setText(QString("%1").arg(Probe_default.Pos_car[1]));
    proLineEdit_3->setText(QString("%1").arg(Probe_default.Pos_car[2]));
    //值改动的时候--修改名字
   // int change_number = Co_Stm_0->currentIndex();
    if(Co_Stm_0->currentIndex() == 0){
        connect(proLineEdit_1,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);
        connect(proLineEdit_2,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);
        connect(proLineEdit_3,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);
    }
    // 布局
    QHBoxLayout* Pro_PosLay_X = new QHBoxLayout;
    QHBoxLayout* Pro_PosLay_Y = new QHBoxLayout;
    QHBoxLayout* Pro_PosLay_Z = new QHBoxLayout;
    Pro_PosLay_X->addWidget(proLabel_3);//把标签和输入框编入一组，水平布局
    Pro_PosLay_X->addWidget(proLineEdit_1);
    Pro_PosLay_Y->addWidget(proLabel_4);
    Pro_PosLay_Y->addWidget(proLineEdit_2);
    Pro_PosLay_Z->addWidget(proLabel_5);
    Pro_PosLay_Z->addWidget(proLineEdit_3);

    QGridLayout* Pos_Grid_Layout = new QGridLayout;//三组之间位置布局
    Pos_Grid_Layout->addLayout(Pro_PosLay_X, 0, 0);
    Pos_Grid_Layout->addLayout(Pro_PosLay_Y, 1, 0);
    Pos_Grid_Layout->addLayout(Pro_PosLay_Z, 2, 0);

    Widget_pos_car->setLayout(Pos_Grid_Layout);//对QWidget* Widget_pos_car窗口设置做好的布局

    // 创建第2个 Pos-sph窗口
    QWidget* Widget_pos_sph = new QWidget;
    QLabel* proLabel_6 = new QLabel("Theta  :");
    QLabel* proLabel_7 = new QLabel("Phi      :");
    QLabel* proLabel_8 = new QLabel("Radius:");
    proLineEdit_4 = new QLineEdit;
    proLineEdit_5 = new QLineEdit;
    proLineEdit_6 = new QLineEdit;
    proLineEdit_4->setText(QString("%1").arg(Probe_default.Pos_sph[0]));
    proLineEdit_5->setText(QString("%1").arg(Probe_default.Pos_sph[1]));
    proLineEdit_6->setText(QString("%1").arg(Probe_default.Pos_sph[2]));

    connect(proLineEdit_4,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);
    connect(proLineEdit_5,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);
    connect(proLineEdit_6,&QLineEdit::textChanged,this,&simulationprobe::Postion_state_index);

    // 布局
    QHBoxLayout* Pro_sphLay_Th = new QHBoxLayout;
    QHBoxLayout* Pro_sphLay_Ph = new QHBoxLayout;
    QHBoxLayout* Pro_sphLay_Ra = new QHBoxLayout;
    Pro_sphLay_Th->addWidget(proLabel_6);
    Pro_sphLay_Th->addWidget(proLineEdit_4);
    Pro_sphLay_Ph->addWidget(proLabel_7);
    Pro_sphLay_Ph->addWidget(proLineEdit_5);
    Pro_sphLay_Ra->addWidget(proLabel_8);
    Pro_sphLay_Ra->addWidget(proLineEdit_6);

    QGridLayout* Pos_sph_layout = new QGridLayout;
    Pos_sph_layout->addLayout(Pro_sphLay_Th, 0, 0);
    Pos_sph_layout->addLayout(Pro_sphLay_Ph, 1, 0);
    Pos_sph_layout->addLayout(Pro_sphLay_Ra, 2, 0);

    Widget_pos_sph->setLayout(Pos_sph_layout);

    Orien_posWdg->addWidget(Widget_pos_car);//把两个单个窗口添加到Orien_posWdg组成折叠窗口
    Orien_posWdg->addWidget(Widget_pos_sph);

    //通过
    //Probe_default.system_index来判断该显示哪一个QStackWidget
    if (Probe_default.system_index == 0) {
        Orien_stackWdg->setCurrentIndex(0);
        Orien_posWdg->setCurrentIndex(0);
    } else {
        Orien_stackWdg->setCurrentIndex(1);
        Orien_posWdg->setCurrentIndex(1);
    }

/*             *********************************************            */


   // 通过co-system来控制Orientation和position的坐标显示
    connect(Co_Stm_0, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int CoStm_ComBox_index) {
        // qDebug()<<"CoStm_ComBox_index" <<CoStm_ComBox_index;//0是直角坐标系 1是球坐标系
        if (CoStm_ComBox_index == 0) {
            Orien_stackWdg->setCurrentIndex(0);
            Orien_posWdg->setCurrentIndex(0);
        } else {
            Orien_stackWdg->setCurrentIndex(1);
            Orien_posWdg->setCurrentIndex(1);
        }
    });
    QVBoxLayout* Pos_layout = new QVBoxLayout;
    Pos_layout->addWidget(Orien_posWdg);//折叠窗口添加到布局变量

    Pos_grpBox->setLayout(Pos_layout);//为定义的框添加整体布局

    QPushButton* Pro_Btn_OK = new QPushButton("OK");
    QPushButton* Pro_Btn_No = new QPushButton("cancel");
    QHBoxLayout* Btn_Layout = new QHBoxLayout;
    Btn_Layout->addWidget(Pro_Btn_OK);
    Btn_Layout->addWidget(Pro_Btn_No);

    // 总体布局
    QGridLayout* Pro_total_layout = new QGridLayout(this);//控件->setLayout；布局->addLayout/addWidget
    Pro_total_layout -> addLayout(Pro_name_Layout,0,0);
    Pro_total_layout->addLayout(Pro_Layout_0, 0, 0);
    Pro_total_layout->addLayout(Pro_Layout_1, 0, 1);
    Pro_total_layout->addWidget(Orien_grpBox, 1, 0);
    Pro_total_layout->addWidget(Pos_grpBox, 1, 1);
    Pro_total_layout->addLayout(Btn_Layout, 2, 1);

    // 调用函数修改名字
    // 目前的想法是 名字 = field + ori + pos 其中ori是分1.直角坐标系 2.球坐标系这两种情况的
    // fiel有5个分类
    //  E-field  H-field  E-field(Fairfield)  H-field(Fairfield)  RCS
    // char field_name[5]
    // field_name
    field_name << "E-field"
               << "H-field"
               << "E-field(Fairfield)"
               << "H-field(Fairfield)"
               << "RCS";
    // orientation_name
    orien_name << " X"
               << " Y"
               << " Z"
               << " Car"
               << " Th"
               << " Ph"
               << " Sph";
    // 分隔符
    symbol_name << " ("
                << ","
                << ")";



    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();
    // 构建文件路径
    QString gridXFile = QDir::cleanPath(appDir + "/structureRegionGridsX.txt");
    QString gridYFile = QDir::cleanPath(appDir + "/structureRegionGridsY.txt");
    QString gridZFile = QDir::cleanPath(appDir + "/structureRegionGridsZ.txt");
    readGridFile(gridXFile, Xlines);
    readGridFile(gridYFile, Ylines);
    readGridFile(gridZFile, Zlines);



    connect(Pro_Btn_OK, &QPushButton::clicked, this, [=](){
        // 获取场类型索引
        int fieldIndex = Field_comBox_0->currentIndex();

        // 根据场类型决定处理方式
        if (fieldIndex == 0 || fieldIndex == 1) {
            // 近场情况：E-field 或 H-field
            handleNearFieldProbe();
        } else {
            // 远场情况：E-field(Farfield)、H-field(Farfield) 或 RCS
            handleFarFieldObservationPoint();
        }

        close();
    });

    // connect(Pro_Btn_OK, &QPushButton::clicked, this, [=](){
    //     // 存储场类型 (E或H)
    //     int fieldIndex = Field_comBox_0->currentIndex();
    //     if (fieldIndex == 0) {
    //         ::nearfieldprobe_EH = "E";
    //     } else if (fieldIndex == 1) {
    //         ::nearfieldprobe_EH = "H";
    //     }

    //     // 存储方向
    //     int directionIndex = Car_Btngroup->checkedId();

    //     QStringList directions;
    //     if (directionIndex == 0) {
    //         directions << "x";
    //     } else if (directionIndex == 1) {
    //         directions << "y";
    //     } else if (directionIndex == 2) {
    //         directions << "z";
    //     } else if (directionIndex == 3) {
    //         // All选项 - 添加三个方向
    //         directions << "x" << "y" << "z";
    //     }

    //     // 获取物理坐标并转换为网格序号
    //     double physicalX = proLineEdit_1->text().toDouble();
    //     double physicalY = proLineEdit_2->text().toDouble();
    //     double physicalZ = proLineEdit_3->text().toDouble();

    //     // 使用xyztomesh函数将物理坐标转换为网格序号
    //     int meshX = xyztomesh(Xlines, physicalX);
    //     int meshY = xyztomesh(Ylines, physicalY);
    //     int meshZ = xyztomesh(Zlines, physicalZ);

    //     // 存储网格序号
    //     ::nearfieldprobe_x = meshX;
    //     ::nearfieldprobe_y = meshY;
    //     ::nearfieldprobe_z = meshZ;

    //     int distanceX = cpmlDistance + cpmlWidth_x;
    //     int distanceY = cpmlDistance + cpmlWidth_y;
    //     int distanceZ = cpmlDistance + cpmlWidth_z;

    //     // 计算最终的网格坐标
    //     int finalX = ::nearfieldprobe_x + distanceX;
    //     int finalY = ::nearfieldprobe_y + distanceY;
    //     int finalZ = ::nearfieldprobe_z + distanceZ;

    //     qDebug() << "输入的探针信息已保存:";
    //     qDebug() << "场类型:" << ::nearfieldprobe_EH;
    //     qDebug() << "方向:" << directions;
    //     qDebug() << "物理位置: (" << physicalX << "," << physicalY << "," << physicalZ << ") mm";
    //     qDebug() << "网格位置: (" << finalX << "," << finalY << "," << finalZ << ")";

    //     // 生成NearFieldProbe.txt文件
    //     // 获取应用程序目录路径
    //     QString appDir = QCoreApplication::applicationDirPath();
    //     QString fileName = QDir::cleanPath(appDir + "/NearFieldProbe.txt");

    //     // 检查文件是否存在
    //     QFile file(fileName);
    //     int probeCount = 0;
    //     QStringList fileLines;
    //     bool isDuplicate = false;

    //     if (file.exists()) {
    //         // 如果文件存在，读取现有内容
    //         if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //             QTextStream in(&file);
    //             while (!in.atEnd()) {
    //                 QString line = in.readLine();
    //                 fileLines.append(line);
    //             }
    //             file.close();

    //             // 获取当前的探针数量（第四行）
    //             if (fileLines.size() > 3) {
    //                 bool ok;
    //                 probeCount = fileLines[3].toInt(&ok);
    //                 if (!ok) {
    //                     probeCount = 0; // 如果转换失败，重置为0
    //                 }
    //             }

    //             // 检查是否有重复的探针
    //             // 从第5行开始是探针数据
    //             for (int i = 4; i < fileLines.size(); i++) {
    //                 QStringList parts = fileLines[i].split('\t');
    //                 if (parts.size() >= 4) {
    //                     int existingX = parts[0].toInt();
    //                     int existingY = parts[1].toInt();
    //                     int existingZ = parts[2].toInt();
    //                     QString existingField = parts[3];

    //                     // 检查每个方向是否有重复
    //                     for (const QString& dir : directions) {
    //                         QString fieldComponent = ::nearfieldprobe_EH + dir;
    //                         if (existingX == finalX && existingY == finalY &&
    //                             existingZ == finalZ && existingField == fieldComponent) {
    //                             isDuplicate = true;
    //                             qDebug() << "发现重复探针，不添加新记录";
    //                             break;
    //                         }
    //                     }
    //                     if (isDuplicate) break;
    //                 }
    //             }
    //         }
    //     }

    //     // 如果是重复探针，显示提示信息并返回
    //     if (isDuplicate) {
    //         QMessageBox::warning(this, "重复探针", "该位置和场分量的探针已存在，请使用不同的设置。");
    //         return; // 不关闭窗口，让用户可以修改输入
    //     }

    //     // 增加探针数量
    //     probeCount += directions.size(); // 根据方向数量增加探针计数

    //     // 重新打开文件进行写入
    //     if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //         QTextStream out(&file);

    //         if (fileLines.isEmpty()) {
    //             // 如果文件原来不存在或为空，写入完整的文件头
    //             out << "==============================\n";
    //             out << "i\tj\tk\tfield_component\n";
    //             out << "==============================\n";
    //             out << probeCount << "\n"; // 探针数量
    //         } else {
    //             // 如果文件已存在，重写所有行，但更新探针数量
    //             for (int i = 0; i < fileLines.size(); i++) {
    //                 if (i == 3) {
    //                     // 更新第四行的探针数量
    //                     out << probeCount << "\n";
    //                 } else {
    //                     out << fileLines[i] << "\n";
    //                 }
    //             }
    //         }

    //         // 写入新的探针数据
    //         for (const QString& dir : directions) {
    //             QString fieldComponent = ::nearfieldprobe_EH + dir;
    //             out << finalX << "\t"
    //                 << finalY << "\t"
    //                 << finalZ << "\t"
    //                 << fieldComponent << "\n";
    //         }

    //         file.close();
    //         qDebug() << "NearFieldProbe.txt文件已更新，当前探针数量:" << probeCount;

    //         // 显示成功消息
    //         QMessageBox::information(this, "成功", QString("探针信息已成功添加到文件中，添加了%1个探针。").arg(directions.size()));
    //     } else {
    //         qDebug() << "无法创建或更新NearFieldProbe.txt文件";
    //         QMessageBox::critical(this, "错误", "无法创建或更新探针文件。");
    //     }

    //     close();
    // });

    connect(Pro_Btn_No, &QPushButton::clicked, this, &simulationprobe::close);

}

Probe_data* simulationprobe::create_Prodefault()
{
    return new Probe_data;
}

void simulationprobe::Fiel_stateChanged()
{
    int c_s_temp = Co_Stm_0->currentIndex();
    Field_ComBox_index = Field_comBox_0->currentIndex(); // 获取当前索引号
    qDebug() << "当前field的索引号" << Field_ComBox_index;
    if (Field_ComBox_index == 0 || Field_ComBox_index == 1) {
        if (c_s_temp == 1) {
            Co_Stm_0->setCurrentIndex(0);
        }
        Co_Stm_0->setEnabled(false);
    } else {
        Co_Stm_0->setEnabled(true);
    }
    Probe_name_total();
}

void simulationprobe::Coorstm_state_index()
{
    Coorstm_ComBox_index = Co_Stm_0->currentIndex(); // 获取当前索引号，0是直角坐标系 1是球坐标系
    qDebug() << "当前的Coor-system的索引号是" << Coorstm_ComBox_index;
    Probe_name_total();
}

void simulationprobe::Postion_state_index()
{
    if (Co_Stm_0->currentIndex() == 0) {
        Position_index = Car_Btngroup->checkedId();
    }
    if(Co_Stm_0->currentIndex() == 1) {
        Position_index = Sph_Btngroup->checkedId();
    }
    qDebug() << "这个是Position ——index" << Position_index;
    Probe_name_total();
}

void simulationprobe::Probe_name_total()
{
    int field_name_index = Field_comBox_0->currentIndex();
    // int coor_name_index = Car_Btngroup ->checkedId();
    int coor_name_index;
    qDebug() << "这个是Co_Stm_0->currentIndex()" << Co_Stm_0->currentIndex();
    if (Co_Stm_0->currentIndex() == 0) {
        coor_name_index = Car_Btngroup->checkedId();//Car_Btngroup是什么？？？？？？
        qDebug() << "coor_name_index = Car_Btngroup->checkedId();" << coor_name_index;
    }
    if (Co_Stm_0->currentIndex() == 1){
        coor_name_index = Sph_Btngroup->checkedId() + 4;
        qDebug() << "coor_name_index = Car_Btngroup->checkedId() + 3;" << coor_name_index + 3;
    }
    QString Position_name = Get_pro_Posi();
    proLineEdit_0->setText(field_name[field_name_index] + orien_name[coor_name_index] + Position_name);
}

QString simulationprobe::Get_pro_Posi() // 传坐标
{
    QString Positon_name;
    int coor_name_index_temp = Co_Stm_0->currentIndex();
    if (coor_name_index_temp == 0) {
        pos_name_X = symbol_name[0] + QString("%1").arg(proLineEdit_1->text().toDouble());
        pos_name_Y = symbol_name[1] + QString("%1").arg(proLineEdit_2->text().toDouble()) + symbol_name[1];
        pos_name_Z = QString("%1").arg(proLineEdit_3->text().toDouble()) + symbol_name[2];
        Positon_name = pos_name_X + pos_name_Y + pos_name_Z;
    }
    if(coor_name_index_temp == 1){
        pos_name_R = symbol_name[0] + QString("%1").arg(proLineEdit_4->text().toDouble());
        pos_name_P = symbol_name[1] + QString("%1").arg(proLineEdit_5->text().toDouble()) + symbol_name[1];
        pos_name_T = QString("%1").arg(proLineEdit_6->text().toDouble()) + symbol_name[2];
        Positon_name = pos_name_R + pos_name_P + pos_name_T;
    }



    return Positon_name;
}

void simulationprobe::Pro_getInfor()
{

}




// 处理近场探针
void simulationprobe::handleNearFieldProbe()
{
    // 存储场类型 (E或H)
    int fieldIndex = Field_comBox_0->currentIndex();
    if (fieldIndex == 0) {
        ::nearfieldprobe_EH = "E";
    } else if (fieldIndex == 1) {
        ::nearfieldprobe_EH = "H";
    }

    // 存储方向
    int directionIndex = Car_Btngroup->checkedId();

    QStringList directions;
    if (directionIndex == 0) {
        directions << "x";
    } else if (directionIndex == 1) {
        directions << "y";
    } else if (directionIndex == 2) {
        directions << "z";
    } else if (directionIndex == 3) {
        // All选项 - 添加三个方向
        directions << "x" << "y" << "z";
    }

    // 获取物理坐标并转换为网格序号
    double physicalX = proLineEdit_1->text().toDouble();
    double physicalY = proLineEdit_2->text().toDouble();
    double physicalZ = proLineEdit_3->text().toDouble();

    // 使用xyztomesh函数将物理坐标转换为网格序号
    int meshX = xyztomesh(Xlines, physicalX);
    int meshY = xyztomesh(Ylines, physicalY);
    int meshZ = xyztomesh(Zlines, physicalZ);

    // 存储网格序号
    ::nearfieldprobe_x = meshX;
    ::nearfieldprobe_y = meshY;
    ::nearfieldprobe_z = meshZ;

    int distanceX = cpmlDistance + cpmlWidth_x;
    int distanceY = cpmlDistance + cpmlWidth_y;
    int distanceZ = cpmlDistance + cpmlWidth_z;

    // 计算最终的网格坐标
    int finalX = ::nearfieldprobe_x + distanceX;
    int finalY = ::nearfieldprobe_y + distanceY;
    int finalZ = ::nearfieldprobe_z + distanceZ;

    qDebug() << "输入的探针信息已保存:";
    qDebug() << "场类型:" << ::nearfieldprobe_EH;
    qDebug() << "方向:" << directions;
    qDebug() << "物理位置: (" << physicalX << "," << physicalY << "," << physicalZ << ") mm";
    qDebug() << "网格位置: (" << finalX << "," << finalY << "," << finalZ << ")";

    // 生成NearFieldProbe.txt文件
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/NearFieldProbe.txt");

    // 检查文件是否存在
    QFile file(fileName);
    int probeCount = 0;
    QStringList fileLines;
    bool isDuplicate = false;

    if (file.exists()) {
        // 如果文件存在，读取现有内容
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                fileLines.append(line);
            }
            file.close();

            // 获取当前的探针数量（第四行）
            if (fileLines.size() > 3) {
                bool ok;
                probeCount = fileLines[3].toInt(&ok);
                if (!ok) {
                    probeCount = 0; // 如果转换失败，重置为0
                }
            }

            // 检查是否有重复的探针
            // 从第5行开始是探针数据
            for (int i = 4; i < fileLines.size(); i++) {
                QStringList parts = fileLines[i].split('\t');
                if (parts.size() >= 4) {
                    int existingX = parts[0].toInt();
                    int existingY = parts[1].toInt();
                    int existingZ = parts[2].toInt();
                    QString existingField = parts[3];

                    // 检查每个方向是否有重复
                    for (const QString& dir : directions) {
                        QString fieldComponent = ::nearfieldprobe_EH + dir;
                        if (existingX == finalX && existingY == finalY &&
                            existingZ == finalZ && existingField == fieldComponent) {
                            isDuplicate = true;
                            qDebug() << "发现重复探针，不添加新记录";
                            break;
                        }
                    }
                    if (isDuplicate) break;
                }
            }
        }
    }

    // 如果是重复探针，显示提示信息并返回
    if (isDuplicate) {
        QMessageBox::warning(this, "重复探针", "该位置和场分量的探针已存在，请使用不同的设置。");
        return; // 不关闭窗口，让用户可以修改输入
    }

    // 增加探针数量
    probeCount += directions.size(); // 根据方向数量增加探针计数

    // 重新打开文件进行写入
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        if (fileLines.isEmpty()) {
            // 如果文件原来不存在或为空，写入完整的文件头
            out << "==============================\n";
            out << "i\tj\tk\tfield_component\n";
            out << "==============================\n";
            out << probeCount << "\n"; // 探针数量
        } else {
            // 如果文件已存在，重写所有行，但更新探针数量
            for (int i = 0; i < fileLines.size(); i++) {
                if (i == 3) {
                    // 更新第四行的探针数量
                    out << probeCount << "\n";
                } else {
                    out << fileLines[i] << "\n";
                }
            }
        }

        // 写入新的探针数据
        for (const QString& dir : directions) {
            QString fieldComponent = ::nearfieldprobe_EH + dir;
            out << finalX << "\t"
                << finalY << "\t"
                << finalZ << "\t"
                << fieldComponent << "\n";
        }

        file.close();
        qDebug() << "NearFieldProbe.txt文件已更新，当前探针数量:" << probeCount;

        // 显示成功消息
        QMessageBox::information(this, "成功", QString("近场探针信息已成功添加到文件中，添加了%1个探针。").arg(directions.size()));
    } else {
        qDebug() << "无法创建或更新NearFieldProbe.txt文件";
        QMessageBox::critical(this, "错误", "无法创建或更新探针文件。");
    }
}

// 处理远场观测点
void simulationprobe::handleFarFieldObservationPoint()
{
    // 获取球坐标下的位置参数
    double r = proLineEdit_6->text().toDouble();      // 半径 r (米)
    double phi = proLineEdit_5->text().toDouble();    // phi (度)
    double theta = proLineEdit_4->text().toDouble();  // theta (度)

    qDebug() << "远场观测点参数:";
    qDebug() << "r =" << r << "m";
    qDebug() << "phi =" << phi << "度";
    qDebug() << "theta =" << theta << "度";

    // 获取场类型索引
    int fieldIndex = Field_comBox_0->currentIndex();
    QString fieldType;
    if (fieldIndex == 2) {
        fieldType = "E-field(Farfield)";
    } else if (fieldIndex == 3) {
        fieldType = "H-field(Farfield)";
    } else if (fieldIndex == 4) {
        fieldType = "RCS";
    }

    qDebug() << "场类型:" << fieldType;

    // 生成FarFieldObservationPoint.txt文件
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/FarFieldObservationPoint.txt");

    // 检查文件是否存在
    QFile file(fileName);
    int pointCount = 0;
    QStringList fileLines;
    bool isDuplicate = false;

    if (file.exists()) {
        // 如果文件存在，读取现有内容
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                fileLines.append(line);
            }
            file.close();

            // 获取当前的观测点数量（第四行）
            if (fileLines.size() > 3) {
                bool ok;
                pointCount = fileLines[3].toInt(&ok);
                if (!ok) {
                    pointCount = 0; // 如果转换失败，重置为0
                }
            }

            // 检查是否有重复的观测点（从第5行开始是数据）
            // 使用容差比较，因为浮点数可能有微小差异
            double tolerance = 1e-6;
            for (int i = 4; i < fileLines.size(); i++) {
                QStringList parts = fileLines[i].split('\t');
                if (parts.size() >= 3) {
                    double existingR = parts[0].toDouble();
                    double existingPhi = parts[1].toDouble();
                    double existingTheta = parts[2].toDouble();

                    if (fabs(existingR - r) < tolerance &&
                        fabs(existingPhi - phi) < tolerance &&
                        fabs(existingTheta - theta) < tolerance) {
                        isDuplicate = true;
                        qDebug() << "发现重复远场观测点，不添加新记录";
                        break;
                    }
                }
            }
        }
    }

    // 如果是重复观测点，显示提示信息并返回
    if (isDuplicate) {
        QMessageBox::warning(this, "重复观测点", "该位置和大小的远场观测点已存在，请使用不同的设置。");
        return; // 不关闭窗口，让用户可以修改输入
    }

    // 增加观测点数量
    pointCount++;

    // 重新打开文件进行写入
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        if (fileLines.isEmpty()) {
            // 如果文件原来不存在或为空，写入完整的文件头
            out << "==============================\n";
            out << "r(m)\tphi(degree)\ttheta(degree)\n";
            out << "==============================\n";
            out << pointCount << "\n"; // 观测点数量
        } else {
            // 如果文件已存在，重写所有行，但更新观测点数量
            for (int i = 0; i < fileLines.size(); i++) {
                if (i == 3) {
                    // 更新第四行的观测点数量
                    out << pointCount << "\n";
                } else {
                    out << fileLines[i] << "\n";
                }
            }
        }

        // 写入新的观测点数据
        out << r << "\t" << phi << "\t" << theta << "\n";

        file.close();
        qDebug() << "FarFieldObservationPoint.txt文件已更新，当前观测点数量:" << pointCount;

        // 显示成功消息
        QMessageBox::information(this, "成功",
                                 QString("远场观测点信息已成功添加到文件中。\n场类型: %1\n位置: r=%2m, φ=%3°, θ=%4°")
                                     .arg(fieldType).arg(r).arg(phi).arg(theta));
    } else {
        qDebug() << "无法创建或更新FarFieldObservationPoint.txt文件";
        QMessageBox::critical(this, "错误", "无法创建或更新远场观测点文件。");
    }
}
