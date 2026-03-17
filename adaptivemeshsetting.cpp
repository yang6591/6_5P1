#include "adaptivemeshsetting.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>


AdaptiveMeshSetting::AdaptiveMeshSetting(QWidget *parent) : QMainWindow(parent)
,adaptivemeshProcess(nullptr)
{
    setWindowTitle("AdaptiveMeshSetting");
    //setMinimumSize(800, 600);

    // 创建中心部件和主布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout *mainLayout = new QGridLayout(centralWidget);

    // 创建标签和输入框
    QLabel *waveLabel = new QLabel("Max Mesh:", this);
    waveEdit = new QLineEdit(this);
    waveEdit->setPlaceholderText("mm");

    QLabel *airLabel = new QLabel("Air Mesh:", this);
    airEdit = new QLineEdit(this);
    airEdit->setPlaceholderText("mm");

    QLabel *minxLabel = new QLabel("X-Min Mesh:", this);
    minxEdit = new QLineEdit(this);
    minxEdit->setPlaceholderText("mm");

    QLabel *minyLabel = new QLabel("Y-Min Mesh:", this);
    minyEdit = new QLineEdit(this);
    minyEdit->setPlaceholderText("mm");

    QLabel *minzLabel = new QLabel("Z-Min Mesh:", this);
    minzEdit = new QLineEdit(this);
    minzEdit->setPlaceholderText("mm");

    // 设置输入验证器（只允许输入浮点数）
    QDoubleValidator *validator = new QDoubleValidator(0.0001, 10000.0, 6, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    waveEdit->setValidator(validator);
    airEdit->setValidator(validator);
    minxEdit->setValidator(validator);
    minyEdit->setValidator(validator);
    minzEdit->setValidator(validator);

    // 创建计算按钮
    QPushButton *saveButton = new QPushButton("Save", this);
    saveButton->setFixedSize(100, 40);
    QPushButton *calculateButton = new QPushButton("Calculate", this);
    calculateButton->setFixedSize(100, 40);  // 宽度150像素，高度50像素
    QPushButton *finishButton = new QPushButton("Finish", this);
    finishButton->setFixedSize(100, 40);



    connect(saveButton, &QPushButton::clicked, this, &AdaptiveMeshSetting::onSaveClicked);
    connect(calculateButton, &QPushButton::clicked, this, &AdaptiveMeshSetting::onAdaptiveMeshingActionTriggered);
    connect(finishButton, &QPushButton::clicked, this, &AdaptiveMeshSetting::close);






    // 将组件添加到布局
    mainLayout->addWidget(waveLabel, 0, 0);
    mainLayout->addWidget(waveEdit, 0, 1);
    mainLayout->addWidget(airLabel, 1, 0);
    mainLayout->addWidget(airEdit, 1, 1);
    mainLayout->addWidget(minxLabel, 2, 0);
    mainLayout->addWidget(minxEdit, 2, 1);
    mainLayout->addWidget(minyLabel, 3, 0);
    mainLayout->addWidget(minyEdit, 3, 1);
    mainLayout->addWidget(minzLabel, 4, 0);
    mainLayout->addWidget(minzEdit, 4, 1);
    //mainLayout->addWidget(calculateButton, 5, 0, 1, 2);

    mainLayout->addWidget(saveButton, 5, 0);  // 计算按钮在第5行第0列
    mainLayout->addWidget(calculateButton, 5, 1);
    mainLayout->addWidget(finishButton, 5, 2);     // 完成按钮在第5行第1列

    // 添加一些间距
    //mainLayout->setRowStretch(5, 1);
    mainLayout->setColumnStretch(2, 1);

    // 设置边距和间距
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(30);

    // 连接按钮信号
    //connect(calculateButton, &QPushButton::clicked, this, &AdaptiveMeshSetting::onCalculateClicked);
}

AdaptiveMeshSetting::~AdaptiveMeshSetting()
{
}


// // 验证输入并获取值
// bool AdaptiveMeshSetting::getInputValues(double &wave,double &air, double &minx, double &miny, double &minz)
// {
//     bool ok1, ok2, ok3, ok4,ok5;
//     wave = waveEdit->text().toDouble(&ok1);
//     air = airEdit->text().toDouble(&ok2);
//     minx = minxEdit->text().toDouble(&ok3);
//     miny = minyEdit->text().toDouble(&ok4);
//     minz = minzEdit->text().toDouble(&ok5);

//     if (!ok1 || !ok2 || !ok3 || !ok4||!ok5) {
//         QMessageBox::warning(this, "输入错误", "请输入有效的数值");
//         return false;
//     }

//     if (wave <= 0 || minx <= 0 || miny <= 0 || minz <= 0) {
//         QMessageBox::warning(this, "输入错误", "所有值必须大于0");
//         return false;
//     }

//     // 确保最大网格大于最小网格
//     if (wave < minx || wave < miny || wave < minz) {
//         QMessageBox::warning(this, "输入错误", "最大网格尺寸必须大于各方向的最小网格");
//         return false;
//     }

//     return true;
// }




// void AdaptiveMeshSetting::onAdaptiveMeshingActionTriggered()
// {
//     // 获取输入值
//     double wave,air, minx, miny, minz;
//     if (!getInputValues(wave, air,minx, miny, minz)) {
//         return; // 验证失败，不启动进程
//     }

//     // 确保只有一个进程实例
//     if (adaptivemeshProcess && adaptivemeshProcess->state() == QProcess::Running) {
//         QMessageBox::information(this, "Process Running", "Adaptive Meshing is already running.");
//         return;
//     }

//     // 清理之前的进程（如果存在）
//     if (adaptivemeshProcess) {
//         delete adaptivemeshProcess;
//     }

//     // 创建新进程
//     adaptivemeshProcess = new QProcess(this);

//     // 设置输入模式为文本模式
//     adaptivemeshProcess->setInputChannelMode(QProcess::ManagedInputChannel);//允许通过Qt接口管理标准输入
//     adaptivemeshProcess->setProcessChannelMode(QProcess::MergedChannels);//合并标准输出和标准错误流，简化处理

//     // 连接信号处理输出和错误
//     connect(adaptivemeshProcess, &QProcess::readyReadStandardOutput, [=]() {
//         QString output = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardOutput());
//         qDebug() << "Output:" << output;
//     });

//     connect(adaptivemeshProcess, &QProcess::readyReadStandardError, [=]() {
//         QString error = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardError());
//         qDebug() << "Error:" << error;
//     });

//     connect(adaptivemeshProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//             [=](int exitCode, QProcess::ExitStatus exitStatus) {
//                 qDebug() << "Process finished with code:" << exitCode;
//                 if (exitCode != 0) {
//                     QString errorMessage = QString("Adaptive Meshing failed with exit code: %1").arg(exitCode);
//                     QMessageBox::critical(this, "Error", errorMessage);
//                 }else {
//                     // 新增：进程成功执行结束时的完成提示
//                     QMessageBox::information(this, "提示", "网格剖分计算已完成");
//                 }
//             });

//     // 设置要运行的程序路径（根据实际路径修改）
//     QString appDir = QCoreApplication::applicationDirPath();
//     QString program = QDir::cleanPath(appDir + "/output.exe");//QDir::cleanPath()确保跨平台路径兼容性

//     // 设置输入和输出文件路径
//     QString inputFile = QDir::cleanPath(appDir + "/stl");//文件夹，里面只能放stl文件
//     QString outputFile = QDir::cleanPath(appDir + "/meshoutput.txt");

//     // 检查输入文件是否存在
//     QFileInfo inputFileInfo(inputFile);
//     if (!inputFileInfo.exists()) {
//         QMessageBox::critical(this, "Error",
//                               QString("Input file does not exist:\n%1").arg(inputFile));
//         return;
//     }

//     // 确保输出目录是否存在
//     QFileInfo outputFileInfo(outputFile);
//     QDir outputDir = outputFileInfo.absoluteDir();
//     if (!outputDir.exists()) {
//         if (!outputDir.mkpath(".")) {
//             QMessageBox::critical(this, "Error",
//                                   QString("Failed to create output directory:\n%1").arg(outputDir.path()));
//             return;
//         }
//     }

//     // 设置参数
//     QStringList arguments;
//     arguments << "--input" << inputFile;//指定输入文件
//     arguments << "--output" << outputFile;//指定输出文件

//     // 设置外部程序的工作目录为输入文件所在目录
//     //确保程序能正确访问相关资源文件
//     adaptivemeshProcess->setWorkingDirectory(inputFileInfo.absolutePath());

//     // 启动程序
//     adaptivemeshProcess->start(program, arguments);

//     // 等待启动（最多5秒）
//     if (!adaptivemeshProcess->waitForStarted(5000)) {//等待程序启动（最多5秒）
//         QString errorMessage;
//         if (adaptivemeshProcess->error() == QProcess::FailedToStart) {
//             errorMessage = "Failed to start program. Check if it exists and has execute permission.";
//         } else {
//             errorMessage = "Process failed to start: " + adaptivemeshProcess->errorString();
//         }

//         QMessageBox::critical(this, "Error",
//                               QString("Failed to start Adaptive Meshing program:\n%1\n%2")
//                                   .arg(program).arg(errorMessage));
//         return;
//     }

//     // 准备要写入的数据 - 格式化为字符串，每个值一行
//     QString inputData = QString("%1\n%2\n%3\n%4\n%5\n")
//                             .arg(wave, 0, 'f', 6)  // 保留6位小数
//                             .arg(air, 0, 'f', 6)
//                             .arg(minx, 0, 'f', 6)
//                             .arg(miny, 0, 'f', 6)
//                             .arg(minz, 0, 'f', 6);

//     qDebug() << "Writing input data to process:\n" << inputData;

//     // 将数据写入进程的标准输入
//     adaptivemeshProcess->write(inputData.toUtf8());

//     // 关闭写入通道，表示输入结束
//     adaptivemeshProcess->closeWriteChannel();
// }

// 验证输入并获取值
bool AdaptiveMeshSetting::getInputValues(double &wave,double &air, double &minx, double &miny, double &minz)
{
    bool ok1, ok2, ok3, ok4,ok5;
    wave = waveEdit->text().toDouble(&ok1);
    air = airEdit->text().toDouble(&ok2);
    minx = minxEdit->text().toDouble(&ok3);
    miny = minyEdit->text().toDouble(&ok4);
    minz = minzEdit->text().toDouble(&ok5);

    if (!ok1 || !ok2 || !ok3 || !ok4||!ok5) {
        QMessageBox::warning(this, "输入错误", "请输入有效的数值");
        return false;
    }

    if (wave <= 0 || minx <= 0 || miny <= 0 || minz <= 0) {
        QMessageBox::warning(this, "输入错误", "所有值必须大于0");
        return false;
    }

    // 确保最大网格大于最小网格
    if (wave < minx || wave < miny || wave < minz) {
        QMessageBox::warning(this, "输入错误", "最大网格尺寸必须大于各方向的最小网格");
        return false;
    }

    return true;
}

// void AdaptiveMeshSetting::onCalculateClicked()
// {
//     double wave, minx, miny, minz;
//     if (!getInputValues(wave, minx, miny, minz)) {
//         return;
//     }

//     qDebug() << "开始网格剖分计算，参数:";
//     qDebug() << "最大网格:" << wave;
//     qDebug() << "X最小网格:" << minx;
//     qDebug() << "Y最小网格:" << miny;
//     qDebug() << "Z最小网格:" << minz;
// }

// 保存设置到文件
void AdaptiveMeshSetting::onSaveClicked()
{
    double wave, air, minx, miny, minz;
    if (!getInputValues(wave, air, minx, miny, minz)) {
        return; // 验证失败，不保存
    }

    // 获取应用程序目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString settingsFile = QDir::cleanPath(appDir + "/adaptivemeshsettings.txt");

    QFile file(settingsFile);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误",
                              QString("无法创建文件: %1\n%2")
                                  .arg(settingsFile)
                                  .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);

    // 写入分隔行
    out << "=====================================\n";

    // 写入标题行（使用制表符分隔）
    out << "Max Mesh\tAir Mesh\tX-Min\tY-Min\tZ-Min\n";

    // 写入分隔行
    out << "=====================================\n";

    // 写入数值行（使用制表符分隔）
    out << QString("%1\t%2\t%3\t%4\t%5\n")
               .arg(wave, 0, 'f', 6)
               .arg(air, 0, 'f', 6)
               .arg(minx, 0, 'f', 6)
               .arg(miny, 0, 'f', 6)
               .arg(minz, 0, 'f', 6);

    file.close();

    QMessageBox::information(this, "成功",
                             QString("设置已保存到:\n%1")
                                 .arg(settingsFile));

    qDebug() << "设置已保存到:" << settingsFile;
    qDebug() << "Max Mesh:" << wave;
    qDebug() << "Air Mesh:" << air;
    qDebug() << "X-Min:" << minx;
    qDebug() << "Y-Min:" << miny;
    qDebug() << "Z-Min:" << minz;
}



// void AdaptiveMeshSetting::onAdaptiveMeshingActionTriggered()
// {
//     // 获取输入值
//     double wave,air, minx, miny, minz;
//     if (!getInputValues(wave, air,minx, miny, minz)) {
//         return; // 验证失败，不启动进程
//     }

//     // 确保只有一个进程实例
//     if (adaptivemeshProcess && adaptivemeshProcess->state() == QProcess::Running) {
//         QMessageBox::information(this, "Process Running", "Adaptive Meshing is already running.");
//         return;
//     }

//     // 清理之前的进程（如果存在）
//     if (adaptivemeshProcess) {
//         delete adaptivemeshProcess;
//     }

//     // 创建新进程
//     adaptivemeshProcess = new QProcess(this);

//     // 设置输入模式为文本模式
//     adaptivemeshProcess->setInputChannelMode(QProcess::ManagedInputChannel);//允许通过Qt接口管理标准输入
//     adaptivemeshProcess->setProcessChannelMode(QProcess::MergedChannels);//合并标准输出和标准错误流，简化处理

//     // 连接信号处理输出和错误
//     connect(adaptivemeshProcess, &QProcess::readyReadStandardOutput, [=]() {
//         QString output = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardOutput());
//         qDebug() << "Output:" << output;
//     });

//     connect(adaptivemeshProcess, &QProcess::readyReadStandardError, [=]() {
//         QString error = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardError());
//         qDebug() << "Error:" << error;
//     });

//     connect(adaptivemeshProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//             [=](int exitCode, QProcess::ExitStatus exitStatus) {
//                 qDebug() << "Process finished with code:" << exitCode;
//                 if (exitCode != 0) {
//                     QString errorMessage = QString("Adaptive Meshing failed with exit code: %1").arg(exitCode);
//                     QMessageBox::critical(this, "Error", errorMessage);
//                 }else {
//                     // 新增：进程成功执行结束时的完成提示
//                     QMessageBox::information(this, "提示", "网格剖分计算已完成");
//                 }
//             });

//     // 设置要运行的程序路径（根据实际路径修改）
//     QString appDir = QCoreApplication::applicationDirPath();
//     QString program = QDir::cleanPath(appDir + "/output.exe");//QDir::cleanPath()确保跨平台路径兼容性

//     // 设置输入和输出文件路径
//     QString inputFile = QDir::cleanPath(appDir + "/stl");//文件夹，里面只能放stl文件
//     QString outputFile = QDir::cleanPath(appDir + "/meshoutput.txt");

//     // 检查输入文件是否存在
//     QFileInfo inputFileInfo(inputFile);
//     if (!inputFileInfo.exists()) {
//         QMessageBox::critical(this, "Error",
//                               QString("Input file does not exist:\n%1").arg(inputFile));
//         return;
//     }

//     // 确保输出目录是否存在
//     QFileInfo outputFileInfo(outputFile);
//     QDir outputDir = outputFileInfo.absoluteDir();
//     if (!outputDir.exists()) {
//         if (!outputDir.mkpath(".")) {
//             QMessageBox::critical(this, "Error",
//                                   QString("Failed to create output directory:\n%1").arg(outputDir.path()));
//             return;
//         }
//     }

//     // 设置参数
//     QStringList arguments;
//     arguments << "--input" << inputFile;//指定输入文件
//     arguments << "--output" << outputFile;//指定输出文件

//     // 设置外部程序的工作目录为输入文件所在目录
//     //确保程序能正确访问相关资源文件
//     adaptivemeshProcess->setWorkingDirectory(inputFileInfo.absolutePath());

//     // 启动程序
//     adaptivemeshProcess->start(program, arguments);

//     // 等待启动（最多5秒）
//     if (!adaptivemeshProcess->waitForStarted(5000)) {//等待程序启动（最多5秒）
//         QString errorMessage;
//         if (adaptivemeshProcess->error() == QProcess::FailedToStart) {
//             errorMessage = "Failed to start program. Check if it exists and has execute permission.";
//         } else {
//             errorMessage = "Process failed to start: " + adaptivemeshProcess->errorString();
//         }

//         QMessageBox::critical(this, "Error",
//                               QString("Failed to start Adaptive Meshing program:\n%1\n%2")
//                                   .arg(program).arg(errorMessage));
//         return;
//     }

//     // 准备要写入的数据 - 格式化为字符串，每个值一行
//     QString inputData = QString("%1\n%2\n%3\n%4\n%5\n")
//                             .arg(wave, 0, 'f', 6)  // 保留6位小数
//                             .arg(air, 0, 'f', 6)
//                             .arg(minx, 0, 'f', 6)
//                             .arg(miny, 0, 'f', 6)
//                             .arg(minz, 0, 'f', 6);

//     qDebug() << "Writing input data to process:\n" << inputData;

//     // 将数据写入进程的标准输入
//     adaptivemeshProcess->write(inputData.toUtf8());

//     // 关闭写入通道，表示输入结束
//     adaptivemeshProcess->closeWriteChannel();
// }



// void AdaptiveMeshSetting::onAdaptiveMeshingActionTriggered()
// {
//     // 获取输入值并进行验证
//     double wave, air, minx, miny, minz;
//     if (!getInputValues(wave, air, minx, miny, minz)) {
//         return; // 验证失败，不启动进程
//     }

//     // 确保只有一个进程实例
//     if (adaptivemeshProcess && adaptivemeshProcess->state() == QProcess::Running) {
//         QMessageBox::information(this, "Process Running", "Adaptive Meshing is already running.");
//         return;
//     }

//     // 清理之前的进程（如果存在）
//     if (adaptivemeshProcess) {
//         delete adaptivemeshProcess;
//     }

//     // 创建新进程
//     adaptivemeshProcess = new QProcess(this);

//     // 合并标准输出和标准错误流
//     adaptivemeshProcess->setProcessChannelMode(QProcess::MergedChannels);

//     // 连接信号处理输出和错误
//     connect(adaptivemeshProcess, &QProcess::readyRead, [=]() {
//         QString output = QString::fromLocal8Bit(adaptivemeshProcess->readAll());
//         qDebug() << "Program output:" << output;
//     });

//     connect(adaptivemeshProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//             [=](int exitCode, QProcess::ExitStatus exitStatus) {
//                 qDebug() << "Process finished with code:" << exitCode;
//                 if (exitCode != 0) {
//                     QString errorMessage = QString("Adaptive Meshing failed with exit code: %1").arg(exitCode);
//                     QMessageBox::critical(this, "Error", errorMessage);
//                 } else {
//                     QMessageBox::information(this, "提示", "网格剖分计算已完成");
//                 }
//             });

//     // 设置要运行的程序路径
//     QString appDir = QCoreApplication::applicationDirPath();
//     QString program = QDir::cleanPath(appDir + "/output.exe");

//     // 启动程序，不带任何参数
//     adaptivemeshProcess->start(program);

//     // 等待启动（最多5秒）
//     if (!adaptivemeshProcess->waitForStarted(5000)) {
//         QString errorMessage;
//         if (adaptivemeshProcess->error() == QProcess::FailedToStart) {
//             errorMessage = "Failed to start program. Check if it exists and has execute permission.";
//         } else {
//             errorMessage = "Process failed to start: " + adaptivemeshProcess->errorString();
//         }

//         QMessageBox::critical(this, "Error",
//                               QString("Failed to start Adaptive Meshing program:\n%1\n%2")
//                                   .arg(program).arg(errorMessage));
//         return;
//     }
// }


void AdaptiveMeshSetting::onAdaptiveMeshingActionTriggered()
{
    // 获取输入值并进行验证
    double wave, air, minx, miny, minz;
    if (!getInputValues(wave, air, minx, miny, minz)) {
        return; // 验证失败，不启动进程
    }

    // 确保只有一个进程实例
    if (adaptivemeshProcess && adaptivemeshProcess->state() == QProcess::Running) {
        QMessageBox::information(this, "Process Running", "Adaptive Meshing is already running.");
        return;
    }

    // 清理之前的进程（如果存在）
    if (adaptivemeshProcess) {
        delete adaptivemeshProcess;
    }

    // 创建新进程
    adaptivemeshProcess = new QProcess(this);

    // 设置输入模式为文本模式
    adaptivemeshProcess->setInputChannelMode(QProcess::ManagedInputChannel);
    adaptivemeshProcess->setProcessChannelMode(QProcess::MergedChannels);

    // 连接信号处理输出和错误
    connect(adaptivemeshProcess, &QProcess::readyReadStandardOutput, [=]() {
        QString output = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardOutput());
        qDebug() << "Output:" << output;
    });

    connect(adaptivemeshProcess, &QProcess::readyReadStandardError, [=]() {
        QString error = QString::fromLocal8Bit(adaptivemeshProcess->readAllStandardError());
        qDebug() << "Error:" << error;
    });

    connect(adaptivemeshProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus) {
                qDebug() << "Process finished with code:" << exitCode;
                if (exitCode != 0) {
                    QString errorMessage = QString("Adaptive Meshing failed with exit code: %1").arg(exitCode);
                    QMessageBox::critical(this, "Error", errorMessage);
                } else {
                    QMessageBox::information(this, "提示", "网格剖分计算已完成");
                }
            });

    // 设置要运行的程序路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString program = QDir::cleanPath(appDir + "/output.exe");

    // 设置输入和输出文件路径（如果程序需要的话）
    QString inputFile = QDir::cleanPath(appDir + "/stl");
    QString outputFile = QDir::cleanPath(appDir + "/meshoutput.txt");

    // 检查输入文件是否存在
    QFileInfo inputFileInfo(inputFile);
    if (!inputFileInfo.exists()) {
        QMessageBox::critical(this, "Error",
                              QString("Input file does not exist:\n%1").arg(inputFile));
        return;
    }

    // 确保输出目录是否存在
    QFileInfo outputFileInfo(outputFile);
    QDir outputDir = outputFileInfo.absoluteDir();
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::critical(this, "Error",
                                  QString("Failed to create output directory:\n%1").arg(outputDir.path()));
            return;
        }
    }

    // 设置参数（根据实际需要调整）
    QStringList arguments;
    arguments << "--input" << inputFile;
    arguments << "--output" << outputFile;

    // 设置外部程序的工作目录
    adaptivemeshProcess->setWorkingDirectory(inputFileInfo.absolutePath());

    // 启动程序
    adaptivemeshProcess->start(program, arguments);

    // 等待启动（最多5秒）
    if (!adaptivemeshProcess->waitForStarted(5000)) {
        QString errorMessage;
        if (adaptivemeshProcess->error() == QProcess::FailedToStart) {
            errorMessage = "Failed to start program. Check if it exists and has execute permission.";
        } else {
            errorMessage = "Process failed to start: " + adaptivemeshProcess->errorString();
        }

        QMessageBox::critical(this, "Error",
                              QString("Failed to start Adaptive Meshing program:\n%1\n%2")
                                  .arg(program).arg(errorMessage));
        return;
    }

    // 注意：移除了向进程写入数据的部分
    // 程序会自行读取参数，不需要通过标准输入传递
}
