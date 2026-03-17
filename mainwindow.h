#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "inputdlg.h"
#include "simulationdlg.h"
#include "simulationprobe.h"
#include "simulationdisp.h"
#include "mpisettingdlg.h"
#include "simulationmeshstruc.h"
#include "simulationresultset.h"
#include "simulationmonitor.h"
#include "farfieldobvpoint.h"
#include "unitssettingdialog.h"
#include "materialparameter.h"
#include "meshsettingsdialog.h"
#include "farfield3dwindow.h"
#include <QDockWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <vector>
#include<QWidgetAction>
#include<openglwidget.h>

#include <QProcess>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QCoreApplication>

class SimulationSetUp;

QT_BEGIN_NAMESPACE//声明命名空间开始
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE//声明命名空间结束

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    static std::vector<Probe_data*> dataVector;//probe的结构体数组
    double tempMin ;
    double tempMax ;
    double Freminvalue;
    double Fremaxvalue;



private:
    openglwidget * glWidget;

    typedef void (MainWindow::*MappingFunc)();
    QTreeWidget* TreeTrunk;
    QTreeWidgetItem* m_compGroup1; // 存储Component节点指针

    Ui::MainWindow* ui;
    // 打表,mapping1和mapping2和treemapDlg，对每一个子项都有争对
    QList<MappingFunc> mapping1; // 删除
    QList<MappingFunc> mapping2; // 添加
    QList<MappingFunc> treemapDlg; // 双击子项调出对话框

    int row; // 当前点击的行
    // 生命周期 值的生命周期通常跟随
    Plane_data* plane;
    QList<Plane_data*> planes; // 列表  次级的row索引 planes[row] -> this.plane
    std::vector<Plane_data*> Pro_array;

    // Probe的数据
    Probe_data* probe;
    //Dis-Port的数据
    DPort_data* dport;
    //MPI的数据
    MPI_Data* mpiinfo;

    QPushButton* changeinputBtn;
    InputDlg* changeinputDlg; // Home界面
    SimulationDlg* SimulationShowWidget; // plane-wave界面
    simulationprobe* Simulation_Pro_Showwidget; // Probe界面
    simulationDisP* Simulation_DP_showWidget; // Discrete Port 界面
    mpisettingdlg* MPI_setting_showWidget; // Simulation子项中的剖分按钮
    simulationmonitor* Simulation_Mon_Showwidget;
    FarFieldObvPoint* farfieldobvpoint_Showwidget;
    simulationmeshstruc *Simulation_MeshStruc_Showwidget;// 添加 mesh structure 窗口指针
    simulationresultset *Simulation_ResultSet_Showwidget;
    UnitsSettingDialog *Simulation_UnitSet_Showwidget;//单位窗口指针
    MaterialParameter *materialDialog_Showwidget ;//网格材料参数窗口指针
    MeshSettingsDialog *MeshSettingsDialog_Showwidget;
    //farfield3dwindow *farfield3dwindow_Showwidget;



    // 频率范围成员变量
    // double Freminvalue;
    // double Fremaxvalue;


    // QDialog* m_meshDialog = nullptr;     // 网格显示对话框
    // QTreeWidgetItem* m_compGroup3 = nullptr; // Mesh节点指针

    void showchangeinputDlg();
    void SettingUnits();

    void Component_delete();
    void Materials_delete();
    void Lumped_delete();
    void plane_delete();
    void Ports_delete();
    void Excitation_delete();
    void Probes_delete();

    void Component_add();
    void Materials_add();
    void Lumped_add();
    void plane_add();
    void Ports_add();
    void Excitation_add();
    void Probes_add();

    void Component_douClick();
    void Materials_douClick();
    void Lumped_douClick();
    void plane_douClick();
    void Ports_douClick();
    void Excitation_douClick();
    void Probes_douClick();

    //添加程序调用部分
    QProcess *postProcess;  // 添加后处理3D输出指针成员变量
    QProcess *adaptivemeshProcess;// 添加自适应网格剖分指针成员变量

    SimulationSetUp *m_simulationSetUp;


    //探针更新
    QTreeWidgetItem* m_probesSignalsItem;

    QTreeWidgetItem* m_probesSignalsItem_p;

    QTreeWidgetItem* m_farfield3dwindowSignalsItem;


private slots:
    // 右键显示菜单


    // 双击显示对话框

    // 弹出菜单 右键添加 点击OK的槽函数
    void add_ok(Plane_data* plane);
    // probe 的槽函数
    void pro_add_ok(Probe_data* probe_array);
    // 获取次级行号
    int rows_get();
    //Pro双击时改变数据
    void pro_doubChange(Probe_data* Pro_data);
    //MPI信号传递
    void MPI_changedata(MPI_Data* mpi_data);

    void onItemClicked(QTreeWidgetItem *item, int column);
    //void onMeshItemClicked(QTreeWidgetItem *item, int column);
    void onMeshActionTriggered(); // 新增：菜单项点击
    void onSetAdaptiveMeshTriggered();

    void onSetUpSimulationTriggered();

    void onPostProcessingActionTriggered();
    //void onAdaptiveMeshingActionTriggered();

    void updateProbeTreeFromFile(QTreeWidgetItem* parentItem = nullptr);


    //opengl
private slots:
    //**************************************************
    void actLoadMaterial_triggered();

public:
    // 添加成员变量
    QMap<QTreeWidgetItem*, int> m_treeItemToModelId; // 树节点到模型ID的映射
    QMap<QString, int> m_modelTypeCounters; // 模型类型计数器

    QVector<QVector3D> materialColors; // 存储36种预定义材料颜色
    int materialIndex = 0;             // 当前材料索引
private:

};
#endif // MAINWINDOW_H
