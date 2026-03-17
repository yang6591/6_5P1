#include "mainwindow.h"
#include <QApplication>


double g_freMin = 0.0;  // 初始化一个默认值
double g_freMax = 0.0;
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();

    return a.exec();
}
