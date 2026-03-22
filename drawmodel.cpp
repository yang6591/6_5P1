#include "drawmodel.h"
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>

namespace {
const QRegularExpression kParameterRegex(QStringLiteral("(?<![0-9.])[A-Za-z_][A-Za-z0-9_]*\b"));

class ExpressionParser
{
public:
    ExpressionParser(const QString &expression,
                     const QMap<QString, double> &parameterValues,
                     QString &errorMessage)
        : m_expression(expression)
        , m_parameterValues(parameterValues)
        , m_errorMessage(errorMessage)
    {
    }

    bool parse(double &result)
    {
        m_pos = 0;
        if (!parseExpression(result)) {
            return false;
        }
        skipSpaces();
        if (m_pos != m_expression.size()) {
            m_errorMessage = QObject::tr("存在无法识别的字符：%1").arg(m_expression.mid(m_pos));
            return false;
        }
        return true;
    }

private:
    bool parseExpression(double &result)
    {
        if (!parseTerm(result)) {
            return false;
        }

        while (true) {
            skipSpaces();
            if (match('+')) {
                double rhs = 0.0;
                if (!parseTerm(rhs)) {
                    return false;
                }
                result += rhs;
            } else if (match('-')) {
                double rhs = 0.0;
                if (!parseTerm(rhs)) {
                    return false;
                }
                result -= rhs;
            } else {
                break;
            }
        }
        return true;
    }

    bool parseTerm(double &result)
    {
        if (!parseFactor(result)) {
            return false;
        }

        while (true) {
            skipSpaces();
            if (match('*')) {
                double rhs = 0.0;
                if (!parseFactor(rhs)) {
                    return false;
                }
                result *= rhs;
            } else if (match('/')) {
                double rhs = 0.0;
                if (!parseFactor(rhs)) {
                    return false;
                }
                if (qFuzzyIsNull(rhs)) {
                    m_errorMessage = QObject::tr("除数不能为 0");
                    return false;
                }
                result /= rhs;
            } else {
                break;
            }
        }
        return true;
    }

    bool parseFactor(double &result)
    {
        skipSpaces();
        if (match('+')) {
            return parseFactor(result);
        }
        if (match('-')) {
            if (!parseFactor(result)) {
                return false;
            }
            result = -result;
            return true;
        }
        if (match('(')) {
            if (!parseExpression(result)) {
                return false;
            }
            skipSpaces();
            if (!match(')')) {
                m_errorMessage = QObject::tr("缺少右括号 )");
                return false;
            }
            return true;
        }
        if (parseNumber(result)) {
            return true;
        }
        if (parseIdentifier(result)) {
            return true;
        }

        m_errorMessage = QObject::tr("表达式语法错误");
        return false;
    }

    bool parseNumber(double &result)
    {
        skipSpaces();
        int start = m_pos;
        bool hasDigit = false;
        bool hasDot = false;

        while (m_pos < m_expression.size()) {
            const QChar ch = m_expression.at(m_pos);
            if (ch.isDigit()) {
                hasDigit = true;
                ++m_pos;
            } else if (ch == '.' && !hasDot) {
                hasDot = true;
                ++m_pos;
            } else {
                break;
            }
        }

        if (!hasDigit) {
            m_pos = start;
            return false;
        }

        bool ok = false;
        const QString numberText = m_expression.mid(start, m_pos - start);
        result = numberText.toDouble(&ok);
        if (!ok) {
            m_errorMessage = QObject::tr("数字无效：%1").arg(numberText);
            return false;
        }
        return true;
    }

    bool parseIdentifier(double &result)
    {
        skipSpaces();
        if (m_pos >= m_expression.size()) {
            return false;
        }

        const QChar first = m_expression.at(m_pos);
        if (!(first.isLetter() || first == '_')) {
            return false;
        }

        const int start = m_pos;
        ++m_pos;
        while (m_pos < m_expression.size()) {
            const QChar ch = m_expression.at(m_pos);
            if (ch.isLetterOrNumber() || ch == '_') {
                ++m_pos;
            } else {
                break;
            }
        }

        const QString identifier = m_expression.mid(start, m_pos - start);
        if (!m_parameterValues.contains(identifier)) {
            m_errorMessage = QObject::tr("缺少参数：%1").arg(identifier);
            return false;
        }

        result = m_parameterValues.value(identifier);
        return true;
    }

    bool match(QChar expected)
    {
        skipSpaces();
        if (m_pos < m_expression.size() && m_expression.at(m_pos) == expected) {
            ++m_pos;
            return true;
        }
        return false;
    }

    void skipSpaces()
    {
        while (m_pos < m_expression.size() && m_expression.at(m_pos).isSpace()) {
            ++m_pos;
        }
    }

    QString m_expression;
    const QMap<QString, double> &m_parameterValues;
    QString &m_errorMessage;
    int m_pos = 0;
};
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

    ExpressionParser parser(trimmedExpression, parameterValues, errorMessage);
    if (!parser.parse(result)) {
        return false;
    }
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

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Enter box name"));
    formLayout->addRow(tr("Name:"), m_nameEdit);

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

    m_materialCombo = new QComboBox(this);
    m_materialCombo->addItem(tr("Metal"), METAL);
    m_materialCombo->addItem(tr("Dielectric 1"), DIELECTRIC1);
    m_materialCombo->addItem(tr("Dielectric 2"), DIELECTRIC2);
    m_materialCombo->addItem(tr("Dielectric 3"), DIELECTRIC3);
    m_materialCombo->addItem(tr("Dielectric 4"), DIELECTRIC4);
    m_materialCombo->addItem(tr("Dielectric 5"), DIELECTRIC5);
    m_materialCombo->addItem(tr("Vacuum"), VACUUM);
    formLayout->addRow(tr("Material:"), m_materialCombo);

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
