#ifndef SIMULATIONPROBE_H
#define SIMULATIONPROBE_H

#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

struct Probe_data {
    // 探针名称
    std::vector<QChar> probe_name;
    // Qstring probe_name;
    QString name_field;
    //    QString name_Posit;
    // X,Y,Z的坐标
    //    double Pos_X;
    //    double Pos_Y;
    //    double Pos_Z;
    double Pos_car[3];
    // Theta-Phi-Radius的数据
    //    double Spher_Th;
    //    double Spher_Ph;
    //    double SPher_Ra;
    double Pos_sph[3];

    // field 和 system的序号
    int field_index;
    int system_index;

    // orientation
    // 坐标系
    int Ori_ca; // 直角坐标系
    int Ori_sp; // 球坐标系

    Probe_data()
    //        //用单引号进行构造函数的初始化值
    //       // probe_name{"hello,world"},
    //        Pos_X(0.0)
    //        ,Pos_Y(0.0)
    //        ,Pos_Z(0.0)
    {
        QString name_field = "E-Field  (0,0,0)";
        for (auto& ch : name_field) {
            probe_name.push_back(ch);
        }
        // 坐标初始化
        //        Pos_X = 0.0;
        //        Pos_Y = 0.0;
        //        Pos_Z = 0.0;
        Pos_car[0] = 0;
        Pos_car[1] = 0;
        Pos_car[2] = 0;
        Pos_sph[0] = 0;
        Pos_sph[1] = 0;
        Pos_sph[2] = 0;
        //        Spher_Th = 0.0;
        //        Spher_Ph = 0.0;
        //        SPher_Ra = 0.0;
        // 序号初始化
        field_index = 0;
        system_index = 0;
        // orientation初始化选择
        Ori_ca = 0;
        Ori_sp = 0;
    }
};

class simulationprobe : public QDialog {
    Q_OBJECT
public:
    simulationprobe(Probe_data* Probe_data1, QWidget* parent = 0);
signals:
    void Probe_emit(Probe_data* Probe_data2);

private:
    // 探针名字
    QLabel* proLabel_0;
    Probe_data* create_Prodefault();
    // field选择菜单
    QComboBox* Field_comBox_0;
    // system选择菜单
    QComboBox* Co_Stm_0;

    // char field_name[5]
    QStringList field_name;
    QStringList orien_name;
    // 坐标名
    QString pos_name_X;
    QString pos_name_Y;
    QString pos_name_Z;
    QString pos_name_T;
    QString pos_name_P;
    QString pos_name_R;

    // 分隔符号
    QStringList symbol_name;

    QButtonGroup* Car_Btngroup;
    QButtonGroup* Sph_Btngroup;

private slots:
    void Fiel_stateChanged();
    void Coorstm_state_index();
    void Postion_state_index();
    void Probe_name_total();
    QString Get_pro_Posi();

    void Pro_getInfor();
    void handleFarFieldObservationPoint();
    void handleNearFieldProbe();


private:
    int Field_ComBox_index; // 获取field索引号
    int Coorstm_ComBox_index; // 获取Coor-system的索引号
    int Position_index;

    // name显示
    QLineEdit* proLineEdit_0;

    QLineEdit* proLineEdit_1;
    QLineEdit* proLineEdit_2;
    QLineEdit* proLineEdit_3;

    QLineEdit* proLineEdit_4;
    QLineEdit* proLineEdit_5;
    QLineEdit* proLineEdit_6;

    // 网格数据
    QVector<QPair<int, double>> Xlines;
    QVector<QPair<int, double>> Ylines;
    QVector<QPair<int, double>> Zlines;

};

#endif // SIMULATIONPROBE_H
