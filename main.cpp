#include "frmmain.h"

#include <QApplication>
#include <QFile>
#include "quihelper.h"
#include "appconfig.h"
#include "savelog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qss_file(":/style.qss");
    qss_file.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss_file.readAll());
    qss_file.close();

    //启动日志功能
    SaveLog::Instance()->start();
    SaveLog::Instance()->setMaxSize(10240);

    FrmMain w;
    w.setWindowTitle("JXB硬件调试平台 (Author: Yisea)");
    w.setFixedSize(1000, 640);
    w.setWindowFlags(w.windowFlags() &~ Qt::WindowMaximizeButtonHint);


    QUIHelper::setFormInCenter(&w);
    w.show();
    return a.exec();
}
