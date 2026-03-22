#include "drawmodel.h"
#include <QJSEngine>
#include <QJSValue>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>

namespace {
const QRegularExpression kParameterRegex(QStringLiteral("(?<![0-9.])[A-Za-z_][A-Za-z0-9_]*\\b"));
}

DrawModel::BoxParams DrawModel::getBoxParams(QWidget *parent, bool *accepted)
{
    if (accepted) {
        *accepted = false;
    }

    BoxParamsDialog dialog(parent);
    if (dialog.exec() != QDialog::Accepted) {
        return BoxParams();
    }

    BoxExpressionInput input = dialog.getInput();
    BoxParams params;
    if (!resolveBoxExpressions(input, params, parent)) {
        return BoxParams();
    }

    if (accepted) {
        *accepted = true;
    }
    return params;
}

bool DrawModel::resolveBoxExpressions(const BoxExpressionInput &input, BoxParams &params, QWidget *parent)
{
    const QStringList expressions = {
        input.xminExpr, input.xmaxExpr,
        input.yminExpr, input.ymaxExpr,
        input.zminExpr, input.zmaxExpr
    };

    const QStringList parameterNames = detectParameters(expressions);
    QMap<QString, double> parameterValues;

    if (!parameterNames.isEmpty()) {
        ParameterValuesDialog parameterDialog(parameterNames, parent);
        if (parameterDialog.exec() != QDialog::Accepted) {
            return false;
        }

        QString errorMessage;
        bool valuesOk = false;
        parameterValues = parameterDialog.getParameterValues(valuesOk, errorMessage);
        if (!valuesOk) {
            QMessageBox::critical(parent, QObject::tr("参数错误"), errorMessage);
            return false;
        }
    }

    const struct {
        QString label;
        QString expression;
        float *target;
    } fields[] = {
        {QObject::tr("xmin"), input.xminExpr, &params.xmin},
        {QObject::tr("xmax"), input.xmaxExpr, &params.xmax},
        {QObject::tr("ymin"), input.yminExpr, &params.ymin},
        {QObject::tr("ymax"), input.ymaxExpr, &params.ymax},
        {QObject::tr("zmin"), input.zminExpr, &params.zmin},
        {QObject::tr("zmax"), input.zmaxExpr, &params.zmax}
    };

    for (const auto &field : fields) {
        double value = 0.0;
        QString errorMessage;
        if (!evaluateExpression(field.expression, parameterValues, value, errorMessage)) {
            QMessageBox::critical(parent,
                                  QObject::tr("表达式错误"),
                                  QObject::tr("%1 表达式无效：%2").arg(field.label, errorMessage));
            return false;
        }
        *field.target = static_cast<float>(value);
    }

    if (!(params.xmin < params.xmax && params.ymin < params.ymax && params.zmin < params.zmax)) {
        QMessageBox::critical(parent,
                              QObject::tr("输入错误"),
                              QObject::tr("坐标范围无效：必须满足 xmin < xmax、ymin < ymax、zmin < zmax。"));
        return false;
    }

    params.name = input.name.trimmed();
    params.materialType = input.materialType;
    return true;
}

QStringList DrawModel::detectParameters(const QStringList &expressions)
{
    QStringList parameters;
    for (const QString &expression : expressions) {
        QRegularExpressionMatchIterator it = kParameterRegex.globalMatch(expression);
        while (it.hasNext()) {
            const QString parameterName = it.next().captured(0);
            if (!parameters.contains(parameterName)) {
                parameters.append(parameterName);
            }
        }
    }
    std::sort(parameters.begin(), parameters.end());
    return parameters;
}

bool DrawModel::evaluateExpression(const QString &expression,
                                   const QMap<QString, double> &parameterValues,
                                   double &result,
                                   QString &errorMessage)
{
    const QString trimmedExpression = expression.trimmed();
    if (trimmedExpression.isEmpty()) {
        errorMessage = QObject::tr("不能为空");
        return false;
    }

    QJSEngine engine;
    for (auto it = parameterValues.constBegin(); it != parameterValues.constEnd(); ++it) {
        engine.globalObject().setProperty(it.key(), it.value());
    }

    QJSValue jsResult = engine.evaluate(trimmedExpression);
    if (jsResult.isError()) {
        errorMessage = jsResult.toString();
        return false;
    }
    if (!jsResult.isNumber()) {
        errorMessage = QObject::tr("结果不是有效数字");
        return false;
    }

    result = jsResult.toNumber();
    if (!std::isfinite(result)) {
        errorMessage = QObject::tr("结果不是有限数字");
        return false;
    }
    return true;
}

BoxParamsDialog::BoxParamsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Box Parameters"));

    QFormLayout *formLayout = new QFormLayout(this);

    // 名称输入
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Enter box name"));
    formLayout->addRow(tr("Name:"), m_nameEdit);

    // 坐标范围输入
    m_xminEdit = new QLineEdit(this);
    m_xminEdit->setPlaceholderText(tr("Minimum X / expression"));
    formLayout->addRow(tr("X min:"), m_xminEdit);

    m_xmaxEdit = new QLineEdit(this);
    m_xmaxEdit->setPlaceholderText(tr("Maximum X / expression"));
    formLayout->addRow(tr("X max:"), m_xmaxEdit);

    m_yminEdit = new QLineEdit(this);
    m_yminEdit->setPlaceholderText(tr("Minimum Y / expression"));
    formLayout->addRow(tr("Y min:"), m_yminEdit);

    m_ymaxEdit = new QLineEdit(this);
    m_ymaxEdit->setPlaceholderText(tr("Maximum Y / expression"));
    formLayout->addRow(tr("Y max:"), m_ymaxEdit);

    m_zminEdit = new QLineEdit(this);
    m_zminEdit->setPlaceholderText(tr("Minimum Z / expression"));
    formLayout->addRow(tr("Z min:"), m_zminEdit);

    m_zmaxEdit = new QLineEdit(this);
    m_zmaxEdit->setPlaceholderText(tr("Maximum Z / expression"));
    formLayout->addRow(tr("Z max:"), m_zmaxEdit);

    // 材料选择
    m_materialCombo = new QComboBox(this);
    m_materialCombo->addItem(tr("Metal"), METAL);
    m_materialCombo->addItem(tr("Dielectric 1"), DIELECTRIC1);
    m_materialCombo->addItem(tr("Dielectric 2"), DIELECTRIC2);
    m_materialCombo->addItem(tr("Dielectric 3"), DIELECTRIC3);
    m_materialCombo->addItem(tr("Dielectric 4"), DIELECTRIC4);
    m_materialCombo->addItem(tr("Dielectric 5"), DIELECTRIC5);
    m_materialCombo->addItem(tr("Vacuum"), VACUUM);
    formLayout->addRow(tr("Material:"), m_materialCombo);

    // 按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    formLayout->addRow(buttonBox);
}

DrawModel::BoxExpressionInput BoxParamsDialog::getInput() const
{
    DrawModel::BoxExpressionInput input;
    input.name = m_nameEdit->text();
    input.xminExpr = m_xminEdit->text();
    input.xmaxExpr = m_xmaxEdit->text();
    input.yminExpr = m_yminEdit->text();
    input.ymaxExpr = m_ymaxEdit->text();
    input.zminExpr = m_zminEdit->text();
    input.zmaxExpr = m_zmaxEdit->text();
    input.materialType = static_cast<MaterialType>(m_materialCombo->currentData().toInt());
    return input;
}

ParameterValuesDialog::ParameterValuesDialog(const QStringList &parameterNames, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Parameter Values"));

    QFormLayout *formLayout = new QFormLayout(this);
    QLabel *hintLabel = new QLabel(tr("Detected parameters. Please assign a numeric value for each one."), this);
    hintLabel->setWordWrap(true);
    formLayout->addRow(hintLabel);

    for (const QString &parameterName : parameterNames) {
        QLineEdit *valueEdit = new QLineEdit(this);
        valueEdit->setPlaceholderText(tr("Numeric value"));
        formLayout->addRow(parameterName + ":", valueEdit);
        m_valueEdits.insert(parameterName, valueEdit);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    formLayout->addRow(buttonBox);
}

QMap<QString, double> ParameterValuesDialog::getParameterValues(bool &ok, QString &errorMessage) const
{
    ok = false;
    QMap<QString, double> values;

    for (auto it = m_valueEdits.constBegin(); it != m_valueEdits.constEnd(); ++it) {
        bool valueOk = false;
        const QString text = it.value()->text().trimmed();
        const double value = text.toDouble(&valueOk);
        if (!valueOk) {
            errorMessage = tr("参数 %1 缺失或不是有效数字。").arg(it.key());
            return {};
        }
        values.insert(it.key(), value);
    }

    ok = true;
    return values;
}
