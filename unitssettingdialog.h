// #ifndef UNITSSETTINGDIALOG_H
// #define UNITSSETTINGDIALOG_H

// #include <QDialog>
// #include <QComboBox>
// #include <QPushButton>
// #include <QLabel>
// #include <QGridLayout>
// #include <QHBoxLayout>
// #include <QVBoxLayout>
// #include <QGroupBox>

// class UnitsSettingDialog : public QDialog
// {
//     Q_OBJECT

// public:
//     explicit UnitsSettingDialog(QWidget *parent = nullptr);
//     ~UnitsSettingDialog();

//     // 获取当前设置的单位
//     QString getDimensionUnit() const;
//     QString getFrequencyUnit() const;
//     QString getTemperatureUnit() const;
//     QString getTimeUnit() const;

// private slots:
//     void onOkClicked();
//     void onCancelClicked();
//     void onHelpClicked();

// private:
//     // 可修改的前四个单位
//     QComboBox *dimensionComboBox;
//     QComboBox *frequencyComboBox;
//     QComboBox *temperatureComboBox;
//     QComboBox *timeComboBox;

//     // 不可修改的后六个单位（显示标签）
//     QLabel *voltageLabel;
//     QLabel *currentLabel;
//     QLabel *conductanceLabel;
//     QLabel *resistanceLabel;
//     QLabel *inductanceLabel;
//     QLabel *capacitanceLabel;

//     // 按钮
//     QPushButton *okButton;
//     QPushButton *cancelButton;
//     QPushButton *helpButton;

//     // 初始化函数
//     void initUI();
//     void initConnections();
// };

// #endif // UNITSSETTINGDIALOG_H


#ifndef UNITSSETTINGDIALOG_H
#define UNITSSETTINGDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

class UnitsSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UnitsSettingDialog(QWidget *parent = nullptr);
    ~UnitsSettingDialog();

    // 获取当前设置的单位
    QString getDimensionUnit() const;
    QString getFrequencyUnit() const;
    QString getTemperatureUnit() const;
    QString getTimeUnit() const;

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onHelpClicked();

private:
    // 可修改的前四个单位
    QComboBox *dimensionComboBox;
    QComboBox *frequencyComboBox;
    QComboBox *temperatureComboBox;
    QComboBox *timeComboBox;

    // 不可修改的后六个单位（显示标签）
    QLabel *voltageLabel;
    QLabel *currentLabel;
    QLabel *conductanceLabel;
    QLabel *resistanceLabel;
    QLabel *inductanceLabel;
    QLabel *capacitanceLabel;

    // 按钮
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *helpButton;

    // 初始化函数
    void initUI();
    void initConnections();

    // 从文件读取设置
    void loadSettingsFromFile();
    // 界面显示的单位转换为文件单位
    QString temperatureDisplayToFile(const QString &displayUnit);
    // 文件单位转换为界面显示的单位
    QString temperatureFileToDisplay(const QString &fileUnit);
};

#endif // UNITSSETTINGDIALOG_H
