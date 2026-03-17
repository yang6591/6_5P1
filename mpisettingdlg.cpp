#include "mpisettingdlg.h"

mpisettingdlg::mpisettingdlg(MPI_Data* MPI_Data1, QWidget* parent)
    : QDialog(parent){
    MPI_Data& MPI_default = MPI_Data1 == nullptr ? *creat_MPIinfo_default() : *MPI_Data1;
    setWindowTitle(tr("Mesh -- Settings"));

    //创建对话框:第一个框
    QGroupBox* MPI_Info_group = new QGroupBox(tr("MPI Information："));

    QLabel* MPI_info_boder_x = new QLabel(tr("Boder X: "));
    QLabel* MPI_info_boder_y = new QLabel(tr("Boder Y: "));
    QLabel* MPI_info_boder_z = new QLabel(tr("Boder Z: "));

    MPI_info_boder_x_EditLine = new QLineEdit;
    // QString Show_MPI_boder_x = QString::number(MPI_default.MPI_Info_boder_x);
    // MPI_info_boder_x_EditLine -> setText(Show_MPI_boder_x);
    MPI_info_boder_x_EditLine -> setText(QString::number(MPI_default.MPI_Info_boder_x));
    MPI_info_boder_x_EditLine -> setFixedWidth(180);

    MPI_info_boder_y_EditLine = new QLineEdit;
    // QString Show_MPI_boder_y = QString::number(MPI_default.MPI_Info_boder_y);
    // MPI_info_boder_y_EditLine -> setText(Show_MPI_boder_y);
    MPI_info_boder_y_EditLine -> setText(QString::number(MPI_default.MPI_Info_boder_y));
    MPI_info_boder_y_EditLine -> setFixedWidth(180);

    MPI_info_boder_z_EditLine = new QLineEdit;
    // QString Show_MPI_boder_z = QString::number(MPI_default.MPI_Info_boder_z);
    // MPI_info_boder_z_EditLine -> setText(Show_MPI_boder_z);
    MPI_info_boder_z_EditLine -> setText(QString::number(MPI_default.MPI_Info_boder_z));
    MPI_info_boder_z_EditLine -> setFixedWidth(180);


    QLabel* MPI_info_Frequency = new QLabel(tr("Frenquency: "));
    MPI_info_Freq_EditLine = new QLineEdit;
    MPI_info_Freq_EditLine -> setText(tr("3"));
    QLabel* MPI_info_Wave = new QLabel(tr("Wave: "));
    MPI_info_Wave_EditLine = new QLineEdit;
    MPI_info_Wave_EditLine->setReadOnly(true); // 设置为只读
    MPI_info_Wave_EditLine->setEnabled(false);

    // QString Show_MPI_wave = QString::number(MPI_default.MPI_Info_wave);
    // MPI_info_Wave_EditLine -> setText(Show_MPI_wave);
    MPI_info_Wave_EditLine -> setText(QString::number(MPI_default.MPI_Info_wave));
    MPI_info_Wave_EditLine -> setFixedWidth(180);

    QLabel* MPI_info_minx = new QLabel(tr("dx-min: "));
    QLabel* MPI_info_miny = new QLabel(tr("dy-min: "));
    QLabel* MPI_info_minz = new QLabel(tr("dz-min: "));

    MPI_info_minx_EditLine = new QLineEdit;
    // QString Show_min_x = QString::number(MPI_default.MPI_Info_min_x);
    // MPI_info_minx_EditLine -> setText(Show_min_x);
    float default_MPI_Info_min_x=0.05;
    MPI_info_minx_EditLine -> setText(QString::number(default_MPI_Info_min_x));
    qDebug()<<default_MPI_Info_min_x;
    MPI_info_minx_EditLine -> setFixedWidth(180);

    MPI_info_miny_EditLine = new QLineEdit;
    // QString Show_min_y = QString::number(MPI_default.MPI_Info_min_y);
    // MPI_info_miny_EditLine -> setText(Show_min_y);
    float default_MPI_Info_min_y=0.05;
    MPI_info_miny_EditLine -> setText(QString::number(default_MPI_Info_min_y));
    MPI_info_miny_EditLine -> setFixedWidth(180);

    MPI_info_minz_EditLine = new QLineEdit;
    // QString Show_min_z = QString::number(MPI_default.MPI_Info_min_z);
    // MPI_info_minz_EditLine -> setText(Show_min_z);
    float default_MPI_Info_min_z=0.05;
    MPI_info_minz_EditLine -> setText(QString::number(default_MPI_Info_min_z));
    MPI_info_minz_EditLine -> setFixedWidth(180);

    QComboBox* dx_mpi_Options = new QComboBox;
    dx_mpi_Options->addItem(tr("m"));
    dx_mpi_Options->addItem(tr("cm"));
    dx_mpi_Options->addItem(tr("mm"));
    dx_mpi_Options->addItem(tr("um"));
    dx_mpi_Options->addItem(tr("nm"));
    dx_mpi_Options->addItem(tr("ft"));
    dx_mpi_Options->setCurrentIndex(2);
    QComboBox* dy_mpi_Options = new QComboBox;
    dy_mpi_Options->addItem(tr("m"));
    dy_mpi_Options->addItem(tr("cm"));
    dy_mpi_Options->addItem(tr("mm"));
    dy_mpi_Options->addItem(tr("um"));
    dy_mpi_Options->addItem(tr("nm"));
    dy_mpi_Options->addItem(tr("ft"));
    dy_mpi_Options->setCurrentIndex(2);
    QComboBox* dz_mpi_Options = new QComboBox;
    dz_mpi_Options->addItem(tr("m"));
    dz_mpi_Options->addItem(tr("cm"));
    dz_mpi_Options->addItem(tr("mm"));
    dz_mpi_Options->addItem(tr("um"));
    dz_mpi_Options->addItem(tr("nm"));
    dz_mpi_Options->addItem(tr("ft"));
    dz_mpi_Options->setCurrentIndex(2);


    QLabel* MPI_info_dstr = new QLabel(tr("dstrictgrid: "));//网格划分严格度
    QLabel* MPI_info_deci = new QLabel(tr("decimalPlaces: "));//输出文件小数位数
    MPI_info_dstr_EditLine = new QLineEdit;
    // QString Show_dstrictgrid = QString::number(MPI_default.MPI_info_dstrictgrid);
    // MPI_info_dstr_EditLine  -> setText(Show_dstrictgrid);
    MPI_info_dstr_EditLine  -> setText(QString::number(MPI_default.MPI_info_dstrictgrid));
    MPI_info_dstr_EditLine  -> setFixedWidth(180);
    MPI_info_deci_EditLine = new QLineEdit;
    // QString Show_decimalPlaces = QString::number(MPI_default.MPI_info_decimalPlaces);
    // MPI_info_deci_EditLine  -> setText(Show_decimalPlaces);
    MPI_info_deci_EditLine  -> setText(QString::number(MPI_default.MPI_info_decimalPlaces));
    MPI_info_deci_EditLine  -> setFixedWidth(180);

    //core-number
    // groupBox_core = new QGroupBox(tr("Calculated using default cores:"));
    // groupBox_core -> setCheckable(true);

    QLabel* MPI_core_number = new QLabel(tr("Core: "));
    MPI_info_core_EditLine = new QLineEdit;
    float default_MPI_info_core_number=1;
    MPI_info_core_EditLine -> setText(QString::number(default_MPI_info_core_number));
    MPI_info_core_EditLine -> setFixedWidth(180);
    MPI_info_core_EditLine->setEnabled(false);

    // QVBoxLayout* Group_core_Layout = new QVBoxLayout;
    // Group_core_Layout -> addWidget(MPI_core_number);
    // Group_core_Layout -> addWidget(MPI_info_core_EditLine);
    // groupBox_core -> setLayout(Group_core_Layout);
    core_checkBox = new QCheckBox(tr("Number of custom processes:"));
    core_checkBox -> setChecked(0);
    // connect(core_checkBox, &QCheckBox::stateChanged, [=](){
    //     // if (MPI_info_core_EditLine) {
    //     //     MPI_info_core_EditLine->setEnabled(1);
    //     // }else{
    //     //     MPI_info_core_EditLine->setEnabled(0);
    //     // }
    //     if (MPI_info_core_EditLine) {
    //         MPI_info_core_EditLine->setEnabled(checked); // 当checked为true时启用，为false时禁用
    //     }
    // });
    connect(core_checkBox, &QCheckBox::stateChanged, [=](int state){
        if (MPI_info_core_EditLine) {
            MPI_info_core_EditLine->setEnabled(state != 0); // 更简单的检查
        }
    });

    //进行标签和名字的布局
    QGridLayout* MPI_information_layout = new QGridLayout;
    // MPI_information_layout -> addWidget(MPI_info_boder_x,0,0);
    // MPI_information_layout -> addWidget(MPI_info_boder_x_EditLine,0,1);
    // MPI_information_layout -> addWidget(MPI_info_boder_y,1,0);
    // MPI_information_layout -> addWidget(MPI_info_boder_y_EditLine,1,1);
    // MPI_information_layout -> addWidget(MPI_info_boder_z,2,0);
    // MPI_information_layout -> addWidget(MPI_info_boder_z_EditLine,2,1);

    // MPI_information_layout -> addWidget(MPI_info_Frequency,0,0);
    // MPI_information_layout -> addWidget(MPI_info_Freq_EditLine,0,1);
    // MPI_information_layout -> addWidget(MPI_info_Wave,1,0);
    // MPI_information_layout -> addWidget(MPI_info_Wave_EditLine,1,1);

    QLabel* MPI_info_statistic = new QLabel(tr("Statistics: "));//统计数字
    MPI_information_layout -> addWidget(MPI_info_statistic,0,0);

    MPI_information_layout -> addWidget(MPI_info_minx,1,0);
    MPI_information_layout -> addWidget(MPI_info_minx_EditLine,1,1);
    MPI_information_layout -> addWidget(MPI_info_miny,2,0);
    MPI_information_layout -> addWidget(MPI_info_miny_EditLine,2,1);
    MPI_information_layout -> addWidget(MPI_info_minz,3,0);
    MPI_information_layout -> addWidget(MPI_info_minz_EditLine,3,1);
    //dx_mpi_Options
    MPI_information_layout -> addWidget(dx_mpi_Options,1,2);
    MPI_information_layout -> addWidget(dy_mpi_Options,2,2);
    MPI_information_layout -> addWidget(dz_mpi_Options,3,2);

    // MPI_information_layout -> addWidget(MPI_info_dstr,7,0);
    // MPI_information_layout -> addWidget(MPI_info_dstr_EditLine,7,1);
    // MPI_information_layout -> addWidget(MPI_info_deci,8,0);
    // MPI_information_layout -> addWidget(MPI_info_deci_EditLine,8,1);

    //groupBox的
    //MPI_information_layout -> addWidget(groupBox_core,8,0);

    MPI_information_layout -> addWidget(core_checkBox,4,0);
    MPI_information_layout -> addWidget(MPI_core_number,5,0);
    MPI_information_layout -> addWidget(MPI_info_core_EditLine,5,1);

    // MPI_information_layout -> addWidget(MPI_core_number,9,0);
    // MPI_information_layout -> addWidget(MPI_info_core_EditLine,9,1);

    // //添加频率单位显示
    // QComboBox* Fre_mpi_Options = new QComboBox;
    // Fre_mpi_Options->addItem(tr("Hz"));
    // Fre_mpi_Options->addItem(tr("KHz"));
    // Fre_mpi_Options->addItem(tr("MHz"));
    // Fre_mpi_Options->addItem(tr("GHz"));
    // Fre_mpi_Options->addItem(tr("THz"));
    // Fre_mpi_Options->addItem(tr("PHz"));
    // int Fre_mpi_UnitIndex = 2;
    // Fre_mpi_Options->setCurrentIndex(Fre_mpi_UnitIndex);
    // MPI_information_layout -> addWidget(Fre_mpi_Options,0,2);
    // //添加波长单位显示
    // QComboBox* wave_mpi_Options = new QComboBox;
    // wave_mpi_Options->addItem(tr("m"));
    // wave_mpi_Options->addItem(tr("cm"));
    // wave_mpi_Options->addItem(tr("mm"));
    // wave_mpi_Options->addItem(tr("um"));
    // wave_mpi_Options->addItem(tr("nm"));
    // wave_mpi_Options->addItem(tr("ft"));
    // wave_mpi_Options->setCurrentIndex(Fre_mpi_UnitIndex);
    // MPI_information_layout -> addWidget(wave_mpi_Options,1,2);


    //添加按钮
    QPushButton* MPI_Btn_OK = new QPushButton("OK");
    QPushButton* MPI_Btn_No = new QPushButton("Cancel");
    QPushButton* MPI_Btn_Calculate = new QPushButton("Calculate");
    MPI_information_layout -> addWidget(MPI_Btn_OK,0,5);
    MPI_information_layout -> addWidget(MPI_Btn_No,1,5);
    MPI_information_layout -> addWidget(MPI_Btn_Calculate,2,5);

    //添加标签模块，判断进行到那一步
    QLabel* MPI_split_title = new QLabel(tr("Splitting process: "));
    QLabel* MPI_grid_line = new QLabel(tr("Grid line division: "));
    MPI_grid_line0 = new QLabel(tr("Calculating......."));
    MPI_grid_line1 = new QLabel(tr("Ready"));
    MPI_grid_line2 = new QLabel(tr("-"));
    QLabel* MPI_interior_fill = new QLabel(tr("Interior Fill: "));
    MPI_interior_fill0 = new QLabel(tr("Calculating......."));
    MPI_interior_fill1 = new QLabel(tr("Ready"));
    MPI_interior_fill2 = new QLabel(tr("-"));
    QLabel* MPI_txt_ouput = new QLabel(tr("File output: "));
    MPI_txt_ouput0 = new QLabel(tr("Calculating......."));
    MPI_txt_ouput1 = new QLabel(tr("Ready"));
    MPI_txt_ouput2 = new QLabel(tr("-"));

    MPI_information_layout -> addWidget(MPI_split_title,10,0);
    MPI_information_layout -> addWidget(MPI_grid_line,11,0);
    //通过在同一个位置放置三个标签，然后在通过可视化来选择显示出哪一个。
    MPI_information_layout -> addWidget(MPI_grid_line0,11,1);
    MPI_information_layout -> addWidget(MPI_grid_line1,11,1);
    MPI_information_layout -> addWidget(MPI_grid_line2,11,1);
    //ture显示，其他隐藏
    MPI_grid_line0 -> setVisible(false);
    MPI_grid_line1 -> setVisible(false);
    MPI_grid_line2 -> setVisible(true);

    MPI_information_layout -> addWidget(MPI_interior_fill,12,0);
    MPI_information_layout -> addWidget(MPI_interior_fill0,12,1);
    MPI_information_layout -> addWidget(MPI_interior_fill1,12,1);
    MPI_information_layout -> addWidget(MPI_interior_fill2,12,1);
    MPI_interior_fill0 -> setVisible(false);
    MPI_interior_fill1 -> setVisible(false);
    MPI_interior_fill2 -> setVisible(true);

    MPI_information_layout -> addWidget(MPI_txt_ouput,13,0);
    MPI_information_layout -> addWidget(MPI_txt_ouput0,13,1);
    MPI_information_layout -> addWidget(MPI_txt_ouput1,13,1);
    MPI_information_layout -> addWidget(MPI_txt_ouput2,13,1);
    MPI_txt_ouput0 -> setVisible(false);
    MPI_txt_ouput1 -> setVisible(false);
    MPI_txt_ouput2 -> setVisible(true);

    // //为后面的文件创建一个新的MPI_Data结构体
    // MPI_Data* MPI_Information_txt = new MPI_Data;

    //捕获信息
    connect(MPI_Btn_OK, &QPushButton::clicked, this, [=](){
        MPI_capture = new MPI_Data;
        qDebug() << "收集数据完成";
        // MPI_capture -> MPI_Info_boder_x = MPI_info_boder_x_EditLine->text().toDouble();
        // MPI_capture -> MPI_Info_boder_y = MPI_info_boder_y_EditLine->text().toDouble();
        // MPI_capture -> MPI_Info_boder_z = MPI_info_boder_z_EditLine->text().toDouble();
        // MPI_capture -> MPI_Info_wave = MPI_info_Wave_EditLine->text().toDouble();

        double freqValue_show = MPI_info_Freq_EditLine->text().toDouble();
        //double waveValue_show = freqValue_show / 3.0;
        double waveValue_show = 10;
        QString waveString = QString::number(waveValue_show, 'f', 2); // 假设你想要两位小数
        MPI_info_Wave_EditLine -> setText(waveString);
        MPI_capture -> MPI_Info_wave = MPI_info_Wave_EditLine->text().toDouble();

        MPI_capture -> MPI_Info_min_x = MPI_info_minx_EditLine->text().toDouble();
        MPI_capture -> MPI_Info_min_y = MPI_info_miny_EditLine->text().toDouble();
        MPI_capture -> MPI_Info_min_z = MPI_info_minz_EditLine->text().toDouble();
        // MPI_capture -> MPI_info_decimalPlaces = MPI_info_deci_EditLine->text().toInt();
        // MPI_capture -> MPI_info_dstrictgrid = MPI_info_dstr_EditLine->text().toInt();
        // MPI_capture -> MPI_info_core_number = MPI_info_core_EditLine->text().toInt();

        qDebug() << "MPI_Info_boder_x: " << MPI_capture -> MPI_Info_boder_x;
        qDebug() << "MPI_Info_boder_y: " << MPI_capture -> MPI_Info_boder_y;
        qDebug() << "MPI_Info_boder_z: " << MPI_capture -> MPI_Info_boder_z;
        qDebug() << "收集数据完成2";

        //将捕获信息的结构体指针传递给MPI_Information_txt
        //MPI_Information_txt = MPI_capture;

        //emit this -> mpiDataChanged(MPI_capture);
        emit this-> mpiDataChanged(MPI_capture);
        //mpiDataChanged
        qDebug() << "收集数据完成3";
        MPI_core_number_default = MPI_capture -> MPI_info_core_number;
        qDebug()<<"第一次core number is ： "<< MPI_core_number_default;

        //close();
    });
    //将数据保存到文件夹之中
    //connect(MPI_Btn_OK, &QPushButton::clicked, this, &mpisettingdlg::MPI_Information_txt);
    //进行运算
    //connect(MPI_Btn_Calculate,&QPushButton::clicked,this,&mpisettingdlg::process_start);
    //connect(MPI_Btn_Calculate,&QPushButton::clicked,this,&mpisettingdlg::process_start);
    connect(MPI_Btn_Calculate,&QPushButton::clicked,this,&mpisettingdlg::process_start_show);

    //总布局
    QGridLayout* MPI_total_layout = new QGridLayout(this);
    MPI_total_layout -> addLayout(MPI_information_layout,1,0);
}

//以下三个函数是进行剖分的函数
void mpisettingdlg:: process_start()
{
    MPI_grid_line0 -> setVisible(true);//网格划分-计算中
    MPI_grid_line1 -> setVisible(false);
    MPI_grid_line2 -> setVisible(false);
    MPI_interior_fill0 -> setVisible(true);//内部填充-计算中
    MPI_interior_fill1 -> setVisible(false);
    MPI_interior_fill2 -> setVisible(false);
    MPI_txt_ouput0 -> setVisible(true);//文件输出-计算中
    MPI_txt_ouput1 -> setVisible(false);
    MPI_txt_ouput2 -> setVisible(false);


    qDebug()<<"正在计算";
    QProcess* myprocess = new QProcess(this);
    //myprocess->setWorkingDirectory("E:\Java\MPIandInfo_Qt\W_MPI\x64\Debug");
    //myprocess->setWorkingDirectory("E:/Java/MPI_Qt/x64/Debug");//第二次
    //myprocess->setWorkingDirectory("E:/Java/MPI_Qt/x64");
    //myprocess->setWorkingDirectory("E:/Java/newMPI01/x64/Debug");//第一次注释
    QStringList arguments;

    //进行修改，将变量2改为读取界面的变量
    qDebug()<<"core number is： "<< MPI_core_number_default;
    //int MPI_corenumber = 1;
    //arguments << "-n" << "2" << "MPI_Qt.exe";
    //arguments << "-n" << QString::number(MPI_corenumber) << "MPI_Qt.exe";// QString::number(MPI_corenumber)在这将int变为string类型
    //arguments << "-n" << QString::number(MPI_core_number_default) << "MPI_Qt.exe";
    //myprocess->start("mpiexec", arguments);

    // //延迟
    // QTimer::singleShot(10000, this, [this]() { // 10000毫秒后执行
    //     // 设置文本输出的可见性
    //     MPI_txt_ouput0->setVisible(true);
    //     MPI_txt_ouput1->setVisible(false); // 注意：这里你可能想要设置为true或其他状态，取决于你的需求
    //     MPI_txt_ouput2->setVisible(false); // 同样，这里可能也需要调整
    // });

    // 连接QProcess的信号以获取输出和错误
    // connect(myprocess, &QProcess::readyReadStandardOutput, this, &mpisettingdlg::Simulation_start_Output);
    // connect(myprocess, &QProcess::readyReadStandardError, this, &mpisettingdlg::Simulation_start_Error);
    // connect(myprocess, &QProcess::finished, myprocess, &QProcess::deleteLater); // 自动删除process对象
}

//处理标准输出
void mpisettingdlg::Simulation_start_Output()
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (process) {
        Standardout_number++;
        //qDebug() << "Standardout_number的次数是：" << Standardout_number;
        QByteArray output = process->readAllStandardOutput();
        qDebug() << "Standard Output:" << output;
        QString utf8String = QString::fromUtf8(output);
        qDebug() << "Standard Output (UTF-8):" << utf8String;
        //进行次数的判断 File之前是5次，File是第六次，File之后是4-5次，所以只要如此划分就可以
        // 当输出包含特定关键词时更新状态
        if (utf8String.contains("File Number", Qt::CaseSensitive)) {
            // 更新网格划分状态为"完成"
            MPI_grid_line0 -> setVisible(false);
            MPI_grid_line1 -> setVisible(true);
            MPI_grid_line2 -> setVisible(false);
            qDebug() << "String contains 'File Number'.";
            // 更新内部填充状态为"完成"
            MPI_interior_fill0 -> setVisible(false);
            MPI_interior_fill1 -> setVisible(true);
            MPI_interior_fill2 -> setVisible(false);
            // MPI_txt_ouput0 -> setVisible(false);
            // MPI_txt_ouput1 -> setVisible(true);
            // MPI_txt_ouput2 -> setVisible(false);
            // 延迟更新文件输出状态
            QTimer::singleShot(8000, this, [this]() { // 10000毫秒后执行
                // 设置文本输出的可见性
                MPI_txt_ouput0->setVisible(false);
                MPI_txt_ouput1->setVisible(true);
                MPI_txt_ouput2->setVisible(false);
            });
        }
    }
    //if(){}
}

//捕获并记录MPI进程的错误输出
void mpisettingdlg::Simulation_start_Error()
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (process) {
        QByteArray output = process->readAllStandardError();
        qDebug() << "Standard Error:" << output;
    }
}

//将用户设置的MPI参数写入文本文件
void mpisettingdlg::MPI_Information_txt()
{
    //qDebug() << "目前到txt还算正常";
    qDebug() << "在txt中保存的" << MPI_capture -> MPI_Info_boder_x;
    // QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    //                    + "wavedata.txt";
    //QString filePath = "E:/Java/wavedata.txt";
    QString filePath = "E:/qt/6_5P1/wavedata.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入：" << filePath;
        return;
    }
    QTextStream out(&file);
    //out << "MPI_Info_boder_x:测试第一行" << "\n";
    out << "border_x: " << MPI_capture -> MPI_Info_boder_x << "\n";
    out << "border_y: " << MPI_capture -> MPI_Info_boder_y << "\n";
    out << "border_z: " << MPI_capture -> MPI_Info_boder_z << "\n";
    out << "wave: " << MPI_capture -> MPI_Info_wave << "\n";
    out << "minx: " << MPI_capture -> MPI_Info_min_x << "\n";
    out << "miny: " << MPI_capture -> MPI_Info_min_y << "\n";
    out << "minz: " << MPI_capture -> MPI_Info_min_z << "\n";
    out << "dstrictgrid: " << MPI_capture -> MPI_info_dstrictgrid << "\n";
    out << "decimalPlaces: " << MPI_capture -> MPI_info_decimalPlaces << "\n";
    //out << "info_core_number:" << MPI_capture -> MPI_info_core_number << "\n";

    file.close();
    qDebug() << "数据已保存到mpi_data.txt";
}

//通过定时器模拟网格计算过程（当前实际使用的函数）
void mpisettingdlg::process_start_show()
{
    MPI_grid_line0 -> setVisible(true);
    MPI_grid_line1 -> setVisible(false);
    MPI_grid_line2 -> setVisible(false);
    MPI_interior_fill0 -> setVisible(true);
    MPI_interior_fill1 -> setVisible(false);
    MPI_interior_fill2 -> setVisible(false);
    MPI_txt_ouput0 -> setVisible(true);
    MPI_txt_ouput1 -> setVisible(false);
    MPI_txt_ouput2 -> setVisible(false);

    QTimer::singleShot(300, this, [this]() { // 10000毫秒后执行
        // 设置文本输出的可见性
        MPI_grid_line0 -> setVisible(false);
        MPI_grid_line1 -> setVisible(true);
        MPI_grid_line2 -> setVisible(false);

        MPI_interior_fill0 -> setVisible(false);
        MPI_interior_fill1 -> setVisible(true);
        MPI_interior_fill2 -> setVisible(false);
    });
    QTimer::singleShot(1000, this, [this]() { // 10000毫秒后执行
        // 设置文本输出的可见性
        MPI_txt_ouput0->setVisible(false);
        MPI_txt_ouput1->setVisible(true);
        MPI_txt_ouput2->setVisible(false);
    });
}

//直接将所有状态设置为"完成"
void mpisettingdlg::updateLabels_show()
{
    //qDebug()<<123;
    MPI_grid_line0 -> setVisible(false);
    MPI_grid_line1 -> setVisible(true);//网格划分-完成
    MPI_grid_line2 -> setVisible(false);

    MPI_interior_fill0 -> setVisible(false);
    MPI_interior_fill1 -> setVisible(true);//内部填充-完成
    MPI_interior_fill2 -> setVisible(false);
}

//生成包含默认值的MPI参数结构体
MPI_Data *mpisettingdlg::creat_MPIinfo_default()
{
    return new MPI_Data;
}
