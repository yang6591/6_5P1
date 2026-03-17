#ifndef MESHSHOW_H
#define MESHSHOW_H
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QToolBar>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPointF>
#include <QVector>
#include <QColor>
#include <QStringList>
#include <QSet>
#include <QPainter>
#include <QStyle>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QToolBar>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QDir>


class MeshShow : public QMainWindow
{
    Q_OBJECT

public:
    MeshShow(QWidget *parent = nullptr);
    ~MeshShow();

private slots:
    void onXYButtonClicked();
    void onXZButtonClicked();
    void onYZButtonClicked();
    void onUpdateClicked();
    void onResetClicked();

public slots:
    void enableMouseEvents();  // 启用鼠标事件
    void disableMouseEvents(); // 禁用鼠标事件


protected:
    bool readGridFile(const QString& filename, QVector<double>& lines);
    bool readColorFile(const QString& filename, QVector<QVector<QVector<int>>>& meshcolor);
    bool readmaterial(const QString& filename, QStringList &materiallabels, QVector<QColor> &materialcolors);
    int findIndex(double locationp, const QVector<double>& XYZlines);
    void paintEvent(QPaintEvent *event) override;
    void updateSizeLabels();
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QButtonGroup *planeGroup;
    QPushButton *xyButton;
    QPushButton *xzButton;
    QPushButton *yzButton;
    QString currentPlane;
    QPushButton *updateButton;
    int currentLayerIndex;
    double locationP;
    QDoubleSpinBox *layerSpinBox;
    QDoubleSpinBox *scaleSpinBox;
    QLabel *xSizeLabel;
    QLabel *ySizeLabel;
    QLabel *zSizeLabel;
    QLabel *currentCoordLabel;
    int xOffset, yOffset;
    int imageWidth, imageHeight;
    double xmin_ori, xmax_ori, ymin_ori, ymax_ori;
    double xSize, ySize, zSize;
    int scaledImageWidth;
    int scaledImageHeight;
    QPoint dragStartPos;
    QPointF totalOffset;
    bool isDragging;
    QPushButton* resetButton;
    bool isViewOffset;
    QVector<double> Xlines;
    QVector<double> Ylines;
    QVector<double> Zlines;
    QVector<QVector<QVector<int>>> meshcolor;
    QSet<int> existingMaterials;
    QStringList legendLabels;
    QVector<QColor> colors;
    double scaleFactor;
    bool mouseEventsEnabled = false;

};
#endif // MESHSHOW_H
