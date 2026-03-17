#ifndef FARFIELDOBVPOINT_H
#define FARFIELDOBVPOINT_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>

class FarFieldObvPoint : public QDialog
{
    Q_OBJECT

public:
    explicit FarFieldObvPoint(QWidget *parent = nullptr);

private slots:
    void addPoint();
    void deletePoint();
    void clearAllPoints();
    void savePoints();

private:
    // 输入控件
    QLineEdit *rEdit;
    QLineEdit *phiEdit;
    QLineEdit *thetaEdit;

    // 表格显示
    QTableWidget *pointsTable;

    // 按钮
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *clearBtn;
    QPushButton *saveBtn;
    QPushButton *cancelBtn;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *inputLayout;
    QVBoxLayout *buttonLayout;
    QGridLayout *inputGridLayout;

    // 表格列定义
    enum TableColumns {
        COL_R = 0,
        COL_PHI,
        COL_THETA,
        COL_COUNT
    };

    void setupUI();
    void setupConnections();
    void loadExistingPoints();

};

#endif // FARFIELDOBVPOINT_H
