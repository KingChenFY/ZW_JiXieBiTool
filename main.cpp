#include "frmmain.h"

#include <QApplication>
#include <QFile>
#include "quihelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qss_file(":/style.qss");
    qss_file.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss_file.readAll());
    qss_file.close();

    FrmMain w;
    w.setWindowTitle("JXB硬件调试平台 (Author: Yisea)");
    QUIHelper::setFormInCenter(&w);
    w.show();
    return a.exec();
}
