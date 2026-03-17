#ifndef ADAPTIVEMESHSETTING_H
#define ADAPTIVEMESHSETTING_H


#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QProcess>
#include <QCoreApplication>

class AdaptiveMeshSetting : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdaptiveMeshSetting(QWidget *parent = nullptr);
    ~AdaptiveMeshSetting();

signals:
    // 声明计算请求信号，带四个double参数
    void calculationRequested(double wave, double minx, double miny, double minz);

private slots:
    // 计算按钮的槽函数
    //void onCalculateClicked();
    void onAdaptiveMeshingActionTriggered();
    void onSaveClicked();
    bool getInputValues(double &wave,double &air, double &minx, double &miny, double &minz);

private:
    // 输入框成员变量
    QLineEdit *waveEdit;   // 非均匀剖分最大网格输入框
    QLineEdit *airEdit;   // 空气网格网格输入框
    QLineEdit *minxEdit;   // X方向最小网格输入框
    QLineEdit *minyEdit;   // Y方向最小网格输入框
    QLineEdit *minzEdit;   // Z方向最小网格输入框

    QProcess *adaptivemeshProcess;// 添加自适应网格剖分指针成员变量

    // 验证输入并获取值
    //bool getInputValues(double &wave,double &air, double &minx, double &miny, double &minz);
};

#endif // ADAPTIVEMESHSETTING_H
