#ifndef DRAWMODEL_H
#define DRAWMODEL_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMap>
#include <QStringList>

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
        float xmin = 0.0f;
        float xmax = 0.0f;
        float ymin = 0.0f;
        float ymax = 0.0f;
        float zmin = 0.0f;
        float zmax = 0.0f;
        MaterialType materialType = METAL;
    };

    struct BoxExpressionInput {
        QString name;
        QString xminExpr;
        QString xmaxExpr;
        QString yminExpr;
        QString ymaxExpr;
        QString zminExpr;
        QString zmaxExpr;
        MaterialType materialType = METAL;
    };

    // 静态方法获取长方体参数
    static BoxParams getBoxParams(QWidget *parent = nullptr, bool *accepted = nullptr);

private:
    static bool resolveBoxExpressions(const BoxExpressionInput &input, BoxParams &params, QWidget *parent = nullptr);
    static QStringList detectParameters(const QStringList &expressions);
    static bool evaluateExpression(const QString &expression,
                                   const QMap<QString, double> &parameterValues,
                                   double &result,
                                   QString &errorMessage);
};

// 参数输入对话框
class BoxParamsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BoxParamsDialog(QWidget *parent = nullptr);
    DrawModel::BoxExpressionInput getInput() const;

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

class ParameterValuesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParameterValuesDialog(const QStringList &parameterNames, QWidget *parent = nullptr);
    QMap<QString, double> getParameterValues(bool &ok, QString &errorMessage) const;

private:
    QMap<QString, QLineEdit *> m_valueEdits;
};

#endif // DRAWMODEL_H
