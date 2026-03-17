#include "materialparameter.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpression>
#include <cmath>

MaterialParameter::MaterialParameter(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    loadExistingMaterials();
}

void MaterialParameter::setupUI()
{
    setWindowTitle(tr("材料参数设置"));
    resize(600, 450);

    // 创建输入控件
    epsEdit = new QLineEdit;
    epsEdit->setPlaceholderText("eps_r (相对介电常数)");
    epsEdit->setMinimumWidth(120);
    epsEdit->setToolTip("相对介电常数");

    muEdit = new QLineEdit;
    muEdit->setPlaceholderText("mu_r (相对磁导率)");
    muEdit->setMinimumWidth(120);
    muEdit->setToolTip("相对磁导率");

    sigmaEdit = new QLineEdit;
    sigmaEdit->setPlaceholderText("sigma (电导率)");
    sigmaEdit->setMinimumWidth(120);
    sigmaEdit->setToolTip("电导率 (S/m)");

    sigmaMEdit = new QLineEdit;
    sigmaMEdit->setPlaceholderText("sigma_m (磁导率)");
    sigmaMEdit->setMinimumWidth(120);
    sigmaMEdit->setToolTip("磁导率");

    // 创建表格
    materialsTable = new QTableWidget;
    materialsTable->setColumnCount(COL_COUNT);
    QStringList headers;
    headers << "eps_r" << "mu_r" << "sigma" << "sigma_m";
    materialsTable->setHorizontalHeaderLabels(headers);
    materialsTable->setSelectionBehavior(QTableWidget::SelectRows);
    materialsTable->setSelectionMode(QTableWidget::SingleSelection);
    materialsTable->horizontalHeader()->setStretchLastSection(true);
    materialsTable->setAlternatingRowColors(true);
    materialsTable->setToolTip("材料编号从0开始自动生成");

    // 隐藏垂直表头（行号列）
    //materialsTable->verticalHeader()->setVisible(false);

    // 显示垂直表头，并从0开始编号
    materialsTable->verticalHeader()->setVisible(true);

    // 连接信号，在行数变化时更新行号
    connect(materialsTable->model(), &QAbstractItemModel::rowsInserted,
            this, [this]() { updateRowNumbers(); });
    connect(materialsTable->model(), &QAbstractItemModel::rowsRemoved,
            this, [this]() { updateRowNumbers(); });

    // 如果需要调整列宽，可以设置每列的拉伸模式
    for (int i = 0; i < COL_COUNT; i++) {
        materialsTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // 创建按钮
    addBtn = new QPushButton("添加");
    deleteBtn = new QPushButton("删除");
    clearBtn = new QPushButton("清空");
    saveBtn = new QPushButton("保存");
    cancelBtn = new QPushButton("取消");

    // 设置布局
    mainLayout = new QVBoxLayout(this);

    // 输入区域布局
    QGroupBox *inputGroupBox = new QGroupBox("添加材料参数");
    inputGridLayout = new QGridLayout;

    inputGridLayout->addWidget(new QLabel("eps_r:"), 0, 0);
    inputGridLayout->addWidget(epsEdit, 0, 1);
    inputGridLayout->addWidget(new QLabel("mu_r:"), 0, 2);
    inputGridLayout->addWidget(muEdit, 0, 3);

    inputGridLayout->addWidget(new QLabel("sigma:"), 1, 0);
    inputGridLayout->addWidget(sigmaEdit, 1, 1);
    inputGridLayout->addWidget(new QLabel("sigma_m:"), 1, 2);
    inputGridLayout->addWidget(sigmaMEdit, 1, 3);

    inputGridLayout->addWidget(addBtn, 2, 2, 1, 2);
    inputGroupBox->setLayout(inputGridLayout);

    // 表格区域
    QGroupBox *tableGroupBox = new QGroupBox("材料参数列表");
    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addWidget(materialsTable);

    QHBoxLayout *tableBtnLayout = new QHBoxLayout;
    tableBtnLayout->addWidget(deleteBtn);
    tableBtnLayout->addWidget(clearBtn);
    tableBtnLayout->addStretch();
    tableLayout->addLayout(tableBtnLayout);

    tableGroupBox->setLayout(tableLayout);

    // 底部按钮布局
    QHBoxLayout *bottomBtnLayout = new QHBoxLayout;
    bottomBtnLayout->addStretch();
    bottomBtnLayout->addWidget(saveBtn);
    bottomBtnLayout->addWidget(cancelBtn);

    // 主布局组合
    mainLayout->addWidget(inputGroupBox);
    mainLayout->addWidget(tableGroupBox);
    mainLayout->addLayout(bottomBtnLayout);
}

void MaterialParameter::updateRowNumbers()
{
    for (int i = 0; i < materialsTable->rowCount(); i++) {
        // 为每行设置行号标签，从0开始
        materialsTable->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }
}

void MaterialParameter::setupConnections()
{
    connect(addBtn, &QPushButton::clicked, this, &MaterialParameter::addMaterial);
    connect(deleteBtn, &QPushButton::clicked, this, &MaterialParameter::deleteMaterial);
    connect(clearBtn, &QPushButton::clicked, this, &MaterialParameter::clearAllMaterials);
    connect(saveBtn, &QPushButton::clicked, this, &MaterialParameter::saveMaterials);
    connect(cancelBtn, &QPushButton::clicked, this, &MaterialParameter::close);
}

// 辅助函数：格式化浮点数为两位小数
QString formatDouble(double value)
{
    return QString::number(value, 'f', 2);
}

void MaterialParameter::loadExistingMaterials()
{
    // 检查是否存在已保存的材料参数文件
    QString fileName = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/MaterialParameter.txt");
    QFile file(fileName);

    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        // 读取前三行（标题行和分隔线）
        for (int i = 0; i < 3; i++) {
            in.readLine();
        }

        // 读取材料个数
        QString countLine = in.readLine().trimmed();
        bool ok;
        int materialCount = countLine.toInt(&ok);

        if (ok && materialCount > 0) {
            for (int i = 0; i < materialCount; i++) {
                if (in.atEnd()) break;

                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;

                QStringList fields = line.split(QRegularExpression("\\s+"));
                if (fields.size() >= 4) {
                    // 将读取的值格式化为两位小数
                    bool convOk1, convOk2, convOk3, convOk4;
                    double eps = fields[0].toDouble(&convOk1);
                    double mu = fields[1].toDouble(&convOk2);
                    double sigma = fields[2].toDouble(&convOk3);
                    double sigmaM = fields[3].toDouble(&convOk4);

                    if (convOk1 && convOk2 && convOk3 && convOk4) {
                        int row = materialsTable->rowCount();
                        materialsTable->insertRow(row);

                        materialsTable->setItem(row, COL_EPS_R, new QTableWidgetItem(formatDouble(eps)));
                        materialsTable->setItem(row, COL_MU_R, new QTableWidgetItem(formatDouble(mu)));
                        materialsTable->setItem(row, COL_SIGMA, new QTableWidgetItem(formatDouble(sigma)));
                        materialsTable->setItem(row, COL_SIGMA_M, new QTableWidgetItem(formatDouble(sigmaM)));
                    }
                }
            }
        }

        file.close();
    }
}

void MaterialParameter::addMaterial()
{
    QString epsStr = epsEdit->text().trimmed();
    QString muStr = muEdit->text().trimmed();
    QString sigmaStr = sigmaEdit->text().trimmed();
    QString sigmaMStr = sigmaMEdit->text().trimmed();

    // 验证输入是否为数字（支持浮点数）
    bool okEps, okMu, okSigma, okSigmaM;
    double eps = epsStr.toDouble(&okEps);
    double mu = muStr.toDouble(&okMu);
    double sigma = sigmaStr.toDouble(&okSigma);
    double sigmaM = sigmaMStr.toDouble(&okSigmaM);

    if (!okEps || eps < 0) {
        QMessageBox::warning(this, "输入错误", "eps_r必须为非负数字");
        epsEdit->setFocus();
        return;
    }

    if (!okMu || mu < 0) {
        QMessageBox::warning(this, "输入错误", "mu_r必须为非负数字");
        muEdit->setFocus();
        return;
    }

    if (!okSigma || sigma < 0) {
        QMessageBox::warning(this, "输入错误", "sigma必须为非负数字");
        sigmaEdit->setFocus();
        return;
    }

    if (!okSigmaM || sigmaM < 0) {
        QMessageBox::warning(this, "输入错误", "sigma_m必须为非负数字");
        sigmaMEdit->setFocus();
        return;
    }

    // 格式化为两位小数
    QString epsFormatted = formatDouble(eps);
    QString muFormatted = formatDouble(mu);
    QString sigmaFormatted = formatDouble(sigma);
    QString sigmaMFormatted = formatDouble(sigmaM);

    // 检查是否已存在相同的材料参数（所有四个参数都相同）
    for (int i = 0; i < materialsTable->rowCount(); i++) {
        QString existingEps = materialsTable->item(i, COL_EPS_R)->text();
        QString existingMu = materialsTable->item(i, COL_MU_R)->text();
        QString existingSigma = materialsTable->item(i, COL_SIGMA)->text();
        QString existingSigmaM = materialsTable->item(i, COL_SIGMA_M)->text();

        // 比较格式化后的字符串
        if (existingEps == epsFormatted &&
            existingMu == muFormatted &&
            existingSigma == sigmaFormatted &&
            existingSigmaM == sigmaMFormatted) {
            QMessageBox::warning(this, "添加失败", "该材料参数已存在，请勿重复添加");

            // 清空输入框
            epsEdit->clear();
            muEdit->clear();
            sigmaEdit->clear();
            sigmaMEdit->clear();
            epsEdit->setFocus();

            return;
        }
    }

    // 添加到表格
    int row = materialsTable->rowCount();
    materialsTable->insertRow(row);

    materialsTable->setItem(row, COL_EPS_R, new QTableWidgetItem(epsFormatted));
    materialsTable->setItem(row, COL_MU_R, new QTableWidgetItem(muFormatted));
    materialsTable->setItem(row, COL_SIGMA, new QTableWidgetItem(sigmaFormatted));
    materialsTable->setItem(row, COL_SIGMA_M, new QTableWidgetItem(sigmaMFormatted));

    // 更新行号
    updateRowNumbers();

    // 清空输入框
    epsEdit->clear();
    muEdit->clear();
    sigmaEdit->clear();
    sigmaMEdit->clear();

    epsEdit->setFocus();
}

void MaterialParameter::deleteMaterial()
{
    int currentRow = materialsTable->currentRow();
    if (currentRow >= 0) {
        materialsTable->removeRow(currentRow);
        // 更新行号
        updateRowNumbers();
    } else {
        QMessageBox::information(this, "提示", "请选择要删除的行");
    }
}

void MaterialParameter::clearAllMaterials()
{
    if (materialsTable->rowCount() > 0) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认", "确定要清空所有材料参数吗？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            materialsTable->setRowCount(0);
        }
    }
}

void MaterialParameter::saveMaterials()
{
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/MaterialParameter.txt");

    // 获取材料个数
    int materialCount = materialsTable->rowCount();

    // 保存到文件
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入前三行固定格式
        out << "=================================\n";
        out << "eps_r\tmu_r\tsigma\tsigma_m\n";
        out << "=================================\n";

        // 写入材料个数
        out << materialCount << "\n";

        // 写入每个材料的参数
        for (int i = 0; i < materialCount; i++) {
            QString eps = materialsTable->item(i, COL_EPS_R)->text();
            QString mu = materialsTable->item(i, COL_MU_R)->text();
            QString sigma = materialsTable->item(i, COL_SIGMA)->text();
            QString sigmaM = materialsTable->item(i, COL_SIGMA_M)->text();

            out << eps << "\t" << mu << "\t" << sigma << "\t" << sigmaM << "\n";
        }

        file.close();

        QMessageBox::information(this, "成功", "材料参数数据已保存");

        // 打印调试信息
        qDebug() << "材料参数数据已保存到:" << fileName;
        qDebug() << "共保存" << materialCount << "种材料";

        close();
    } else {
        QMessageBox::critical(this, "错误", "无法保存材料参数文件");
    }
}
