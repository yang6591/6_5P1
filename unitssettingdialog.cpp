#include "unitssettingdialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QTextStream>

UnitsSettingDialog::UnitsSettingDialog(QWidget *parent)
    : QDialog(parent)
{
    initUI();
    initConnections();

    // 从文件加载设置
    loadSettingsFromFile();

    // 设置窗口标题和大小
    setWindowTitle("Units");
    setFixedSize(400, 400);
}

UnitsSettingDialog::~UnitsSettingDialog()
{
}

void UnitsSettingDialog::initUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建项目结果分组框
    QGroupBox *projectGroupBox = new QGroupBox("Project Results");
    QGridLayout *gridLayout = new QGridLayout(projectGroupBox);

    // 第1行：尺寸单位
    QLabel *dimensionLabel = new QLabel("Dimensions:");
    dimensionComboBox = new QComboBox();
    dimensionComboBox->addItems({"mm", "cm", "m", "um", "nm"});
    // 注意：这里不设置默认值，将从文件中读取

    gridLayout->addWidget(dimensionLabel, 0, 0);
    gridLayout->addWidget(dimensionComboBox, 0, 1);

    // 第2行：频率单位
    QLabel *frequencyLabel = new QLabel("Frequency:");
    frequencyComboBox = new QComboBox();
    frequencyComboBox->addItems({"GHz", "MHz", "kHz", "Hz"});
    // 注意：这里不设置默认值，将从文件中读取

    gridLayout->addWidget(frequencyLabel, 1, 0);
    gridLayout->addWidget(frequencyComboBox, 1, 1);

    // 第3行：温度单位
    QLabel *temperatureLabel = new QLabel("Temperature:");
    temperatureComboBox = new QComboBox();
    temperatureComboBox->addItems({"℃", "K", "℉"});
    // 注意：这里不设置默认值，将从文件中读取

    gridLayout->addWidget(temperatureLabel, 2, 0);
    gridLayout->addWidget(temperatureComboBox, 2, 1);

    // 第4行：时间单位
    QLabel *timeLabel = new QLabel("Time:");
    timeComboBox = new QComboBox();
    timeComboBox->addItems({"ns", "us", "ms", "s"});
    // 注意：这里不设置默认值，将从文件中读取

    gridLayout->addWidget(timeLabel, 3, 0);
    gridLayout->addWidget(timeComboBox, 3, 1);

    // 第5行：电压单位（不可修改）
    QLabel *voltageLabelText = new QLabel("Voltage:");
    voltageLabel = new QLabel("V");
    voltageLabel->setStyleSheet("color: gray;");  // 灰色表示不可修改

    gridLayout->addWidget(voltageLabelText, 4, 0);
    gridLayout->addWidget(voltageLabel, 4, 1);

    // 第6行：电流单位（不可修改）
    QLabel *currentLabelText = new QLabel("Current:");
    currentLabel = new QLabel("A");
    currentLabel->setStyleSheet("color: gray;");

    gridLayout->addWidget(currentLabelText, 5, 0);
    gridLayout->addWidget(currentLabel, 5, 1);

    // 第7行：电导单位（不可修改）
    QLabel *conductanceLabelText = new QLabel("Conductance:");
    conductanceLabel = new QLabel("S");
    conductanceLabel->setStyleSheet("color: gray;");

    gridLayout->addWidget(conductanceLabelText, 6, 0);
    gridLayout->addWidget(conductanceLabel, 6, 1);

    // 第8行：电阻单位（不可修改）
    QLabel *resistanceLabelText = new QLabel("Resistance:");
    resistanceLabel = new QLabel("Ohm");
    resistanceLabel->setStyleSheet("color: gray;");

    gridLayout->addWidget(resistanceLabelText, 7, 0);
    gridLayout->addWidget(resistanceLabel, 7, 1);

    // 第9行：电感单位（不可修改）
    QLabel *inductanceLabelText = new QLabel("Inductance:");
    inductanceLabel = new QLabel("nH");
    inductanceLabel->setStyleSheet("color: gray;");

    gridLayout->addWidget(inductanceLabelText, 8, 0);
    gridLayout->addWidget(inductanceLabel, 8, 1);

    // 第10行：电容单位（不可修改）
    QLabel *capacitanceLabelText = new QLabel("Capacitance:");
    capacitanceLabel = new QLabel("pF");
    capacitanceLabel->setStyleSheet("color: gray;");

    gridLayout->addWidget(capacitanceLabelText, 9, 0);
    gridLayout->addWidget(capacitanceLabel, 9, 1);

    // 将分组框添加到主布局
    mainLayout->addWidget(projectGroupBox);

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    okButton = new QPushButton("Ok");
    cancelButton = new QPushButton("Cancel");
    //helpButton = new QPushButton("帮助");

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    //buttonLayout->addWidget(helpButton);

    // 将按钮布局添加到主布局
    mainLayout->addLayout(buttonLayout);

    // 设置布局
    setLayout(mainLayout);
}

void UnitsSettingDialog::initConnections()
{
    connect(okButton, &QPushButton::clicked, this, &UnitsSettingDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &UnitsSettingDialog::onCancelClicked);
    //connect(helpButton, &QPushButton::clicked, this, &UnitsSettingDialog::onHelpClicked);
}

void UnitsSettingDialog::loadSettingsFromFile()
{
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/unit.txt");

    qDebug() << "尝试从文件加载设置:" << fileName;

    QFile file(fileName);
    if (!file.exists()) {
        qDebug() << "unit.txt文件不存在，使用默认设置";
        // 设置默认值
        dimensionComboBox->setCurrentText("mm");
        frequencyComboBox->setCurrentText("GHz");
        temperatureComboBox->setCurrentText("℃");
        timeComboBox->setCurrentText("ns");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开unit.txt文件，使用默认设置";
        // 设置默认值
        dimensionComboBox->setCurrentText("mm");
        frequencyComboBox->setCurrentText("GHz");
        temperatureComboBox->setCurrentText("℃");
        timeComboBox->setCurrentText("ns");
        return;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    file.close();

    // 检查文件是否有足够的内容
    if (lines.size() < 4) {
        qDebug() << "unit.txt文件内容不足4行，使用默认设置";
        // 设置默认值
        dimensionComboBox->setCurrentText("mm");
        frequencyComboBox->setCurrentText("GHz");
        temperatureComboBox->setCurrentText("℃");
        timeComboBox->setCurrentText("ns");
        return;
    }

    // 第4行是数据行（索引为3）
    QString dataLine = lines[3].trimmed();
    if (dataLine.isEmpty()) {
        qDebug() << "unit.txt文件第4行为空，使用默认设置";
        // 设置默认值
        dimensionComboBox->setCurrentText("mm");
        frequencyComboBox->setCurrentText("GHz");
        temperatureComboBox->setCurrentText("℃");
        timeComboBox->setCurrentText("ns");
        return;
    }

    // 解析数据行
    QStringList units = dataLine.split('\t');
    if (units.size() < 4) {
        qDebug() << "unit.txt文件第4行格式不正确，使用默认设置";
        // 设置默认值
        dimensionComboBox->setCurrentText("mm");
        frequencyComboBox->setCurrentText("GHz");
        temperatureComboBox->setCurrentText("℃");
        timeComboBox->setCurrentText("ns");
        return;
    }

    // 设置从文件中读取的值
    QString dimension = units[0].trimmed();
    QString frequency = units[1].trimmed();
    QString temperatureFile = units[2].trimmed();
    QString time = units[3].trimmed();

    // 将文件单位转换为界面显示单位
    QString temperatureDisplay = temperatureFileToDisplay(temperatureFile);

    qDebug() << "从文件加载的单位设置:";
    qDebug() << "尺寸单位:" << dimension;
    qDebug() << "频率单位:" << frequency;
    qDebug() << "温度单位(文件):" << temperatureFile;
    qDebug() << "温度单位(显示):" << temperatureDisplay;
    qDebug() << "时间单位:" << time;

    // 设置下拉框的值
    int dimIndex = dimensionComboBox->findText(dimension);
    if (dimIndex >= 0) {
        dimensionComboBox->setCurrentIndex(dimIndex);
    } else {
        dimensionComboBox->setCurrentText("mm");
        qDebug() << "尺寸单位" << dimension << "不在选项中，使用默认值mm";
    }

    int freqIndex = frequencyComboBox->findText(frequency);
    if (freqIndex >= 0) {
        frequencyComboBox->setCurrentIndex(freqIndex);
    } else {
        frequencyComboBox->setCurrentText("GHz");
        qDebug() << "频率单位" << frequency << "不在选项中，使用默认值GHz";
    }

    int tempIndex = temperatureComboBox->findText(temperatureDisplay);
    if (tempIndex >= 0) {
        temperatureComboBox->setCurrentIndex(tempIndex);
    } else {
        temperatureComboBox->setCurrentText("℃");
        qDebug() << "温度单位" << temperatureDisplay << "不在选项中，使用默认值℃";
    }

    int timeIndex = timeComboBox->findText(time);
    if (timeIndex >= 0) {
        timeComboBox->setCurrentIndex(timeIndex);
    } else {
        timeComboBox->setCurrentText("ns");
        qDebug() << "时间单位" << time << "不在选项中，使用默认值ns";
    }
}

QString UnitsSettingDialog::temperatureDisplayToFile(const QString &displayUnit)
{
    if (displayUnit == "℃") {
        return "C";
    } else if (displayUnit == "K") {
        return "K";
    } else if (displayUnit == "℉") {
        return "F";
    } else {
        return displayUnit; // 兜底处理
    }
}

QString UnitsSettingDialog::temperatureFileToDisplay(const QString &fileUnit)
{
    if (fileUnit == "C") {
        return "℃";
    } else if (fileUnit == "K") {
        return "K";
    } else if (fileUnit == "F") {
        return "℉";
    } else {
        return fileUnit; // 兜底处理
    }
}

void UnitsSettingDialog::onOkClicked()
{
    // 获取当前选择的单位
    QString dimension = dimensionComboBox->currentText();
    QString frequency = frequencyComboBox->currentText();
    QString temperature = temperatureComboBox->currentText();
    QString time = timeComboBox->currentText();

    qDebug() << "单位设置已保存:";
    qDebug() << "尺寸单位:" << dimension;
    qDebug() << "频率单位:" << frequency;
    qDebug() << "温度单位:" << temperature;
    qDebug() << "时间单位:" << time;

    // 将温度单位转换为文件写入格式
    QString temperatureFile = temperatureDisplayToFile(temperature);

    // 生成unit.txt文件
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/unit.txt");

    // 生成unit.txt文件
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入前三行固定格式
        out << "=============================================\n";
        out << "dimension\tfrequency\ttemperature\ttime\n";
        out << "=============================================\n";

        // 写入第四行：四个单位的值，用制表符分隔
        out << dimension << "\t"
            << frequency << "\t"
            << temperatureFile << "\t"  // 使用转换后的温度单位
            << time << "\n";

        file.close();
        qDebug() << "unit.txt文件已生成";

        // 显示成功消息
        QMessageBox::information(this, "成功", "单位设置已保存并写入文件。");

    } else {
        qDebug() << "无法创建unit.txt文件";
        QMessageBox::critical(this, "错误", "无法创建单位设置文件。");
    }

    accept();  // 关闭对话框并返回QDialog::Accepted
}

void UnitsSettingDialog::onCancelClicked()
{
    reject();  // 关闭对话框并返回QDialog::Rejected
}

void UnitsSettingDialog::onHelpClicked()
{
    // 显示帮助信息
    qDebug() << "显示单位设置帮助";
    // 这里可以添加显示帮助文档的逻辑
}

QString UnitsSettingDialog::getDimensionUnit() const
{
    return dimensionComboBox->currentText();
}

QString UnitsSettingDialog::getFrequencyUnit() const
{
    return frequencyComboBox->currentText();
}

QString UnitsSettingDialog::getTemperatureUnit() const
{
    return temperatureComboBox->currentText();
}

QString UnitsSettingDialog::getTimeUnit() const
{
    return timeComboBox->currentText();
}
