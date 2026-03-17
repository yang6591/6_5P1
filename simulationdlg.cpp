#include "simulationdlg.h"
#include <QFile>
#include <QCoreApplication>
#include <QDir>

SimulationDlg::SimulationDlg(Plane_data* data, QWidget* parent)
    : QDialog(parent)
{
    // 准备一个默认值
    Plane_data& data_default = data == nullptr ? *create_default() : *data;

    setWindowTitle(tr("Plane Wave"));

    // Plane wave 不是窗口分割而是GroupBox
    // 创建Polarization settings
    // 第一框：第一行
    QGroupBox* groupBox_0 = new QGroupBox(tr("Polarization settings"));
    QLabel* pwLabel_0 = new QLabel("Type:");
    QLabel* pwLabel_1 = new QLabel("                    ");
    QRadioButton* TypeBtn_0 = new QRadioButton(tr("Linear"));
    QRadioButton* TypeBtn_1 = new QRadioButton(tr("Circular"));
    QRadioButton* TypeBtn_2 = new QRadioButton(tr("Elliptical"));
    // TypeBtn_2->setChecked(1);
    switch (data_default.polar_mode) {
    case 0:
        TypeBtn_0->setChecked(true);
        break;
    case 1:
        TypeBtn_1->setChecked(true);
        break;
    case 2:
        TypeBtn_2->setChecked(true);
        break;
    default:
        break;
    }

    QHBoxLayout* hpwLayout_0 = new QHBoxLayout;
    hpwLayout_0->addWidget(pwLabel_0);
    hpwLayout_0->addWidget(pwLabel_1);
    hpwLayout_0->addWidget(TypeBtn_0);
    hpwLayout_0->addWidget(TypeBtn_1);
    hpwLayout_0->addWidget(TypeBtn_2);

    // 按钮组 对极化方式 进行排序
    // 这条语句：上面已经将几个按钮合并在一起了，现在将合并在一起的布局放在RadioBtnGroup里面
    RadioBtnGroup.addButton(TypeBtn_0, 0);
    RadioBtnGroup.addButton(TypeBtn_1, 1);
    RadioBtnGroup.addButton(TypeBtn_2, 2);

    // 先进行排版
    // 第二行
    QHBoxLayout* hpwLayout_1 = new QHBoxLayout;
    QLabel* pwLabel_2 = new QLabel("Ref.frequency:");
    QLabel* pwLabel_21 = new QLabel("                        ");
    pwLineEdit_0 = new QLineEdit;
    // pwLineEdit_0->setEnabled(false);
    // 若是圆极化 || 椭圆极化则可以设定
    if (data_default.polar_mode == 1 || data_default.polar_mode == 2) {
        pwLineEdit_0->setEnabled(true);
    } else {
        pwLineEdit_0->setEnabled(false);
    }
    pwLineEdit_0->setFixedWidth(80);
    pwLineEdit_0->setText(QString("%1").arg(data_default.Ref_fre_data));
    hpwLayout_1->addWidget(pwLabel_2);
    hpwLayout_1->addWidget(pwLabel_21);
    hpwLayout_1->addWidget(pwLineEdit_0);
    hpwLayout_1->addStretch();
    /*
     *  布局管理器在添加完控件后使用函数addStretch()，则控件排布向左靠
        布局管理器在添加控件前使用函数addStretch()，则控件排布向右靠齐
        布局管理器在添加每个控件前后使用函数addStretch()，则控件居中排布
        原文链接：https://blog.csdn.net/qq_51150032/article/details/110847666
     */

    // 第三行圆极化
    QHBoxLayout* ChpwLayout_1 = new QHBoxLayout;
    QHBoxLayout* ChpwLayout_11 = new QHBoxLayout;
    QLabel* CircularLabel_0 = new QLabel("Phase.difference:");
    QLabel* CircularLabel_01 = new QLabel("                    ");
    CircularLeftRadioBtn = new QRadioButton(tr("Left"));
    CircularRightRadioBtn = new QRadioButton(tr("Right"));
    ChpwLayout_1->addWidget(CircularLabel_0);
    ChpwLayout_1->addWidget(CircularLabel_01);
    ChpwLayout_11->addWidget(CircularLeftRadioBtn);
    ChpwLayout_11->addWidget(CircularRightRadioBtn);
    ChpwLayout_11->addStretch();
    ChpwLayout_11->setSpacing(44); // 设置控件之间的距离
    ChpwLayout_1->addLayout(ChpwLayout_11);
    // 初始状态下为椭圆极化，则这两个按钮初始状态也是不能选择
    if (data_default.Phase_data == 0) {
        CircularLeftRadioBtn->setChecked(true);
    } else {
        CircularRightRadioBtn->setChecked(true);
    }
    if (data_default.polar_mode == 1) {
        CircularLeftRadioBtn->setEnabled(true);
        CircularRightRadioBtn->setEnabled(true);
    } else {
        CircularLeftRadioBtn->setEnabled(false);
        CircularRightRadioBtn->setEnabled(false);
    }

    // 第四行(椭圆极化)
    QHBoxLayout* ChpwLayout_2 = new QHBoxLayout;
    QLabel* pwLabel_3 = new QLabel("Phase.difference:");
    pwLineEdit_1 = new QLineEdit;
    pwLineEdit_1->setFixedWidth(80);
    pwLineEdit_1->setText(QString("%1").arg(data_default.Phdeg_data));
    QLabel* pwLabel_4 = new QLabel("degrees");
    ChpwLayout_2->addWidget(pwLabel_3);
    ChpwLayout_2->addWidget(pwLineEdit_1);
    ChpwLayout_2->addWidget(pwLabel_4);

    // 第五行
    QHBoxLayout* ChpwLayout_3 = new QHBoxLayout;
    QLabel* pwLabel_5 = new QLabel("Axial ratio:");
    QLabel* pwLabel_51 = new QLabel("                              ");
    pwLineEdit_2 = new QLineEdit;
    pwLineEdit_2->setFixedWidth(80);
    pwLineEdit_2->setText(QString("%1").arg(data_default.Axial_data));
    ChpwLayout_3->addWidget(pwLabel_5);
    ChpwLayout_3->addWidget(pwLabel_51);
    ChpwLayout_3->addWidget(pwLineEdit_2);
    ChpwLayout_3->addStretch();
    if (data_default.polar_mode == 2) {
        pwLineEdit_1->setEnabled(true);
        pwLineEdit_2->setEnabled(true);
    } else {
        pwLineEdit_1->setEnabled(false);
        pwLineEdit_2->setEnabled(false);
    }

    // 线极化
    connect(TypeBtn_0, &QPushButton::clicked, this, &SimulationDlg::LineChangeBtnChangeState);
    // 圆极化
    connect(TypeBtn_1, &QPushButton::clicked, this, &SimulationDlg::CircularBtnChangeState);
    // 椭圆极化
    connect(TypeBtn_2, &QPushButton::clicked, this, &SimulationDlg::EllipticalBtnChangeState);

    // 第六行
    checkBox_0 = new QCheckBox("Automatic detection of decoupling plane");

    // 对于checkBox_0 ： true 默认选中，false默认不选中
    // 对于AutoStat_data ： 0是被选中的状态（初始），1是未被选中的状态
    if (data_default.AutoStat_data == 0) {
        checkBox_0->setChecked(1);
    }
    if (data_default.AutoStat_data == 1) {
        checkBox_0->setChecked(0);
    }

    QGridLayout* hpwLayout_2 = new QGridLayout;
    hpwLayout_2->addWidget(checkBox_0, 0, 0);
    // 第一个框的布局总和
    QGridLayout* FirLayout = new QGridLayout; // 总体布局
    FirLayout->addLayout(hpwLayout_0, 0, 0);
    FirLayout->addLayout(hpwLayout_1, 1, 0);
    FirLayout->addLayout(ChpwLayout_1, 2, 0);
    FirLayout->addLayout(ChpwLayout_2, 3, 0);
    FirLayout->addLayout(ChpwLayout_3, 4, 0);
    FirLayout->addLayout(hpwLayout_2, 5, 0);

    // 第二框：第一行
    QGroupBox* groupBox_1 = new QGroupBox(tr("Propagation and electric field settings"));
    QLabel* pwLabel_6 = new QLabel("Progation normal in x,y,z:");
    QLineEdit* pwLineEdit_3 = new QLineEdit;
    QLineEdit* pwLineEdit_4 = new QLineEdit;
    QLineEdit* pwLineEdit_5 = new QLineEdit;
    pwLineEdit_3->setFixedWidth(80);
    pwLineEdit_4->setFixedWidth(80);
    pwLineEdit_5->setFixedWidth(80);

    pwLineEdit_3->setText(QString("%1").arg(data_default.normal_coordinate[0]));
    pwLineEdit_4->setText(QString("%1").arg(data_default.normal_coordinate[1]));
    pwLineEdit_5->setText(QString("%1").arg(data_default.normal_coordinate[2]));

    QGridLayout* SecLayout = new QGridLayout;
    SecLayout->addWidget(pwLabel_6, 0, 0);
    SecLayout->addWidget(pwLineEdit_3, 0, 2);
    SecLayout->addWidget(pwLineEdit_4, 0, 3);
    SecLayout->addWidget(pwLineEdit_5, 0, 4);

    // 第二行
    QLabel* pwLabel_7 = new QLabel("Electric field vector in x,y,z:");
    QLineEdit* pwLineEdit_6 = new QLineEdit;
    QLineEdit* pwLineEdit_7 = new QLineEdit;
    QLineEdit* pwLineEdit_8 = new QLineEdit;
    QLabel* pwLabel_8 = new QLabel("V/m");
    pwLineEdit_6->setFixedWidth(80);
    pwLineEdit_7->setFixedWidth(80);
    pwLineEdit_8->setFixedWidth(80);

    pwLineEdit_6->setText(QString("%1").arg(data_default.vector_coordinate[0]));
    pwLineEdit_7->setText(QString("%1").arg(data_default.vector_coordinate[1]));
    pwLineEdit_8->setText(QString("%1").arg(data_default.vector_coordinate[2]));

    SecLayout->addWidget(pwLabel_7, 1, 0);
    SecLayout->addWidget(pwLineEdit_6, 1, 2);
    SecLayout->addWidget(pwLineEdit_7, 1, 3);
    SecLayout->addWidget(pwLineEdit_8, 1, 4);
    SecLayout->addWidget(pwLabel_8, 1, 5);

    // 第三框：第一行
    groupBox_2 = new QGroupBox(tr("Use decoupling plane"));
    // groupBox_2->setEnabled(false);
    groupBox_2->setCheckable(true);
    if (data_default.AutoStat_data == 1) {
        groupBox_2->setEnabled(true);
        if (data_default.UdpState_data == 1) {
            groupBox_2->setChecked(true);
        } else {
            groupBox_2->setChecked(false);
        }
    } else {
        groupBox_2->setEnabled(false);
    }
    // 根据Automatic和Use decoupling plan，两个的勾选状态判断是否可勾选Use decoupling plane
    if (data_default.AutoStat_data == 1) {
        if (data_default.UdpState_data == 1) {
            groupBox_2->setChecked(true);
        }
    }

    QLabel* pwLabel_9 = new QLabel("Plane normal:");
    QRadioButton* planenormalBtn_0 = new QRadioButton(tr("X"));
    QRadioButton* planenormalBtn_1 = new QRadioButton(tr("Y"));
    QRadioButton* planenormalBtn_2 = new QRadioButton(tr("Z"));
    switch (data_default.Plnormal_data) {
    case 0:
        planenormalBtn_0->setChecked(true);
        break;
    case 1:
        planenormalBtn_1->setChecked(true);
        break;
    case 2:
        planenormalBtn_2->setChecked(true);
        break;
    default:
        break;
    }
    RadioBtnGroup_Pnormal.addButton(planenormalBtn_0, 0);
    RadioBtnGroup_Pnormal.addButton(planenormalBtn_1, 1);
    RadioBtnGroup_Pnormal.addButton(planenormalBtn_2, 2);

    QLabel* pwLabel_10 = new QLabel("Position x:");
    QLineEdit* pwLineEdit_9 = new QLineEdit;
    pwLineEdit_9->setFixedWidth(80);
    pwLineEdit_9->setText(QString("%1").arg(data_default.PosX_data));
    QLabel* pwLabel_11 = new QLabel("                    ");

    QHBoxLayout* plnLayout_1 = new QHBoxLayout;
    plnLayout_1->addWidget(pwLabel_9);
    plnLayout_1->addWidget(pwLabel_11);
    plnLayout_1->addWidget(planenormalBtn_0);
    plnLayout_1->addWidget(planenormalBtn_1);
    plnLayout_1->addWidget(planenormalBtn_2);

    QGridLayout* plnLayout_2 = new QGridLayout;
    plnLayout_2->addWidget(pwLabel_10, 0, 0);
    plnLayout_2->addWidget(pwLineEdit_9, 0, 1);

    QGridLayout* ThirdLayout = new QGridLayout;
    ThirdLayout->addLayout(plnLayout_1, 0, 0);
    ThirdLayout->addLayout(plnLayout_2, 1, 0);

    // 第四栏：按钮
    QGroupBox* groupBox_3 = new QGroupBox;
    groupBox_3->setStyleSheet("QGroupBox {border: 0;}");
    QPushButton* ChooseBtn_0 = new QPushButton(tr("OK"));
    QPushButton* ChooseBtn_1 = new QPushButton(tr("Cancel"));
    QPushButton* ChooseBtn_2 = new QPushButton(tr("Apply"));
    QPushButton* ChooseBtn_3 = new QPushButton(tr("Help"));
    QVBoxLayout* groupBox_3Btn = new QVBoxLayout;
    groupBox_3Btn->addWidget(ChooseBtn_0);
    groupBox_3Btn->addWidget(ChooseBtn_1);
    groupBox_3Btn->addWidget(ChooseBtn_2);
    groupBox_3Btn->addWidget(ChooseBtn_3);

    // 汇总
    groupBox_0->setLayout(FirLayout);
    groupBox_1->setLayout(SecLayout);
    groupBox_2->setLayout(ThirdLayout);
    groupBox_3->setLayout(groupBox_3Btn);

    gpwLayout = new QGridLayout(this);
    gpwLayout->addWidget(groupBox_0, 0, 0, 1, 1);
    gpwLayout->addWidget(groupBox_1, 1, 0, 1, 1);
    gpwLayout->addWidget(groupBox_2, 2, 0, 1, 1);
    gpwLayout->addWidget(groupBox_3, 0, 1, 1, 1);

    // 这个connect连接第一个框和第三个框
    /*Polarization settings中的 checkBox_0：("Automatic detection of decoupling plane")来控制
      Use decoupling plane中的groupBox_2这个整体*/
    connect(checkBox_0, &QCheckBox::stateChanged, this, &SimulationDlg::changeUdpState);

    // 连接 添加ok的信号到ok信号

    // 捕获列表
    connect(ChooseBtn_0, &QPushButton::clicked, this, [=]() { //=是this的时候 下面报错，这里改为=就可以了
        // 拿到界面的数据
        // 向结构体内传递数据
        Plane_data* Plwave_data = new Plane_data;
        // 先判断是哪种极化
        Plwave_data->polar_mode = RadioBtnGroup.checkedId();
        qDebug() << "目前选择的是" << Plwave_data->polar_mode;
        Plwave_data->normal_coordinate[0] = pwLineEdit_3->text().toDouble();
        Plwave_data->normal_coordinate[1] = pwLineEdit_4->text().toDouble();
        Plwave_data->normal_coordinate[2] = pwLineEdit_5->text().toDouble();


        Plwave_data->vector_coordinate[0] = pwLineEdit_6->text().toDouble();
        Plwave_data->vector_coordinate[1] = pwLineEdit_7->text().toDouble();
        Plwave_data->vector_coordinate[2] = pwLineEdit_8->text().toDouble();

        double kx= Plwave_data->normal_coordinate[0];
        double ky= Plwave_data->normal_coordinate[1];
        double kz= Plwave_data->normal_coordinate[2];
        double Ex= Plwave_data->vector_coordinate[0];
        double Ey= Plwave_data->vector_coordinate[1];
        double Ez= Plwave_data->vector_coordinate[2];


        qDebug() << Plwave_data->normal_coordinate[0];

        qDebug() << "传播方向:" << kx << ky << kz;
        qDebug() << "极化方向:" << Ex << Ey << Ez;

        // ========== 计算phi, theta, alpha ==========
        // 归一化传播方向向量
        double k_length = sqrt(kx*kx + ky*ky + kz*kz);
        if (k_length > 0) {
            kx /= k_length;
            ky /= k_length;
            kz /= k_length;
        }

        // 归一化电场方向向量
        double E_length = sqrt(Ex*Ex + Ey*Ey + Ez*Ez);
        if (E_length > 0) {
            Ex /= E_length;
            Ey /= E_length;
            Ez /= E_length;
        }

        // 计算传播方向的phi和theta
        double phi_rad = atan2(ky, kx);
        Plwave_data->phi = phi_rad * 180.0 / M_PI;
        if (Plwave_data->phi < 0) Plwave_data->phi += 360.0;

        double r_xy = sqrt(kx*kx + ky*ky);
        Plwave_data->theta = atan2(r_xy, kz) * 180.0 / M_PI;

        // 计算极化角alpha
        // 构建球坐标系的单位矢量
        double sin_phi = sin(phi_rad);
        double cos_phi = cos(phi_rad);
        double theta_rad = Plwave_data->theta * M_PI / 180.0;
        double sin_theta = sin(theta_rad);
        double cos_theta = cos(theta_rad);

        // e_phi 方向单位矢量
        double e_phi_x = -sin_phi;
        double e_phi_y = cos_phi;
        double e_phi_z = 0;

        // e_theta 方向单位矢量
        double e_theta_x = cos_theta * cos_phi;
        double e_theta_y = cos_theta * sin_phi;
        double e_theta_z = -sin_theta;

        // 将电场矢量分解到球坐标系
        double E_phi = Ex * e_phi_x + Ey * e_phi_y + Ez * e_phi_z;
        double E_theta = Ex * e_theta_x + Ey * e_theta_y + Ez * e_theta_z;

        // 计算极化角alpha
        Plwave_data->alpha = atan2(E_phi, E_theta) * 180.0 / M_PI;
        if (Plwave_data->alpha < 0) Plwave_data->alpha += 360.0;

        qDebug() << "计算得到的角度: phi =" << Plwave_data->phi
                 << "theta =" << Plwave_data->theta
                 << "alpha =" << Plwave_data->alpha;
        // ========== 计算结束 ==========



        switch (Plwave_data->polar_mode) {
        case 0:

            break;
        case 1:
            Plwave_data->Ref_fre_data = pwLineEdit_0->text().toDouble();
            if (CircularLeftRadioBtn->isChecked()) {
                Plwave_data->Phase_data = 0;
            } else {
                Plwave_data->Phase_data = 1;
            }
            break;
        case 2:
            Plwave_data->Ref_fre_data = pwLineEdit_0->text().toDouble();
            //    double Phdeg_data;
            //    double Axial_data;
            Plwave_data->Phdeg_data = pwLineEdit_1->text().toDouble();
            Plwave_data->Axial_data = pwLineEdit_2->text().toDouble();
            break;
        default:
            break;
        }
        // 记录Automatic 的勾选状态
        if (checkBox_0->checkState() == Qt::Checked) {
            Plwave_data->AutoStat_data = 0;
            //qDebug() << "AutoStat_data 已被勾选";
        } else {
            Plwave_data->AutoStat_data = 1;
            //qDebug() << "AutoStat_data 未被勾选";
        }

        // 记录Use decoupling plane的勾选状态
        if (groupBox_2->isChecked()) {
            Plwave_data->UdpState_data = 1;
            //qDebug() << "groupBox_2 已被勾选" << Plwave_data->UdpState_data;
        } else {
            Plwave_data->UdpState_data = 0;
            //qDebug() << "groupBox_2 未被勾选" << Plwave_data->UdpState_data;
        }
        // // 根据Automatic和Use decoupling plan，两个的勾选状态判断是否可勾选Use decoupling plane
        // if (Plwave_data->AutoStat_data = 1) {
        //     if (Plwave_data->UdpState_data = 1) {
        //         groupBox_2->setChecked(true);
        //     }
        // }
        Plwave_data->Plnormal_data = RadioBtnGroup_Pnormal.checkedId();
        qDebug() << Plwave_data->Plnormal_data;
        Plwave_data->PosX_data = pwLineEdit_9->text().toDouble();



        // 获取应用程序目录路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString fileName = QDir::cleanPath(appDir + "/PlaneWave.txt");

        // 生成meshstructure.txt文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // // 写入文件头
            // out << "============================================================================\n";
            // out << "propagation_x\tpropagation_y\tpropagation_z\telectricfield_x\telectricfield_y\telectricfield_z\n";
            // out << "============================================================================\n";
            // // 写入数据行

            //     out << Plwave_data->normal_coordinate[0] << "\t"
            //         << Plwave_data->normal_coordinate[1] << "\t"
            //         << Plwave_data->normal_coordinate[2] << "\t"
            //         << Plwave_data->vector_coordinate[0] << "\t"
            //         << Plwave_data->vector_coordinate[1] << "\t"
            //         << Plwave_data->vector_coordinate[2] << "\n";

            out << "==================\n";
            out << "phi\ttheta\talpha\n";
            out << "==================\n";

            out << Plwave_data->phi << "\t"
                << Plwave_data->theta << "\t"
                << Plwave_data->alpha << "\t";


                file.close();
                qDebug() << "PlaneWave.txt 文件已写入";
            } else {
                qDebug() << "无法打开文件进行写入";
            }



        // 组装Plane对象
        // 传递
        emit this->plane_ok(Plwave_data);

        close();


    });
}

Plane_data* SimulationDlg::create_default()
{
    return new Plane_data;
}

void SimulationDlg::LineChangeBtnChangeState()
{

    int PolarNumber = RadioBtnGroup.checkedId();
    // qDebug() << PolarNumber ;
    if (PolarNumber == 0) {
        pwLineEdit_0->setEnabled(false);
        CircularLeftRadioBtn->setEnabled(false);
        CircularRightRadioBtn->setEnabled(false);
        pwLineEdit_1->setEnabled(false);
        pwLineEdit_2->setEnabled(false);
    }
}

void SimulationDlg::changeUdpState()
{
    Qt::CheckState checkBox0State = checkBox_0->checkState();
    if (checkBox0State == Qt::Checked) {
        groupBox_2->setEnabled(false);
    } else {
        groupBox_2->setEnabled(true);
    }
}

void SimulationDlg::CircularBtnChangeState()
{
    int CirculRadioBtn = RadioBtnGroup.checkedId();
    if (CirculRadioBtn == 1) {
        pwLineEdit_0->setEnabled(true);
        CircularLeftRadioBtn->setEnabled(true);
        CircularRightRadioBtn->setEnabled(true);
        pwLineEdit_1->setEnabled(false);
        pwLineEdit_2->setEnabled(false);
    }
}
void SimulationDlg::EllipticalBtnChangeState()
{
    int EllipticalRadioBtn = RadioBtnGroup.checkedId();
    if (EllipticalRadioBtn == 2) {
        pwLineEdit_0->setEnabled(true);
        CircularLeftRadioBtn->setEnabled(false);
        CircularRightRadioBtn->setEnabled(false);
        pwLineEdit_1->setEnabled(true);
        pwLineEdit_2->setEnabled(true);
    }
}
// void SimulationDlg:: CirularChangeState()
// {
//     PolarNumber = RadioBtnGroup .checkedId();
// }
// void SimulationDlg:: EllipChangeState()
// {
//     PolarNumber = RadioBtnGroup .checkedId();
// }
