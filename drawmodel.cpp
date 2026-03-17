#include "drawmodel.h"
#include <QMessageBox>

DrawModel::BoxParams DrawModel::getBoxParams(QWidget *parent)
{
    BoxParamsDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.getParams();
    }
    return BoxParams(); // 返回空参数
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
    m_xminEdit->setPlaceholderText(tr("Minimum X"));
    formLayout->addRow(tr("X min:"), m_xminEdit);

    m_xmaxEdit = new QLineEdit(this);
    m_xmaxEdit->setPlaceholderText(tr("Maximum X"));
    formLayout->addRow(tr("X max:"), m_xmaxEdit);

    m_yminEdit = new QLineEdit(this);
    m_yminEdit->setPlaceholderText(tr("Minimum Y"));
    formLayout->addRow(tr("Y min:"), m_yminEdit);

    m_ymaxEdit = new QLineEdit(this);
    m_ymaxEdit->setPlaceholderText(tr("Maximum Y"));
    formLayout->addRow(tr("Y max:"), m_ymaxEdit);

    m_zminEdit = new QLineEdit(this);
    m_zminEdit->setPlaceholderText(tr("Minimum Z"));
    formLayout->addRow(tr("Z min:"), m_zminEdit);

    m_zmaxEdit = new QLineEdit(this);
    m_zmaxEdit->setPlaceholderText(tr("Maximum Z"));
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

DrawModel::BoxParams BoxParamsDialog::getParams() const
{
    DrawModel::BoxParams params;
    params.name = m_nameEdit->text();
    params.xmin = m_xminEdit->text().toFloat();
    params.xmax = m_xmaxEdit->text().toFloat();
    params.ymin = m_yminEdit->text().toFloat();
    params.ymax = m_ymaxEdit->text().toFloat();
    params.zmin = m_zminEdit->text().toFloat();
    params.zmax = m_zmaxEdit->text().toFloat();
    params.materialType = static_cast<MaterialType>(m_materialCombo->currentData().toInt());
    return params;
}
//改进：
//输入验证：当前实现缺少输入验证，用户可能输入无效值（如非数字、最小值大于最大值等）
//默认值：可以为输入框提供合理的默认值
//单位提示：添加单位说明（如米、厘米等）
//材料颜色映射：可以将材料类型与颜色关联，便于在主程序中设置模型颜色
