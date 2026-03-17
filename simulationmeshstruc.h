#ifndef SIMULATIONMESHSTRUC_H
#define SIMULATIONMESHSTRUC_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

class simulationmeshstruc : public QDialog
{
    Q_OBJECT

public:
    explicit simulationmeshstruc(QWidget *parent = nullptr);

private slots:
    // void onOkClicked();
    // void onCancelClicked();

private:
    // 左侧输入栏
    QLineEdit *meshLineEdit1;
    QLineEdit *meshLineEdit2;
    QLineEdit *meshLineEdit3;
    QLineEdit *meshLineEdit4;
    QLineEdit *meshLineEdit5;
    QLineEdit *meshLineEdit6;

    // 右侧输入栏
    QLineEdit *meshLineEdit7;
    QLineEdit *meshLineEdit8;
    QLineEdit *meshLineEdit9;
    QLineEdit *meshLineEdit10;
    QLineEdit *meshLineEdit11;
    QLineEdit *meshLineEdit12;

    // 按钮
    QPushButton *meshBtnOk;
    QPushButton *meshBtnCancel;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *inputLayout;
    QGridLayout *leftGridLayout;
    QGridLayout *rightGridLayout;
    QHBoxLayout *buttonLayout;



    void setupUI();
    void setupConnections();

};

#endif // SIMULATIONMESHSTRUC_H
