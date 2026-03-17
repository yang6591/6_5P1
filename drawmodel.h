#ifndef DRAWMODEL_H
#define DRAWMODEL_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

// 定义材料类型枚举
enum MaterialType {
    METAL,
    DIELECTRIC1,
    DIELECTRIC2,
    DIELECTRIC3,
    DIELECTRIC4,
    DIELECTRIC5,
    VACUUM
};

class DrawModel : public QObject
{
    Q_OBJECT
public:
    // 长方体参数结构体
    struct BoxParams {
        QString name;
        float xmin;
        float xmax;
        float ymin;
        float ymax;
        float zmin;
        float zmax;
        MaterialType materialType;
    };

    // 静态方法获取长方体参数
    static BoxParams getBoxParams(QWidget *parent = nullptr);
};

// 参数输入对话框
class BoxParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BoxParamsDialog(QWidget *parent = nullptr);
    DrawModel::BoxParams getParams() const;

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_xminEdit;
    QLineEdit *m_xmaxEdit;
    QLineEdit *m_yminEdit;
    QLineEdit *m_ymaxEdit;
    QLineEdit *m_zminEdit;
    QLineEdit *m_zmaxEdit;
    QComboBox *m_materialCombo;
};

#endif // DRAWMODEL_H
