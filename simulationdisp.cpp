#include "simulationdisp.h"
#include "global.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

int source_type_int = 0;
int impedance = 0;
int min_x = 0;
int max_x = 0;
int min_y = 0;
int max_y = 0;
int min_z = 0;
int max_z = 0;
QString DiscretePorts_direction = "";

void printGlobalVariables() {
    qDebug() << "=== 输入的端口全局变量值 ===";
    qDebug() << "source_type:" << source_type_int << "地址:" << &source_type_int;
    qDebug() << "impedance:" << impedance << "地址:" << &impedance;
    qDebug() << "min_x:" << min_x << "地址:" << &min_x;
    qDebug() << "max_x:" << max_x << "地址:" << &max_x;
    qDebug() << "min_y:" << min_y << "地址:" << &min_y;
    qDebug() << "max_y:" << max_y << "地址:" << &max_y;
    qDebug() << "min_z:" << min_z << "地址:" << &min_z;
    qDebug() << "max_z:" << max_z << "地址:" << &max_z;
    qDebug() << "DiscretePorts_direction:" << DiscretePorts_direction << "地址:" << &DiscretePorts_direction;
    qDebug() << "===================";
}

simulationDisP::simulationDisP(DPort_data* DPort_data1,QWidget* parent)
    : QDialog(parent)
{
    DPort_data& DPort_default = DPort_data1 == nullptr ? *creat_DPordefault() : *DPort_data1;
    setWindowTitle(tr("Discrete Edge Port"));

    //全局变量
    // 定义全局变量


    //创建对话框:第一个框
    QGroupBox* Property_group = new QGroupBox(tr("Properties"));
    //第一行
    QLabel* Port_type_label = new QLabel(tr("Port type:"));
    QRadioButton* RadioBtn_SPara = new QRadioButton("S-Parameter");
    QRadioButton* RadioBtn_Volta = new QRadioButton("Voltage");
    QRadioButton* RadioBtn_Curre = new QRadioButton("Current");
    switch(DPort_default.PType_index){
    case 0:
        RadioBtn_SPara->setChecked(true);
        break;
    case 1:
        RadioBtn_Volta->setChecked(true);
        break;
    case 2:
        RadioBtn_Curre->setChecked(true);
        break;
    default:
        break;
    }

    QButtonGroup* PortType_Btngroup = new QButtonGroup;
    PortType_Btngroup->addButton(RadioBtn_SPara,0);
    PortType_Btngroup->addButton(RadioBtn_Volta,1);
    PortType_Btngroup->addButton(RadioBtn_Curre,2);
    PortType_Btngroup -> setExclusive(true);//设置为单选
    PortType_Btngroup -> button(DPort_default.PType_index) -> setCheckable(true);//让对应DPort_default.PType_index的按钮被选中

    QHBoxLayout* PortType_Layout0 = new QHBoxLayout;
    PortType_Layout0 -> addWidget(RadioBtn_SPara);
    PortType_Layout0 -> addWidget(RadioBtn_Volta);
    PortType_Layout0 -> addWidget(RadioBtn_Curre);
    QFormLayout* DP_formlayout0 = new QFormLayout;
    DP_formlayout0 ->addRow(Port_type_label,PortType_Layout0);

    // 连接Port type选择信号
    connect(RadioBtn_SPara, &QRadioButton::toggled, this, [](bool checked){
        if (checked) {
            ::source_type_int = 0;  // S-Parameter
        }
    });
    connect(RadioBtn_Volta, &QRadioButton::toggled, this, [](bool checked){
        if (checked) {
            ::source_type_int = 1;
        }
    });
    connect(RadioBtn_Curre, &QRadioButton::toggled, this, [](bool checked){
        if (checked) {
            ::source_type_int = 2;
        }
    });

    //第二行

    QLabel* PType_Folder = new QLabel(tr("Name:"));
    QComboBox* PType_ComBox_Folder = new QComboBox;
    PType_ComBox_Folder -> setFixedWidth(180);
    QString Pt_Folder_item0 = "0";
    QString Pt_Folder_item1 = "1";
    QString Pt_Folder_item2 = "2";
    QString Pt_Folder_item3 = "3";
    QString Pt_Folder_item4 = "4";
    QString Pt_Folder_item5 = "5";
    QString Pt_Folder_item6 = "6";
    QString Pt_Folder_item7 = "7";
    QString Pt_Folder_item8 = "8";
    QString Pt_Folder_item9 = "9";
    PType_ComBox_Folder->addItem(Pt_Folder_item0);
    PType_ComBox_Folder->addItem(Pt_Folder_item1);
    PType_ComBox_Folder->addItem(Pt_Folder_item2);
    PType_ComBox_Folder->addItem(Pt_Folder_item3);
    PType_ComBox_Folder->addItem(Pt_Folder_item4);
    PType_ComBox_Folder->addItem(Pt_Folder_item5);
    PType_ComBox_Folder->addItem(Pt_Folder_item6);
    PType_ComBox_Folder->addItem(Pt_Folder_item7);
    PType_ComBox_Folder->addItem(Pt_Folder_item8);
    PType_ComBox_Folder->addItem(Pt_Folder_item9);
    PType_ComBox_Folder -> setCurrentIndex(DPort_default.PFolder);//设置默认选项


    DP_formlayout0 -> addRow(PType_Folder,PType_ComBox_Folder);


    //第三行
    QLabel* PType_Name = new QLabel(tr("Folder:"));
    QComboBox* Ptype_ComBox_Name = new QComboBox;
    Ptype_ComBox_Name -> setFixedWidth(180);
    Ptype_ComBox_Name->setEditable(true);
    QString Ptype_ComBox_Option0 = "[New Folder]";
    QString Ptype_ComBox_Option1 = "[Folder1]";
    Ptype_ComBox_Name ->addItem(Ptype_ComBox_Option0);
    Ptype_ComBox_Name ->addItem(Ptype_ComBox_Option1);
    //Ptype_ComBox_Name -> setCurrentIndex(DPort_default.Port_name);//设置默认选项
    Ptype_ComBox_Name->setCurrentText("");
    // 设置插入策略，防止用户输入的文本被添加到下拉列表中
    Ptype_ComBox_Name->setInsertPolicy(QComboBox::NoInsert);
    DP_formlayout0 -> addRow(PType_Name,Ptype_ComBox_Name);

    //第四行
    QLabel* PType_Label = new QLabel(tr("Label:"));
    QLineEdit* PType_label_EditLine = new QLineEdit;
    PType_label_EditLine -> setText(DPort_default.PLabel_name);
    PType_label_EditLine -> setFixedWidth(180);

    DP_formlayout0 -> addRow(PType_Label,PType_label_EditLine);

    //第五行
    //点击不同的按钮显示不同的内容，可以不用QStackedWidget,而是根据不同的按钮直接改变标签的名字和内容
    QLabel* Ptype_ImVocu = new QLabel;
    QLineEdit* PType_Impedance_EditLine = new QLineEdit;
    PType_Impedance_EditLine -> setFixedWidth(180);
    QLabel* PType_ImVoCu_Unit = new QLabel;

    connect(PType_Impedance_EditLine, &QLineEdit::textChanged, this, [=](const QString &text){
        bool ok;
        double value = text.toDouble(&ok);
        if (ok && RadioBtn_SPara->isChecked()) {
            ::impedance = value;  // 只有当选择S-Parameter时才更新impedance
        }
    });

    switch (DPort_default.PType_index) { // 根据传递进来的数据设定显示
    case 0:
        qDebug() <<"这个时候的DPort_default.PType_index是"<<DPort_default.PType_index;
        qDebug()<<"在执行Impendence的语句";
        Ptype_ImVocu -> setText("Impendance:");
        PType_Impedance_EditLine -> setText(QString("%1").arg(DPort_default.SP_Impdance));
        PType_ImVoCu_Unit -> setText("Ohm");
        break;
    case 1:
        qDebug()<<"在执行Impendence2的语句";
        Ptype_ImVocu -> setText("Voltrage:");
        PType_ImVoCu_Unit -> setText("V");
        PType_Impedance_EditLine -> setText(QString("%1").arg(DPort_default.VO_Voltage));
        break;
    case 2:
        qDebug()<<"在执行Impendence3的语句";
        Ptype_ImVocu -> setText("current");
        PType_ImVoCu_Unit -> setText("A");
        PType_Impedance_EditLine -> setText(QString("%1").arg(DPort_default.CU_current));
        break;
    default:
        break;
    }

    //点击不同的按钮显示不同的标签
    connect(RadioBtn_SPara,&QPushButton::clicked,this,[=](){
        qDebug()<<"在执行Impendence的语句11";
        Ptype_ImVocu -> setText("Impedance:");
        PType_ImVoCu_Unit -> setText("Ohm");
    });
    connect(RadioBtn_Volta,&QPushButton::clicked,this,[=](){
        qDebug()<<"在执行Impendence的语句22";
        Ptype_ImVocu -> setText("Voltrage:");
        PType_ImVoCu_Unit -> setText("V");
    });
    connect(RadioBtn_Curre,&QPushButton::clicked,this,[=](){
        qDebug()<<"在执行Impendence的语句33";
        Ptype_ImVocu -> setText("current");
        PType_ImVoCu_Unit -> setText("A");
    });

    QHBoxLayout* PortType_Layout4_0 = new QHBoxLayout;
    PortType_Layout4_0 -> addWidget(PType_Impedance_EditLine);
    PortType_Layout4_0 -> addWidget(PType_ImVoCu_Unit);
    DP_formlayout0 -> addRow(Ptype_ImVocu,PortType_Layout4_0);

    //第六行
    QLabel* PType_Radius = new  QLabel(tr("Radius:"));
    QLineEdit* PType_Radius_EditLine = new QLineEdit;
    PType_Radius_EditLine -> setFixedWidth(180);
    PType_Radius_EditLine -> setText(QString("%1").arg(DPort_default.DP_Radious));// 根据传递进来的数据设定显示

    DP_formlayout0 -> addRow(PType_Radius,PType_Radius_EditLine);

    //第七行
    QCheckBox* PType_Mvac = new QCheckBox(tr("Monitor voltage and current"));
    if(DPort_default.Mon_vo_cu == 0){// 根据传递进来的数据设定显示
        PType_Mvac -> setChecked(true);
    }
    if(DPort_default.Mon_vo_cu == 1){
        PType_Mvac -> setChecked(false);
    }

    //第一个框的总布局
    QVBoxLayout* DPort_VLayout = new QVBoxLayout;
    DPort_VLayout -> addLayout(DP_formlayout0);
    DPort_VLayout -> addWidget(PType_Mvac);
    Property_group -> setLayout(DPort_VLayout);


    //设置第二个框
    QGroupBox* DPLocation_group = new QGroupBox(tr("Location"));
    QFormLayout* DPLocation_form = new QFormLayout;

    //按钮
    QLabel* DPLo_type = new QLabel(tr("Type:"));
    QRadioButton* DPLo_Btn_Coor = new QRadioButton(tr("Coordinates"));
    QRadioButton* DPLo_Btn_Wire = new QRadioButton(tr("Wire"));
    DPLo_Btn_Coor -> setChecked(true);
    DPLo_Btn_Wire -> setCheckable(false);

    QLabel* DPLo_X1 = new QLabel("X1");
    QLabel* DPLo_Y1 = new QLabel("Y1");
    QLabel* DPLo_Z1 = new QLabel("Z1");

    QLineEdit* DPlo_X1_edit = new QLineEdit;
    QLineEdit* DPlo_Y1_edit = new QLineEdit;
    QLineEdit* DPlo_Z1_edit = new QLineEdit;
    DPlo_X1_edit -> setFixedWidth(100);
    DPlo_Y1_edit -> setFixedWidth(100);
    DPlo_Z1_edit -> setFixedWidth(100);

    QCheckBox* Lo_usepick1 = new QCheckBox(tr("Use pick"));
    Lo_usepick1 -> setCheckable(false);

    QGridLayout* DPLO_editline1_layout = new QGridLayout;
    DPLO_editline1_layout -> addWidget(DPLo_type,0,0);
    DPLO_editline1_layout -> addWidget(DPLo_Btn_Coor,0,1);
    DPLO_editline1_layout -> addWidget(DPLo_Btn_Wire,0,2);
    DPLO_editline1_layout -> addWidget(DPLo_X1,1,0);
    DPLO_editline1_layout -> addWidget(DPLo_Y1,1,1);
    DPLO_editline1_layout -> addWidget(DPLo_Z1,1,2);
    DPLO_editline1_layout -> addWidget(DPlo_X1_edit,2,0);
    DPLO_editline1_layout -> addWidget(DPlo_Y1_edit,2,1);
    DPLO_editline1_layout -> addWidget(DPlo_Z1_edit,2,2);
    DPLO_editline1_layout -> addWidget(Lo_usepick1,2,3);

    QLabel* DPLo_X2 = new QLabel("X2");
    QLabel* DPLo_Y2 = new QLabel("Y2");
    QLabel* DPLo_Z2 = new QLabel("Z2");

    QLineEdit* DPlo_X2_edit = new QLineEdit;
    QLineEdit* DPlo_Y2_edit = new QLineEdit;
    QLineEdit* DPlo_Z2_edit = new QLineEdit;
    DPlo_X2_edit -> setFixedWidth(100);
    DPlo_Y2_edit -> setFixedWidth(100);
    DPlo_Z2_edit -> setFixedWidth(100);


    // 连接坐标输入框信号
    auto updateCoordinateVars = [=]() {
        bool ok;
        // X坐标
        double x1 = DPlo_X1_edit->text().toDouble(&ok);
        if (ok) ::min_x = x1;

        double x2 = DPlo_X2_edit->text().toDouble(&ok);
        if (ok) ::max_x = x2;

        // Y坐标
        double y1 = DPlo_Y1_edit->text().toDouble(&ok);
        if (ok) ::min_y = y1;

        double y2 = DPlo_Y2_edit->text().toDouble(&ok);
        if (ok) ::max_y = y2;

        // Z坐标
        double z1 = DPlo_Z1_edit->text().toDouble(&ok);
        if (ok) ::min_z = z1;

        double z2 = DPlo_Z2_edit->text().toDouble(&ok);
        if (ok) ::max_z = z2;
    };
    connect(DPlo_X1_edit, &QLineEdit::textChanged, this, updateCoordinateVars);
    connect(DPlo_Y1_edit, &QLineEdit::textChanged, this, updateCoordinateVars);
    connect(DPlo_Z1_edit, &QLineEdit::textChanged, this, updateCoordinateVars);
    connect(DPlo_X2_edit, &QLineEdit::textChanged, this, updateCoordinateVars);
    connect(DPlo_Y2_edit, &QLineEdit::textChanged, this, updateCoordinateVars);
    connect(DPlo_Z2_edit, &QLineEdit::textChanged, this, updateCoordinateVars);


    QCheckBox* Lo_usepick2 = new QCheckBox(tr("Use pick"));
    Lo_usepick2 -> setCheckable(false);

    DPLO_editline1_layout -> addWidget(DPLo_X2,3,0);
    DPLO_editline1_layout -> addWidget(DPLo_Y2,3,1);
    DPLO_editline1_layout -> addWidget(DPLo_Z2,3,2);
    DPLO_editline1_layout -> addWidget(DPlo_X2_edit,4,0);
    DPLO_editline1_layout -> addWidget(DPlo_Y2_edit,4,1);
    DPLO_editline1_layout -> addWidget(DPlo_Z2_edit,4,2);
    DPLO_editline1_layout -> addWidget(Lo_usepick2,4,3);

    // // 新增：端口方向行
    // QLabel* DPLo_PortDirection = new QLabel(tr("端口方向:"));
    // QLineEdit* DPLo_PortDirection_Edit = new QLineEdit;
    // DPLo_PortDirection_Edit -> setFixedWidth(100);
    // DPLO_editline1_layout -> addWidget(DPLo_PortDirection,5,0);
    // DPLO_editline1_layout -> addWidget(DPLo_PortDirection_Edit,5,1,1,2); // 跨两列

    // // 连接端口方向输入框信号
    // connect(DPLo_PortDirection_Edit, &QLineEdit::textChanged, this, [=](const QString &text){
    //     ::DiscretePorts_direction = text;
    // });

    QLabel* DPLo_PortDirection = new QLabel(tr("端口方向:"));
    QComboBox* DPLo_PortDirection_Combo = new QComboBox;
    DPLo_PortDirection_Combo->addItems({"xn", "xp", "yn", "yp", "zn", "zp"});
    DPLo_PortDirection_Combo->setFixedWidth(100);
    DPLO_editline1_layout->addWidget(DPLo_PortDirection,5,0);
    DPLO_editline1_layout->addWidget(DPLo_PortDirection_Combo,5,1,1,2); // 跨两列



    // 连接端口方向组合框信号
    connect(DPLo_PortDirection_Combo, &QComboBox::currentTextChanged, this, [=](const QString &text){
        ::DiscretePorts_direction = text;
    });


    QLabel* LO_temp1 = new QLabel;//空白行
    DPLO_editline1_layout -> addWidget(LO_temp1,6,0);

    QComboBox* Lo_combox1 = new QComboBox;
    DPLO_editline1_layout -> addWidget(Lo_combox1,7,0,1,3);

    DPLO_editline1_layout -> addWidget(LO_temp1,8,0);

    QCheckBox* Lo_Invert_checkbox = new QCheckBox(tr("Invert direction"));
    DPLO_editline1_layout -> addWidget(Lo_Invert_checkbox,9,0,1,1);

    QLabel* Lo_Position_label = new QLabel(tr("Position:"));
    QComboBox* Lo_combox2 = new QComboBox;
    QString Lo_combox2_option1 = "end1";
    Lo_combox2 ->addItem(Lo_combox2_option1);
    Lo_combox2 -> setEnabled(false);

    QHBoxLayout* Lo_Poend1_layout = new QHBoxLayout;
    Lo_Poend1_layout -> addWidget(Lo_Position_label);
    Lo_Poend1_layout -> addWidget(Lo_combox2);
    DPLO_editline1_layout -> addLayout(Lo_Poend1_layout,9,2);
    // DPLO_editline1_layout -> addWidget(Lo_combox2,8,3);

    DPLocation_form -> addItem(DPLO_editline1_layout);
    DPLocation_group -> setLayout(DPLocation_form);

    //右侧按钮键
    QPushButton* DP_Btn_OK = new QPushButton(tr("OK"));
    QPushButton* DP_Btn_Cancel = new QPushButton(tr("Cancel"));
    QPushButton* DP_Btn_Apply = new QPushButton(tr("Apply"));
    QPushButton* DP_Btn_Preview = new QPushButton(tr("Preview"));
    QPushButton* DP_Btn_Help = new QPushButton(tr("Help"));
    QGridLayout* Btn_Layout = new QGridLayout;
    Btn_Layout -> addWidget(DP_Btn_OK,0,0);
    Btn_Layout -> addWidget(DP_Btn_Cancel,1,0);
    Btn_Layout -> addWidget(DP_Btn_Apply,2,0);
    Btn_Layout -> addWidget(DP_Btn_Preview,3,0);
    Btn_Layout -> addWidget(DP_Btn_Help,4,0);

    //总布局
    QGridLayout* DPort_total_layout = new QGridLayout(this);
    DPort_total_layout -> addWidget(Property_group,1,0);
    DPort_total_layout -> addLayout(Btn_Layout,1,1);
    DPort_total_layout -> addWidget(DPLocation_group,2,0);

    connect(DP_Btn_OK, &QPushButton::clicked, this, [=](){
        // 在关闭对话框前确保所有值都已更新
        updateCoordinateVars();
        ::DiscretePorts_direction = DPLo_PortDirection_Combo->currentText();

        // 输出全局变量
        printGlobalVariables();

        // 获取应用程序目录路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString fileName = QDir::cleanPath(appDir + "/DiscretePorts.txt");

        // 确定源类型字符串
        QString sourceTypeStr;
        switch(::source_type_int) {
        case 0:
            sourceTypeStr = "s_parameter";  // S-Parameter
            break;
        case 1:
            sourceTypeStr = "voltage_source";  // Voltage
            break;
        case 2:
            sourceTypeStr = "current_source";  // Current
            break;
        default:
            sourceTypeStr = "s_parameter";  // 默认值
            break;
        }

        // 生成DiscretePorts.txt文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写入文件头
            out << "======================================================================================\n";
            out << "port_order\tsource_type\tmin_x(m)\tmax_x(m)\tmin_y(m)\tmax_y(m)\tmin_z(m)\tmax_z(m)\tdirection\tR(Ω)\twaveform\n";
            out << "======================================================================================\n";

            // 写入端口数量
            out << "1\n";

            // 写入端口数据
            out << "1\t"  // 端口号，固定为1
                << sourceTypeStr << "\t"  // 源类型
                << ::min_x << "\t"  // min_x
                << ::max_x << "\t"  // max_x
                << ::min_y << "\t"  // min_y
                << ::max_y << "\t"  // max_y
                << ::min_z << "\t"  // min_z
                << ::max_z << "\t"  // max_z
                << ::DiscretePorts_direction << "\t"  // 方向
                << ::impedance << "\t"  // 阻抗
                << "waveform1\n";  // 固定为waveform1

            file.close();
            qDebug() << "DiscretePorts.txt文件已生成到:" << fileName;
        } else {
            qDebug() << "无法创建DiscretePorts.txt文件，目标路径:" << fileName;
        }

        this->accept();


    });
    // connect(DP_Btn_Apply, &QPushButton::clicked, this, [=](){
    //     updateCoordinateVars();

    //     // 输出全局变量
    //     printGlobalVariables();


    // });

    connect(DP_Btn_Apply, &QPushButton::clicked, this, [=](){
        updateCoordinateVars();

        // 输出全局变量
        printGlobalVariables();

        // 获取应用程序目录路径
        QString appDir = QCoreApplication::applicationDirPath();
        QString fileName = QDir::cleanPath(appDir + "/DiscretePorts.txt");

        // 确定源类型字符串
        QString sourceTypeStr;
        switch(::source_type_int) {
        case 0:
            sourceTypeStr = "s_parameter";  // S-Parameter
            break;
        case 1:
            sourceTypeStr = "voltage_source";  // Voltage
            break;
        case 2:
            sourceTypeStr = "current_source";  // Current
            break;
        default:
            sourceTypeStr = "s_parameter";  // 默认值
            break;
        }

        // 生成DiscretePorts.txt文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写入文件头
            out << "======================================================================================\n";
            out << "port_order\tsource_type\tmin_x(m)\tmax_x(m)\tmin_y(m)\tmax_y(m)\tmin_z(m)\tmax_z(m)\tdirection\tR(Ω)\twaveform\n";
            out << "======================================================================================\n";

            // 写入端口数量
            out << "1\n";

            // 写入端口数据
            out << "1\t"  // 端口号，固定为1
                << sourceTypeStr << "\t"  // 源类型
                << ::min_x << "\t"  // min_x
                << ::max_x << "\t"  // max_x
                << ::min_y << "\t"  // min_y
                << ::max_y << "\t"  // max_y
                << ::min_z << "\t"  // min_z
                << ::max_z << "\t"  // max_z
                << ::DiscretePorts_direction << "\t"  // 方向
                << ::impedance << "\t"  // 阻抗
                << "waveform1\n";  // 固定为waveform1

            file.close();
            qDebug() << "DiscretePorts.txt文件已生成到:" << fileName;
        } else {
            qDebug() << "无法创建DiscretePorts.txt文件，目标路径:" << fileName;
        }
    });

}

DPort_data *simulationDisP::creat_DPordefault()
{
    return new DPort_data;
}


