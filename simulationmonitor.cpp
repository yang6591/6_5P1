#include "simulationmonitor.h"
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

simulationmonitor::simulationmonitor(Monitor_data* data, QWidget* parent)
    : QDialog(parent)
    , m_monitorData(data)
{
    setupUI();
    setupConnections();

    // 设置对话框属性
    setWindowTitle("Monitor");
    setModal(true);
    resize(500, 450);

    // 初始化数据
    if (m_monitorData) {
        // 设置监视器类型
        if (m_monitorData->monitor_type >= 0 && m_monitorData->monitor_type <= 8) {
            QAbstractButton* button = monitorTypeGroup->button(m_monitorData->monitor_type);
            if (button) {
                button->setChecked(true);
            }
        }

        // 设置名称
        nameEdit->setText(m_monitorData->monitor_name);
        autoNameCheck->setChecked(m_monitorData->auto_name);

        // 设置规格类型
        if (m_monitorData->spec_type >= 0 && m_monitorData->spec_type <= 2) {
            QAbstractButton* button = specTypeGroup->button(m_monitorData->spec_type);
            if (button) {
                button->setChecked(true);
            }
        }

        // 设置频率数据
        freqListEdit->setText(m_monitorData->frequency_list);
        freqMinEdit->setText(QString::number(m_monitorData->freq_min));
        freqMaxEdit->setText(QString::number(m_monitorData->freq_max));

        // 设置时间数据
        startTimeEdit->setText(QString::number(m_monitorData->start_time));
        stepWidthEdit->setText(QString::number(m_monitorData->step_width));
        endTimeEdit->setText(QString::number(m_monitorData->end_time));
        endTimeCheck->setChecked(m_monitorData->use_end_time);

        // 设置瞬态宽带数据
        samplesEdit->setText(QString::number(m_monitorData->samples));
        accuracyEdit->setText(QString::number(m_monitorData->accuracy, 'g', 3));
        transientFarfieldsCheck->setChecked(m_monitorData->transient_farfields);

        // 根据初始监视器类型更新界面
        onMonitorTypeChanged();

        // 根据规格类型显示对应的设置页面
        settingsStack->setCurrentIndex(m_monitorData->spec_type);

        // 更新监视器名称
        updateMonitorName();
    }
}

simulationmonitor::~simulationmonitor()
{
    // 注意：m_monitorData 的内存由创建者管理，这里不删除
}

void simulationmonitor::setupUI()
{
    mainLayout = new QVBoxLayout(this);

    // 创建Type部分
    typeGroup = new QGroupBox("Type", this);
    typeLayout = new QGridLayout(typeGroup);

    monitorTypeGroup = new QButtonGroup(this);

    efieldRadio = new QRadioButton("E-field", typeGroup);
    hfieldRadio = new QRadioButton("H-Field and Surface current", typeGroup);
    farfieldRadio = new QRadioButton("Farfield/RCS", typeGroup);
    fieldsourceRadio = new QRadioButton("Field Source", typeGroup);
    surfaceCurrentRadio = new QRadioButton("Surface current (TLM only)", typeGroup);
    powerFlowRadio = new QRadioButton("Power flow", typeGroup);
    currentDensityRadio = new QRadioButton("Current density", typeGroup);
    powerLossRadio = new QRadioButton("Power loss density/SAR", typeGroup);
    electricEnergyRadio = new QRadioButton("Electric energy density", typeGroup);
    magneticEnergyRadio = new QRadioButton("Magnetic energy density", typeGroup);

    // 将单选按钮添加到按钮组
    monitorTypeGroup->addButton(efieldRadio, 0);
    monitorTypeGroup->addButton(hfieldRadio, 1);
    monitorTypeGroup->addButton(farfieldRadio, 2);
     monitorTypeGroup->addButton(fieldsourceRadio, 3);
    monitorTypeGroup->addButton(surfaceCurrentRadio, 4);
    monitorTypeGroup->addButton(powerFlowRadio, 5);
    monitorTypeGroup->addButton(currentDensityRadio, 6);
    monitorTypeGroup->addButton(powerLossRadio, 7);
    monitorTypeGroup->addButton(electricEnergyRadio, 8);
    monitorTypeGroup->addButton(magneticEnergyRadio, 9);

    // 布局Type部分的单选按钮（两列布局）
    typeLayout->addWidget(efieldRadio, 0, 0);
    typeLayout->addWidget(surfaceCurrentRadio, 0, 1);
    typeLayout->addWidget(hfieldRadio, 1, 0);
    typeLayout->addWidget(powerFlowRadio, 1, 1);
    typeLayout->addWidget(farfieldRadio, 2, 0);
    typeLayout->addWidget(currentDensityRadio, 2, 1);
    typeLayout->addWidget(fieldsourceRadio, 3, 0);
    typeLayout->addWidget(powerLossRadio, 3, 1);
    typeLayout->addWidget(electricEnergyRadio, 4, 1);
    typeLayout->addWidget(magneticEnergyRadio, 5, 1);

    // 默认选中E-field
    efieldRadio->setChecked(true);

    mainLayout->addWidget(typeGroup);

    // 创建Label部分
    labelGroup = new QGroupBox("Label", this);
    labelLayout = new QHBoxLayout(labelGroup);

    nameLabel = new QLabel("Name:", labelGroup);
    nameEdit = new QLineEdit(labelGroup);
    nameEdit->setPlaceholderText("e-field (f=5)");
    autoNameCheck = new QCheckBox("Automatic", labelGroup);
    autoNameCheck->setChecked(true);

    labelLayout->addWidget(nameLabel);
    labelLayout->addWidget(nameEdit);
    labelLayout->addStretch();
    labelLayout->addWidget(autoNameCheck);

    mainLayout->addWidget(labelGroup);

    // 创建Specification部分
    specGroup = new QGroupBox("Specification", this);
    specLayout = new QVBoxLayout(specGroup);

    // 规格类型选择
    specTypeGroup = new QButtonGroup(this);
    freqRadio = new QRadioButton("Frequency", specGroup);
    timeRadio = new QRadioButton("Time", specGroup);
    transientRadio = new QRadioButton("Transient Broadband", specGroup);

    specTypeGroup->addButton(freqRadio, 0);
    specTypeGroup->addButton(timeRadio, 1);
    specTypeGroup->addButton(transientRadio, 2);

    // 默认选中Frequency
    freqRadio->setChecked(true);

    // 水平布局规格类型单选按钮
    QHBoxLayout* specTypeLayout = new QHBoxLayout();
    specTypeLayout->addWidget(freqRadio);
    specTypeLayout->addWidget(timeRadio);
    specTypeLayout->addWidget(transientRadio);
    specTypeLayout->addStretch();

    specLayout->addLayout(specTypeLayout);

    // 创建堆叠窗口，用于显示不同的设置内容
    settingsStack = new QStackedWidget(specGroup);

    // 频率设置页面
    freqPage = new QWidget();
    freqLayout = new QVBoxLayout(freqPage);

    // 频率列表
    QHBoxLayout* freqListLayout = new QHBoxLayout();
    freqListLabel = new QLabel("Frequency list (f0; f1;...)", freqPage);
    freqListEdit = new QLineEdit(freqPage);
    freqListEdit->setPlaceholderText("5");
    freqListLayout->addWidget(freqListLabel);
    freqListLayout->addWidget(freqListEdit);
    freqListLayout->addStretch();

    // 频率范围
    QHBoxLayout* freqRangeLayout1 = new QHBoxLayout();
    freqMinLabel = new QLabel("Frequency minimum:", freqPage);
    freqMinEdit = new QLineEdit(freqPage);
    freqMinEdit->setPlaceholderText("0.0");
    freqRangeLayout1->addWidget(freqMinLabel);
    freqRangeLayout1->addWidget(freqMinEdit);
    freqRangeLayout1->addStretch();

    QHBoxLayout* freqRangeLayout2 = new QHBoxLayout();
    freqMaxLabel = new QLabel("Frequency maximum:", freqPage);
    freqMaxEdit = new QLineEdit(freqPage);
    freqMaxEdit->setPlaceholderText("10");
    freqRangeLayout2->addWidget(freqMaxLabel);
    freqRangeLayout2->addWidget(freqMaxEdit);
    freqRangeLayout2->addStretch();

    freqLayout->addLayout(freqListLayout);
    freqLayout->addLayout(freqRangeLayout1);
    freqLayout->addLayout(freqRangeLayout2);
    freqLayout->addStretch();

    settingsStack->addWidget(freqPage);

    // 时间设置页面
    timePage = new QWidget();
    timeLayout = new QGridLayout(timePage);

    startTimeLabel = new QLabel("Start time:", timePage);
    startTimeEdit = new QLineEdit(timePage);
    startTimeEdit->setText("0");

    stepWidthLabel = new QLabel("Step width:", timePage);
    stepWidthEdit = new QLineEdit(timePage);
    stepWidthEdit->setText("0.02");

    endTimeCheck = new QCheckBox("End time:", timePage);
    endTimeEdit = new QLineEdit(timePage);
    endTimeEdit->setText("0");
    endTimeEdit->setEnabled(false);  // 默认禁用，直到复选框被选中

    // 将控件添加到网格布局
    timeLayout->addWidget(startTimeLabel, 0, 0);
    timeLayout->addWidget(startTimeEdit, 0, 1);
    timeLayout->addWidget(stepWidthLabel, 1, 0);
    timeLayout->addWidget(stepWidthEdit, 1, 1);
    timeLayout->addWidget(endTimeCheck, 2, 0);
    timeLayout->addWidget(endTimeEdit, 2, 1);

    // 添加弹性空间
    timeLayout->setRowStretch(3, 1);

    settingsStack->addWidget(timePage);
    // 瞬态宽带页面
    transientPage = new QWidget();
    transientLayout = new QGridLayout(transientPage);

    samplesLabel = new QLabel("Samples:", transientPage);
    samplesEdit = new QLineEdit(transientPage);
    samplesEdit->setText("21");

    accuracyLabel = new QLabel("Accuracy:", transientPage);
    accuracyEdit = new QLineEdit(transientPage);
    accuracyEdit->setText("1e-3");

    transientFarfieldsCheck = new QCheckBox("Transient farfields", transientPage);

    // 将控件添加到网格布局
    transientLayout->addWidget(samplesLabel, 0, 0);
    transientLayout->addWidget(samplesEdit, 0, 1);
    transientLayout->addWidget(accuracyLabel, 1, 0);
    transientLayout->addWidget(accuracyEdit, 1, 1);
    transientLayout->addWidget(transientFarfieldsCheck, 2, 0, 1, 2);

    // 添加弹性空间
    transientLayout->setRowStretch(3, 1);

    settingsStack->addWidget(transientPage);





    specLayout->addWidget(settingsStack);

    mainLayout->addWidget(specGroup);

    // 创建按钮部分
    buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    // 设置按钮默认大小
    okButton->setMinimumWidth(80);
    cancelButton->setMinimumWidth(80);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
}

void simulationmonitor::setupConnections()
{
    connect(okButton, &QPushButton::clicked, this, &simulationmonitor::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &simulationmonitor::onCancelClicked);

    // 监视器类型改变时更新界面
    connect(monitorTypeGroup, &QButtonGroup::buttonClicked, this, &simulationmonitor::onMonitorTypeChanged);

    // 自动命名复选框状态改变
    connect(autoNameCheck, &QCheckBox::toggled, this, &simulationmonitor::onAutoNameToggled);

    // 当频率列表改变时，如果自动命名开启，更新名称
    connect(freqListEdit, &QLineEdit::textChanged, this, &simulationmonitor::updateMonitorName);

    // 结束时间复选框状态改变
    connect(endTimeCheck, &QCheckBox::toggled, this, &simulationmonitor::onEndTimeToggled);
    // 当瞬态宽带设置改变时，如果自动命名开启，更新名称
    connect(samplesEdit, &QLineEdit::textChanged, this, &simulationmonitor::updateMonitorName);

    // 规格类型改变时，切换设置页面
    connect(specTypeGroup, &QButtonGroup::buttonClicked, [this](QAbstractButton* button) {
        Q_UNUSED(button);

        // 获取当前选中的监视器类型和规格类型
        int monitorType = monitorTypeGroup->checkedId();
        int specType = specTypeGroup->checkedId();

        // 根据监视器类型和规格类型更新界面
        if (monitorType == 0) { // E-field
            // E-field支持Frequency和Time
            if (specType == 0) { // Frequency
                settingsStack->setCurrentIndex(0);
            } else if (specType == 1) { // Time
                settingsStack->setCurrentIndex(1);
            } else { // Transient Broadband (不应该出现，因为E-field不支持)
                settingsStack->setCurrentIndex(2);
            }
        } else if (monitorType == 2) { // Farfield/RCS
            // Farfield/RCS支持Frequency和Transient Broadband
            if (specType == 0) { // Frequency
                settingsStack->setCurrentIndex(0);
            } else if (specType == 2) { // Transient Broadband
                settingsStack->setCurrentIndex(2);
            } else { // Time (不应该出现，因为Farfield/RCS不支持)
                settingsStack->setCurrentIndex(1);
            }
        } else {
            if (specType == 0) { // Frequency
                settingsStack->setCurrentIndex(0);
            } else if (specType == 1) { // Time
                settingsStack->setCurrentIndex(1);
            } else { // Transient Broadband (不应该出现，因为E-field不支持)
                settingsStack->setCurrentIndex(2);
            }
        }
    });
}

void simulationmonitor::onOkClicked()
{
    if (m_monitorData) {
        // 保存监视器类型
        m_monitorData->monitor_type = monitorTypeGroup->checkedId();

        // 保存名称
        m_monitorData->monitor_name = nameEdit->text();
        m_monitorData->auto_name = autoNameCheck->isChecked();

        // 保存规格类型
        m_monitorData->spec_type = specTypeGroup->checkedId();

        // 保存频率数据
        m_monitorData->frequency_list = freqListEdit->text();
        m_monitorData->freq_min = freqMinEdit->text().toDouble();
        m_monitorData->freq_max = freqMaxEdit->text().toDouble();

        // 保存时间数据
        m_monitorData->start_time = startTimeEdit->text().toDouble();
        m_monitorData->step_width = stepWidthEdit->text().toDouble();
        m_monitorData->end_time = endTimeEdit->text().toDouble();
        m_monitorData->use_end_time = endTimeCheck->isChecked();

        // 保存瞬态宽带数据
        m_monitorData->samples = samplesEdit->text().toInt();
        m_monitorData->accuracy = accuracyEdit->text().toDouble();
        m_monitorData->transient_farfields = transientFarfieldsCheck->isChecked();
    }

    // 检查是否是Farfield/RCS且规格类型为Frequency
    int monitorType = monitorTypeGroup->checkedId();
    int specType = specTypeGroup->checkedId();

    if (monitorType == 2 && specType == 0) { // Farfield/RCS 且选择 Frequency
        QString frequencyList = freqListEdit->text().trimmed();

        if (!frequencyList.isEmpty()) {
            // 解析频率列表
            QStringList frequencyStrings;

            // 检查输入格式：可能是单个数字，也可能是分号分隔的多个频率
            if (frequencyList.contains(';')) {
                // 使用分号分隔
                frequencyStrings = frequencyList.split(';', Qt::SkipEmptyParts);
            } else {
                // 单个频率
                frequencyStrings << frequencyList;
            }

            // 清理每个频率字符串（移除空格）
            for (int i = 0; i < frequencyStrings.size(); ++i) {
                frequencyStrings[i] = frequencyStrings[i].trimmed();
            }

            // 移除空字符串
            frequencyStrings.removeAll("");

            if (!frequencyStrings.isEmpty()) {
                // 将频率从GHz转换为Hz
                QVector<qint64> frequenciesHz;
                for (const QString& freqStr : frequencyStrings) {
                    bool ok;
                    double freqGHz = freqStr.toDouble(&ok);
                    if (ok) {
                        // 将GHz转换为Hz: 1 GHz = 1e9 Hz
                        qint64 freqHz = static_cast<qint64>(freqGHz * 1e9);
                        frequenciesHz.append(freqHz);
                    }
                }

                if (!frequenciesHz.isEmpty()) {
                    // 写入文件
                    writeFarFieldFrequencyFile(frequenciesHz);
                } else {
                    QMessageBox::warning(this, "警告", "频率列表中包含无效的数字格式。");
                }
            } else {
                QMessageBox::warning(this, "警告", "频率列表为空。");
            }
        }
    }


    emit Monitor_emit(m_monitorData);
    accept();
}

void simulationmonitor::onCancelClicked()
{
    reject();
}

void simulationmonitor::onMonitorTypeChanged()
{
    // 根据选中的监视器类型更新规格选项
    int monitorType = monitorTypeGroup->checkedId();

    // 启用/禁用不同的规格选项
    if (monitorType == 0) { // E-field
        freqRadio->setEnabled(true);
        timeRadio->setEnabled(true);
        transientRadio->setEnabled(false);

        // 默认选中Frequency
        freqRadio->setChecked(true);

        // 显示频率设置页面
        settingsStack->setCurrentIndex(0);
    } else if (monitorType == 2) { // Farfield/RCS
        freqRadio->setEnabled(true);
        timeRadio->setEnabled(false);
        transientRadio->setEnabled(true);

        // 默认选中Frequency
        freqRadio->setChecked(true);

        // 显示频率设置页面
        settingsStack->setCurrentIndex(0);
    } else {
        // 其他监视器类型只显示Frequency
        freqRadio->setEnabled(true);
        timeRadio->setEnabled(true);
        transientRadio->setEnabled(false);

        freqRadio->setChecked(true);

        // 显示频率设置页面
        settingsStack->setCurrentIndex(0);
    }

    // 更新监视器名称
    updateMonitorName();
}

void simulationmonitor::onAutoNameToggled(bool checked)
{
    nameEdit->setEnabled(!checked);

    if (checked) {
        updateMonitorName();
    }
}

void simulationmonitor::updateMonitorName()
{
    if (autoNameCheck->isChecked()) {
        int monitorType = monitorTypeGroup->checkedId();
        QString specValue;

        // 根据规格类型获取不同的值
        int specType = specTypeGroup->checkedId();
        if (specType == 0) { // Frequency
            specValue = freqListEdit->text().isEmpty() ? "5" : freqListEdit->text();
        } else if (specType == 1) { // Time
            specValue = startTimeEdit->text().isEmpty() ? "0" : startTimeEdit->text();
        } else { // Transient Broadband
            specValue = "transient";
        }

        QString baseName;
        switch (monitorType) {
        case 0: baseName = "e-field"; break;
        case 1: baseName = "h-field"; break;
        case 2: baseName = "farfield"; break;
        case 3: baseName = "surface-current"; break;
        case 4: baseName = "power-flow"; break;
        case 5: baseName = "current-density"; break;
        case 6: baseName = "power-loss"; break;
        case 7: baseName = "electric-energy"; break;
        case 8: baseName = "magnetic-energy"; break;
        default: baseName = "monitor"; break;
        }

        QString specPrefix;
        switch (specType) {
        case 0: specPrefix = "f"; break;
        case 1: specPrefix = "t"; break;
        case 2: specPrefix = "tb"; break;
        default: specPrefix = ""; break;
        }

        QString name = QString("%1 (%2=%3)").arg(baseName).arg(specPrefix).arg(specValue);
        nameEdit->setText(name);
    }
}

void simulationmonitor::onEndTimeToggled(bool checked)
{
    // 启用或禁用结束时间输入框
    endTimeEdit->setEnabled(checked);
}

void simulationmonitor::writeFarFieldFrequencyFile(const QVector<qint64>& frequenciesHz)
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/FarFieldFrequencyPoint.txt");

    // 读取现有文件内容
    QVector<qint64> existingFrequencies;
    bool fileExists = false;

    QFile file(fileName);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fileExists = true;
        QTextStream in(&file);

        // 跳过前三行（分隔线和标题）
        for (int i = 0; i < 3; ++i) {
            in.readLine();
        }

        // 读取频率点数量
        QString countLine = in.readLine();
        bool ok;
        int count = countLine.toInt(&ok);

        if (ok && count > 0) {
            for (int i = 0; i < count && !in.atEnd(); ++i) {
                QString freqLine = in.readLine();
                qint64 freqHz = freqLine.toLongLong(&ok);
                if (ok) {
                    existingFrequencies.append(freqHz);
                }
            }
        }
        file.close();
    }

    // 合并现有频率和新频率，去重
    QVector<qint64> allFrequencies = existingFrequencies;
    for (qint64 newFreq : frequenciesHz) {
        if (!allFrequencies.contains(newFreq)) {
            allFrequencies.append(newFreq);
        }
    }

    // 写入文件
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入文件头
        out << "==============================\n";
        out << "f(Hz)\n";
        out << "==============================\n";

        // 写入频率数量
        out << allFrequencies.size() << "\n";

        // 写入每个频率值
        for (qint64 freqHz : allFrequencies) {
            out << freqHz << "\n";
        }

        file.close();

        qDebug() << "FarFieldFrequencyPoint.txt 文件已更新";

        // 显示成功消息
        int addedCount = allFrequencies.size() - existingFrequencies.size();
        QString message;

        if (!fileExists) {
            message = QString("创建了远场频率点文件。\n\n"
                              "频率点数: %1").arg(allFrequencies.size());
        } else if (addedCount > 0) {
            message = QString("更新了远场频率点文件。\n\n"
                              //"原有频率点数: %1\n"
                              //"新增频率点数: %2\n"
                              "当前频率点数: %3")
                          //.arg(existingFrequencies.size())
                          //.arg(addedCount)
                          .arg(allFrequencies.size());
        } else {
            message = QString("所有频率点已存在，文件未更改。\n\n"
                              "频率点数: %1").arg(allFrequencies.size());
        }

        QMessageBox::information(this, "成功", message);
    } else {
        qDebug() << "无法写入 FarFieldFrequencyPoint.txt 文件";
        QMessageBox::critical(this, "错误", "无法写入频率点文件。");
    }
}
