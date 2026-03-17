#ifndef SIMULATIONMONITOR_H
#define SIMULATIONMONITOR_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

struct Monitor_data {
    // 监视器类型
    int monitor_type; // 0--E-field 1--H-Field 2--Farfield/RCS 3--Surface current 4--Power flow 5--Current density 6--Power loss density/SAR 7--Electric energy density 8--Magnetic energy density
    // 监视器名称
    QString monitor_name;
    // 自动命名
    bool auto_name;
    // 规格类型
    int spec_type; // 0--Frequency 1--Time 2--Transient Broadband
    // 频率列表
    QString frequency_list;
    // 频率最小值
    double freq_min;
    // 频率最大值
    double freq_max;

    // 时间设置
    double start_time;
    double step_width;
    double end_time;
    bool use_end_time;
    // 瞬态宽带设置
    int samples;
    double accuracy;
    bool transient_farfields;

    Monitor_data()
        : monitor_type(0)
        , monitor_name("")
        , auto_name(true)
        , spec_type(0)
        , frequency_list("")
        , freq_min(0.0)
        , freq_max(0.0)
        , start_time(0.0)
        , step_width(0.02)
        , end_time(0.0)
        , use_end_time(false)
        , samples(21)
        , accuracy(1e-3)
        , transient_farfields(false)
    {
    }
};

class simulationmonitor : public QDialog {
    Q_OBJECT
public:
    explicit simulationmonitor(Monitor_data* data, QWidget* parent = nullptr);
    ~simulationmonitor();

signals:
    void Monitor_emit(Monitor_data* monitor);

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onMonitorTypeChanged();
    void onAutoNameToggled(bool checked);
    void updateMonitorName();
    void onEndTimeToggled(bool checked);

private:
    void setupUI();
    void setupConnections();
    void writeFarFieldFrequencyFile(const QVector<qint64>& frequenciesHz);

    Monitor_data* m_monitorData;

    // Type部分
    QGroupBox* typeGroup;
    QGridLayout* typeLayout;
    QButtonGroup* monitorTypeGroup;
    QRadioButton* efieldRadio;
    QRadioButton* hfieldRadio;
    QRadioButton* farfieldRadio;
    QRadioButton* fieldsourceRadio;
    QRadioButton* surfaceCurrentRadio;
    QRadioButton* powerFlowRadio;
    QRadioButton* currentDensityRadio;
    QRadioButton* powerLossRadio;
    QRadioButton* electricEnergyRadio;
    QRadioButton* magneticEnergyRadio;

    // Label部分
    QGroupBox* labelGroup;
    QHBoxLayout* labelLayout;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QCheckBox* autoNameCheck;

    // Specification部分
    QGroupBox* specGroup;
    QVBoxLayout* specLayout;
    QButtonGroup* specTypeGroup;
    QRadioButton* freqRadio;
    QRadioButton* timeRadio;
    QRadioButton* transientRadio;

    // 堆叠窗口，用于显示不同的设置内容
    QStackedWidget* settingsStack;

    // 频率设置页面
    QWidget* freqPage;
    QVBoxLayout* freqLayout;
    QLabel* freqListLabel;
    QLineEdit* freqListEdit;
    QLabel* freqMinLabel;
    QLineEdit* freqMinEdit;
    QLabel* freqMaxLabel;
    QLineEdit* freqMaxEdit;

    // 时间设置页面
    QWidget* timePage;
    QGridLayout* timeLayout;
    QLabel* startTimeLabel;
    QLineEdit* startTimeEdit;
    QLabel* stepWidthLabel;
    QLineEdit* stepWidthEdit;
    QCheckBox* endTimeCheck;
    QLabel* endTimeLabel;
    QLineEdit* endTimeEdit;


    // 瞬态宽带页面
    QWidget* transientPage;
    QGridLayout* transientLayout;
    QLabel* samplesLabel;
    QLineEdit* samplesEdit;
    QLabel* accuracyLabel;
    QLineEdit* accuracyEdit;
    QCheckBox* transientFarfieldsCheck;


    // 按钮
    QHBoxLayout* buttonLayout;
    QPushButton* okButton;
    QPushButton* cancelButton;

    // 主布局
    QVBoxLayout* mainLayout;
};

#endif // SIMULATIONMONITOR_H
