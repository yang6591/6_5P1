#ifndef MPISETTINGDLG_H
#define MPISETTINGDLG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <vector>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QCheckBox>


struct MPI_Data
{
    // int PType_index; // Properties
    // int Port_name;
    // int PFolder;
    // std::vector<QChar> PLabel_name_vector;
    // QString PLabel_name;
    // double SP_Impdance;//Impendance
    // double VO_Voltage;//Voltage
    // double CU_current;//Current
    // double DP_Radious;
    // int Mon_vo_cu; //Monitor voltage and current 的勾选状态 0--勾选 1--不选

    // int PLocation_Type; //Location-Type 0--Coordinates 1--Wire
    // double Loc_XYZ_1[3];
    // double Loc_XYZ_2[3];

    // DPort_data() {
    //     PType_index = 0;
    //     Port_name = 1;
    //     PFolder = 0;
    //     PLabel_name = " ";
    //     for (auto& ch : PLabel_name) {
    //         PLabel_name_vector.push_back(ch);
    //     }

    //     SP_Impdance = 50.0; // Ohm
    //     VO_Voltage = 1.0; // V
    //     CU_current = 1.0; //A
    //     DP_Radious = 0.0;
    //     Mon_vo_cu = 0; // 选中
    //     PLocation_Type = 0;
    //     Loc_XYZ_1[0] = Loc_XYZ_1[1] = Loc_XYZ_1[2] = 0.0;
    //     Loc_XYZ_2[0] = Loc_XYZ_2[1] = Loc_XYZ_2[2] = 0.0;
    // }
    //需要创建5个大类，9个小类
    double MPI_Info_boder_x;//边界尺寸
    double MPI_Info_boder_y;//波长
    double MPI_Info_boder_z;
    double MPI_Info_wave;
    double MPI_Info_min_x;//最小网格尺寸
    double MPI_Info_min_y;
    double MPI_Info_min_z;
    int MPI_info_dstrictgrid;//网格划分严格度
    int MPI_info_decimalPlaces;//输出文件小数位数
    int MPI_info_core_number;//MPI核心数

    MPI_Data(){
        MPI_Info_boder_x = 2;
        MPI_Info_boder_y = 2;
        MPI_Info_boder_z = 2;
        MPI_Info_wave = 1;
        MPI_Info_min_x = 1;
        MPI_Info_min_y = 1;
        MPI_Info_min_z = 1;
        MPI_info_dstrictgrid = 2;
        MPI_info_decimalPlaces = 4;
        MPI_info_core_number = 1;
    }
};
class mpisettingdlg : public QDialog
{
    Q_OBJECT
public:
    mpisettingdlg(MPI_Data* MPI_Data1, QWidget* parent);
    MPI_Data* MPI_capture;//存储用户输入的MPI参数
    //放在这是为了txt读取的时候方便一些

    int MPI_core_number_default;//默认核心数
signals:
    void mpiDataChanged(MPI_Data* data);//点击OK时发射，传递用户设置
private:
    MPI_Data* creat_MPIinfo_default();

    //QGroupBox* groupBox_core;//核数的对话框
    QCheckBox* core_checkBox;

    QLineEdit* MPI_info_boder_x_EditLine;
    QLineEdit* MPI_info_boder_y_EditLine;
    QLineEdit* MPI_info_boder_z_EditLine;
    QLineEdit* MPI_info_Freq_EditLine;
    QLineEdit* MPI_info_Wave_EditLine;
    QLineEdit* MPI_info_minx_EditLine;
    QLineEdit* MPI_info_miny_EditLine;
    QLineEdit* MPI_info_minz_EditLine;
    QLineEdit* MPI_info_dstr_EditLine;
    QLineEdit* MPI_info_deci_EditLine;
    QLineEdit* MPI_info_core_EditLine;

    int Standardout_number = 0;

    QLabel* MPI_grid_line0;
    QLabel* MPI_grid_line1;
    QLabel* MPI_grid_line2;
    QLabel* MPI_interior_fill0;
    QLabel* MPI_interior_fill1;
    QLabel* MPI_interior_fill2;
    QLabel* MPI_txt_ouput0;
    QLabel* MPI_txt_ouput1;
    QLabel* MPI_txt_ouput2;

    QTimer timer_show;
public slots:
    //界面仿真开始按钮
    void process_start();//启动实际网格计算
    void Simulation_start_Output();//处理计算进程输出
    void Simulation_start_Error();//处理计算进程输出
    void MPI_Information_txt();//保存参数到文件
    void process_start_show();//模拟网格生成过程

    void updateLabels_show();//更新进度标签状态
};

#endif // MPISETTINGDLG_H
