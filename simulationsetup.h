#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QProcess>
#include <QCoreApplication>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QVBoxLayout>
#include "mainwindow.h"
//#include "global.h"


class SimulationSetUp : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulationSetUp(QWidget *parent = nullptr);
    ~SimulationSetUp();

    bool getInputValues(int &input_timestep);
    bool getInputValues(double &simulationTime);
    double Freminvalue=0;
    double Fremaxvalue=0;

    double simulationTime=0;

    QPushButton *simulationButton;
    QPushButton *cancelButton;

signals:


public slots:
    void onSimulationSetUpActionTriggered();
    //void onStopSimulation();

private slots:
    void onProcessStarted();
    void onProcessReadyRead();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessErrorOccurred(QProcess::ProcessError error);
    void updateTimer();

private:

    void setupUI();
    void updateProgress(const QString &status);
    void addLogMessage(const QString &message);
    void parseOutputLine(const QString &line);
    void stopFdtdProcess();

    double dt ;
    //QLineEdit *timeStepEdit;
    QLineEdit *SimulationTimeEdit;
    QProcess *simulationProcess;
    // double m_freMin = 0.0;  // 初始化为0，用于后续验证
    // double m_freMax = 0.0;

    // 新添加的进度显示控件
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    QLabel *m_timeLabel;
    QTimer *m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_elapsedSeconds;


    bool m_isUserStop;  // 标记是否是用户主动停止

};




#endif // SIMULATIONSETUP_H
