#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "meshshow.h"
#include "spshow.h"
#include "dirshow.h"
#include "fielddiagramshow.h"
#include "adaptivemeshsetting.h"
#include "simulationsetup.h"
#include "simulationmonitor.h"
#include "simulationmeshstruc.h"
#include "simulationresultset.h"
#include "unitssettingdialog.h"
#include "farfieldobvpoint.h"
#include "meshsettingsdialog.h"
#include "global.h"

#include <QColorDialog> //颜色选择
#include <QComboBox>
#include <QDebug> //输出
#include <QDialog> //对话框
#include <QFile> //文件
#include <QFileDialog> //文件对话框
#include <QFormLayout> //排版
#include <QHBoxLayout> //水平排版
#include <QInputDialog>
#include <QLabel> //标签
#include <QLineEdit> //文字编辑
#include <QMenuBar> //菜单栏
#include <QMessageBox> //消息窗
#include <QPushButton> //按钮
#include <QStatusBar> //状态
#include <QString>
#include <QTextEdit> //文本
#include <QThread>
#include <QToolBar> // 工具栏
#include <QVBoxLayout> //竖直排版

//#include <QProcess>  // 用于启动外部程序


// 全局变量，设定的最大，最小频率值
// 在主界面只用声明全局变量，在引用界面声明额外变量extern即可
double Freminvalue;
double Fremaxvalue;

// 索引序数
int DimUnitIndex;
int FreUnitIndex;
int TimUnitIndex;

QComboBox* DimOptions;
QComboBox* FreOptions;
QComboBox* TimeOptions;

std::vector<Probe_data*> MainWindow::dataVector;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , plane(nullptr)
    , probe(nullptr)
    , dport(nullptr)
    , postProcess(nullptr)
    , Freminvalue(0.0)  // 初始化频率最小值
    , Fremaxvalue(0.0)  // 初始化频率最大值
//,adaptivemeshProcess(nullptr)// 初始化进程指针
{
    ui->setupUi(this);

    setMinimumSize(800, 600);


    //清除探针设置
    // 清空 NearFieldProbe.txt 文件
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/NearFieldProbe.txt");
    QString fileName2 = QDir::cleanPath(appDir + "/FarFieldFrequencyPoint.txt");
    QString fileName3 = QDir::cleanPath(appDir + "/FarFieldObservationPoint.txt");
    QString fileName4 = QDir::cleanPath(appDir + "/unit.txt");

    //需要添加近场探针和远场观察频点才能进行仿真
    //每次打开主窗口表示一次新的仿真 需要写入全部的探针和远场观察频点
    // QFile file(fileName);
    // if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    //     file.close();
    //     qDebug() << "NearFieldProbe.txt 文件已清空";
    // }
    // QFile file2(fileName2);
    // if (file2.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    //     file2.close();
    //     qDebug() << "FarFieldFrequencyPoint.txt 文件已清空";
    // }
    // QFile file3(fileName3);
    // if (file3.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    //     file3.close();
    //     qDebug() << "FarFieldObservationPoint.txt 文件已清空";
    // }
    // QFile file4(fileName4);
    // if (file4.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    //     file4.close();
    //     qDebug() << "unit.txt 文件已清空";
    // }




    //设置主窗口的中心部件为布局
    QWidget* centralWidget = new QWidget(this);

    //创建窗口布局
    QHBoxLayout* gl_layout = new QHBoxLayout(centralWidget);

    //添加openglwidget窗口
    glWidget = new openglwidget(centralWidget);

    glWidget -> setGeometry(60,60,60,60);
    gl_layout -> addWidget(glWidget);

    //中心区部件只能有一个
    setCentralWidget(glWidget);
    // 窗口大小
    resize(600, 400);




    m_simulationSetUp = new SimulationSetUp(this);  // 父窗口设为MainWindow，自动管理内存
    m_simulationSetUp = new SimulationSetUp(this);




    // 菜单栏创建  菜单栏最多只能有一个
    QMenuBar* mainmenuBar = menuBar(); // 主菜单栏
    // 菜单栏放到窗口中
    setMenuBar(mainmenuBar);
    // 创建主菜单
    QMenu* mmFile = mainmenuBar->addMenu("File");
    QMenu* mmHome = mainmenuBar->addMenu("Home");
    QMenu* mmModel = mainmenuBar->addMenu("Modeling");
    QMenu* mmSimul = mainmenuBar->addMenu("Simulation");
    QMenu* mmPProc = mainmenuBar->addMenu("Post-Processing");
    QMenu* mmView = mainmenuBar->addMenu("View");
    QMenu* mmOther = mainmenuBar->addMenu("Other");

    // 创建File项
    // 建立参数QAction 接受打开，保存这些行为
    QAction* mmFopen = mmFile->addAction("Open");
    mmFile->addSeparator();
    QAction* mmFsave = mmFile->addAction("Save");
    QAction* mmsavas = mmFile->addAction("Save As");
    mmFile->addSeparator();
    QAction* mmother = mmFile->addAction("Other");

    // Home项
    QAction* mUnits = mmHome->addAction("Units");
    mmHome->addSeparator();
    mmHome->addAction("Simulation Project");
    mmHome->addAction("Start Simulation");
    mmHome->addSeparator();
    QAction* mHHaveDone = mmHome->addAction("Have Done");
    mmHome->addSeparator();
    mmHome->addAction("Others");

    // Model项
    //************************************************************************
    // 创建子菜单(Import/Export)
    QMenu *subMenu = mmModel->addMenu("Import/Export");

    // 创建一个 WidgetAction 用于显示加粗标签(import)
    QWidgetAction *labelAction_imp = new QWidgetAction(subMenu);
    QLabel *label_imp = new QLabel("<b>Import</b>");  // 使用 HTML 加粗
    label_imp->setMargin(5);  // 设置边距
    labelAction_imp->setDefaultWidget(label_imp);
    // 添加到菜单（先加标签，再加选项）
    subMenu->addAction(labelAction_imp);
    // 单个"导入材料"按钮
    QAction* loadMaterial = subMenu->addAction("导入材料");
    // 初始化36种不同颜色 (HSV色相循环)

    for (int i = 0; i < 36; i++) {
        float hue = (i * 10.0f); // 每10度一个颜色 (0-360度)
        QColor qtColor = QColor::fromHsv(hue, 180, 220); // 中等饱和度和亮度
        materialColors.append(QVector3D(
            qtColor.redF(),
            qtColor.greenF(),
            qtColor.blueF()
            ));

    }
    // 连接信号到槽函数
    connect(loadMaterial, &QAction::triggered, [=]() {
        actLoadMaterial_triggered(); // 新增的统一材料导入函数
    });
    // connect(loadpec , SIGNAL(triggered()), this, SLOT(actLoadpec_triggered()));
    // connect(loadmaterial1 , SIGNAL(triggered()), this, SLOT(actLoadMedium1_triggered()));
    // connect(loadmaterial2 , SIGNAL(triggered()), this, SLOT(actLoadMedium2_triggered()));
    // connect(loadmaterial3 , SIGNAL(triggered()), this, SLOT(actLoadMedium3_triggered()));
    // connect(loadmaterial4 , SIGNAL(triggered()), this, SLOT(actLoadMedium4_triggered()));
    // connect(loadmaterial5 , SIGNAL(triggered()), this, SLOT(actLoadMedium5_triggered()));
    // connect(loadair , SIGNAL(triggered()), this, SLOT(actLoadVacuum_triggered()));

    //export
    QWidgetAction *labelAction_exp = new QWidgetAction(subMenu);
    QLabel *label_exp = new QLabel("<b>Export</b>");  // 使用 HTML 加粗
    label_exp->setMargin(5);  // 设置边距
    labelAction_exp->setDefaultWidget(label_exp);
    // 添加到菜单（先加标签，再加选项）
    subMenu->addAction(labelAction_exp);
    subMenu->addAction("Submenu Item 1");
    subMenu->addAction("Submenu Item 2");
    //*********************************************************

    //
    QMenu *drawsubMenu = mmModel->addMenu("draw");
    QAction* drawBox=drawsubMenu->addAction("Box");
    connect(drawBox, &QAction::triggered, [=]() {
        DrawModel::BoxParams params = DrawModel::getBoxParams(this);
        if (!params.name.isEmpty()) {
            // 创建长方体
            int modelId = glWidget->createBox(
                params.name,
                params.xmin, params.xmax,
                params.ymin, params.ymax,
                params.zmin, params.zmax,
                static_cast<openglwidget::MaterialType>(params.materialType)
                );

            if (modelId != -1) {
                // 在导航树中添加条目
                QTreeWidgetItem* boxItem = new QTreeWidgetItem(m_compGroup1);
                boxItem->setText(0, params.name);
                m_compGroup1->setExpanded(true);
                m_treeItemToModelId[boxItem] = modelId;
            }
        }
    });
    QAction* drawSphere=drawsubMenu->addAction("Sphere");
    mmModel->addSeparator();
    mmModel->addAction("Background");
    mmModel->addAction("Material Library");
    //mmModel->addAction("Material Parameter");
    QAction* mmmaterailpara = mmModel->addAction("Material Parameter");
    mmModel->addSeparator();

    mmModel->addAction("Others");




    // 创建Simulation项


    mmSimul->addSeparator();
    QAction* mmsFrequency = mmSimul->addAction("Frequency");
    mmSimul->addAction("Background");
    mmSimul->addAction("Boundaries");
    mmSimul->addSeparator();
    QAction* mmDportShow = mmSimul->addAction("Discrete Port");
    QAction* mmPlanWaveShow = mmSimul->addAction("Plane Wave");
    mmSimul->addSeparator();
    QAction* mmMonitorShow = mmSimul->addAction("Field Monitor");
    QAction* mmProbeShow = mmSimul->addAction("Field Probe");
    QAction* mmFarObvPointShow = mmSimul->addAction("FarFieldObvPoint setting");
    mmSimul->addSeparator();
    QAction* mmStartUpSimul = mmSimul->addAction("Start Up Simulation");
    mmSimul->addSeparator();
    QAction*MeshPropertiesAction = mmSimul->addAction("Mesh Properties");
    QAction*AdaptiveMeshSetAction = mmSimul->addAction("Set--AdaptiveMesh");
    QAction* Sim_start = mmSimul->addAction("Mesh -- Settings");
    QAction* Sim_meshshow = mmSimul->addAction("Mesh -- Show");
    mmSimul->addSeparator();
    QAction* mmMeshStrucShow = mmSimul->addAction("Mesh Structure");
    QAction* mmResultSettingShow = mmSimul->addAction("Result Setting");


    //频率部分添加到主窗口cpp中设置
    // 设置频率
    connect(mmsFrequency, &QAction::triggered, [=]() {
        QDialog* mmsfredlg = new QDialog;
        mmsfredlg->setWindowTitle(QString::fromUtf8("Frequency Range Settings"));
        mmsfredlg->setMinimumSize(400, 200);
        mmsfredlg->setMaximumSize(400, 200);

        QLineEdit* sFminlineEdit = new QLineEdit(mmsfredlg);
        sFminlineEdit->setReadOnly(false);
        QLineEdit* sFmaxlineEdit = new QLineEdit(mmsfredlg);
        sFmaxlineEdit->setReadOnly(false);

        // 给输入框加提示占位符
        sFminlineEdit->setPlaceholderText("Enter min frequency/GHz (e.g. 1.0)");
        sFmaxlineEdit->setPlaceholderText("Enter max frequency/GHz (e.g. 10.0)");

        QPushButton* FreBtnok = new QPushButton("ok", mmsfredlg);
        QPushButton* FreBtnno = new QPushButton("cancel", mmsfredlg);

        // 布局
        QFormLayout* Frelayout = new QFormLayout();
        Frelayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
        Frelayout->addRow("Min.frequency:", sFminlineEdit);
        Frelayout->addRow("Max.frequency:", sFmaxlineEdit);

        QHBoxLayout* BottomLayout = new QHBoxLayout();
        BottomLayout->addStretch();
        BottomLayout->addWidget(FreBtnok);
        BottomLayout->addWidget(FreBtnno);

        QGridLayout* mainFreLayout = new QGridLayout(mmsfredlg);
        mainFreLayout->setSpacing(15);
        mainFreLayout->addLayout(Frelayout, 0, 0);
        mainFreLayout->addLayout(BottomLayout, 1, 0);

        connect(FreBtnok, &QPushButton::clicked, [=]()  {

            qDebug() << "OK按钮被点击了，开始处理频率设置";
            // 1. 从输入框解析用户输入的最小频率（tempMin）和最大频率（tempMax）
            bool okMin, okMax;
            double tempMin = sFminlineEdit->text().toDouble(&okMin);  // 最小频率输入框
            double tempMax = sFmaxlineEdit->text().toDouble(&okMax);  // 最大频率输入框


            if (!okMin || !okMax) {
                //qDebug() << "验证失败：输入不是有效数字";
                QMessageBox::warning(mmsfredlg, "输入错误", "请输入有效的频率值（GHz）");
                return;
            }
            if (tempMin < 0 || tempMax <= 0) {
                //qDebug() << "验证失败：频率小于等于0";
                QMessageBox::warning(mmsfredlg, "输入错误", "频率值必须大于0");
                return;
            }
            if (tempMin >= tempMax) {
                //qDebug() << "验证失败：最小频率不小于最大频率";
                QMessageBox::critical(mmsfredlg, "输入错误", "最小频率必须小于最大频率");
                return;
            }

            // 3. 将频率值保存到 MainWindow 的成员变量中
            Freminvalue = tempMin;
            Fremaxvalue = tempMax;
            g_freMin = tempMin;  // 存入全局变量
            g_freMax = tempMax;

            qDebug() << "[频率设置] g_freMin=" << g_freMin << "，地址=" << &g_freMin;
            qDebug() << "[频率设置] g_freMax=" << g_freMax << "，地址=" << &g_freMax;

            // 生成FrequencyRange.txt文件
            QString appDir = QCoreApplication::applicationDirPath();
            QString fileName = QDir::cleanPath(appDir + "/FrequencyRange.txt");

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);

                // 写入文件头
                out << "========================\n";
                out << "lower_frequency\tupper_frequency\n";
                out << "========================\n";

                // 写入频率数据，将GHz转换为Hz（乘以E9）
                out << QString::number(tempMin, 'f', 6) + "E9" << "\t"
                    << QString::number(tempMax, 'f', 6) + "E9" << "\n";

                file.close();
                qDebug() << "FrequencyRange.txt文件已生成到:" << fileName;
                qDebug() << "频率范围:" << tempMin << "GHz 到" << tempMax << "GHz";
            } else {
                qDebug() << "无法创建FrequencyRange.txt文件，目标路径:" << fileName;
            }


            mmsfredlg->close();
        });

        connect(FreBtnno, &QPushButton::clicked, [=]() {
            mmsfredlg->close();
        });

        mmsfredlg->exec();
        mmsfredlg->setAttribute(Qt::WA_DeleteOnClose);
    });

    //*****************************
    //仿真开始设置窗口
    connect(mmStartUpSimul, &QAction::triggered, this, &MainWindow::onSetUpSimulationTriggered);


    //**************************************
    //调用自适应网格剖分

    connect(AdaptiveMeshSetAction, &QAction::triggered, this, &MainWindow::onSetAdaptiveMeshTriggered);
    // QAction* runAdaptiveMeshAction = mmSimul->addAction("Run-AdaptiveMeshing");
    // connect(runAdaptiveMeshAction, &QAction::triggered, this, &MainWindow::onAdaptiveMeshingActionTriggered);



    //打开仿真界面的窗口
    connect(Sim_start,&QAction::triggered,[=](){
        qDebug()<<"目前正确";
        MPI_setting_showWidget = new mpisettingdlg(this->mpiinfo, this);
        qDebug() << "第一次MPI_Info_boder_x: ";
        //qDebug() << "第一次MPI_Info_boder_y: " << mpiinfo  -> MPI_Info_boder_y;
        //qDebug() << "第一次MPI_Info_boder_z: " << mpiinfo  -> MPI_Info_boder_z;
        MPI_setting_showWidget -> setAttribute(Qt::WA_DeleteOnClose);
        //connect(Simulation_Pro_Showwidget, SIGNAL(Probe_emit(Probe_data*)), this, SLOT(pro_add_ok(Probe_data*)));

        //connect(MPI_setting_showWidget,SIGNAL(mpiDataChanged(MPI_Data*)),this,SLOT(MPI_changedata(MPI_Data*)));

        // qDebug() << "第2次MPI_Info_boder_x: " << mpiinfo  -> MPI_Info_boder_x;
        // qDebug() << "第2次MPI_Info_boder_y: " << mpiinfo  -> MPI_Info_boder_y;
        // qDebug() << "第2次MPI_Info_boder_z: " << mpiinfo  -> MPI_Info_boder_z;
        MPI_setting_showWidget -> show();
        //connect(MPI_setting_showWidget,SIGNAL(mpiDataChanged(MPI_Data*)),this,&MainWindow::process_start);
        // SimulationShowWidget = new SimulationDlg(this->plane, this);
        // SimulationShowWidget->setAttribute(Qt::WA_DeleteOnClose); // 析构函数要注意！！
        // connect(SimulationShowWidget, SIGNAL(plane_ok(Plane_data*)), this, SLOT(add_ok(Plane_data*))); // 点击确认，数据保存
        // SimulationShowWidget->show();
    });






    //connect(TreeTrunk, &QTreeWidget::itemClicked, this, &MainWindow::onMeshItemClicked);
    connect(Sim_meshshow, &QAction::triggered, this, &MainWindow::onMeshActionTriggered);




    // connect(Sim_meshshow,&QAction::triggered, [=]() {
    //     QDialog* meshshowdlg = new QDialog;
    //     meshshowdlg->setWindowTitle(QString::fromUtf8("截面网格显示"));
    //     // meshshowdlg->setMinimumSize(400, 200);
    //     // meshshowdlg->setMaximumSize(400, 300);

    //     QLabel* meshshow_label = new QLabel(meshshowdlg);
    //     // QString meshimagePath = QString("C:\\Users\\lenovo\\Desktop\\微信图片_20250610154722.jpg");
    //     // QPixmap pixmap(meshimagePath);
    //     // if (pixmap.isNull()) {
    //     //     qDebug() << "Failed to load pixmap from" << meshimagePath;
    //     //     meshshowdlg->close();
    //     //     return;
    //     // }

    //     // 调整标签大小以适应图片
    //     //meshshow_label->setPixmap(pixmap.scaled(meshshowdlg->size(), Qt::KeepAspectRatio,Qt::SmoothTransformation));

    //     // 设置布局并添加标签
    //     QVBoxLayout* meshshow_layout = new QVBoxLayout(meshshowdlg);
    //     meshshow_layout->addWidget(meshshow_label);
    //     meshshowdlg->setLayout(meshshow_layout);

    //     meshshowdlg->exec();
    //     meshshowdlg->setAttribute(Qt::WA_DeleteOnClose);
    // });
    mmSimul->addSeparator();
    mmSimul->addAction("Others");

    // Post-Processing项
    mmPProc->addAction("Import/Export");
    QAction* runPostProcAction = mmPProc->addAction("Run-Import/Export");

    mmPProc->addSeparator();
    mmPProc->addAction("Others");

    //***********
    connect(runPostProcAction, &QAction::triggered, this, &MainWindow::onPostProcessingActionTriggered);


    // View项
    QAction* NavTree = mmView->addAction("Nav.Tree");
    mmView->addAction("Others");

    // Others项
    mmOther->addAction("Others");



    //解决菜单栏焦点问题

    // 在构造函数末尾添加以下代码
    QTimer::singleShot(0, this, [this]() {//延迟到下一个事件循环
        // 强制菜单栏重绘和激活
        menuBar()->setVisible(true);//强制重新计算菜单栏的可见状态，可能解决某些窗口管理器的显示问题
        menuBar()->setEnabled(true);//确保菜单栏处于活动状态，能够接收鼠标和键盘事件
        menuBar()->raise();//将菜单栏提升到Z轴最上层，确保没有其他部件遮挡它

        // 重置焦点链
        QWidget::setTabOrder(menuBar(), glWidget);//Qt通过焦点链确定Tab键切换焦点的顺序
        //OpenGL Widget可能无意中成为了焦点链的起点，导致键盘/鼠标焦点处理异常
        //明确设置焦点顺序，告诉Qt："先关注菜单栏，再关注OpenGL Widget"
    });

    //构造函数执行 → UI创建 → 事件循环开始 → 延迟函数执行 → 菜单栏状态重置

    // 确保菜单栏有正确的Z-order
    menuBar()->stackUnder(glWidget);




    // 导航树**********************************************************************************************************************************/
    //  //创建子窗口
    //  QDockWidget * NavgTreeWidget = new QDockWidget(this);
    //  NavgTreeWidget -> setWindowTitle(tr("Navigation Tree"));
    //  NavgTreeWidget -> setMinimumWidth(150);
    //  NavgTreeWidget -> setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //  addDockWidget(Qt::LeftDockWidgetArea,NavgTreeWidget);
    //  //导航树
    //  NavTreeWidgetShow = new TreeWidget(NavgTreeWidget);
    //  NavTreeWidgetShow ->show();

    // // QGridLayout * TreeShowLeft = new QGridLayout(NavgTreeWidget);
    // // TreeShowLeft -> addWidget(NavTreeWidgetShow,1,0);//通过布局，让TreeWidget铺满整个界面
    // NavgTreeWidget -> setWidget(NavTreeWidgetShow);
    // // NavgTreeWidget ->setLayout(TreeShowLeft);
    // connect(NavTree,SIGNAL(triggered()),NavgTreeWidget,SLOT(show()));
    /**********************************************************************************************************************************/

    /**********************************************************************************************************************************/
    // 创建子窗口
    QDockWidget* NavgTreeWidget = new QDockWidget(this);
    NavgTreeWidget->setWindowTitle(tr("Navigation Tree"));
    NavgTreeWidget->setMinimumWidth(140);
    NavgTreeWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, NavgTreeWidget);

    // 点击按钮打开导航树
    connect(NavTree, SIGNAL(triggered()), NavgTreeWidget, SLOT(show()));

    // 创建NavigationTree（导航树在子窗口中）
    //  QTreeWidget  *TreeTrunk;//导航树中的主干
    TreeTrunk = new QTreeWidget(NavgTreeWidget);
    TreeTrunk->setHeaderHidden(true); // 取消header的默认值1

    // 设置上下文菜单策略
    // 修改选中模式为多选
    TreeTrunk->setSelectionMode(QAbstractItemView::ExtendedSelection);  // 允许Ctrl/Shift多选
    TreeTrunk->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(TreeTrunk, &QTreeWidget::customContextMenuRequested, [=](const QPoint &pos){
        QList<QTreeWidgetItem*> selectedItems = TreeTrunk->selectedItems();

        if(selectedItems.isEmpty()) return;

        QMenu menu;
        QAction* deleteAction = menu.addAction("删除模型");

        QAction* selectedAction = menu.exec(TreeTrunk->viewport()->mapToGlobal(pos));

        if(selectedAction == deleteAction) {
            // 处理多选删除
            for(QTreeWidgetItem* item : selectedItems) {
                if(item && item->parent() == m_compGroup1) {
                    // 获取关联的模型ID
                    int modelId = m_treeItemToModelId.value(item, -1);
                    if (modelId != -1) {
                        // 删除OpenGL模型
                        glWidget->removeModelById(modelId);
                        // 从映射中移除
                        m_treeItemToModelId.remove(item);
                    }
                    // 删除树节点
                    delete item;
                }
            }
        }
    });


    QGridLayout* TreeShowLeft = new QGridLayout;
    TreeShowLeft->addWidget(TreeTrunk, 1, 0); // 通过布局，让TreeWidget铺满整个界面
    NavgTreeWidget->setWidget(TreeTrunk); // 这个语句，让导航树自动填充满DockWidget窗口
    // 创建分支节点
    //**************************************************************************
    QTreeWidgetItem* CompGroup1 = new QTreeWidgetItem(TreeTrunk); // component
    // 强制显示加减号（即使没有子项也会显示）
    CompGroup1->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    m_compGroup1 = CompGroup1; // 在创建CompGroup1后保存


    // 在构造函数中添加连接(模型单独显示部分）
    connect(TreeTrunk, &QTreeWidget::itemDoubleClicked, [=](QTreeWidgetItem *item, int column) {
        Q_UNUSED(column);

        QList<QTreeWidgetItem*> selectedItems = TreeTrunk->selectedItems();

        if (selectedItems.isEmpty()) {
            glWidget->resetHighlight();
            return;
        }

        // 收集所有选中的模型ID
        QList<int> modelIds;
        for(QTreeWidgetItem* selectedItem : selectedItems) {
            if(selectedItem->parent() == m_compGroup1) {
                int modelId = m_treeItemToModelId.value(selectedItem, -1);
                if(modelId != -1) {
                    modelIds.append(modelId);
                }
            }
        }

        if(!modelIds.isEmpty()) {
            // 高亮所有选中的模型
            glWidget->highlightModels(modelIds);
        } else {
            glWidget->resetHighlight();
        }
    });
    // 添加空白区域点击重置(模型单独显示部分）
    //TreeTrunk->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(TreeTrunk, &QTreeWidget::clicked, [=](const QModelIndex &index) {
        if (!index.isValid()) {
            glWidget->resetHighlight();
        }
    });
    //****************************************************************************

    QTreeWidgetItem* CompGroup2 = new QTreeWidgetItem(TreeTrunk); // Materials
    QTreeWidgetItem* CompGroup3 = new QTreeWidgetItem(TreeTrunk); // Lumped Elements
    QTreeWidgetItem* CompGroup4 = new QTreeWidgetItem(TreeTrunk); // Plane wave
    // TreeTrunk->setContextMenuPolicy(Qt::CustomContextMenu);

    // connect(ui->treeWidget_EditFlow,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slots_showrightMenu(QPoint)));
    //  SIGNAL(customContextMenuRequested(QPoint)) -> #arg "customContextMenuRequested(QPoint)"
    //  //右键显示快捷菜单
    //  connect(TreeTrunk,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slots_showrightMenu(QPoint)));
    //  CompGroup4 -> setContextMenuPolicy(Qt::CustomContextMenu);
    QTreeWidgetItem* CompGroup5 = new QTreeWidgetItem(TreeTrunk); // Ports
    QTreeWidgetItem* CompGroup6 = new QTreeWidgetItem(TreeTrunk); // Excitation Signals
    QTreeWidgetItem* CompGroup7 = new QTreeWidgetItem(TreeTrunk); // Probes
    QTreeWidgetItem* CompGroup8 = new QTreeWidgetItem(TreeTrunk); // Field Monitors
    QTreeWidgetItem* CompGroup9 = new QTreeWidgetItem(TreeTrunk); // 1D results
    QTreeWidgetItem* CompGroup10 = new QTreeWidgetItem(TreeTrunk); // 2D/3D results
    QTreeWidgetItem* CompGroup11 = new QTreeWidgetItem(TreeTrunk); // 2D/3D results
    CompGroup1->setText(0, "Component");
    mapping1 << &MainWindow::Component_delete;
    mapping2 << &MainWindow::Component_add;
    treemapDlg << &MainWindow::Component_douClick;

    CompGroup2->setText(0, "Materials");
    mapping1 << &MainWindow::Materials_delete;
    mapping2 << &MainWindow::Materials_add;
    treemapDlg << &MainWindow::Materials_douClick;

    CompGroup3->setText(0, "Lumped Elements");
    mapping1 << &MainWindow::Lumped_delete;
    mapping2 << &MainWindow::Lumped_add;
    treemapDlg << &MainWindow::Lumped_douClick;

    CompGroup4->setText(0, "Plane wave");
    mapping1 << &MainWindow::plane_delete;
    mapping2 << &MainWindow::plane_add;
    treemapDlg << &MainWindow::plane_douClick;

    CompGroup5->setText(0, "Ports");
    mapping1 << &MainWindow::Ports_delete;
    mapping2 << &MainWindow::Ports_add;
    treemapDlg << &MainWindow::Ports_douClick;

    CompGroup6->setText(0, "Excitation Signals");
    mapping1 << &MainWindow::Excitation_delete;
    mapping2 << &MainWindow::Excitation_add;
    treemapDlg << &MainWindow::Excitation_douClick;

    CompGroup7->setText(0, "Probes");
    mapping1 << &MainWindow::Probes_delete;
    mapping2 << &MainWindow::Probes_add;
    treemapDlg << &MainWindow::Probes_douClick;


    //添加probes分支
    QTreeWidgetItem* probesSignals_p = new QTreeWidgetItem(CompGroup7);
    probesSignals_p->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    probesSignals_p->setText(0, "probes");

    m_probesSignalsItem_p = probesSignals_p;

    // connect(TreeTrunk, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item){
    //     if (item == m_probesSignalsItem_p) {
    //         updateProbeTreeFromFile();
    //     }
    // });
    connect(TreeTrunk, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item){
        if (item == m_probesSignalsItem_p) {
            updateProbeTreeFromFile(m_probesSignalsItem_p);
        }
    });


    CompGroup8->setText(0, "Field Monitors");
    CompGroup9->setText(0, "1D results");
    CompGroup10->setText(0, "2D/3D results");
    CompGroup11->setText(0, "Farfields");
    // 设置图标
    QIcon icon1;
    icon1.addPixmap(QPixmap("E:/qt/6_5P1/JPG/Picture1.png"), QIcon::Selected);
    icon1.addPixmap(QPixmap("E:/qt/6_5P1/JPG/Picture1.png"), QIcon::Normal);
    CompGroup1->setIcon(0, icon1);
    CompGroup2->setIcon(0, icon1);
    CompGroup3->setIcon(0, icon1);
    CompGroup4->setIcon(0, icon1);
    CompGroup5->setIcon(0, icon1);
    CompGroup6->setIcon(0, icon1);
    CompGroup7->setIcon(0, icon1);
    CompGroup8->setIcon(0, icon1);
    CompGroup9->setIcon(0, icon1);
    CompGroup10->setIcon(0, icon1);
    CompGroup11->setIcon(0, icon1);

    //添加分支
    CompGroup9->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    // 创建子分支
    QTreeWidgetItem* S_Parameter = new QTreeWidgetItem(CompGroup9);
    S_Parameter->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    S_Parameter->setText(0, "S-Parameter");
    // QTreeWidgetItem* RCS = new QTreeWidgetItem(CompGroup9);
    // RCS->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    // RCS->setText(0, "RCS");

    QTreeWidgetItem* portSignals = new QTreeWidgetItem(CompGroup9);
    portSignals->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    portSignals->setText(0, "port signals");

    QTreeWidgetItem* power = new QTreeWidgetItem(CompGroup9);
    power->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    power->setText(0, "power");

    QTreeWidgetItem* energy = new QTreeWidgetItem(CompGroup9);
    energy->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    energy->setText(0, "energy");

    QTreeWidgetItem* probes = new QTreeWidgetItem(CompGroup9);
    probes->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    probes->setText(0, "probes");

    QTreeWidgetItem* crossSections = new QTreeWidgetItem(CompGroup9);
    crossSections->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    crossSections->setText(0, "cross sections");

    // probes 再次分支
    QTreeWidgetItem* eFields = new QTreeWidgetItem(probes);
    eFields->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    eFields->setText(0, "E-fields");

    QTreeWidgetItem* rcs = new QTreeWidgetItem(probes);
    rcs->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    rcs->setText(0, "RCS");

    // E-fields 分支
    QTreeWidgetItem* probesSignals = new QTreeWidgetItem(eFields);
    probesSignals->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    probesSignals->setText(0, "probes signals");

    m_probesSignalsItem = probesSignals;


    // 在创建完所有树形结构之后，添加连接
    // 连接probesSignals项的展开信号，以动态加载探针
    connect(TreeTrunk, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item){
        if (item == m_probesSignalsItem) {
            updateProbeTreeFromFile(m_probesSignalsItem);
        }
    });




    CompGroup10->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    // 创建E和H子分支
    QTreeWidgetItem* branchE = new QTreeWidgetItem(CompGroup10);
    branchE->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    branchE->setText(0, "E");
    //m_branchE = branchE; // 保存E分支指针（如果需要）

    QTreeWidgetItem* branchH = new QTreeWidgetItem(CompGroup10);
    branchH->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    branchH->setText(0, "H");
    //m_branchH = branchH; // 保存H分支指针（如果需要）

    CompGroup11->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    // 创建子分支
    QTreeWidgetItem* Fre3d = new QTreeWidgetItem(CompGroup11);
    Fre3d->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    Fre3d->setText(0, "Farfields_3D");


    QTreeWidgetItem* Fre1 = new QTreeWidgetItem(CompGroup11);
    Fre1->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    Fre1->setText(0, "Farfields(f1)");

    QTreeWidgetItem* Fre2 = new QTreeWidgetItem(CompGroup11);
    Fre2->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    Fre2->setText(0, "Farfields(f2)");


    connect(TreeTrunk, &QTreeWidget::itemClicked, this, &MainWindow::onItemClicked);

    /**********************************************************************************************************************************/

    // 2024.1.13 需要将工具栏取消，放置Treewidget。
    // 工具栏可以有多个
    //  QToolBar * mmtoolbar = new QToolBar(this);//放在对象树上，QWidget
    //  addToolBar(Qt::LeftToolBarArea,mmtoolbar);
    //  //设置移动总开关：现在关闭
    //  mmtoolbar ->setMovable(false);
    //  //只允许停靠左右两侧
    //  mmtoolbar ->setAllowedAreas( Qt::LeftToolBarArea | Qt::RightToolBarArea);
    //  //禁止浮动
    //  mmtoolbar ->setFloatable(false);
    // //工具栏中内容设置
    //  mmtoolbar ->addAction(mmFopen);//将保存行为也放在工具栏中，保证文件内的打开和工具栏中的打开都是公用的
    //  mmtoolbar ->addSeparator();
    //  mmtoolbar ->addAction(mmFsave);

    // 状态栏  最多只能有一个
    QStatusBar* mmstbar = statusBar();
    setStatusBar(mmstbar);
    // 放置提示信息
    QLabel* mmlabel = new QLabel("提示信息:  ", this);
    mmstbar->addWidget(mmlabel);

    // 铆接部件（浮动窗口）可以有多个
    QDockWidget* mmdockWidget = new QDockWidget("浮动窗口", this);
    addDockWidget(Qt::BottomDockWidgetArea, mmdockWidget);
    // 停靠区域
    mmdockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    // // 中心区域部件（暂时放置文本编辑器）只能有一个%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // QTextEdit* pracedit = new QTextEdit(this);
    // setCentralWidget(pracedit);

    // 以上均为变量的创建，现在开始建立信号和槽之间的关联
    // 文件打开
    connect(mmFopen, &QAction::triggered, [=]() {
        // 对话框分类
        // 1.模态对话框 ：弹出对话框的时候，不可以对其他窗口进行操作
        // 2.非模态对话框 ： 弹出对话框的时候，可以对其他窗口进行操作
        // 模态对话框使用exec（）--模态显示
        // 非模态对话框使用show（）--非模态显示
        // 创建模态对话框  ----阻塞功能：点击按钮后，停止在dlg.exec()处，只有关闭对话框，才继续执行qDebug()语句
        // QDialog mmfodlg(this);//在lambda中的函数体写了this，则在[]中需要写“=”
        // 这个是在栈上创建的，因为在下方阻塞了所以没有释放掉  （23.12.27--对比139行）
        // mmfodlg.resize(400,200);
        // mmfodlg.setWindowTitle(QString::fromUtf8("Open"));//设置名字

        // mmfodlg.exec();
        // mmfodlg.setAttribute(Qt::WA_DeleteOnClose);//Qt::WA_DeleteOnClose 55号属性,关闭对话框后自动删除内存
        // 若无此语句，则在关闭整个窗口的时候才会释放内存
        // 在open界面加入读取文件
        // folineEdit = new QLineEdit(mmfodlg);

        // QString mfopenpath = QFileDialog::getOpenFileName(this,"打开文件","C:\\Users\\Li\\Desktop"); //读取文件路径

        // 设置打开界面对话框
        QDialog* mmfodlg = new QDialog;
        mmfodlg->setWindowTitle(QString::fromUtf8("Open"));
        mmfodlg->resize(500, 100);
        // 设置选择，打开按钮
        QPushButton* OCFilekBtn = new QPushButton("Choose", mmfodlg);
        OCFilekBtn->setGeometry(10, 10, 100, 30);
        QPushButton* OOFileBtn = new QPushButton("OK", mmfodlg);
        OOFileBtn->setGeometry(10, 10, 100, 30);
        // 设置路径读取窗口
        QLineEdit* folineEdit = new QLineEdit(mmfodlg);
        folineEdit->setReadOnly(true); // 设置权限：只读

        // 按钮和显示路径布局
        QFormLayout* FOpenFIlelayout = new QFormLayout();
        FOpenFIlelayout->setRowWrapPolicy(QFormLayout::WrapAllRows); // QFormLayout::WrapAllRows：文本框总是出现在标签后面
        FOpenFIlelayout->addRow("Open File Path:", folineEdit);
        QHBoxLayout* HOpenfileLayout = new QHBoxLayout();
        HOpenfileLayout->addStretch();
        HOpenfileLayout->addWidget(OCFilekBtn);
        HOpenfileLayout->addWidget(OOFileBtn);
        // 将两个模块合二为一
        QGridLayout* GOpenfileLayout = new QGridLayout(mmfodlg);
        GOpenfileLayout->setSpacing(15);
        GOpenfileLayout->addLayout(FOpenFIlelayout, 0, 0);
        GOpenfileLayout->addLayout(HOpenfileLayout, 1, 0);
        // 获得文件路径
        connect(OCFilekBtn, &QPushButton::clicked, [=]() {
            QString OCFilepath = QFileDialog::getOpenFileName(this, "Open File", "C:\\Users\\Li\\Desktop");
            // getOpenFileName（parent,caption标题,路径,过滤器（多个过滤则使用;;分割）,默认选择器,options（对话框参数设定，用|分隔开））
            folineEdit->setText(OCFilepath);
        });
        // 打开文件并显示内容

        mmfodlg->exec();
        mmfodlg->setAttribute(Qt::WA_DeleteOnClose);
    });

    // 文件保存
    connect(mmFsave, &QAction::triggered, [=]() {
        // 创建非模态对话框
        //  QDialog mmfsdlg(this);//在lambda中的函数体写了this，则在[]中需要写“=”
        //  mmfsdlg.resize(400,200);
        //  mmfsdlg.show(); //但在此处，非模态对话框展示完后会快速消失  （23.12.27--对比126）
        //                  //解决办法放在堆上
        //  QDialog * mmfsdlg = new QDialog(this);
        //  mmfsdlg ->resize(400,200); //这点必须要用 "->"了，不能在使用"."了
        //  mmfsdlg ->show();

        // QDialog mmfsdlg(this);
        // mmfsdlg.resize(400,200);
        // mmfsdlg.setWindowTitle(QString::fromUtf8("Save"));
        // mmfsdlg.exec();
        // mmfsdlg.setAttribute(Qt::WA_DeleteOnClose);

        QDialog* mmfsdlg = new QDialog;
        mmfsdlg->setWindowTitle(QString::fromUtf8("Save"));
        mmfsdlg->resize(500, 100);
        QPushButton* SCFilekBtn = new QPushButton("Choose", mmfsdlg);
        SCFilekBtn->setGeometry(10, 10, 100, 30);
        QPushButton* SSFilekBtn = new QPushButton("OK", mmfsdlg);
        SSFilekBtn->setGeometry(10, 10, 100, 30);
        // 设置路径读取窗口
        QLineEdit* fslineEdit = new QLineEdit(mmfsdlg);
        fslineEdit->setReadOnly(true); // 设置权限：只读
        // 按钮和显示路径布
        QFormLayout* FSaveFIlelayout = new QFormLayout();
        FSaveFIlelayout->setRowWrapPolicy(QFormLayout::WrapAllRows); // QFormLayout::WrapAllRows：文本框总是出现在标签后面
        FSaveFIlelayout->addRow("Save File Path:", fslineEdit);
        QHBoxLayout* HSavefileLayout = new QHBoxLayout();
        HSavefileLayout->addStretch();
        HSavefileLayout->addWidget(SCFilekBtn);
        HSavefileLayout->addWidget(SSFilekBtn);
        // 将两个模块合二为一
        QGridLayout* GOpenfileLayout = new QGridLayout(mmfsdlg);
        GOpenfileLayout->setSpacing(15);
        GOpenfileLayout->addLayout(FSaveFIlelayout, 0, 0);
        GOpenfileLayout->addLayout(HSavefileLayout, 1, 0);

        // 文件路径显示
        connect(SCFilekBtn, &QPushButton::clicked, [=]() {
            QString SCFilepath = QFileDialog::getOpenFileName(this, "Save File", "C:\\Users\\Li\\Desktop");
            fslineEdit->setText(SCFilepath);
        });

        mmfsdlg->exec();
        mmfsdlg->setAttribute(Qt::WA_DeleteOnClose);
    });
    // 错误对话框
    // QMessageBox::critical(this,"critical","warning!"); //错误对话框全选择模态对话框类型

    // 消息对话框
    // QMessageBox::information(this,"Information","提示");

    // 提问对话框中：  参数1-父亲  参数2-标题  参数3-中间显示内容  参数4-按键类型  参数5-默认关联回车
    // QMessageBox::question(this,"Question","Prompt");
    //  if ( QMessageBox::Save ==  QMessageBox::question(this,"Question","Prompt",QMessageBox::Save | QMessageBox::Cancel,QMessageBox::Save) )
    //  {
    //      qDebug() <<  q"选择的是保存！";
    //  }
    //  else
    //  {
    //      qDebug() <<"选择的是取消！";
    //  }

    // 警告对话框
    // QMessageBox::warning(this,"warning","warning!");

    // 颜色对话框
    // QColor ccolor = QColorDialog::getColor(QColor(255,0,0)); //默认初始颜色为255红色,返回值为QColor
    // qDebug() <<"r =" << ccolor.red() << " g = " << ccolor.green() << " b = " << color.blue();

    // 参数1-父亲  参数2-标题  参数3-默认路径 参数4-过滤的文件格式  返回值是选取的路径
    // QFileDialog::getOpenFileName(this,"打开文件","C:\\Users\\Li\\Desktop","(*.txt)"); //最后（*。txt）表示只能打开txt文档

    // 设置频率
    // label标签
    connect(mmsFrequency, &QAction::triggered, [=]() {
        QDialog* mmsfredlg = new QDialog;
        mmsfredlg->setWindowTitle(QString::fromUtf8("Frequency Range Settings"));
        // mmsfredlg -> resize(600,200);
        mmsfredlg->setMinimumSize(400, 200);
        mmsfredlg->setMaximumSize(400, 200);

        // QLabel *sFremin =new QLabel;
        // sFremin -> setParent(mmsfredlg);
        // sFremin -> setText(QString::fromUtf8("Min.frequency："));

        QLineEdit* sFminlineEdit = new QLineEdit(mmsfredlg);
        sFminlineEdit->setReadOnly(false);
        QLineEdit* sFmaxlineEdit = new QLineEdit(mmsfredlg);
        sFmaxlineEdit->setReadOnly(false);

        // 给输入框加提示占位符
        sFminlineEdit->setPlaceholderText("Enter min frequency (e.g. 1.0)");
        sFmaxlineEdit->setPlaceholderText("Enter max frequency (e.g. 10.0)");

        QPushButton* FreBtnok = new QPushButton("ok", mmsfredlg);
        QPushButton* FreBtnno = new QPushButton("cancel", mmsfredlg);

        // 添加标签和输入框
        // 分为两个区域，区域1：标签+路径，区域2：底部区域按钮
        QFormLayout* Frelayout = new QFormLayout();
        Frelayout->setRowWrapPolicy(QFormLayout::WrapAllRows); // QFormLayout::WrapAllRows：文本框总是出现在标签后面
        Frelayout->addRow("Min.frequency:", sFminlineEdit);
        Frelayout->addRow("Max.frequency:", sFmaxlineEdit);
        // 添加按钮
        QHBoxLayout* BottomLayout = new QHBoxLayout();
        BottomLayout->addStretch();
        BottomLayout->addWidget(FreBtnok);
        BottomLayout->addWidget(FreBtnno);
        QGridLayout* mainFreLayout = new QGridLayout(mmsfredlg);
        mainFreLayout->setSpacing(15);
        mainFreLayout->addLayout(Frelayout, 0, 0);
        mainFreLayout->addLayout(BottomLayout, 1, 0);



        // connect(FreBtnok, &QPushButton::clicked, [=]()  {

        //     qDebug() << "OK按钮被点击了，开始处理频率设置";
        //     // 1. 从输入框解析用户输入的最小频率（tempMin）和最大频率（tempMax）
        //     bool okMin, okMax;
        //     double tempMin = sFminlineEdit->text().toDouble(&okMin);  // 最小频率输入框
        //     double tempMax = sFmaxlineEdit->text().toDouble(&okMax);  // 最大频率输入框


        //     if (!okMin || !okMax) {
        //         //qDebug() << "验证失败：输入不是有效数字";
        //         QMessageBox::warning(mmsfredlg, "输入错误", "请输入有效的频率值（数字）");
        //         return;
        //     }
        //     if (tempMin < 0 || tempMax <= 0) {
        //         //qDebug() << "验证失败：频率小于等于0";
        //         QMessageBox::warning(mmsfredlg, "输入错误", "频率值必须大于0");
        //         return;
        //     }
        //     if (tempMin >= tempMax) {
        //         //qDebug() << "验证失败：最小频率不小于最大频率";
        //         QMessageBox::critical(mmsfredlg, "输入错误", "最小频率必须小于最大频率");
        //         return;
        //     }

        //     // 3. 将频率值保存到 MainWindow 的成员变量中
        //     Freminvalue = tempMin;
        //     Fremaxvalue = tempMax;
        //     g_freMin = tempMin;  // 存入全局变量
        //     g_freMax = tempMax;

        //     qDebug() << "[频率设置] g_freMin=" << g_freMin << "，地址=" << &g_freMin;
        //     qDebug() << "[频率设置] g_freMax=" << g_freMax << "，地址=" << &g_freMax;

        //     mmsfredlg->close();
        // });
        // qDebug() << "[频率设置] g_freMin=" << g_freMin << "，地址=" << &g_freMin;
        // qDebug() << "[频率设置] g_freMax=" << g_freMax << "，地址=" << &g_freMax;

        connect(FreBtnno, &QPushButton::clicked, [=]() {
            mmsfredlg->close();
        });

        //mmsfredlg->exec();
        mmsfredlg->setAttribute(Qt::WA_DeleteOnClose);

    });
    // qDebug() << "[窗口关闭后] 频率值已设置：Min=" << Freminvalue << ", Max=" << Fremaxvalue;

    // // 设置单位
    // QDialog* mUnitsetFile = new QDialog;
    // mUnitsetFile->setWindowTitle(QString::fromUtf8("Units"));
    // mUnitsetFile->setFixedSize(500, 300);
    // QLabel* UnitTittlename = new QLabel;
    // UnitTittlename->setText(tr("Units"));

    // // 尺度单位
    // // 索引为 0~5
    // QLabel* Dimname = new QLabel;
    // Dimname->setText(tr("Dimensions:"));
    // DimOptions = new QComboBox;
    // DimOptions->addItem(tr("m"));
    // DimOptions->addItem(tr("cm"));
    // DimOptions->addItem(tr("mm"));
    // DimOptions->addItem(tr("um"));
    // DimOptions->addItem(tr("nm"));
    // DimOptions->addItem(tr("ft"));
    // DimUnitIndex = 2;
    // DimOptions->setCurrentIndex(DimUnitIndex); // 默认显示

    // // 频率单位
    // // 索引为 0~5
    // QLabel* Frename = new QLabel;
    // Frename->setText(tr("Frequency:"));
    // FreOptions = new QComboBox;
    // FreOptions->addItem(tr("Hz"));
    // FreOptions->addItem(tr("KHz"));
    // FreOptions->addItem(tr("MHz"));
    // FreOptions->addItem(tr("GHz"));
    // FreOptions->addItem(tr("THz"));
    // FreOptions->addItem(tr("PHz"));
    // FreUnitIndex = 3;
    // FreOptions->setCurrentIndex(FreUnitIndex);

    // // 时间单位
    // // 索引为 0~5
    // QLabel* Timename = new QLabel;
    // Timename->setText(tr("Time:"));
    // TimeOptions = new QComboBox;
    // TimeOptions->addItem(tr("s"));
    // TimeOptions->addItem(tr("ms"));
    // TimeOptions->addItem(tr("us"));
    // TimeOptions->addItem(tr("ns"));
    // TimeOptions->addItem(tr("ps"));
    // TimeOptions->addItem(tr("fs"));
    // TimUnitIndex = 3;
    // TimeOptions->setCurrentIndex(TimUnitIndex);

    // // 按钮
    // QPushButton* UnitBtnok = new QPushButton; // 按钮大小设置 ->setFixedSize(**,**);
    // QPushButton* UnitBtncancel = new QPushButton;
    // UnitBtnok->setText(tr("OK"));
    // UnitBtncancel->setText(tr("Cancel"));

    // // 布局
    // QGridLayout* FileUnitsLayout = new QGridLayout(mUnitsetFile);
    // FileUnitsLayout->addWidget(Dimname, 0, 0);
    // FileUnitsLayout->addWidget(DimOptions, 1, 0);
    // FileUnitsLayout->addWidget(Frename, 2, 0);
    // FileUnitsLayout->addWidget(FreOptions, 3, 0);
    // FileUnitsLayout->addWidget(Timename, 4, 0);
    // FileUnitsLayout->addWidget(TimeOptions, 5, 0);
    // FileUnitsLayout->setSpacing(10);
    // FileUnitsLayout->addWidget(UnitBtnok, 6, 0);
    // FileUnitsLayout->addWidget(UnitBtncancel, 6, 1);

    // connect(mUnits, &QAction::triggered, [=]() {
    //     // 获取当前索引数
    //     connect(UnitBtnok, &QPushButton::clicked, [=]() {
    //         DimUnitIndex = DimOptions->currentIndex();
    //         FreUnitIndex = FreOptions->currentIndex();
    //         TimUnitIndex = TimeOptions->currentIndex();

    //         // qDebug() << tr("The DimIndex is ")<<DimUnitIndex;
    //         // qDebug() << tr("The FreIndex is ")<<FreUnitIndex;
    //         // qDebug() << tr("The TimIndex is ")<<TimUnitIndex;

    //         DimOptions->setCurrentIndex(DimUnitIndex);
    //         FreOptions->setCurrentIndex(FreUnitIndex);
    //         TimeOptions->setCurrentIndex(TimUnitIndex);
    //     });

    //     connect(UnitBtncancel, SIGNAL(clicked()), mUnitsetFile, SLOT(close()));

    //     mUnitsetFile->exec();
    //     mUnitsetFile->setAttribute(Qt::WA_DeleteOnClose);
    // });



    //网格参数设置窗口
    connect(MeshPropertiesAction, &QAction::triggered, [=]() {
        MeshSettingsDialog_Showwidget = new MeshSettingsDialog(this);
        MeshSettingsDialog_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        MeshSettingsDialog_Showwidget->show();

        // 创建或获取 Mesh_data 对象
        // Mesh_data* meshData = new Mesh_data(); // 或者使用现有的数据
        // MeshSettingsDialog_Showwidget = new MeshSettingsDialog(meshData, this);
        // MeshSettingsDialog_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        // // 连接信号，确保在对话框关闭时删除数据
        // connect(MeshSettingsDialog_Showwidget, &MeshSettingsDialog::meshSettingsApplied,
        //         this, [meshData](Mesh_data* data) {
        //             // 在这里处理数据
        //             // 注意：不要在这里删除 data，除非您确定不再需要
        //         });
        // connect(MeshSettingsDialog_Showwidget, &MeshSettingsDialog::destroyed,
        //         this, [meshData]() {
        //             // 在对话框销毁时删除数据（如果需要）
        //             // delete meshData;
        //         });
        // MeshSettingsDialog_Showwidget->show();
    });


    //网格材料参数设置窗口
    connect(mmmaterailpara, &QAction::triggered, [=]() {
    materialDialog_Showwidget = new MaterialParameter(this);
    materialDialog_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
    materialDialog_Showwidget->show();
    });



    //单位修改窗口
    connect(mUnits, &QAction::triggered, [=]() {
        Simulation_UnitSet_Showwidget = new UnitsSettingDialog(this);
        Simulation_UnitSet_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        Simulation_UnitSet_Showwidget->show();
    });

    // 自定义消息对话框，显示已设定的参数信息
    // Have Done 界面
    connect(mHHaveDone, &QAction::triggered, [=]() {
        changeinputDlg = new InputDlg(this);
        changeinputDlg->show();
        changeinputDlg->setAttribute(Qt::WA_DeleteOnClose); // 析构函数要注意！！
    });

    // Plane Wave界面
    connect(mmPlanWaveShow, &QAction::triggered, [=]() {
        SimulationShowWidget = new SimulationDlg(this->plane, this);
        SimulationShowWidget->setAttribute(Qt::WA_DeleteOnClose); // 析构函数注意
        connect(SimulationShowWidget, SIGNAL(plane_ok(Plane_data*)), this, SLOT(add_ok(Plane_data*))); // 点击确认，数据保存
        SimulationShowWidget->show();

        // 没有析构函数，则会出现反复点击窗口，程序自动退出的情况
    });

    //  Probe 界面
    connect(mmProbeShow, &QAction::triggered, [=]() {
        Probe_data *probe_show = new Probe_data;
        Simulation_Pro_Showwidget = new simulationprobe(probe_show, this);
        Simulation_Pro_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        connect(Simulation_Pro_Showwidget, SIGNAL(Probe_emit(Probe_data*)), this, SLOT(pro_add_ok(Probe_data*)));
        Simulation_Pro_Showwidget->show();
    });

    //Discrete Port 界面
    connect(mmDportShow,&QAction::triggered,[=](){
        qDebug()<<"目前正确";
        DPort_data *dport_show = new DPort_data;
        Simulation_DP_showWidget = new simulationDisP(dport_show,this);
        Simulation_DP_showWidget -> setAttribute(Qt::WA_DeleteOnClose);
        Simulation_DP_showWidget -> show();
    });

    connect(mmMonitorShow, &QAction::triggered, [=]() {
        Monitor_data *monitor_show = new Monitor_data;
        Simulation_Mon_Showwidget = new simulationmonitor(monitor_show, this);
        Simulation_Mon_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        //connect(Simulation_Mon_Showwidget, SIGNAL(Monitor_emit(Monitor_data*)), this, SLOT(mon_add_ok(Monitor_data*)));
        Simulation_Mon_Showwidget->show();
    });
    //远场观察点
    // connect(mmFarObvPointShow, &QAction::triggered, [=]() {

    //     farfieldobvpoint_Showwidget=new FarFieldObvPoint(this);
    //     farfieldobvpoint_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
    //     farfieldobvpoint_Showwidget->show();
    // });

    // Mesh Structure 界面
    connect(mmMeshStrucShow, &QAction::triggered, [=]() {
        Simulation_MeshStruc_Showwidget = new simulationmeshstruc(this);
        Simulation_MeshStruc_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
        // 如果需要传递数据，可以在这里连接信号槽
        // connect(Simulation_MeshStruc_Showwidget, SIGNAL(MeshStruc_emit(SomeData*)), this, SLOT(meshstruc_add_ok(SomeData*)));
        Simulation_MeshStruc_Showwidget->show();
    });

    // Result Setting 界面
    // connect(mmResultSettingShow, &QAction::triggered, [=]() {
    //     Simulation_ResultSet_Showwidget = new simulationresultset(this);
    //     Simulation_ResultSet_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
    //     Simulation_ResultSet_Showwidget->show();
    // });




}



MainWindow::~MainWindow()
{
    delete ui;

    // 清理进程资源
    if (postProcess) {
        postProcess->kill();
        delete postProcess;
    }
}

// bool MainWindow::eventFilter(QObject* watched, QEvent* event)
// {
//     if (watched == menuBar()) {
//         if (event->type() == QEvent::MouseMove) {
//             // 鼠标移动到菜单栏时，确保它被激活
//             QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//             if (menuBar()->rect().contains(mouseEvent->pos())) {
//                 menuBar()->setFocus();
//                 menuBar()->activateWindow();
//             }
//         }
//     }
//     return QMainWindow::eventFilter(watched, event);
// }






void MainWindow::Component_add()
{
    qDebug() << "创建Component";
}
void MainWindow::Component_delete()
{
    qDebug() << "删除Component";
}

void MainWindow::Materials_add()
{
    qDebug() << "创建Materials";
}
void MainWindow::Materials_delete()
{
    qDebug() << "删除Materials";
}

void MainWindow::Lumped_add()
{
    qDebug() << "创建Lumped";
}
void MainWindow::Lumped_delete()
{
    qDebug() << "删除Lumped";
}

void MainWindow::plane_add()
{
    SimulationShowWidget = new SimulationDlg(this->plane, this);
    SimulationShowWidget->setAttribute(Qt::WA_DeleteOnClose); // 析构函数要注意！！
    connect(SimulationShowWidget, SIGNAL(plane_ok(Plane_data*)), this, SLOT(add_ok(Plane_data*))); // 点击确认，数据保存
    SimulationShowWidget->exec();
}
void MainWindow::plane_delete()
{
    if (this->plane == nullptr) {
        return;
    }
    delete this->plane;
    this->plane = nullptr;
    auto topLevel = TreeTrunk->topLevelItem(3);
    if (topLevel->childCount() == 0) {
        return;
    }
    topLevel->removeChild(topLevel->child(0));

    qDebug() << "删除plane";
}

void MainWindow::Ports_add()
{
    qDebug() << "创建Ports";
}
void MainWindow::Ports_delete()
{
    qDebug() << "删除Ports";
}

void MainWindow::Excitation_add()
{
    qDebug() << "创建Excitation";
}
void MainWindow::Excitation_delete()
{
    qDebug() << "删除Excitation";
}

void MainWindow::Probes_add()
{
    Probe_data *probe_addshow = new Probe_data;
    Simulation_Pro_Showwidget = new simulationprobe(probe_addshow, this);
    Simulation_Pro_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(Simulation_Pro_Showwidget, SIGNAL(Probe_emit(Probe_data*)), this, SLOT(pro_add_ok(Probe_data*)));
    Simulation_Pro_Showwidget->show();
    // qDebug() << "创建Probes";
}

void MainWindow::Probes_delete()
{
    auto topLevel = TreeTrunk->topLevelItem(6);
    if (topLevel->childCount() == 0) {
        return;
    }
    int pro_dele_rowsindex = rows_get();
    topLevel->removeChild(topLevel->child(pro_dele_rowsindex));
    dataVector.erase(dataVector.begin()+pro_dele_rowsindex);
    qDebug() << "删除Probes";
}

void MainWindow::add_ok(Plane_data* plane)
{
    //
    if (this->plane != nullptr) {
        // clean
    }
    this->plane = plane;
    qDebug() << "参数" << plane;
    // 主窗口
    // 获取当前选中的行
    auto item = TreeTrunk->topLevelItem(3);
    // 卫语句  扁平化 条件反转
    if (item->childCount() > 0) { // 决定有几个子项
        return;
    }
    QTreeWidgetItem* Plane_Item = new QTreeWidgetItem;
    Plane_Item->setText(0, "plane wave");
    item->addChild(Plane_Item);
    qDebug() << "添加";
}

void MainWindow::pro_add_ok(Probe_data* probe)
{
    this->probe = probe;
    qDebug()<<"this->probe"<<this->probe;
    auto item_pro = TreeTrunk->topLevelItem(6);
    if (item_pro->childCount() > 20) {
        return;
    }
    QTreeWidgetItem* Probe_items = new QTreeWidgetItem;
    QString Pro_item_name = QString(probe->probe_name.data(),probe->probe_name.size());
    Probe_items->setText(0,Pro_item_name);
    item_pro->addChild(Probe_items);

    dataVector.push_back(this->probe);
}

// 双击打开对应对话框
void MainWindow::Component_douClick()
{
    qDebug() << "双击打开Component";
}

void MainWindow::Materials_douClick()
{
    qDebug() << "双击打开Materials";
}

void MainWindow::Lumped_douClick()
{
    qDebug() << "双击打开Lumped";
}

void MainWindow::plane_douClick()
{
    SimulationShowWidget = new SimulationDlg(this->plane, this);
    SimulationShowWidget->setAttribute(Qt::WA_DeleteOnClose); // 析构函数要注意！！
    connect(SimulationShowWidget, &SimulationDlg::plane_ok, this, &MainWindow::add_ok); // 点击确认，数据保存
    SimulationShowWidget->exec();
    qDebug() << "双击打开Plane wave";
}

void MainWindow::Ports_douClick()
{
    qDebug() << "双击打开Ports";
}

void MainWindow::Excitation_douClick()
{
    qDebug() << "双击打开Excitation";
}

void MainWindow::Probes_douClick()
{
    int pro_rowsindex = rows_get();
    qDebug()<<"在Probe这个函数里pro_rowsindex是"<<pro_rowsindex;
    Probe_data* currentData = dataVector[pro_rowsindex];
    qDebug()<<currentData->Ori_ca;
    //双击打开应该只显示，不在保存数据
    //Simulation_Pro_Showwidget = new simulationprobe(this->probe, this);
    Simulation_Pro_Showwidget = new simulationprobe(currentData , this);//只调用currentData显示，却不修改
    Simulation_Pro_Showwidget->setAttribute(Qt::WA_DeleteOnClose);
    //connect(Simulation_Pro_Showwidget, SIGNAL(Probe_emit(Probe_data*)), this, SLOT(pro_add_ok(Probe_data*)));
    // 这句话是对信息的修改，没有了上面的这句话，双击只负责打开信息不再负责改动信息，pro_add_ok只负责在QTreeWidget下面添加新的子项
    //connect(&Simulation_Pro_Showwidget, &simulationprobe::Probe_emit,this,&MainWindow::pro_doubChange);
    connect(Simulation_Pro_Showwidget, SIGNAL(Probe_emit(Probe_data*)), this, SLOT(pro_doubChange(Probe_data*)));
    Simulation_Pro_Showwidget->show();
    qDebug() << "双击打开Probes";
}

int MainWindow::rows_get()
{
    auto rows = TreeTrunk->selectionModel()->selectedRows();
    auto itemIndex = rows[0];
    qDebug() << "次级行号" << itemIndex.row(); // 次级行号
    int rowsindex = itemIndex.row();
    qDebug()<<"rowsindex 是"<< rowsindex;
    return rowsindex;
}

void MainWindow::pro_doubChange(Probe_data* Pro_data)
{
    qDebug()<<"prodou正在运行";
    int pro_doub_rowsindex = rows_get();
    qDebug()<<"pro_doub_rowsindex的值是"<<pro_doub_rowsindex;
    dataVector[pro_doub_rowsindex ] = Pro_data;
    auto item_pro = TreeTrunk->topLevelItem(6);
    auto Pro_newchilren_name = item_pro->child(pro_doub_rowsindex);

    QString Pro_chilnew_name = QString(dataVector[pro_doub_rowsindex ]->probe_name.data(),dataVector[pro_doub_rowsindex ]->probe_name.size());
    Pro_newchilren_name ->setText(0,Pro_chilnew_name);
}

void MainWindow::MPI_changedata(MPI_Data* mpi_data)
{
    this -> mpiinfo = mpi_data;
    qDebug()<<"this->mpi_data"<< this -> mpiinfo;
    qDebug() << "第3次MPI_Info_boder_x: " << this ->  mpiinfo  -> MPI_Info_boder_x;
    qDebug() << "第3次MPI_Info_boder_y: " << this ->  mpiinfo  -> MPI_Info_boder_y;
    qDebug() << "第3次MPI_Info_boder_z: " << this ->  mpiinfo  -> MPI_Info_boder_z;
}

void MainWindow::actLoadMaterial_triggered()
{
    QString str = QFileDialog::getOpenFileName(this, "选择模型文件", "",
                                               "STL (*.stl);;OBJ (*.obj);;FBX(*.fbx);;ALL FILES( *.* )");

    if (!str.isEmpty()) {
        // 获取下一个颜色
        QVector3D color = materialColors[materialIndex];
        materialIndex = (materialIndex + 1) % 36; // 循环使用36种颜色

        // 生成材料名称
        QString modelName = QString("Material%1").arg(materialIndex + 1);

        // 加载模型
        int modelId = glWidget->loadModel(str.toStdString(),
                                          openglwidget::MATERIAL,
                                          color);

        if (modelId != -1) {
            // 创建树节点
            QTreeWidgetItem* newItem = new QTreeWidgetItem(m_compGroup1);
            newItem->setText(0, modelName);
            m_compGroup1->setExpanded(true);
            m_treeItemToModelId[newItem] = modelId;
        }
    }
}

void MainWindow::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QString itemText = item->text(0);
    QRegularExpression probePattern("^1-\\d+-probe-\\d+-\\d+-\\d+-[A-Za-z]+$");
    QRegularExpressionMatch match = probePattern.match(itemText);

    if (match.hasMatch()) {
        qDebug() << "探针节点被点击:" << itemText;
        SpShow *spWindow = new SpShow(this, itemText);
        spWindow->show();
        return;  // 处理完成后直接返回
    }
    if (match.hasMatch()) {
        qDebug() << "探针节点被点击:" << itemText;
        SpShow *spWindow = new SpShow(this, itemText);
        spWindow->show();
        return;  // 处理完成后直接返回
    }

    // 检查点击的是否是sp节点
    if (item->text(0) == "S-Parameter") {
        qDebug() << "S-Parameter Node clicked!"; // 添加调试输出
        SpShow *spWindow = new SpShow(this,"s21");// 创建sp窗口，传入数据路径
        spWindow->show();// 显示窗口
    }

    // if (item->text(0) == "1-0-probe-52-52-67-Ex") {
    //     qDebug() << "1-0-probe-52-52-67-Ex Node clicked!"; // 添加调试输出
    //     SpShow *spWindow = new SpShow(this,item->text(0));// 创建sp窗口，传入数据路径
    //     spWindow->show();// 显示窗口
    // }

    // 检查点击的是否是farfields节点
    if (item->text(0) == "Farfields(f1)") {
        qDebug() << "Farfields(f1) Node clicked!"; // 添加调试输出
        DirShow *dirWindow = new DirShow(this);// 创建dir窗口，传入数据路径
        dirWindow->show();// 显示窗口
    }
    // 检查点击的是否是E节点
    if (item->text(0) == "E") {
        qDebug() << "E Node clicked!"; // 添加调试输出
        FieldDiagramShow *diagramWindow = new FieldDiagramShow(this);// 创建diagram窗口，传入数据路径
        diagramWindow->show();// 显示窗口
    }
    // 检查点击的是否是E节点
    if (item->text(0) == "Farfields_3D") {
        qDebug() << "Farfields_3D Node clicked!"; // 添加调试输出
        farfield3dwindow *farfield3dwindow_Showwidget= new farfield3dwindow(this);
        farfield3dwindow_Showwidget->show();// 显示窗口
    }
}

void MainWindow::onMeshActionTriggered()
{
    //Q_UNUSED(column);

    MeshShow *meshWindow = new MeshShow(this);// 创建 MeshShow 窗口，传入数据路径
    meshWindow->show();// 显示窗口


}

void MainWindow::onSetAdaptiveMeshTriggered()
{
    //Q_UNUSED(column);

    AdaptiveMeshSetting *meshsettingWindow = new AdaptiveMeshSetting(this);// 创建 MeshShow 窗口，传入数据路径
    meshsettingWindow->show();// 显示窗口


}

void MainWindow::onSetUpSimulationTriggered()
{
    //Q_UNUSED(column);

    SimulationSetUp *simulationsettingWindow = new SimulationSetUp(this);// 创建 MeshShow 窗口，传入数据路径
    simulationsettingWindow->show();// 显示窗口


}


void MainWindow::onPostProcessingActionTriggered()
{

    // 确保只有一个进程实例
    if (postProcess && postProcess->state() == QProcess::Running) {
        QMessageBox::information(this, "Process Running", "Post-processing is already running.");
        return;
    }

    // 清理之前的进程（如果存在）
    if (postProcess) {
        delete postProcess;
    }

    // 创建新进程
    postProcess = new QProcess(this);

    // 连接信号处理输出和错误
    connect(postProcess, &QProcess::readyReadStandardOutput, [=]() {
        QString output = QString::fromLocal8Bit(postProcess->readAllStandardOutput());
        qDebug() << "Output:" << output;
    });

    connect(postProcess, &QProcess::readyReadStandardError, [=]() {
        QString error = QString::fromLocal8Bit(postProcess->readAllStandardError());
        qDebug() << "Error:" << error;
    });

    connect(postProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process finished with code:" << exitCode;
                if (exitCode != 0) {
                    QString errorMessage = QString("Post-processing failed with exit code: %1").arg(exitCode);
                    QMessageBox::critical(this, "Error", errorMessage);
                }
            });

    // 设置要运行的程序路径（根据实际路径修改）
    // 获取应用程序可执行文件所在目录
    QString appDir = QCoreApplication::applicationDirPath();

    // QString program = "E:/qt/6_5P1/txtobj.exe"; // 使用绝对路径
    QString program = QDir::cleanPath(appDir + "/txtobj.exe");

    // 设置输入和输出文件路径（使用绝对路径）
    // QString inputFile = "E:/qt/6_5P1/dir_input.txt"; // 使用绝对路径
    // QString outputFile = "E:/qt/6_5P1/dir_3d_output.obj"; // 使用绝对路径
    QString inputFile = QDir::cleanPath(appDir +"/dir_input.txt"); // 使用绝对路径
    QString outputFile = QDir::cleanPath(appDir +"/dir_3d_output.obj"); // 使用绝对路径

    // 检查输入文件是否存在
    QFileInfo inputFileInfo(inputFile);
    if (!inputFileInfo.exists()) {
        QMessageBox::critical(this, "Error",
                              QString("Input file does not exist:\n%1").arg(inputFile));
        return;
    }

    // 确保输出目录存在
    QFileInfo outputFileInfo(outputFile);
    QDir outputDir = outputFileInfo.absoluteDir();
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::critical(this, "Error",
                                  QString("Failed to create output directory:\n%1").arg(outputDir.path()));
            return;
        }
    }

    // 设置参数
    QStringList arguments;
    arguments << "--input" << inputFile;
    arguments << "--output" << outputFile;

    // 设置工作目录为输入文件所在目录
    postProcess->setWorkingDirectory(inputFileInfo.absolutePath());

    // 启动程序
    postProcess->start(program, arguments);

    // 等待启动（最多5秒）
    if (!postProcess->waitForStarted(5000)) {
        QString errorMessage;
        if (postProcess->error() == QProcess::FailedToStart) {
            errorMessage = "Failed to start program. Check if it exists and has execute permission.";
        } else {
            errorMessage = "Process failed to start: " + postProcess->errorString();
        }

        QMessageBox::critical(this, "Error",
                              QString("Failed to start post-processing program:\n%1\n%2")
                                  .arg(program).arg(errorMessage));
    }


}

//探针更新函数
void MainWindow::updateProbeTreeFromFile(QTreeWidgetItem* parentItem)
{


    // 如果没有指定父项，使用默认的
    if (!parentItem) {
        parentItem = m_probesSignalsItem;
    }

    if (!parentItem) {
        qDebug() << "探针父项为空!";
        return;
    }

    qDebug() << "正在更新探针树...";

    // 清空现有项
    while (parentItem->childCount() > 0) {
        QTreeWidgetItem* child = parentItem->takeChild(0);
        delete child;
    }

    // 读取探针文件
    QString fileName = QCoreApplication::applicationDirPath() + "/NearFieldProbe.txt";
    QFile file(fileName);

    if (!file.exists()) {
        QTreeWidgetItem* hintItem = new QTreeWidgetItem(parentItem);
        hintItem->setText(0, "暂无探针，请添加");
        hintItem->setForeground(0, QBrush(Qt::gray));
        return;
    }

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QStringList lines;

        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();

        int probeCount = 0;

        // 跳过文件头，从第5行开始
        for (int i = 4; i < lines.size(); i++) {
            QString line = lines[i].trimmed();
            if (line.isEmpty()) continue;

            QStringList parts = line.split('\t');
            if (parts.size() >= 4) {
                QTreeWidgetItem* probeItem = new QTreeWidgetItem(parentItem);
                probeItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

                QString displayText = QString("1-%1-probe-%2-%3-%4-%5")
                                          .arg(probeCount)
                                          .arg(parts[0])
                                          .arg(parts[1])
                                          .arg(parts[2])
                                          .arg(parts[3]);

                probeItem->setText(0, displayText);
                probeCount++;
            }
        }

        qDebug() << "成功加载" << probeCount << "个探针";
    }
}
