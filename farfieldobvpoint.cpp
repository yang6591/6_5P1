#include "farfieldobvpoint.h"
#include "global.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>


FarFieldObvPoint::FarFieldObvPoint(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
    loadExistingPoints();
}

void FarFieldObvPoint::setupUI()
{
    setWindowTitle(tr("远场观测点设置"));
    resize(500, 400);

    // 创建输入控件
    rEdit = new QLineEdit;
    rEdit->setPlaceholderText("r(m)");
    rEdit->setMinimumWidth(120);

    phiEdit = new QLineEdit;
    phiEdit->setPlaceholderText("phi(degree)");
    phiEdit->setMinimumWidth(120);

    thetaEdit = new QLineEdit;
    thetaEdit->setPlaceholderText("theta(degree)");
    thetaEdit->setMinimumWidth(120);

    // 创建表格
    pointsTable = new QTableWidget;
    pointsTable->setColumnCount(COL_COUNT);
    QStringList headers;
    headers << "r(m)" << "phi(degree)" << "theta(degree)";
    pointsTable->setHorizontalHeaderLabels(headers);
    pointsTable->setSelectionBehavior(QTableWidget::SelectRows);
    pointsTable->setSelectionMode(QTableWidget::SingleSelection);
    pointsTable->horizontalHeader()->setStretchLastSection(true);
    pointsTable->setAlternatingRowColors(true);

    // 创建按钮
    addBtn = new QPushButton("添加");
    deleteBtn = new QPushButton("删除");
    clearBtn = new QPushButton("清空");
    saveBtn = new QPushButton("保存");
    cancelBtn = new QPushButton("取消");

    // 设置布局
    mainLayout = new QVBoxLayout(this);

    // 输入区域布局
    QGroupBox *inputGroupBox = new QGroupBox("添加远场观测点");
    inputGridLayout = new QGridLayout;
    inputGridLayout->addWidget(new QLabel("r(m):"), 0, 0);
    inputGridLayout->addWidget(rEdit, 0, 1);
    inputGridLayout->addWidget(new QLabel("phi(degree):"), 0, 2);
    inputGridLayout->addWidget(phiEdit, 0, 3);
    inputGridLayout->addWidget(new QLabel("theta(degree):"), 1, 0);
    inputGridLayout->addWidget(thetaEdit, 1, 1);
    inputGridLayout->addWidget(addBtn, 1, 2, 1, 2);
    inputGroupBox->setLayout(inputGridLayout);

    // 表格区域
    QGroupBox *tableGroupBox = new QGroupBox("观测点列表");
    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addWidget(pointsTable);
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

void FarFieldObvPoint::setupConnections()
{
    connect(addBtn, &QPushButton::clicked, this, &FarFieldObvPoint::addPoint);
    connect(deleteBtn, &QPushButton::clicked, this, &FarFieldObvPoint::deletePoint);
    connect(clearBtn, &QPushButton::clicked, this, &FarFieldObvPoint::clearAllPoints);
    connect(saveBtn, &QPushButton::clicked, this, &FarFieldObvPoint::savePoints);
    connect(cancelBtn, &QPushButton::clicked, this, &FarFieldObvPoint::close);
}

void FarFieldObvPoint::loadExistingPoints()
{
    // 检查是否存在已保存的观测点文件
    QString fileName = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/FarFieldObservationPoint.txt");
    QFile file(fileName);

    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        // 读取前三行（标题行和分隔线）
        for (int i = 0; i < 3; i++) {
            in.readLine();
        }

        // 读取观测点个数
        QString countLine = in.readLine().trimmed();
        bool ok;
        int pointCount = countLine.toInt(&ok);

        if (ok && pointCount > 0) {
            for (int i = 0; i < pointCount; i++) {
                if (in.atEnd()) break;

                QString line = in.readLine().trimmed();
                if (line.isEmpty()) continue;

                QStringList fields = line.split(QRegularExpression("\\s+"));
                if (fields.size() >= 3) {
                    int row = pointsTable->rowCount();
                    pointsTable->insertRow(row);

                    pointsTable->setItem(row, COL_R, new QTableWidgetItem(fields[0]));
                    pointsTable->setItem(row, COL_PHI, new QTableWidgetItem(fields[1]));
                    pointsTable->setItem(row, COL_THETA, new QTableWidgetItem(fields[2]));
                }
            }
        }

        file.close();
    }
}

void FarFieldObvPoint::addPoint()
{
    QString rStr = rEdit->text().trimmed();
    QString phiStr = phiEdit->text().trimmed();
    QString thetaStr = thetaEdit->text().trimmed();

    // 验证输入
    bool okR, okPhi, okTheta;
    int r = rStr.toInt(&okR);
    int phi = phiStr.toInt(&okPhi);
    int theta = thetaStr.toInt(&okTheta);

    if (!okR) {
        QMessageBox::warning(this, "输入错误", "r(m)必须为数字");
        rEdit->setFocus();
        return;
    }

    if (!okPhi) {
        QMessageBox::warning(this, "输入错误", "phi(degree)必须为数字");
        phiEdit->setFocus();
        return;
    }

    if (!okTheta) {
        QMessageBox::warning(this, "输入错误", "theta(degree)必须为数字");
        thetaEdit->setFocus();
        return;
    }


    // 检查是否已存在相同的观测点
    for (int i = 0; i < pointsTable->rowCount(); i++) {
        QString existingR = pointsTable->item(i, COL_R)->text();
        QString existingPhi = pointsTable->item(i, COL_PHI)->text();
        QString existingTheta = pointsTable->item(i, COL_THETA)->text();

        bool okExistingR, okExistingPhi, okExistingTheta;
        int existingRInt = existingR.toInt(&okExistingR);
        int existingPhiInt = existingPhi.toInt(&okExistingPhi);
        int existingThetaInt = existingTheta.toInt(&okExistingTheta);

        // 如果三个值都相同，则视为重复
        if (okExistingR && okExistingPhi && okExistingTheta &&
            existingRInt == r && existingPhiInt == phi && existingThetaInt == theta) {
            QMessageBox::warning(this, "添加失败", "该观测点已存在，请勿重复添加");

            // 清空输入框
            rEdit->clear();
            phiEdit->clear();
            thetaEdit->clear();
            rEdit->setFocus();

            return;
        }
    }

    // 添加到表格
    int row = pointsTable->rowCount();
    pointsTable->insertRow(row);

    pointsTable->setItem(row, COL_R, new QTableWidgetItem(QString::number(r)));
    pointsTable->setItem(row, COL_PHI, new QTableWidgetItem(QString::number(phi)));
    pointsTable->setItem(row, COL_THETA, new QTableWidgetItem(QString::number(theta)));

    // 清空输入框
    rEdit->clear();
    phiEdit->clear();
    thetaEdit->clear();

    rEdit->setFocus();
}

void FarFieldObvPoint::deletePoint()
{
    int currentRow = pointsTable->currentRow();
    if (currentRow >= 0) {
        pointsTable->removeRow(currentRow);
    } else {
        QMessageBox::information(this, "提示", "请选择要删除的行");
    }
}

void FarFieldObvPoint::clearAllPoints()
{
    if (pointsTable->rowCount() > 0) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认", "确定要清空所有观测点吗？",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            pointsTable->setRowCount(0);
        }
    }
}

void FarFieldObvPoint::savePoints()
{
    // 获取应用程序目录路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString fileName = QDir::cleanPath(appDir + "/FarFieldObservationPoint.txt");

    // 获取观测点个数
    int pointCount = pointsTable->rowCount();

    // 保存到文件
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入前三行固定格式
        out << "==============================\n";
        out << "r(m)\tphi(degree)\ttheta(degree)\n";
        out << "==============================\n";

        // 写入观测点个数
        out << pointCount << "\n";

        // 写入每个观测点的数据
        for (int i = 0; i < pointCount; i++) {
            QString r = pointsTable->item(i, COL_R)->text();
            QString phi = pointsTable->item(i, COL_PHI)->text();
            QString theta = pointsTable->item(i, COL_THETA)->text();

            out << r << "\t" << phi << "\t" << theta << "\n";
        }

        file.close();

        QMessageBox::information(this, "成功", "远场观测点数据已保存");

        // 打印调试信息
        qDebug() << "远场观测点数据已保存到:" << fileName;
        qDebug() << "共保存" << pointCount << "个观测点";

        close();
    } else {
        QMessageBox::critical(this, "错误", "无法保存观测点文件");
    }
}
