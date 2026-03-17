#include "inputdlg.h"
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <QTextStream>

// 全局变量，设定的最大，最小频率值
// 在主界面只用声明全局变量，在引用界面声明额外变量extern即可
extern double Freminvalue;
extern double Fremaxvalue;

// 索引序号
extern int DimUnitIndex;
extern int FreUnitIndex;
extern int TimUnitIndex;
// 列表选项
extern QComboBox* DimOptions;
extern QComboBox* FreOptions;
extern QComboBox* TimeOptions;

QString ExpPathLine;
QLineEdit* ExpFileShowName;
QString createTime;
QString fileName;
QString Freminvaluetemp1; // Fre.min value
QString Fremaxvaluetemp1; // Fre.max value

// Units symbol
QString Dimshowtemp;
QString Freshowtemp;
QString Timshowtemp;

// 显示home -> have done中的内容
InputDlg::InputDlg(QWidget* parent)
    : QDialog(parent)
{
    // 设置频率范围
    setWindowTitle(tr("Parameters that have been set"));

    FreMinshow = new QLabel;
    FreMinshow->setText(tr("Min.frequency:"));
    FreMinshowtemp = new QLabel;
    Freminvaluetemp1 = QString::number(Freminvalue); // extern double Freminvalue;//将double值转化为Qstring的值    //Freminvalue，Fremaxvalue;是频率选项菜单设定的频率值
    FreMinshowtemp->setText(Freminvaluetemp1); // 将Freminvalue的值显示到FreMinshowtemp中去
    FreMinshowtemp->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    FreMaxshow = new QLabel;
    FreMaxshow->setText(tr("Max.frequency:"));
    FreMaxshowtemp = new QLabel;
    Fremaxvaluetemp1 = QString::number(Fremaxvalue);
    FreMaxshowtemp->setText(Fremaxvaluetemp1);
    FreMaxshowtemp->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    minvalueBtn = new QPushButton;
    minvalueBtn->setText(tr("Change Min.Value"));
    maxvalueBtn = new QPushButton;
    maxvalueBtn->setText(tr("Change Max.Value"));

    QLabel* Freshow = new QLabel;
    Freshow->setText(tr("Frequency:"));
    mainLayoutvalue = new QGridLayout(this);
    mainLayoutvalue->addWidget(Freshow, 0, 0);
    mainLayoutvalue->addWidget(FreMinshow, 1, 0);
    mainLayoutvalue->addWidget(FreMinshowtemp, 1, 1);
    mainLayoutvalue->addWidget(minvalueBtn, 1, 2);

    mainLayoutvalue->addWidget(FreMaxshow, 2, 0);
    mainLayoutvalue->addWidget(FreMaxshowtemp, 2, 1);
    mainLayoutvalue->addWidget(maxvalueBtn, 2, 2);
    mainLayoutvalue->setSpacing(10);

    connect(minvalueBtn, SIGNAL(clicked()), this, SLOT(changeFreminvalue()));
    connect(maxvalueBtn, SIGNAL(clicked()), this, SLOT(changeFremaxvalue()));

    // 设置尺度
    setWindowTitle(tr("Units"));
    DimLabelshow = new QLabel;
    DimLabelshow->setText(tr("Dimensions:"));

    // DimOptions = new QComboBox;
    DimOptions->setCurrentIndex(DimUnitIndex); // 将Units中的单位传递到此处
    Dimshowtemp = DimOptions->currentText();

    DimLabelshowtemp = new QLabel; // 显示标签
    DimLabelshowtemp->setText(Dimshowtemp);
    DimLabelshowtemp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    DimBtnok = new QPushButton;
    DimBtnok->setText(tr("Change Dim.Unit"));

    QLabel* UnitsTextshow = new QLabel;
    UnitsTextshow->setText(tr("Units"));
    mainLayoutvalue->addWidget(UnitsTextshow, 4, 0);
    mainLayoutvalue->addWidget(DimLabelshow, 5, 0);
    mainLayoutvalue->addWidget(DimLabelshowtemp, 5, 1);
    mainLayoutvalue->addWidget(DimBtnok, 5, 2);
    connect(DimBtnok, SIGNAL(clicked()), this, SLOT(changeDimensions()));

    // 设置频率
    FreUnitshow = new QLabel;
    FreUnitshow->setText(tr("Frequency:"));

    FreOptions->setCurrentIndex(FreUnitIndex); // 将Units中的单位传递到此处
    Freshowtemp = FreOptions->currentText();

    FreUnitshowtemp = new QLabel;
    FreUnitshowtemp->setText(Freshowtemp);
    FreUnitshowtemp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    FreBtnok = new QPushButton;
    FreBtnok->setText(tr("Change Fre.Units"));

    mainLayoutvalue->addWidget(FreUnitshow, 7, 0);
    mainLayoutvalue->addWidget(FreUnitshowtemp, 7, 1);
    mainLayoutvalue->addWidget(FreBtnok, 7, 2);
    connect(FreBtnok, SIGNAL(clicked()), this, SLOT(changeFreUnits()));

    // 设置时间
    setWindowTitle(tr("Time"));
    TimLabelshow = new QLabel;
    TimLabelshow->setText(tr("Time:"));

    TimeOptions->setCurrentIndex(TimUnitIndex); // 将Units中的单位传递到此处
    Timshowtemp = TimeOptions->currentText();

    TimLabelshowtemp = new QLabel;
    TimLabelshowtemp->setText(Timshowtemp);
    TimLabelshowtemp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    TimBtnok = new QPushButton;
    TimBtnok->setText(tr("Change Time.Units"));

    mainLayoutvalue->addWidget(TimLabelshow, 8, 0);
    mainLayoutvalue->addWidget(TimLabelshowtemp, 8, 1);
    mainLayoutvalue->addWidget(TimBtnok, 8, 2);
    connect(TimBtnok, SIGNAL(clicked()), this, SLOT(changeTimeUnits()));

    // 输出txt文件
    QLabel* Exporttxt = new QLabel;
    Exporttxt->setText(tr("Export Files(.txt)"));
    QPushButton* ExportBtn = new QPushButton;
    ExportBtn->setText(tr("Export"));
    mainLayoutvalue->addWidget(Exporttxt, 9, 0);
    mainLayoutvalue->addWidget(ExportBtn, 9, 2);

    connect(ExportBtn, SIGNAL(clicked()), this, SLOT(ExportTXTFile()));

    // 创建文件夹和文件
    // 如果放在这，则点开Have Done就会自动创建文件
    // QString pathName = createMultipleFolders("D:\\Qt");
    // QString pathTxtFile = createTxtFile(pathName,".txt");

    // connect(ExpFileOKBtn,SIGNAL(clicked()),this,SLOT(pathTxtFile()));
}

void InputDlg::changeFreminvalue()
{
    bool ok;
    double newFreminvalue = QInputDialog::getDouble(this, tr("Change Value"), tr("Enter a New Value"),
        FreMinshowtemp->text().toDouble(&ok), 0, 100, 6, &ok);
    if (ok) {
        FreMinshowtemp->setText(QString(tr("%6")).arg(newFreminvalue));
        Freminvalue = newFreminvalue;
    }
}

void InputDlg::changeFremaxvalue()
{
    bool ok;
    double newFremaxvalue = QInputDialog::getDouble(this, tr("Change Value"), tr("Enter a New Value"),
        FreMaxshowtemp->text().toDouble(&ok), 0, 100, 6, &ok);
    if (ok) {
        if (newFremaxvalue > Freminvalue) {
            FreMaxshowtemp->setText(QString(tr("%6")).arg(newFremaxvalue));
            Fremaxvalue = newFremaxvalue;
        } else {
            QMessageBox::critical(this, tr("Critical"), tr("The maximum value must be larger than the minmum value!"));
            newFremaxvalue = Freminvalue;
            FreMaxshowtemp->setText(QString(tr("%6")).arg(newFremaxvalue));
        }
    }
}

void InputDlg::changeDimensions()
{
    bool ok;
    QStringList DimChangeOptions;
    DimChangeOptions << tr("m") << tr("cm") << tr("mm") << tr("um") << tr("nm") << tr("ft");
    QString DimensionsOption = QInputDialog::getItem(this, tr("Unit-Dimensions"), tr("Unit selection"), DimChangeOptions, 0, false, &ok);
    QString JugDimNum;
    if (ok && !DimensionsOption.isEmpty()) {
        DimLabelshowtemp->setText(DimensionsOption);
        for (int DimNum = 0; DimNum < 6; DimNum++) {
            DimOptions->setCurrentIndex(DimNum);
            JugDimNum = DimOptions->currentText();
            if (JugDimNum == DimensionsOption) {
                DimUnitIndex = DimNum;
            }
        }
    }
}

void InputDlg::changeTimeUnits()
{
    bool ok;
    QStringList TimChangeOptions;
    TimChangeOptions << tr("s") << tr("ms") << tr("us") << tr("ns") << tr("ps") << tr("fs");
    QString TimeOption = QInputDialog::getItem(this, tr("Unit-Time"), tr("Unit selection"), TimChangeOptions, 0, false, &ok);
    QString JugTimeNum;
    if (ok && !TimeOption.isEmpty()) {
        TimLabelshowtemp->setText(TimeOption);
        for (int TimeNum = 0; TimeNum < 6; TimeNum++) {
            TimeOptions->setCurrentIndex(TimeNum);
            JugTimeNum = TimeOptions->currentText();
            if (JugTimeNum == TimeOption) {
                TimUnitIndex = TimeNum;
            }
        }
    }
}
void InputDlg::changeFreUnits()
{
    bool ok;
    QStringList FreChangeOptions;
    FreChangeOptions << tr("Hz") << tr("KHz") << tr("MHz") << tr("GHz") << tr("THz") << tr("PHz");
    QString FrequencyOption = QInputDialog::getItem(this, tr("Unit-Frequency"), tr("Unit selection"), FreChangeOptions, 0, false, &ok);
    QString JugFreNum;
    if (ok && !FrequencyOption.isEmpty()) {
        FreUnitshowtemp->setText(FrequencyOption);
        for (int FreNum = 0; FreNum < 6; FreNum++) {
            FreOptions->setCurrentIndex(FreNum);
            JugFreNum = FreOptions->currentText();
            if (JugFreNum == FrequencyOption) {
                TimUnitIndex = FreNum;
            }
        }
    }
}

void InputDlg::ExportTXTFile()
{
    QDialog* ExpPathDlg = new QDialog;
    ExpPathDlg->setWindowTitle(QString::fromUtf8("Choose Path"));
    ExpPathDlg->resize(500, 100);

    QPushButton* ExpFileChBtn = new QPushButton(ExpPathDlg);
    ExpFileChBtn->setText(tr("Choose"));
    QPushButton* ExpFileNameBtn = new QPushButton(ExpPathDlg);
    ExpFileNameBtn->setText(tr("ok"));
    QPushButton* ExpFileOKBtn = new QPushButton(ExpPathDlg);
    ExpFileOKBtn->setText(tr("Create"));
    QPushButton* ExpFileNOBtn = new QPushButton(ExpPathDlg);
    ExpFileNOBtn->setText(tr("Cancel"));

    // 设置路径读取窗口
    QLabel* ExpPathName = new QLabel;
    ExpPathName->setText(tr("Export Path:"));
    QLineEdit* ExpPathShow = new QLineEdit(ExpPathDlg);
    ExpPathShow->setReadOnly(true); // 设置权限：只读

    QLabel* ExpFileName = new QLabel;
    ExpFileName->setText(tr("File Name:"));
    ExpFileShowName = new QLineEdit(ExpPathDlg);

    QGridLayout* ExpFileLayout = new QGridLayout(ExpPathDlg);
    ExpFileLayout->addWidget(ExpPathName, 0, 0);
    ExpFileLayout->addWidget(ExpPathShow, 1, 0);
    ExpFileLayout->addWidget(ExpFileChBtn, 1, 2);
    ExpFileLayout->addWidget(ExpFileName, 2, 0);
    ExpFileLayout->addWidget(ExpFileShowName, 3, 0);
    ExpFileLayout->addWidget(ExpFileNameBtn, 3, 2);
    ExpFileLayout->addWidget(ExpFileOKBtn, 4, 2);
    ExpFileLayout->addWidget(ExpFileNOBtn, 4, 3);

    /***************创建文件***************/
    connect(ExpFileChBtn, &QPushButton::clicked, [=]() {
        // ExpPathLine = QFileDialog::getOpenFileName(this,"Export File","C:\\Users\\Li\\Desktop");
        ExpPathLine = QFileDialog::getExistingDirectory(this, "Export File", "C:\\Users\\Li\\Desktop");
        ExpPathShow->setText(ExpPathLine);
    });

    connect(ExpFileNameBtn, &QPushButton::clicked, [=]() {
        createTime = ExpFileShowName->text(); // 文件名不能用“:”命名
        qDebug() << createTime;
    });

    connect(ExpFileOKBtn, &QPushButton::clicked, [=]() {
        QString pathName = ExpPathLine;
        QString pathTxtFile = createTxtFile(pathName, ".txt");
        /***************向文件中写入数据***************/

        QFile MakeTxtFile(fileName);
        if (!MakeTxtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "打开文件失败，请重新检查？";
        } else {
            qDebug() << "恭喜你，打开文件成功！";
        }
        QTextStream TxtStreamWrite(&MakeTxtFile);
        QString WriteFileConcept = "The Data and Units you have set";
//        TxtStreamWrite << WriteFileConcept << Qt::endl;
//        TxtStreamWrite << "=======================" << Qt::endl;
//        TxtStreamWrite << "The Frequency Value(min && max):" << Qt::endl;
//        TxtStreamWrite << "Fre.min Value:" << Freminvaluetemp1 << Qt::endl;
//        TxtStreamWrite << "Fre.max Value:" << Fremaxvaluetemp1 << Qt::endl;
//        TxtStreamWrite << Qt::endl;
//        TxtStreamWrite << Qt::endl;
//        TxtStreamWrite << "The unit symbol that has been set" << Qt::endl;
//        TxtStreamWrite << "=======================" << Qt::endl;
//        TxtStreamWrite << "The Dimension symbol:"
//                       << "      " << Dimshowtemp << Qt::endl;
//        TxtStreamWrite << "The Time symbol:"
//                       << "              " << Freshowtemp << Qt::endl;
//        TxtStreamWrite << "The Frequency symbol:"
//                       << "      " << Timshowtemp << Qt::endl;

//        MakeTxtFile.close();
    });

    ExpPathDlg->exec();
    ExpPathDlg->setAttribute(Qt::WA_DeleteOnClose);
}

QString InputDlg::createMultipleFolders(const QString pathName)
{

    QDir dir(pathName); // QDir类提供了访问系统目录结构及其内容的与平台无关的方式
    if (dir.exists(pathName)) // 判断路径是否存在，如果存在则返回路径值
    {
        return pathName;
    }

    QString parentDir = createMultipleFolders(pathName.mid(0, pathName.lastIndexOf('\\'))); // 截取根目录
    // QString.mid意思是获取指定位置的子串
    // QString QString::mid(int position, int n) ：：从position开始，n为截取长度
    // lastIndexOf 是从后往前数的？？？？
    QString dirName = pathName.mid(pathName.lastIndexOf('\\') + 1); // 截取父目录
    // 理解 以上两句
    QDir parentPath(parentDir);
    if (!dirName.isEmpty()) {
        parentPath.mkpath(dirName);
    }
    return parentDir + "\\" + dirName;
}

QString InputDlg::createTxtFile(const QString pathName, const QString suffix)
{
    // QString createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");//文件名不能用“:”命名

    // connect(ExpFileNameBtn,&QPushButton::clicked,[=](){
    // QString createTime = ExpFileShowName -> text();//文件名不能用“:”命名
    // qDebug() << createTime;
    // });
    fileName = pathName + "\\" + createTime + suffix;
    // QString fileName = pathName + "\\" + suffix;
    QFile file(fileName);
    if (file.exists()) {
        return fileName;
    } else {
        // 如果没有此文件，就创建
        file.open(QFile::WriteOnly | QFile::Text | QIODevice::Append);
    }
    return fileName;
}
