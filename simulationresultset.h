#ifndef SIMULATIONRESULTSET_H
#define SIMULATIONRESULTSET_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QTextEdit>

class simulationresultset : public QDialog
{
    Q_OBJECT

public:
    explicit simulationresultset(QWidget *parent = nullptr);



    std::vector<double> getPhiDValues() const;
    std::vector<double> getThetaDValues() const;
    std::vector<double> getPhiRCSValues() const;
    std::vector<double> getThetaRCSValues() const;

private slots:

    void onPhiDYesToggled(bool checked);
    void onThetaDYesToggled(bool checked);
    void onPhiRCSYesToggled(bool checked);
    void onThetaRCSYesToggled(bool checked);

private:
    // 左侧参数
    QRadioButton* sParamYes;
    QRadioButton* sParamNo;
    QRadioButton* fieldXYes;
    QRadioButton* fieldXNo;
    QRadioButton* fieldYYes;
    QRadioButton* fieldYNo;
    QRadioButton* fieldZYes;
    QRadioButton* fieldZNo;

    // 右侧参数
    QRadioButton* directivityYes;
    QRadioButton* directivityNo;
    QRadioButton* phiDYes;
    QRadioButton* phiDNo;
    QRadioButton* thetaDYes;
    QRadioButton* thetaDNo;
    QRadioButton* phiRCSYes;
    QRadioButton* phiRCSNo;
    QRadioButton* thetaRCSYes;
    QRadioButton* thetaRCSNo;

    // 角度输入控件
    QTextEdit* phiDTextEdit;
    QTextEdit* thetaDTextEdit;
    QTextEdit* phiRCSTextEdit;
    QTextEdit* thetaRCSTextEdit;

    // 按钮
    QPushButton *resultBtnOk;
    QPushButton *resultBtnCancel;

    //布局
    QVBoxLayout* mainLayout;
    QHBoxLayout* inputLayout;
    QHBoxLayout* buttonLayout;


    // 存储角度值
    std::vector<double> phiDValues;
    std::vector<double> thetaDValues;
    std::vector<double> phiRCSValues;
    std::vector<double> thetaRCSValues;

    void setupUI();
    void setupConnections();

    std::vector<double> parseAngleValues(const QString &text);



};


#endif // SIMULATIONRESULTSET_H
