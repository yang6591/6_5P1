#ifndef MATERIALPARAMETER_H
#define MATERIALPARAMETER_H

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>

class MaterialParameter : public QDialog
{
    Q_OBJECT

public:
    explicit MaterialParameter(QWidget *parent = nullptr);
    ~MaterialParameter() = default;

private slots:
    void addMaterial();
    void deleteMaterial();
    void clearAllMaterials();
    void saveMaterials();
    void loadExistingMaterials();

    void updateRowNumbers();

private:
    void setupUI();
    void setupConnections();

    // 枚举定义表格列
    enum TableColumns {
        //COL_INDEX = 0, // 编号列
        COL_EPS_R = 0,
        COL_MU_R,
        COL_SIGMA,
        COL_SIGMA_M,
        COL_COUNT
    };

    // 输入控件
    QLineEdit *epsEdit;
    QLineEdit *muEdit;
    QLineEdit *sigmaEdit;
    QLineEdit *sigmaMEdit;

    // 表格控件
    QTableWidget *materialsTable;

    // 按钮控件
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *clearBtn;
    QPushButton *saveBtn;
    QPushButton *cancelBtn;

    // 布局
    QVBoxLayout *mainLayout;
    QGridLayout *inputGridLayout;
};

#endif // MATERIALPARAMETER_H
