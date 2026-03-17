#ifndef SIMULATIONDISP_H
#define SIMULATIONDISP_H

#include <QDialog>
#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>
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
#include <vector>
#include <QCheckBox>
#include <QFormLayout>
#include <QSpacerItem>

struct DPort_data
{
    int PType_index; // Properties
    int Port_name;
    int PFolder;
    std::vector<QChar> PLabel_name_vector;
    QString PLabel_name;
    double SP_Impdance;//Impendance
    double VO_Voltage;//Voltage
    double CU_current;//Current
    double DP_Radious;
    int Mon_vo_cu; //Monitor voltage and current 的勾选状态 0--勾选 1--不选

    int PLocation_Type; //Location-Type 0--Coordinates 1--Wire
    double Loc_XYZ_1[3];
    double Loc_XYZ_2[3];

    DPort_data() {
        PType_index = 0;
        Port_name = 1;
        PFolder = 0;
        PLabel_name = " ";
        for (auto& ch : PLabel_name) {
            PLabel_name_vector.push_back(ch);
        }

        SP_Impdance = 50.0; // Ohm
        VO_Voltage = 1.0; // V
        CU_current = 1.0; //A
        DP_Radious = 0.0;
        Mon_vo_cu = 0; // 选中
        PLocation_Type = 0;
        Loc_XYZ_1[0] = Loc_XYZ_1[1] = Loc_XYZ_1[2] = 0.0;
        Loc_XYZ_2[0] = Loc_XYZ_2[1] = Loc_XYZ_2[2] = 0.0;
    }
};
class simulationDisP : public QDialog
{
public:
    simulationDisP(DPort_data* DPort_data1,QWidget* parent =0);
private:
    DPort_data* creat_DPordefault();
};

#endif // SIMULATIONDISP_H
