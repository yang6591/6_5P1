#include "simulationresultset.h"
#include "global.h"

#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QCoreApplication>
#include <QDir>
#include <QRadioButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

simulationresultset::simulationresultset(QWidget *parent)
    : QDialog(parent)
{

    setupUI();
    setupConnections();
}

void simulationresultset::setupUI()
{
    setWindowTitle(tr("Result Settings"));

    // 创建左侧和右侧的组框
    QGroupBox* leftGroupBox = new QGroupBox("S/E");
    QGroupBox* rightGroupBox = new QGroupBox("频域结果");

    // 创建左侧的网格布局
    QGridLayout* leftGridLayout = new QGridLayout;

    // 创建按钮组
    //yes和no互斥
    QButtonGroup* sParamGroup = new QButtonGroup(this);
    QButtonGroup* fieldXGroup = new QButtonGroup(this);
    QButtonGroup* fieldYGroup = new QButtonGroup(this);
    QButtonGroup* fieldZGroup = new QButtonGroup(this);
    QButtonGroup* directivityGroup = new QButtonGroup(this);
    QButtonGroup* phiDGroup = new QButtonGroup(this);
    QButtonGroup* thetaDGroup = new QButtonGroup(this);
    QButtonGroup* phiRCSGroup = new QButtonGroup(this);
    QButtonGroup* thetaRCSGroup = new QButtonGroup(this);

    // 是否计算s参数
    QLabel* sParamLabel = new QLabel("S参数:");
    sParamYes = new QRadioButton("YES");
    sParamNo = new QRadioButton("NO");
    sParamNo->setChecked(true); // 默认选择NO
    // 将YES和NO按钮添加到同一个按钮组
    sParamGroup->addButton(sParamYes);
    sParamGroup->addButton(sParamNo);

    QHBoxLayout* sParamLayout = new QHBoxLayout;
    sParamLayout->addWidget(sParamYes);
    sParamLayout->addWidget(sParamNo);
    sParamLayout->addStretch();
    leftGridLayout->addWidget(sParamLabel, 0, 0);
    leftGridLayout->addLayout(sParamLayout, 0, 1);

    // 远场电场X方向
    QLabel* fieldXLabel = new QLabel("远场电场X方向:");
    fieldXYes = new QRadioButton("YES");
    fieldXNo = new QRadioButton("NO");
    fieldXNo->setChecked(true); // 默认选择NO
    fieldXGroup->addButton(fieldXYes);
    fieldXGroup->addButton(fieldXNo);
    QHBoxLayout* fieldXLayout = new QHBoxLayout;
    fieldXLayout->addWidget(fieldXYes);
    fieldXLayout->addWidget(fieldXNo);
    fieldXLayout->addStretch();
    leftGridLayout->addWidget(fieldXLabel, 1, 0);
    leftGridLayout->addLayout(fieldXLayout, 1, 1);

    // 远场电场Y方向
    QLabel* fieldYLabel = new QLabel("远场电场Y方向:");
    fieldYYes = new QRadioButton("YES");
    fieldYNo = new QRadioButton("NO");
    fieldYNo->setChecked(true); // 默认选择NO
    fieldYGroup->addButton(fieldYYes);
    fieldYGroup->addButton(fieldYNo);
    QHBoxLayout* fieldYLayout = new QHBoxLayout;
    fieldYLayout->addWidget(fieldYYes);
    fieldYLayout->addWidget(fieldYNo);
    fieldYLayout->addStretch();
    leftGridLayout->addWidget(fieldYLabel, 2, 0);
    leftGridLayout->addLayout(fieldYLayout, 2, 1);

    // 远场电场Z方向
    QLabel* fieldZLabel = new QLabel("远场电场Z方向:");
    fieldZYes = new QRadioButton("YES");
    fieldZNo = new QRadioButton("NO");
    fieldZNo->setChecked(true); // 默认选择NO
    fieldZGroup->addButton(fieldZYes);
    fieldZGroup->addButton(fieldZNo);
    QHBoxLayout* fieldZLayout = new QHBoxLayout;
    fieldZLayout->addWidget(fieldZYes);
    fieldZLayout->addWidget(fieldZNo);
    fieldZLayout->addStretch();
    leftGridLayout->addWidget(fieldZLabel, 3, 0);
    leftGridLayout->addLayout(fieldZLayout, 3, 1);

    // 设置左侧组框布局
    leftGroupBox->setLayout(leftGridLayout);

    // 创建右侧的网格布局
    QGridLayout* rightGridLayout = new QGridLayout;

    // 计算总方向性系数D
    QLabel* directivityLabel = new QLabel("总方向性系数D:");
    directivityYes = new QRadioButton("YES");
    directivityNo = new QRadioButton("NO");
    directivityNo->setChecked(true); // 默认选择NO
    directivityGroup->addButton(directivityYes);
    directivityGroup->addButton(directivityNo);
    QHBoxLayout* directivityLayout = new QHBoxLayout;
    directivityLayout->addWidget(directivityYes);
    directivityLayout->addWidget(directivityNo);
    directivityLayout->addStretch();
    rightGridLayout->addWidget(directivityLabel, 0, 0);
    rightGridLayout->addLayout(directivityLayout, 0, 1);

    // 各频点phi平面D
    QLabel* phiDLabel = new QLabel("各频点phi平面D:");
    phiDYes = new QRadioButton("YES");
    phiDNo = new QRadioButton("NO");
    phiDNo->setChecked(true); // 默认选择NO
    phiDGroup->addButton(phiDYes);
    phiDGroup->addButton(phiDNo);
    QHBoxLayout* phiDLayout = new QHBoxLayout;
    phiDLayout->addWidget(phiDYes);
    phiDLayout->addWidget(phiDNo);
    phiDLayout->addStretch();
    rightGridLayout->addWidget(phiDLabel, 1, 0);
    rightGridLayout->addLayout(phiDLayout, 1, 1);

    // 各频点theta平面D
    QLabel* thetaDLabel = new QLabel("各频点theta平面D:");
    thetaDYes = new QRadioButton("YES");
    thetaDNo = new QRadioButton("NO");
    thetaDNo->setChecked(true); // 默认选择NO
    thetaDGroup->addButton(thetaDYes);
    thetaDGroup->addButton(thetaDNo);
    QHBoxLayout* thetaDLayout = new QHBoxLayout;
    thetaDLayout->addWidget(thetaDYes);
    thetaDLayout->addWidget(thetaDNo);
    thetaDLayout->addStretch();
    rightGridLayout->addWidget(thetaDLabel, 2, 0);
    rightGridLayout->addLayout(thetaDLayout, 2, 1);

    // 各频点phi平面RCS
    QLabel* phiRCSLabel = new QLabel("各频点phi平面RCS:");
    phiRCSYes = new QRadioButton("YES");
    phiRCSNo = new QRadioButton("NO");
    phiRCSNo->setChecked(true); // 默认选择NO
    phiRCSGroup->addButton(phiRCSYes);
    phiRCSGroup->addButton(phiRCSNo);
    QHBoxLayout* phiRCSLayout = new QHBoxLayout;
    phiRCSLayout->addWidget(phiRCSYes);
    phiRCSLayout->addWidget(phiRCSNo);
    phiRCSLayout->addStretch();
    rightGridLayout->addWidget(phiRCSLabel, 3, 0);
    rightGridLayout->addLayout(phiRCSLayout, 3, 1);

    // 各频点theta平面RCS
    QLabel* thetaRCSLabel = new QLabel("各频点theta平面RCS:");
    thetaRCSYes = new QRadioButton("YES");
    thetaRCSNo = new QRadioButton("NO");
    thetaRCSNo->setChecked(true); // 默认选择NO
    thetaRCSGroup->addButton(thetaRCSYes);
    thetaRCSGroup->addButton(thetaRCSNo);
    QHBoxLayout* thetaRCSLayout = new QHBoxLayout;
    thetaRCSLayout->addWidget(thetaRCSYes);
    thetaRCSLayout->addWidget(thetaRCSNo);
    thetaRCSLayout->addStretch();
    rightGridLayout->addWidget(thetaRCSLabel, 4, 0);
    rightGridLayout->addLayout(thetaRCSLayout, 4, 1);

    // 设置右侧组框布局
    rightGroupBox->setLayout(rightGridLayout);

    // 创建角度输入组框（放在下方）
    QGroupBox* angleGroupBox = new QGroupBox("角度设置（仅在对应选项选择YES时启用）");

    // 创建角度输入的网格布局
    QGridLayout* angleGridLayout = new QGridLayout;

    // phi平面D的角度输入
    QLabel* phiDAngleLabel = new QLabel("phi平面D - Phi值:");
    phiDTextEdit = new QTextEdit();
    phiDTextEdit->setMaximumHeight(50);
    phiDTextEdit->setPlaceholderText("输入Phi角度值，用空格分隔");
    phiDTextEdit->setEnabled(false); // 默认禁用
    angleGridLayout->addWidget(phiDAngleLabel, 0, 0);
    angleGridLayout->addWidget(phiDTextEdit, 0, 1);

    // theta平面D的角度输入
    QLabel* thetaDAngleLabel = new QLabel("theta平面D - Theta值:");
    thetaDTextEdit = new QTextEdit();
    thetaDTextEdit->setMaximumHeight(50);
    thetaDTextEdit->setPlaceholderText("输入Theta角度值，用空格分隔");
    thetaDTextEdit->setEnabled(false); // 默认禁用
    angleGridLayout->addWidget(thetaDAngleLabel, 0, 2);
    angleGridLayout->addWidget(thetaDTextEdit, 0, 3);

    // phi平面RCS的角度输入
    QLabel* phiRCSAngleLabel = new QLabel("phi平面RCS - Phi值:");
    phiRCSTextEdit = new QTextEdit();
    phiRCSTextEdit->setMaximumHeight(50);
    phiRCSTextEdit->setPlaceholderText("输入Phi角度值，用空格分隔");
    phiRCSTextEdit->setEnabled(false); // 默认禁用
    angleGridLayout->addWidget(phiRCSAngleLabel, 1, 0);
    angleGridLayout->addWidget(phiRCSTextEdit, 1, 1);

    // theta平面RCS的角度输入
    QLabel* thetaRCSAngleLabel = new QLabel("theta平面RCS - Theta值:");
    thetaRCSTextEdit = new QTextEdit();
    thetaRCSTextEdit->setMaximumHeight(50);
    thetaRCSTextEdit->setPlaceholderText("输入RCS的Theta角度值，用空格分隔");
    thetaRCSTextEdit->setEnabled(false); // 默认禁用
    angleGridLayout->addWidget(thetaRCSAngleLabel, 1, 2);
    angleGridLayout->addWidget(thetaRCSTextEdit, 1, 3);

    // 设置角度组框布局
    angleGroupBox->setLayout(angleGridLayout);


    // 创建按钮
    resultBtnOk = new QPushButton("OK");
    resultBtnCancel = new QPushButton("Cancel");



    // 创建布局
    mainLayout = new QVBoxLayout(this);
    inputLayout = new QHBoxLayout;
    QHBoxLayout* upperLayout = new QHBoxLayout; // 上部分布局（左右组框）
    buttonLayout = new QHBoxLayout;

    // 按钮布局
    buttonLayout->addStretch();
    buttonLayout->addWidget(resultBtnOk);
    buttonLayout->addWidget(resultBtnCancel);

    // 将左右两个组框添加到上部分布局
    upperLayout->addWidget(leftGroupBox);
    upperLayout->addSpacing(30); // 增加间距
    upperLayout->addWidget(rightGroupBox);

    // 将上部分布局添加到输入布局
    inputLayout->addLayout(upperLayout);

    // 主布局：上部分（左右组框） -> 角度输入部分 -> 按钮部分
    mainLayout->addLayout(inputLayout);
    mainLayout->addSpacing(20); // 添加间距
    mainLayout->addWidget(angleGroupBox);
    mainLayout->addSpacing(20); // 添加间距
    mainLayout->addLayout(buttonLayout);
}

void simulationresultset::setupConnections()
{

    connect(phiDYes, &QRadioButton::toggled, this, &simulationresultset::onPhiDYesToggled);
    connect(thetaDYes, &QRadioButton::toggled, this, &simulationresultset::onThetaDYesToggled);
    connect(phiRCSYes, &QRadioButton::toggled, this, &simulationresultset::onPhiRCSYesToggled);
    connect(thetaRCSYes, &QRadioButton::toggled, this, &simulationresultset::onThetaRCSYesToggled);

    connect(resultBtnOk, &QPushButton::clicked, this, [=](){
        // 获取所有单选按钮的值
        bool calcSParam = sParamYes->isChecked();
        bool fieldX = fieldXYes->isChecked();
        bool fieldY = fieldYYes->isChecked();
        bool fieldZ = fieldZYes->isChecked();
        bool calcDirectivity = directivityYes->isChecked();
        bool phiD = phiDYes->isChecked();
        bool thetaD = thetaDYes->isChecked();
        bool phiRCS = phiRCSYes->isChecked();
        bool thetaRCS = thetaRCSYes->isChecked();


        // 解析角度值
        if (phiD) {
            phiDValues = parseAngleValues(phiDTextEdit->toPlainText());
            if (phiDValues.empty()) {
                QMessageBox::warning(this, "输入错误",
                                     "请输入有效的phi平面D角度值。");
                return;
            }
        }

        if (thetaD) {
            thetaDValues = parseAngleValues(thetaDTextEdit->toPlainText());
            if (thetaDValues.empty()) {
                QMessageBox::warning(this, "输入错误",
                                     "请输入有效的theta平面D角度值。");
                return;
            }
        }

        if (phiRCS) {
            phiRCSValues = parseAngleValues(phiRCSTextEdit->toPlainText());
            if (phiRCSValues.empty()) {
                QMessageBox::warning(this, "输入错误",
                                     "请输入有效的phi平面RCS角度值。");
                return;
            }
        }

        if (thetaRCS) {
            thetaRCSValues = parseAngleValues(thetaRCSTextEdit->toPlainText());
            if (thetaRCSValues.empty()) {
                QMessageBox::warning(this, "输入错误",
                                     "请输入有效的theta平面RCS角度值。");
                return;
            }
        }


        // 输出调试信息
        qDebug() << "模拟参数设置已保存:";
        qDebug() << "是否计算s参数:" << (calcSParam ? "YES" : "NO");
        qDebug() << "远场电场X方向:" << (fieldX ? "YES" : "NO");
        qDebug() << "远场电场Y方向:" << (fieldY ? "YES" : "NO");
        qDebug() << "远场电场Z方向:" << (fieldZ ? "YES" : "NO");
        qDebug() << "计算总方向性系数D:" << (calcDirectivity ? "YES" : "NO");
        qDebug() << "各频点phi平面D:" << (phiD ? "YES" : "NO");
        qDebug() << "各频点theta平面D:" << (thetaD ? "YES" : "NO");
        qDebug() << "各频点phi平面RCS:" << (phiRCS ? "YES" : "NO");
        qDebug() << "各频点theta平面RCS:" << (thetaRCS ? "YES" : "NO");

        if (phiD) {
            qDebug() << "phiD角度值:";
            for (double val : phiDValues) {
                qDebug() << val;
            }
        }

        if (thetaD) {
            qDebug() << "thetaD角度值:";
            for (double val : thetaDValues) {
                qDebug() << val;
            }
        }

        if (phiRCS) {
            qDebug() << "phiRCS角度值:";
            for (double val : phiRCSValues) {
                qDebug() << val;
            }
        }

        if (thetaRCS) {
            qDebug() << "thetaRCS角度值:";
            for (double val : thetaRCSValues) {
                qDebug() << val;
            }
        }


        // 生成参数设置文件
        QString appDir = QCoreApplication::applicationDirPath();
        QString fileName = QDir::cleanPath(appDir + "/simulation_parameters.txt");

        // 生成文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写入文件头
            out << "=======================================================================================\n";
            out << "S-Parameter\tFarfield-Ex\tFarfield-Ey\tFarfield-Ez\tD-all\tD-phi\tD-theta\tRCS-phi\tRCS-theta\n";
            out << "=======================================================================================\n";

            out<<calcSParam<<"\t"<<fieldX<<"\t"<<fieldY<<"\t"<<fieldZ<<"\t"<<calcDirectivity<<"\t"<<phiD<<"\t"<<thetaD<<"\t"<<phiRCS<<"\t"<<thetaRCS;
            out<<"\n";
            // // 写入左侧参数
            // out << "是否计算s参数: " << (calcSParam ? "YES" : "NO") << "\n";
            // out << "远场电场X方向: " << (fieldX ? "YES" : "NO") << "\n";
            // out << "远场电场Y方向: " << (fieldY ? "YES" : "NO") << "\n";
            // out << "远场电场Z方向: " << (fieldZ ? "YES" : "NO") << "\n";

            // // 写入右侧参数
            // out << "计算总方向性系数D: " << (calcDirectivity ? "YES" : "NO") << "\n";
            // out << "各频点phi平面D: " << (phiD ? "YES" : "NO") << "\n";
            // out << "各频点theta平面D: " << (thetaD ? "YES" : "NO") << "\n";
            // out << "各频点phi平面RCS: " << (phiRCS ? "YES" : "NO") << "\n";
            // out << "各频点theta平面RCS: " << (thetaRCS ? "YES" : "NO") << "\n";

            // 写入角度值
            if (!phiDValues.empty()) {
                //out << "\nphi平面D的角度值: ";
                for (size_t i = 0; i < phiDValues.size(); ++i) {
                    out << phiDValues[i];
                    if (i < phiDValues.size() - 1) out << "\t";
                }
                out << "\n";
            }

            if (!thetaDValues.empty()) {
                //out << "theta平面D的角度值: ";
                for (size_t i = 0; i < thetaDValues.size(); ++i) {
                    out << thetaDValues[i];
                    if (i < thetaDValues.size() - 1) out << "\t";
                }
                out << "\n";
            }

            if (!phiRCSValues.empty()) {
                //out << "phi平面RCS的角度值: ";
                for (size_t i = 0; i < phiRCSValues.size(); ++i) {
                    out << phiRCSValues[i];
                    if (i < phiRCSValues.size() - 1) out << "\t";
                }
                out << "\n";
            }

            if (!thetaRCSValues.empty()) {
                //out << "theta平面RCS的角度值: ";
                for (size_t i = 0; i < thetaRCSValues.size(); ++i) {
                    out << thetaRCSValues[i];
                    if (i < thetaRCSValues.size() - 1) out << "\t";
                }
                out << "\n";
            }

            file.close();
            qDebug() << "simulation_parameters.txt文件已生成";
        } else {
            qDebug() << "无法创建simulation_parameters.txt文件";
        }

        close();
    });

    connect(resultBtnCancel, &QPushButton::clicked, this, &simulationresultset::close);
}


void simulationresultset::onPhiDYesToggled(bool checked)
{
    phiDTextEdit->setEnabled(checked);
    if (!checked) {
        phiDTextEdit->clear();
    }
}

void simulationresultset::onThetaDYesToggled(bool checked)
{
    thetaDTextEdit->setEnabled(checked);
    if (!checked) {
        thetaDTextEdit->clear();
    }
}

void simulationresultset::onPhiRCSYesToggled(bool checked)
{
    phiRCSTextEdit->setEnabled(checked);
    if (!checked) {
        phiRCSTextEdit->clear();
    }
}

void simulationresultset::onThetaRCSYesToggled(bool checked)
{
    thetaRCSTextEdit->setEnabled(checked);
    if (!checked) {
        thetaRCSTextEdit->clear();
    }
}



std::vector<double> simulationresultset::parseAngleValues(const QString &text)
{
    std::vector<double> result;
    QString trimmedText = text.trimmed();

    if (trimmedText.isEmpty()) {
        return result;
    }

    QStringList valueList;
    if (trimmedText.contains(',')) {
        valueList = trimmedText.split(',', Qt::SkipEmptyParts);
    } else {
        valueList = trimmedText.split(' ', Qt::SkipEmptyParts);
    }

    for (const QString &str : valueList) {
        bool ok;
        double value = str.trimmed().toDouble(&ok);
        if (ok) {
            result.push_back(value);
        } else {
            // 如果有一个值无效，清空结果并返回
            result.clear();
            return result;
        }
    }

    return result;
}

std::vector<double> simulationresultset::getPhiDValues() const
{
    return phiDValues;
}

std::vector<double> simulationresultset::getThetaDValues() const
{
    return thetaDValues;
}

std::vector<double> simulationresultset::getPhiRCSValues() const
{
    return phiRCSValues;
}

std::vector<double> simulationresultset::getThetaRCSValues() const
{
    return thetaRCSValues;
}
