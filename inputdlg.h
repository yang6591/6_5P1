#ifndef INPUTDLG_H
#define INPUTDLG_H
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class InputDlg : public QDialog {
    Q_OBJECT
public:
    InputDlg(QWidget* parent = 0);
    QString createMultipleFolders(const QString path);
    QString createTxtFile(const QString path, const QString suffix);
private slots:
    void changeFreminvalue();
    void changeFremaxvalue();
    void changeDimensions();
    void changeTimeUnits();
    void changeFreUnits();
    void ExportTXTFile();

private:
    // 频率标签
    QLabel* FreMinshow;
    QLabel* FreMinshowtemp;
    QLabel* FreMaxshow;
    QLabel* FreMaxshowtemp;
    QPushButton* minvalueBtn;
    QPushButton* maxvalueBtn;
    QGridLayout* mainLayoutvalue;
    // 尺度标签
    QLabel* DimLabelshow;
    QLabel* DimLabelshowtemp;
    // QLabel *DimLabelm;
    // QLabel *DimLabelcm;
    // QLabel *DimLabelmm;
    // QLabel *DimLabelum;
    // QLabel *DimLabelnm;
    QPushButton* DimBtnok;
    // QPushButton * DimBtnno;
    // 时间标签
    QLabel* TimLabelshow;
    QLabel* TimLabelshowtemp;
    QPushButton* TimBtnok;
    // 频率标签
    QLabel* FreUnitshow;
    QLabel* FreUnitshowtemp;
    QPushButton* FreBtnok;
};

#endif // INPUTDLG_H
