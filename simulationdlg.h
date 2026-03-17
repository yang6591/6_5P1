#ifndef SIMULATIONDLG_H
#define SIMULATIONDLG_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

struct Plane_data {
    int polar_mode; // 0--线 1--圆 2--椭圆
    // 正规化坐标--线/圆/椭圆都需要使用
    double normal_coordinate[3];
    double vector_coordinate[3];
    // 频率圆极化，椭圆极化共有
    double Ref_fre_data;
    // 圆极化 特有
    int Phase_data; // 0--Left 1--Right
    // 椭圆极化特有
    double Phdeg_data;
    double Axial_data;
    // Automatic detection的勾选状态
    int AutoStat_data; // 0--初始状态被勾选  1--未被勾选
    // Use decoupling plane框
    int UdpState_data; // 0--初始状态未被勾选  1--已被勾选
    int Plnormal_data; // 0--X 1--Y 2--Z
    double PosX_data;

    double phi;      // 传播方向方位角
    double theta;    // 传播方向极角
    double alpha;    // 极化角

    Plane_data()
        : polar_mode(0) // 因为 0--为Line极化 所以在simulation.cpp中，把初始的QLineEdit都设定为不可选择的参量
        , normal_coordinate { 0, 0, 0 }
        , vector_coordinate { 0 }
        , Ref_fre_data(0.0)
        , Phase_data(0)
        , Phdeg_data(0.0)
        , Axial_data(0.0)
        , AutoStat_data(0)
        , UdpState_data(0)
        , Plnormal_data(0)
        , PosX_data(0.0)

    {
    }
};

class SimulationDlg : public QDialog {
    Q_OBJECT
public:
    SimulationDlg(Plane_data* data, QWidget* parent = 0);

private:
    Plane_data* create_default();

signals:
    void plane_ok(Plane_data* plane); // 点击ok的时候

private slots:
    void changeUdpState();
    void LineChangeBtnChangeState();
    void CircularBtnChangeState();
    void EllipticalBtnChangeState();

private:
    QGridLayout* gpwLayout; // 总布局
    QCheckBox* checkBox_0;
    QGroupBox* groupBox_2;
    QButtonGroup RadioBtnGroup; // 计划方式的按钮组
    // QLabel * CircularLabel_0;
    QRadioButton* CircularLeftRadioBtn;
    QRadioButton* CircularRightRadioBtn;
    QLineEdit* pwLineEdit_0;
    QLineEdit* pwLineEdit_1;
    QLineEdit* pwLineEdit_2;

    QButtonGroup RadioBtnGroup_Pnormal;
};

#endif // SIMULATIONDLG_H
