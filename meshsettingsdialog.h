#ifndef MESHSETTINGSDIALOG_H
#define MESHSETTINGSDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>

class MeshSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeshSettingsDialog(QWidget *parent = nullptr);
    ~MeshSettingsDialog() = default;

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onApplyClicked();
    void onUpdateClicked();
    void onSpecialsClicked();
    void onSimplifyModelClicked();
    void onHelpClicked();

    void onCellsPerWavelengthSameToggled(bool checked);
    void onCellsPerBoxEdgeSameToggled(bool checked);
    void onSameInAllDirectionsToggled(bool checked);  // 修正函数名
    void updateStatistics();
    void onBoxEdgeTypeChanged(int index);
    //***********
    void showRightControls(int index);

private:
    void setupUI();
    void setupConnections();

    // Maximum cell 部分 - Cells per wavelength
    QGroupBox* maxCellGroup;
    QGridLayout* maxLayout;
    QLabel* cellsPerWavelengthLabel;
    QLabel* nearToModelLabel;
    QSpinBox* nearCellsSpinBox;
    QLabel* farFromModelLabel;
    QSpinBox* farCellsSpinBox;
    QCheckBox* cellsPerWavelengthSameCheckbox;

    // Maximum cell 部分 - Cells per max model box edge
    QLabel* cellsPerBoxEdgeLabel;
    QSpinBox* boxEdgeNearSpinBox;
    QSpinBox* boxEdgeFarSpinBox;
    QCheckBox* cellsPerBoxEdgeSameCheckbox;
    QComboBox* boxEdgeTypeComboBox;
    int savedCellsPerBoxEdgeNearValue;
    int savedCellsPerBoxEdgeFarValue;
    int savedAbsoluteValue1;
    int savedAbsoluteValue2;

    // Minimum cell 部分
    QGroupBox* minCellGroup;
    QVBoxLayout* minLayout;
    QComboBox* minboxEdgeTypeComboBox;
    QHBoxLayout* absoluteValueLayout;
    QLabel* absoluteValueLabel;
    QSpinBox* xValueSpinBox;      // 添加声明
    QSpinBox* yValueSpinBox;      // 添加声明
    QSpinBox* zValueSpinBox;      // 添加声明
    QCheckBox* sameInAllDirectionsCheckbox;  // 修正变量名

    //***************
    QWidget *rightContainer;
    QHBoxLayout *rightContainerLayout;
    QWidget *fractionContainer;
    QLabel *fractionLabel;
    QDoubleSpinBox *fractionSpinBox;
    QWidget *absoluteContainer;

    // Statistics 部分
    QGroupBox* statsGroup;
    QGridLayout* statsLayout;

    // Smallest cell 部分
    QLabel* smallestCellLabel;
    QLabel* nxLabel;
    QDoubleSpinBox* nxSizeSpinBox;
    QSpinBox* nxCountSpinBox;      // 修正变量名

    // Largest cell 部分
    QLabel* largestCellLabel;
    QLabel* nyLabel;
    QDoubleSpinBox* nySizeSpinBox;
    QSpinBox* nyCountSpinBox;      // 修正变量名

    // Number of cells 部分
    QLabel* numberOfCellsLabel;
    QLabel* nzLabel;
    QDoubleSpinBox* totalCellsBox;
    QSpinBox* nzCountSpinBox;      // 修正变量名

    // 按钮
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* applyButton;
    QPushButton* updateButton;
    QPushButton* specialsButton;
    QPushButton* simplifyButton;
    QPushButton* helpButton;

    // 布局
    QHBoxLayout* mainLayout;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
};

#endif // MESHSETTINGSDIALOG_H
