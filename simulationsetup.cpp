#include "simulationsetup.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTime>

// 类的成员变量补充（需在 simulationsetup.h 中声明）
// 在 simulationsetup.h 的 SimulationSetUp 类中添加：
/*
private:
    QLineEdit *SimulationTimeEdit;  // 已存在
    QProcess *simulationProcess;    // 已存在
    double simulationTime;          // 新增：存储仿真时间的成员变量
*/

SimulationSetUp::SimulationSetUp(QWidget *parent) : QMainWindow(parent)
    ,simulationProcess(nullptr)
    ,SimulationTimeEdit(nullptr)  // 初始化成员变量
    ,simulationTime(0.0)          // 初始化仿真时间
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_timer(nullptr)
    , m_elapsedSeconds(0)
    , simulationButton(nullptr)
    , cancelButton(nullptr)
    , m_isUserStop(false)
{

    setupUI();


}

SimulationSetUp::~SimulationSetUp()
{
    // 清理进程
    if (simulationProcess) {
        if (simulationProcess->state() == QProcess::Running) {
            simulationProcess->terminate();
            simulationProcess->waitForFinished();
        }
        delete simulationProcess;
    }

    if (m_timer) {
        m_timer->stop();
        delete m_timer;
    }
}

void SimulationSetUp::setupUI()
{
    setWindowTitle("Start Up Simulation Settings");
    setMinimumSize(450, 300);

    // 创建中心部件和主布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 1. 参数输入区域
    QGroupBox *paramGroup = new QGroupBox("Simulation Settings", this);
    QGridLayout *paramLayout = new QGridLayout(paramGroup);

    QLabel *SimulationtimeLabel = new QLabel("SimulationTime (ns):", paramGroup);
    paramLayout->addWidget(SimulationtimeLabel, 0, 0);

    SimulationTimeEdit = new QLineEdit(paramGroup);
    QDoubleValidator *validator = new QDoubleValidator(0.000001, 1000000.0, 6, this);
    SimulationTimeEdit->setValidator(validator);
    SimulationTimeEdit->setPlaceholderText("Enter Time ");
    //SimulationTimeEdit->setText("1e-9");
    paramLayout->addWidget(SimulationTimeEdit, 0, 1);

    mainLayout->addWidget(paramGroup);

    // 2. 进度显示区域
    QGroupBox *progressGroup = new QGroupBox("Calculation Progress", this);
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);

    // 进度条
    m_progressBar = new QProgressBar(progressGroup);
    m_progressBar->setRange(0, 100);
    //m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    progressLayout->addWidget(m_progressBar);

    // 状态标签
    m_statusLabel = new QLabel("Status: Awaiting For Calculation", progressGroup);
    m_statusLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
    progressLayout->addWidget(m_statusLabel);

    // 时间标签
    m_timeLabel = new QLabel("Time: 00:00:00", progressGroup);
    progressLayout->addWidget(m_timeLabel);

    mainLayout->addWidget(progressGroup);

    // 3. 控制按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    simulationButton = new QPushButton("Start Simulation", this);
    simulationButton->setMinimumHeight(40);

    cancelButton = new QPushButton("Cancel", this);
    cancelButton->setMinimumHeight(40);

    buttonLayout->addWidget(simulationButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // 设置布局间距
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 连接信号和槽
    connect(simulationButton, &QPushButton::clicked, this, &SimulationSetUp::onSimulationSetUpActionTriggered);
    //connect(cancelButton, &QPushButton::clicked, this, &SimulationSetUp::close);
    connect(cancelButton, &QPushButton::clicked, this, &SimulationSetUp::stopFdtdProcess);  // 修改连接

    // 初始化计时器
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationSetUp::updateTimer);
}




bool SimulationSetUp::getInputValues(double &simulationTime)
{
    if (!SimulationTimeEdit) {
        QMessageBox::critical(this, "错误", "仿真时间输入框未初始化");
        return false;
    }

    QString text = SimulationTimeEdit->text().trimmed();

    if (text.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "仿真时间不能为空");
        SimulationTimeEdit->setFocus();
        return false;
    }

    bool ok;
    simulationTime = text.toDouble(&ok);

    if (!ok) {
        QMessageBox::warning(this, "输入错误", "请输入有效的数值（可以包含小数点）");
        SimulationTimeEdit->setFocus();
        SimulationTimeEdit->selectAll();
        return false;
    }

    if (simulationTime <= 0) {
        QMessageBox::warning(this, "输入错误", "仿真时间必须大于0");
        SimulationTimeEdit->setFocus();
        SimulationTimeEdit->selectAll();
        return false;
    }

    return true;
}

void SimulationSetUp::updateProgress(const QString &status)
{
    m_statusLabel->setText("Status: " + status);

    // 根据状态更新进度条
    if (status.contains("正在启动")) {
        //m_progressBar->setValue(5);
        m_progressBar->setRange(0, 0);
    } else if (status.contains("初始化")) {
        //m_progressBar->setValue(20);
        m_progressBar->setRange(0, 0);
    } else if (status.contains("迭代计算")) {
        // 迭代阶段，设置为不确定进度
        m_progressBar->setRange(0, 0);  // 设置为不确定进度模式
    } else if (status.contains("后处理计算")) {
        //m_progressBar->setRange(0, 100);  // 恢复为确定进度模式
        //m_progressBar->setValue(90);
        m_progressBar->setRange(0, 0);
    } else if (status.contains("计算完成")) {
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
    }
}

void SimulationSetUp::addLogMessage(const QString &message)
{
    qDebug() << message;
}

void SimulationSetUp::parseOutputLine(const QString &line)
{
    QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty()) {
        return;
    }

    // 解析Project1.exe的输出，更新进度状态
    if (trimmedLine.contains("正在进行第") && trimmedLine.contains("步迭代")) {
        updateProgress("FDTD iterative calculating");
        addLogMessage("检测到迭代步骤: " + trimmedLine);
    } else if (trimmedLine.contains("后处理结果参数") ||
               trimmedLine.contains("后处理")) {
        updateProgress("FDTD post-processing calculating");
        addLogMessage("检测到后处理阶段");
    } else if (trimmedLine.contains("开始迭代")) {
        updateProgress("Start FDTD calculation");
    } else if (trimmedLine.contains("fdtd参数") ||
               trimmedLine.contains("cpml参数") ||
               trimmedLine.contains("connect参数") ||
               trimmedLine.contains("extraTime参数") ||
               trimmedLine.contains("extraFre参数")) {
        updateProgress("Initializing FDTD parameters");
    }
}

void SimulationSetUp::updateTimer()
{
    m_elapsedSeconds++;
    QTime time(0, 0);
    time = time.addSecs(m_elapsedSeconds);
    m_timeLabel->setText("Time: " + time.toString("hh:mm:ss"));
}

void printGlobalVariables_run() {
    // qDebug() << "=== 运行时全局变量值 ===";
    // qDebug() << "端口信息:";
    // qDebug() << "source_type:" << source_type_int << "地址:" << &source_type_int;
    // qDebug() << "impedance:" << impedance << "地址:" << &impedance;
    // qDebug() << "min_x:" << min_x << "地址:" << &min_x;
    // qDebug() << "max_x:" << max_x << "地址:" << &max_x;
    // qDebug() << "min_y:" << min_y << "地址:" << &min_y;
    // qDebug() << "max_y:" << max_y << "地址:" << &max_y;
    // qDebug() << "min_z:" << min_z << "地址:" << &min_z;
    // qDebug() << "max_z:" << max_z << "地址:" << &max_z;
    // qDebug() << "DiscretePorts_direction:" << DiscretePorts_direction << "地址:" << &DiscretePorts_direction;
    // qDebug() << "探针信息:";
    // qDebug() << "场类型:" << nearfieldprobe_EH;
    // qDebug() << "方向:" << nearfieldprobe_direction;
    // qDebug() << "位置: (" << nearfieldprobe_x << "," << nearfieldprobe_y << "," << nearfieldprobe_z << ")";
    // qDebug() << "===================";
}

void SimulationSetUp::onProcessStarted()
{
    qDebug() << "进程已启动，准备发送仿真时间：" << simulationTime;

    // 重置计时器
    m_elapsedSeconds = 0;
    m_elapsedTimer.start();
    m_timer->start(1000);  // 每秒更新一次

    // 更新进度状态
    updateProgress("Starting FDTD calculation");
    m_timeLabel->setText("Time: 00:00:00");

    // 发送仿真时间参数
    QString input = QString::number(simulationTime, 'f', 6) + "\n";
    QTimer::singleShot(100, [this, input]() {
        if (simulationProcess && simulationProcess->state() == QProcess::Running) {
            simulationProcess->write(input.toLocal8Bit());
            simulationProcess->closeWriteChannel();
            qDebug() << "已发送仿真时间到外部程序：" << input;
            updateProgress("The FDTD program has been initiated now initializing");
        }
    });
}

void SimulationSetUp::onProcessReadyRead()
{
    if (!simulationProcess) {
        return;
    }

    QByteArray output = simulationProcess->readAll();
    QString outputText = QString::fromLocal8Bit(output);

    // 按行分割输出
    QStringList lines = outputText.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        qDebug() << "Process output:" << line;
        parseOutputLine(line);
    }
}

void SimulationSetUp::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_timer->stop();
    // 重置用户停止标志
    m_isUserStop = false;

    // 更新按钮状态
    simulationButton->setEnabled(true);
    cancelButton->setEnabled(true);

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        updateProgress("Calculation completed");
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
        QMessageBox::information(this, "提示", "仿真计算已完成");
    } else {
        updateProgress("Abnormal end of calculation");
        QString errorMessage = QString("Simulation failed with exit code: %1").arg(exitCode);
        QMessageBox::critical(this, "Error", errorMessage);
    }
}

void SimulationSetUp::onProcessErrorOccurred(QProcess::ProcessError error)
{
    m_timer->stop();

    // 如果是用户主动停止导致的错误，不显示错误提示
    if (m_isUserStop) {
        m_isUserStop = false;  // 重置标志
        return;  // 直接返回，不显示错误提示
    }
    // 更新按钮状态
    simulationButton->setEnabled(true);
    cancelButton->setEnabled(true);

    QString errorMsg;
    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = "无法启动程序，请检查文件是否存在及权限";
        break;
    case QProcess::Crashed:
        errorMsg = "程序崩溃";
        break;
    case QProcess::Timedout:
        errorMsg = "程序超时";
        break;
    case QProcess::WriteError:
        errorMsg = "写入错误";
        break;
    case QProcess::ReadError:
        errorMsg = "读取错误";
        break;
    default:
        errorMsg = "未知错误";
        break;
    }

    updateProgress("启动失败");
    QMessageBox::critical(this, "Error",
                          QString("启动仿真程序失败:\n%1").arg(errorMsg));
}

void SimulationSetUp::onSimulationSetUpActionTriggered()
{
    // 1. 先校验并获取用户输入的仿真时间
    if (!getInputValues(simulationTime)) {
        return;
    }
    qDebug() << "用户输入的仿真时间：" << simulationTime << " ns";

    // 2. 确保只有一个进程实例
    if (simulationProcess && simulationProcess->state() == QProcess::Running) {
        QMessageBox::information(this, "Process Running", "Simulation is already running.");
        return;
    }

    // 3. 清理之前的进程
    if (simulationProcess) {
        if (simulationProcess->state() == QProcess::Running) {
            simulationProcess->terminate();
            if (!simulationProcess->waitForFinished(1000)) {
                simulationProcess->kill();
            }
        }
        delete simulationProcess;
        simulationProcess = nullptr;
    }

    // 4. 重置进度显示
    m_progressBar->setRange(0, 0);
    //m_progressBar->setValue(0);
    updateProgress("Start Up...");
    m_timeLabel->setText("Time: 00:00:00");

    // 5. 创建新进程
    simulationProcess = new QProcess(this);

    // 6. 连接信号
    connect(simulationProcess, &QProcess::started, this, &SimulationSetUp::onProcessStarted);
    connect(simulationProcess, &QProcess::readyRead, this, &SimulationSetUp::onProcessReadyRead);
    connect(simulationProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SimulationSetUp::onProcessFinished);
    connect(simulationProcess, &QProcess::errorOccurred,
            this, &SimulationSetUp::onProcessErrorOccurred);

    // 7. 设置程序路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString exePath = QDir::cleanPath(appDir + "/Project1.exe");
    qDebug() << "外部程序路径：" << exePath;

    // 检查程序是否存在
    if (!QFile::exists(exePath)) {
        QMessageBox::critical(this, "错误",
                              QString("找不到程序文件:\n%1\n请确保Project1.exe存在于应用程序目录中").arg(exePath));
        delete simulationProcess;
        simulationProcess = nullptr;
        updateProgress("找不到程序文件");
        return;
    }

    // 8. 启动进程
    simulationProcess->setWorkingDirectory(appDir);
    simulationProcess->start(exePath);

    // 9. 等待启动
    if (!simulationProcess->waitForStarted(5000)) {
        QString errorMessage;
        if (simulationProcess->error() == QProcess::FailedToStart) {
            errorMessage = "无法启动程序，请检查文件是否存在及权限";
        } else {
            errorMessage = "启动失败: " + simulationProcess->errorString();
        }

        QMessageBox::critical(this, "Error",
                              QString("启动仿真程序失败:\n%1\n%2").arg(exePath).arg(errorMessage));
        delete simulationProcess;
        simulationProcess = nullptr;
        updateProgress("启动失败");
        return;
    }
}

void SimulationSetUp::stopFdtdProcess()
{
    if (simulationProcess && simulationProcess->state() == QProcess::Running) {
        addLogMessage("正在停止计算...");
        updateProgress("正在停止计算");

        // 设置用户停止标志
        m_isUserStop = true;

        // 先尝试正常终止
        simulationProcess->terminate();

        // 如果5秒后还在运行，强制终止
        if (!simulationProcess->waitForFinished(1000)) {
            simulationProcess->kill();
            addLogMessage("计算已强制终止");
            updateProgress("计算已强制终止");

            // 手动清理进程，避免错误信号
            if (simulationProcess) {
                simulationProcess->deleteLater();
                simulationProcess = nullptr;
            }
        } else {
            addLogMessage("计算已正常停止");
            updateProgress("计算已停止 ");
        }

        // 停止计时器
        if (m_timer->isActive()) {
            m_timer->stop();
        }

        // 更新按钮状态
        simulationButton->setEnabled(true);
        cancelButton->setEnabled(true);

        // 重置进度条
        m_progressBar->setRange(0, 100);
        //m_progressBar->setValue(0);
    }
}


