#include "meshsettingsdialog.h"
#include <QFrame>

MeshSettingsDialog::MeshSettingsDialog(QWidget* parent)
    : QDialog(parent)
    , savedCellsPerBoxEdgeNearValue(20)  // 默认值
    , savedCellsPerBoxEdgeFarValue(1)    // 默认值
    , savedAbsoluteValue1(0)
    , savedAbsoluteValue2(0)
{
    setupUI();
    setupConnections();

    // 设置对话框属性
    //setWindowTitle("Mesh Properties - Hexahedral");
    setWindowTitle("Mesh Properties");
    setModal(true);
    setFixedSize(700, 500);
}

void MeshSettingsDialog::setupUI()
{
    mainLayout = new QHBoxLayout(this);

    // 左侧布局 - 放置所有设置组
    leftLayout = new QVBoxLayout();

    // 创建Maximum cell部分
    maxCellGroup = new QGroupBox("Maximum cell", this);
    maxLayout = new QGridLayout(maxCellGroup);

    // Cells per wavelength 部分
    int row = 0;
    cellsPerWavelengthLabel = new QLabel("Cells per wavelength:", maxCellGroup);
    maxLayout->addWidget(cellsPerWavelengthLabel, row, 0, 1, 3);
    //row++;
    nearToModelLabel = new QLabel("Near to model:", maxCellGroup);
    maxLayout->addWidget(nearToModelLabel,row, 2);
    farFromModelLabel = new QLabel("Far from model:", maxCellGroup);
    maxLayout->addWidget(farFromModelLabel, row, 3);
    row++;
    nearCellsSpinBox = new QSpinBox(maxCellGroup);
    nearCellsSpinBox->setRange(1, 100);
    nearCellsSpinBox->setValue(15);
    maxLayout->addWidget(nearCellsSpinBox, row, 2);
    maxLayout->addItem(new QSpacerItem(20, 0), row, 2);

    farCellsSpinBox = new QSpinBox(maxCellGroup);
    farCellsSpinBox->setRange(1, 100);
    farCellsSpinBox->setValue(15);
    maxLayout->addWidget(farCellsSpinBox, row, 3);
    row++;

    cellsPerWavelengthSameCheckbox = new QCheckBox("Use same setting as near to model", maxCellGroup);
    maxLayout->addWidget(cellsPerWavelengthSameCheckbox, row, 2, 1, 2);
    row++;


    // Cells per max model box edge 部分
    //cellsPerBoxEdgeLabel = new QLabel("Cells per max model box edge:", maxCellGroup);
    //maxLayout->addWidget(cellsPerBoxEdgeLabel, row, 0, 1, 3);

    // 创建组合框替代原来的标签
    boxEdgeTypeComboBox = new QComboBox(maxCellGroup);
    boxEdgeTypeComboBox->addItem("Cells per max model box edge");
    boxEdgeTypeComboBox->addItem("Absolute Value");
    maxLayout->addWidget(boxEdgeTypeComboBox, row, 0, 1, 2);

    boxEdgeNearSpinBox = new QSpinBox(maxCellGroup);
    boxEdgeNearSpinBox->setRange(1, 100);
    boxEdgeNearSpinBox->setValue(20);
    maxLayout->addWidget(boxEdgeNearSpinBox, row, 2);

    boxEdgeFarSpinBox = new QSpinBox(maxCellGroup);
    boxEdgeFarSpinBox->setRange(1, 100);
    boxEdgeFarSpinBox->setValue(1);
    maxLayout->addWidget(boxEdgeFarSpinBox, row, 3);
    row++;

    cellsPerBoxEdgeSameCheckbox = new QCheckBox("Use same setting as near to model", maxCellGroup);
    maxLayout->addWidget(cellsPerBoxEdgeSameCheckbox, row, 2, 1, 2);

    maxCellGroup->setLayout(maxLayout);
    leftLayout->addWidget(maxCellGroup);
    row++;


    // // 创建Minimum cell部分
    // minCellGroup = new QGroupBox("Minimum cell", this);
    // minLayout = new QVBoxLayout(minCellGroup);

    // // 创建网格布局用于第一行
    // QGridLayout *firstRowLayout = new QGridLayout();

    // minboxEdgeTypeComboBox = new QComboBox(minCellGroup);
    // minboxEdgeTypeComboBox->addItem("Fraction of maximum cell near to model");
    // minboxEdgeTypeComboBox->addItem("Absolute Value");
    // firstRowLayout->addWidget(minboxEdgeTypeComboBox, 0, 0);

    // // 添加标签和SpinBox
    // firstRowLayout->addWidget(new QLabel("X:"), 0, 1);
    // xValueSpinBox = new QSpinBox(minCellGroup);
    // xValueSpinBox->setRange(0, 1000);
    // xValueSpinBox->setValue(0);
    // firstRowLayout->addWidget(xValueSpinBox, 0, 2);

    // firstRowLayout->addWidget(new QLabel("Y:"), 0, 3);
    // yValueSpinBox = new QSpinBox(minCellGroup);
    // yValueSpinBox->setRange(0, 1000);
    // yValueSpinBox->setValue(0);
    // firstRowLayout->addWidget(yValueSpinBox, 0, 4);

    // firstRowLayout->addWidget(new QLabel("Z:"), 0, 5);
    // zValueSpinBox = new QSpinBox(minCellGroup);
    // zValueSpinBox->setRange(0, 1000);
    // zValueSpinBox->setValue(0);
    // firstRowLayout->addWidget(zValueSpinBox, 0, 6);

    // // 添加拉伸
    // firstRowLayout->setColumnStretch(7, 1);

    // // 将第一行添加到主布局
    // minLayout->addLayout(firstRowLayout);

    // sameInAllDirectionsCheckbox = new QCheckBox("Use same setting in all three directions", minCellGroup);
    // minLayout->addWidget(sameInAllDirectionsCheckbox);
    // minLayout->addStretch();

    // minCellGroup->setLayout(minLayout);
    // leftLayout->addWidget(minCellGroup);


    //****************************
    // // 创建Minimum cell部分
    // minCellGroup = new QGroupBox("Minimum cell", this);
    // minLayout = new QVBoxLayout(minCellGroup);

    // // 创建第一行：ComboBox和右侧控件
    // QHBoxLayout *firstRowLayout = new QHBoxLayout();

    // minboxEdgeTypeComboBox = new QComboBox(minCellGroup);
    // minboxEdgeTypeComboBox->addItem("Fraction of maximum cell near to model");
    // minboxEdgeTypeComboBox->addItem("Absolute Value");
    // firstRowLayout->addWidget(minboxEdgeTypeComboBox);

    // // 创建右侧控件的容器（用于动态切换）
    // rightContainer = new QWidget(minCellGroup);
    // rightContainerLayout = new QHBoxLayout(rightContainer);
    // firstRowLayout->addWidget(rightContainer);

    // // 将第一行添加到主布局
    // minLayout->addLayout(firstRowLayout);

    // // 创建两种不同的右侧控件布局
    // fractionContainer = new QWidget(rightContainer);
    // QHBoxLayout *fractionLayout = new QHBoxLayout(fractionContainer);
    // fractionLabel = new QLabel("Fraction:", fractionContainer);
    // fractionSpinBox = new QDoubleSpinBox(fractionContainer);
    // fractionSpinBox->setRange(0.0, 1.0);
    // fractionSpinBox->setSingleStep(0.01);
    // fractionSpinBox->setValue(0.1);
    // fractionLayout->addWidget(fractionLabel);
    // fractionLayout->addWidget(fractionSpinBox);
    // fractionLayout->addStretch();

    // absoluteContainer = new QWidget(rightContainer);
    // QHBoxLayout *absoluteLayout = new QHBoxLayout(absoluteContainer);
    // QLabel *xLabel = new QLabel("X:", absoluteContainer);
    // xValueSpinBox = new QSpinBox(absoluteContainer);
    // xValueSpinBox->setRange(0, 1000);
    // xValueSpinBox->setValue(0);
    // QLabel *yLabel = new QLabel("Y:", absoluteContainer);
    // yValueSpinBox = new QSpinBox(absoluteContainer);
    // yValueSpinBox->setRange(0, 1000);
    // yValueSpinBox->setValue(0);
    // QLabel *zLabel = new QLabel("Z:", absoluteContainer);
    // zValueSpinBox = new QSpinBox(absoluteContainer);
    // zValueSpinBox->setRange(0, 1000);
    // zValueSpinBox->setValue(0);
    // absoluteLayout->addWidget(xLabel);
    // absoluteLayout->addWidget(xValueSpinBox);
    // absoluteLayout->addWidget(yLabel);
    // absoluteLayout->addWidget(yValueSpinBox);
    // absoluteLayout->addWidget(zLabel);
    // absoluteLayout->addWidget(zValueSpinBox);
    // absoluteLayout->addStretch();

    // // 初始显示对应的布局
    // showRightControls(minboxEdgeTypeComboBox->currentIndex());

    // // 连接信号，当ComboBox改变时切换布局
    // connect(minboxEdgeTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //         this, &MeshSettingsDialog::showRightControls);

    // sameInAllDirectionsCheckbox = new QCheckBox("Use same setting in all three directions", minCellGroup);
    // minLayout->addWidget(sameInAllDirectionsCheckbox);
    // minLayout->addStretch();

    // minCellGroup->setLayout(minLayout);
    // leftLayout->addWidget(minCellGroup);



    // 创建Minimum cell部分
    minCellGroup = new QGroupBox("Minimum cell", this);
    minLayout = new QVBoxLayout(minCellGroup);

    // 创建第一行：ComboBox和右侧控件
    QHBoxLayout *firstRowLayout = new QHBoxLayout();

    minboxEdgeTypeComboBox = new QComboBox(minCellGroup);
    minboxEdgeTypeComboBox->addItem("Fraction of maximum cell near to model");
    minboxEdgeTypeComboBox->addItem("Absolute Value");
    firstRowLayout->addWidget(minboxEdgeTypeComboBox);

    // 创建右侧容器（不设置父对象）
    rightContainer = new QWidget();
    rightContainerLayout = new QHBoxLayout(rightContainer);
    firstRowLayout->addWidget(rightContainer);

    // 将第一行添加到主布局
    minLayout->addLayout(firstRowLayout);

    // 创建Fraction控件
    fractionLabel = new QLabel("Fraction:", rightContainer);
    fractionSpinBox = new QDoubleSpinBox(rightContainer);
    fractionSpinBox->setRange(0.0, 1.0);
    fractionSpinBox->setSingleStep(0.01);
    fractionSpinBox->setValue(0.1);
    fractionSpinBox->setFixedWidth(150);  // 这里设置了固定宽度

    // 创建Absolute Value控件
    QLabel *xLabel = new QLabel("X:", rightContainer);
    xValueSpinBox = new QSpinBox(rightContainer);
    xValueSpinBox->setRange(0, 1000);
    xValueSpinBox->setValue(0);
    xValueSpinBox->setFixedWidth(60);  // 也可以设置xyz SpinBox的宽度

    QLabel *yLabel = new QLabel("Y:", rightContainer);
    yValueSpinBox = new QSpinBox(rightContainer);
    yValueSpinBox->setRange(0, 1000);
    yValueSpinBox->setValue(0);
    yValueSpinBox->setFixedWidth(60);

    QLabel *zLabel = new QLabel("Z:", rightContainer);
    zValueSpinBox = new QSpinBox(rightContainer);
    zValueSpinBox->setRange(0, 1000);
    zValueSpinBox->setValue(0);
    zValueSpinBox->setFixedWidth(60);

    // 初始显示Fraction控件，隐藏Absolute Value控件
    fractionLabel->show();
    fractionSpinBox->show();
    xLabel->hide();
    xValueSpinBox->hide();
    yLabel->hide();
    yValueSpinBox->hide();
    zLabel->hide();
    zValueSpinBox->hide();

    // 添加到布局
    rightContainerLayout->addWidget(fractionLabel);
    rightContainerLayout->addWidget(fractionSpinBox);
    rightContainerLayout->addWidget(xLabel);
    rightContainerLayout->addWidget(xValueSpinBox);
    rightContainerLayout->addWidget(yLabel);
    rightContainerLayout->addWidget(yValueSpinBox);
    rightContainerLayout->addWidget(zLabel);
    rightContainerLayout->addWidget(zValueSpinBox);
    rightContainerLayout->addStretch();

    // 连接信号，当ComboBox改变时切换显示
    connect(minboxEdgeTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index) {
                if (index == 0) {
                    // "Fraction of maximum cell near to model"
                    fractionLabel->show();
                    fractionSpinBox->show();
                    xLabel->hide();
                    xValueSpinBox->hide();
                    yLabel->hide();
                    yValueSpinBox->hide();
                    zLabel->hide();
                    zValueSpinBox->hide();
                } else {
                    // "Absolute Value"
                    fractionLabel->hide();
                    fractionSpinBox->hide();
                    xLabel->show();
                    xValueSpinBox->show();
                    yLabel->show();
                    yValueSpinBox->show();
                    zLabel->show();
                    zValueSpinBox->show();
                }
            });

    sameInAllDirectionsCheckbox = new QCheckBox("Use same setting in all three directions", minCellGroup);
    minLayout->addWidget(sameInAllDirectionsCheckbox);
    minLayout->addStretch();

    minCellGroup->setLayout(minLayout);
    leftLayout->addWidget(minCellGroup);



    // 创建Statistics部分
    statsGroup = new QGroupBox("Statistics", this);
    statsLayout = new QGridLayout(statsGroup);

    row = 0;

    // Smallest cell 行
    smallestCellLabel = new QLabel("Smallest cell:", statsGroup);
    nxLabel = new QLabel("Nx:", statsGroup);

    statsLayout->addWidget(smallestCellLabel, row, 0);
    statsLayout->addWidget(nxLabel, row, 2);

    // 第二行：Smallest cell 的值
    row++;
    nxSizeSpinBox = new QDoubleSpinBox(statsGroup);
    nxSizeSpinBox->setRange(0.0, 10.0);
    nxSizeSpinBox->setValue(0.5);
    nxSizeSpinBox->setDecimals(4);
    nxSizeSpinBox->setMinimumWidth(80);
    statsLayout->addWidget(nxSizeSpinBox, row, 0);

    nxCountSpinBox = new QSpinBox(statsGroup);
    nxCountSpinBox->setRange(0, 1000);
    nxCountSpinBox->setValue(135);
    nxCountSpinBox->setMinimumWidth(80);
    statsLayout->addWidget(nxCountSpinBox, row, 2);

    // 添加一些间距
    statsLayout->setColumnMinimumWidth(1, 20);  // 列1作为间距
    statsLayout->setColumnMinimumWidth(3, 20);  // 列3作为间距

    // Largest cell 行
    row++;
    largestCellLabel = new QLabel("Largest cell:", statsGroup);
    nyLabel = new QLabel("Ny:", statsGroup);

    statsLayout->addWidget(largestCellLabel, row, 0);
    statsLayout->addWidget(nyLabel, row, 2);

    // 第二行：Largest cell 的值
    row++;
    nySizeSpinBox = new QDoubleSpinBox(statsGroup);
    nySizeSpinBox->setRange(0.0, 10.0);
    nySizeSpinBox->setValue(3.88927);
    nySizeSpinBox->setDecimals(5);
    nySizeSpinBox->setMinimumWidth(80);
    statsLayout->addWidget(nySizeSpinBox, row, 0);

    nyCountSpinBox = new QSpinBox(statsGroup);
    nyCountSpinBox->setRange(0, 1000);
    nyCountSpinBox->setValue(123);
    nyCountSpinBox->setMinimumWidth(80);
    statsLayout->addWidget(nyCountSpinBox, row, 2);

    // Number of cells 行
    row++;
    numberOfCellsLabel = new QLabel("Number of cells:", statsGroup);
    nzLabel = new QLabel("Nz:", statsGroup);

    statsLayout->addWidget(numberOfCellsLabel, row, 0);
    statsLayout->addWidget(nzLabel, row, 2);

    // 第二行：Number of cells 的值
    row++;
    totalCellsBox = new QDoubleSpinBox(statsGroup);
    totalCellsBox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    totalCellsBox->setMinimumWidth(80);
    statsLayout->addWidget(totalCellsBox, row, 0);

    nzCountSpinBox = new QSpinBox(statsGroup);
    nzCountSpinBox->setRange(0, 100);
    nzCountSpinBox->setValue(32);
    nzCountSpinBox->setMinimumWidth(80);
    statsLayout->addWidget(nzCountSpinBox, row, 2);

    // 添加弹性空间
    statsLayout->setRowStretch(row + 1, 1);

    statsGroup->setLayout(statsLayout);
    leftLayout->addWidget(statsGroup);

    leftLayout->addStretch();

    // 右侧布局 - 放置按钮
    rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(10, 0, 0, 0);

    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    applyButton = new QPushButton("Apply", this);
    updateButton = new QPushButton("Update", this);
    specialsButton = new QPushButton("Specials...", this);
    simplifyButton = new QPushButton("Simplify Model...", this);
    helpButton = new QPushButton("Help", this);

    // 设置按钮大小
    okButton->setMinimumWidth(120);
    cancelButton->setMinimumWidth(120);
    applyButton->setMinimumWidth(120);
    updateButton->setMinimumWidth(120);
    specialsButton->setMinimumWidth(120);
    simplifyButton->setMinimumWidth(120);
    helpButton->setMinimumWidth(120);

    // 垂直排列按钮
    rightLayout->addWidget(okButton);
    rightLayout->addWidget(cancelButton);
    rightLayout->addWidget(applyButton);
    rightLayout->addWidget(updateButton);
    rightLayout->addWidget(specialsButton);
    rightLayout->addWidget(simplifyButton);
    rightLayout->addWidget(helpButton);
    rightLayout->addStretch();

    // 将左右布局添加到主布局
    mainLayout->addLayout(leftLayout, 1);  // 左侧占更大空间
    mainLayout->addLayout(rightLayout, 0); // 右侧占固定空间

    setLayout(mainLayout);
}

void MeshSettingsDialog::setupConnections()
{
    connect(okButton, &QPushButton::clicked, this, &MeshSettingsDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &MeshSettingsDialog::onCancelClicked);
    connect(applyButton, &QPushButton::clicked, this, &MeshSettingsDialog::onApplyClicked);
    connect(updateButton, &QPushButton::clicked, this, &MeshSettingsDialog::onUpdateClicked);
    connect(specialsButton, &QPushButton::clicked, this, &MeshSettingsDialog::onSpecialsClicked);
    connect(simplifyButton, &QPushButton::clicked, this, &MeshSettingsDialog::onSimplifyModelClicked);
    connect(helpButton, &QPushButton::clicked, this, &MeshSettingsDialog::onHelpClicked);

    // 连接复选框信号
    connect(cellsPerWavelengthSameCheckbox, &QCheckBox::toggled,
            this, &MeshSettingsDialog::onCellsPerWavelengthSameToggled);
    connect(cellsPerBoxEdgeSameCheckbox, &QCheckBox::toggled,
            this, &MeshSettingsDialog::onCellsPerBoxEdgeSameToggled);
    connect(sameInAllDirectionsCheckbox, &QCheckBox::toggled,
            this, &MeshSettingsDialog::onSameInAllDirectionsToggled);  // 修正函数名

    // 连接组合框的信号
    connect(boxEdgeTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MeshSettingsDialog::onBoxEdgeTypeChanged);

    // 当数值改变时更新统计数据
    connect(nearCellsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(farCellsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(boxEdgeNearSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(boxEdgeFarSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);

    connect(xValueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(yValueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(zValueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);

    // // Statistics部分的输入框改变时也更新
    // connect(nxSizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    //         this, &MeshSettingsDialog::updateStatistics);
    // connect(nxCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
    //         this, &MeshSettingsDialog::updateStatistics);
    // connect(nySizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
    //         this, &MeshSettingsDialog::updateStatistics);
    // connect(nyCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
    //         this, &MeshSettingsDialog::updateStatistics);
    // connect(nzCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
    //         this, &MeshSettingsDialog::updateStatistics);

    // Statistics部分的输入框改变时也更新
    connect(nxSizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(nxCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(nySizeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(nyCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(nzCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
    connect(totalCellsBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MeshSettingsDialog::updateStatistics);
}

void MeshSettingsDialog::onBoxEdgeTypeChanged(int index)
{
    if (index == 0) {  // "Cells per max model box edge"
        // 保存当前Absolute Value的值
        savedAbsoluteValue1 = boxEdgeNearSpinBox->value();
        savedAbsoluteValue2 = boxEdgeFarSpinBox->value();
        // 恢复Cells per max model box edge的值
        boxEdgeNearSpinBox->setValue(savedCellsPerBoxEdgeNearValue);
        boxEdgeFarSpinBox->setValue(savedCellsPerBoxEdgeFarValue);
    } else {  // "Absolute Value"

        // 保存当前Cells per max model box edge的值
        savedCellsPerBoxEdgeNearValue = boxEdgeNearSpinBox->value();
        savedCellsPerBoxEdgeFarValue = boxEdgeFarSpinBox->value();
        // 恢复或设置Absolute Value的值
        boxEdgeNearSpinBox->setValue(savedAbsoluteValue1);
        boxEdgeFarSpinBox->setValue(savedAbsoluteValue2);
    }
}


void MeshSettingsDialog::showRightControls(int index)
{
    // 清除当前右侧容器的内容
    QLayoutItem* item;
    while ((item = rightContainerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }

    // 根据选择显示对应的控件
    if (index == 0) {
        // "Fraction of maximum cell near to model"
        rightContainerLayout->addWidget(fractionContainer);
    } else {
        // "Absolute Value"
        rightContainerLayout->addWidget(absoluteContainer);
    }
}


void MeshSettingsDialog::onOkClicked()
{
    // 获取所有用户输入的值
    int nearCells = nearCellsSpinBox->value();
    int farCells = farCellsSpinBox->value();
    bool useSameWavelength = cellsPerWavelengthSameCheckbox->isChecked();
    int boxEdgeNear = boxEdgeNearSpinBox->value();
    int boxEdgeFar = boxEdgeFarSpinBox->value();
    bool useSameBoxEdge = cellsPerBoxEdgeSameCheckbox->isChecked();

    // Minimum cell 部分
    int xValue = xValueSpinBox->value();
    int yValue = yValueSpinBox->value();
    int zValue = zValueSpinBox->value();
    bool sameInAllDirections = sameInAllDirectionsCheckbox->isChecked();

    // // Statistics 部分
    // double smallestNxSize = nxSizeSpinBox->value();
    // int smallestNxCount = nxCountSpinBox->value();
    // double largestNySize = nySizeSpinBox->value();
    // int largestNyCount = nyCountSpinBox->value();
    // int nzCount = nzCountSpinBox->value();
    // int totalCells = totalCellsBox->text().remove(',').toInt();


    // Statistics 部分
    double smallestNxSize = nxSizeSpinBox->value();
    int smallestNxCount = nxCountSpinBox->value();
    double largestNySize = nySizeSpinBox->value();
    int largestNyCount = nyCountSpinBox->value();
    int nzCount = nzCountSpinBox->value();
    int totalCells = static_cast<int>(totalCellsBox->value());

    // 这里可以将数据保存到文件或发送到其他地方
    QMessageBox::information(this, "信息",
                             QString("网格设置已保存。\n"
                                     "Cells per wavelength - 近场: %1, 远场: %2\n"
                                     "Cells per max model box edge - 近场: %3, 远场: %4\n"
                                     "Minimum cell - X: %5, Y: %6, Z: %7\n"
                                     "总单元格数: %8")
                                 .arg(nearCells).arg(farCells).arg(boxEdgeNear).arg(boxEdgeFar)
                                 .arg(xValue).arg(yValue).arg(zValue)
                                 .arg(totalCells));

    accept();
}

void MeshSettingsDialog::onCancelClicked()
{
    reject();
}

void MeshSettingsDialog::onApplyClicked()
{
    // 应用设置但不关闭窗口
    updateStatistics();
    QMessageBox::information(this, "应用成功", "网格设置已应用。");
}

void MeshSettingsDialog::onUpdateClicked()
{
    updateStatistics();
    QMessageBox::information(this, "更新成功", "统计数据已更新。");
}

void MeshSettingsDialog::onSpecialsClicked()
{
    QMessageBox::information(this, "特殊设置", "特殊网格设置功能。");
}

void MeshSettingsDialog::onSimplifyModelClicked()
{
    QMessageBox::information(this, "简化模型", "简化模型功能。");
}

void MeshSettingsDialog::onHelpClicked()
{
    QMessageBox::information(this, "帮助",
                             "网格设置帮助：\n\n"
                             "1. Maximum cell:\n"
                             "   - Cells per wavelength: 设置每个波长的单元格数\n"
                             "   - Cells per max model box edge: 设置最大模型边界上的单元格数\n"
                             "2. Minimum cell: 设置X、Y、Z方向的最小单元格绝对值\n"
                             "3. Statistics: 显示网格统计信息\n\n"
                             "注意：\n"
                             "- 'Use same setting as near to model' 会将远场设置为与近场相同的值\n"
                             "- 'Use same setting in all three directions' 会在X、Y、Z三个方向使用相同的设置");
}

void MeshSettingsDialog::onCellsPerWavelengthSameToggled(bool checked)
{
    // 当选中"Use same setting as near to model"时，
    // 将"Far from model"设置为与"Near to model"相同的值，并禁用输入
    farCellsSpinBox->setEnabled(!checked);

    if (checked) {
        farCellsSpinBox->setValue(nearCellsSpinBox->value());
    }

    // 更新统计数据
    updateStatistics();
}

void MeshSettingsDialog::onCellsPerBoxEdgeSameToggled(bool checked)
{
    // 当选中"Use same setting as near to model"时，
    // 将"Far from model"设置为与"Near to model"相同的值，并禁用输入
    boxEdgeFarSpinBox->setEnabled(!checked);

    if (checked) {
        boxEdgeFarSpinBox->setValue(boxEdgeNearSpinBox->value());
    }

    // 更新统计数据
    updateStatistics();
}

void MeshSettingsDialog::onSameInAllDirectionsToggled(bool checked)
{
    // 当选中"Use same setting in all three directions"时，
    // 将Y和Z值设置为与X相同的值，并禁用Y和Z输入框
    yValueSpinBox->setEnabled(!checked);
    zValueSpinBox->setEnabled(!checked);

    if (checked) {
        int xValue = xValueSpinBox->value();
        yValueSpinBox->setValue(xValue);
        zValueSpinBox->setValue(xValue);
    }

    // 更新统计数据
    updateStatistics();
}

// void MeshSettingsDialog::updateStatistics()
// {
//     // 根据当前设置计算新的统计数据
//     int nearCells = nearCellsSpinBox->value();
//     int farCells = farCellsSpinBox->value();
//     int boxEdgeNear = boxEdgeNearSpinBox->value();
//     int boxEdgeFar = boxEdgeFarSpinBox->value();

//     // Minimum cell 部分
//     int xValue = xValueSpinBox->value();
//     int yValue = sameInAllDirectionsCheckbox->isChecked() ? xValue : yValueSpinBox->value();
//     int zValue = sameInAllDirectionsCheckbox->isChecked() ? xValue : zValueSpinBox->value();

//     // 计算总单元格数（示例计算）
//     int baseCells = (nearCells + farCells) * 10 + (boxEdgeNear + boxEdgeFar) * 5;
//     int adjustedCells = baseCells * (xValue + yValue + zValue + 1);

//     // 更新总单元格数显示（使用逗号分隔千位）
//     QString totalCellsStr = QString::number(adjustedCells);
//     for (int i = totalCellsStr.length() - 3; i > 0; i -= 3) {
//         totalCellsStr.insert(i, ',');
//     }
//     totalCellsBox->setText(totalCellsStr);

//     // 更新最小和最大单元格尺寸（示例计算）
//     double smallestSize = 1.0 / (nearCells > 0 ? nearCells : 1);
//     double largestSize = 1.0 / (farCells > 0 ? farCells : 1);

//     nxSizeSpinBox->setValue(smallestSize);
//     nySizeSpinBox->setValue(largestSize);

//     // 更新单元格数量统计（示例）
//     int smallestCount = nearCells * 10;
//     int largestCount = farCells * 8;
//     int nzCount = (boxEdgeNear + boxEdgeFar) * 2;

//     nxCountSpinBox->setValue(smallestCount);
//     nyCountSpinBox->setValue(largestCount);
//     nzCountSpinBox->setValue(nzCount);
// }


void MeshSettingsDialog::updateStatistics()
{
    // 根据当前设置计算新的统计数据
    int nearCells = nearCellsSpinBox->value();
    int farCells = farCellsSpinBox->value();
    int boxEdgeNear = boxEdgeNearSpinBox->value();
    int boxEdgeFar = boxEdgeFarSpinBox->value();

    // Minimum cell 部分
    int xValue = xValueSpinBox->value();
    int yValue = sameInAllDirectionsCheckbox->isChecked() ? xValue : yValueSpinBox->value();
    int zValue = sameInAllDirectionsCheckbox->isChecked() ? xValue : zValueSpinBox->value();

    // 计算总单元格数（示例计算）
    int baseCells = (nearCells + farCells) * 10 + (boxEdgeNear + boxEdgeFar) * 5;
    int adjustedCells = baseCells * (xValue + yValue + zValue + 1);

    // 更新总单元格数 - 对于 QDoubleSpinBox 使用 setValue()
    totalCellsBox->setValue(adjustedCells);  // 修正：使用 setValue()

    // 更新最小和最大单元格尺寸（示例计算）
    double smallestSize = 1.0 / (nearCells > 0 ? nearCells : 1);
    double largestSize = 1.0 / (farCells > 0 ? farCells : 1);

    nxSizeSpinBox->setValue(smallestSize);
    nySizeSpinBox->setValue(largestSize);

    // 更新单元格数量统计（示例）
    int smallestCount = nearCells * 10;
    int largestCount = farCells * 8;
    int nzCount = (boxEdgeNear + boxEdgeFar) * 2;

    nxCountSpinBox->setValue(smallestCount);
    nyCountSpinBox->setValue(largestCount);
    nzCountSpinBox->setValue(nzCount);
}
