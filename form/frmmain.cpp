#include "frmmain.h"
#include "ui_frmmain.h"
#include "formctrl.h"
#include "global.h"

FrmMain::FrmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

FrmMain::~FrmMain()
{
    delete ui;
}

void FrmMain::initForm()
{
    slot_netNoLink();
    ui->tabWidget->addTab(new FormCtrl, "控制窗口");
    //启动日志功能
    SaveLog::Instance()->start();
}

void FrmMain::initConfig()
{
    qDebug() << "frmmain thread:" << QThread::currentThread();

    //实例化网络类
    ctrlClient = new CtrlTcpClient;
    //生成网络线程
    ctrlClient->moveToThread(&ctrlNetThread);
    //关联网络线程的启动信号
    connect(&ctrlNetThread, &QThread::started, ctrlClient, &CtrlTcpClient::slot_initCtrlClient);
    qDebug() << "start net thread";

    //实例化数据管理类
    actnDataManage = new ActionDataManage;
    //生成数据管理线程
    actnDataManage->moveToThread(&dataManageThread);
    //关联数据管理线程的启动信号
    connect(&dataManageThread, &QThread::started, actnDataManage, &ActionDataManage::slot_initDataMange);
    qDebug() << "start data manage thread";


    //关联这两个类的信号和槽
    connect(ctrlClient, &CtrlTcpClient::signal_netRecvData, actnDataManage, &ActionDataManage::slot_netRecvData, Qt::QueuedConnection);
    connect(ctrlClient, &CtrlTcpClient::signal_netConnected, this, &FrmMain::slot_netConnected);
    connect(ctrlClient, &CtrlTcpClient::signal_netNoLink, this, &FrmMain::slot_netNoLink);

    //启动线程 ,
    dataManageThread.start();
    ctrlNetThread.start();
}

void FrmMain::slot_netConnected()
{
    ui->tabWidget->setEnabled(true);
}

void FrmMain::slot_netNoLink()
{
    ui->tabWidget->setDisabled(true);
}
