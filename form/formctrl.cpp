#include "formctrl.h"
#include "ui_formctrl.h"
#include <QMetaEnum>
#include "quihelper.h"
#include "global.h"

FormCtrl::FormCtrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCtrl)
{
    ui->setupUi(this);
    initForm();
    initConfig();
}

FormCtrl::~FormCtrl()
{
    delete ui;
}

void FormCtrl::initForm()
{
    ui->ledit_moveX->setEnabled(false);
    ui->ledit_moveY->setEnabled(false);
    ui->ledit_moveZ->setEnabled(false);
}

void FormCtrl::initConfig()
{
    emTaskStatus = QMetaEnum::fromType<FormCtrl::E_TASK_STATUS>();
    m_pActionBeltIn = new ActionBeltIn;
    m_pActionBeltOut = new ActionBeltOut;
    m_pActionPoleIn = new ActionPoleIn;
    m_pActionPoleOut = new ActionPoleOut;
    m_pActionUpender = new ActionUpender;
    m_pActionMotorV = new ActionMotorV;
    m_pActionMotorXYZ = new ActionMotorXYZ;
    connect(m_pActionBeltIn, &ActionBeltIn::signal_UiUpdate, this, &FormCtrl::slot_BeltIN_UiUpdate);
    connect(m_pActionBeltOut, &ActionBeltOut::signal_UiUpdate, this, &FormCtrl::slot_BeltOUT_UiUpdate);
    connect(m_pActionPoleIn, &ActionPoleIn::signal_UiUpdate, this, &FormCtrl::slot_PoleIN_UiUpdate);
    connect(m_pActionPoleOut, &ActionPoleOut::signal_UiUpdate, this, &FormCtrl::slot_PoleOUT_UiUpdate);
    connect(m_pActionUpender, &ActionUpender::signal_UiUpdate, this, &FormCtrl::slot_Upender_UiUpdate);
    connect(m_pActionMotorV, &ActionMotorV::signal_UiUpdate, this, &FormCtrl::slot_MotorV_UiUpdate);
    connect(m_pActionMotorXYZ, &ActionMotorXYZ::signal_UiUpdate, this, &FormCtrl::slot_MotorXYZ_UiUpdate);

    m_pActionBTransport = new ActionBTransport(m_pActionBeltIn, m_pActionBeltOut, m_pActionPoleIn,
                                               m_pActionPoleOut, m_pActionUpender, m_pActionMotorV);
    connect(m_pActionBTransport, &QThread::finished, this, &FormCtrl::slot_TransportTestThread_Stop);
}

/*
 * ******************************************************运输仓测试流程******************************************************
 */
void FormCtrl::on_pbtn_transportTestStart_clicked()
{
    if("开始运输仓测试" == ui->pbtn_transportTestStart->text())
    {
        ui->page_transport->setDisabled(true);
        m_pActionBTransport->m_bNeedStop = false;
        _LOG("{Trans_Auto_Test}: THREAD START =================================");
        m_pActionBTransport->start();
        ui->pbtn_transportTestStart->setText("停止运输仓测试");
    }
    else if("停止运输仓测试" == ui->pbtn_transportTestStart->text())
    {
        m_pActionBTransport->m_bNeedStop = true;
        _LOG("{Trans_Auto_Test}: STOP THREAD =================================");
        ui->pbtn_transportTestStart->setText("正在停止运输仓测试");
    }
    else
    {
        return;
    }
}

void FormCtrl::slot_TransportTestThread_Stop()
{
    on_rbtn_BIstop_clicked();
    on_rbtn_BOstop_clicked();
    on_rbtn_PIstop_clicked();
    on_rbtn_POstop_clicked();
    on_pbtn_UPstop_clicked();
    on_pbtn_MVstop_clicked();
    ui->page_transport->setEnabled(true);
    _LOG("{Trans_Auto_Test}: THREAD FINISH END=================================");
    ui->pbtn_transportTestStart->setText("开始运输仓测试");
}
/*
 * ******************************************************UI******************************************************
 */
void FormCtrl::slot_BeltIN_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emBeltSelute = QMetaEnum::fromType<ActionBeltIn::E_SELUTE_BELTIN>();
    ui->lab_BIcolor->setText(QString(emBeltSelute.valueToKey(m_pActionBeltIn->m_eTaskSeluteD)));
    if((ActionBeltIn::NoErr == m_pActionBeltIn->m_eTaskSeluteD) || (ActionBeltIn::Outtime == m_pActionBeltIn->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionBeltIn->m_eTaskStatusD)
            ui->lab_BIcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionBeltIn->m_eTaskStatusD)
            ui->lab_BIcolor->setStyleSheet("background-color:green");
        else
            ui->lab_BIcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_BIcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_BeltOUT_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emBeltSelute = QMetaEnum::fromType<ActionBeltOut::E_SELUTE_BELTOUT>();
    ui->lab_BOcolor->setText(QString(emBeltSelute.valueToKey(m_pActionBeltOut->m_eTaskSeluteD)));
    if((ActionBeltOut::NoErr == m_pActionBeltOut->m_eTaskSeluteD) || (ActionBeltOut::Outtime == m_pActionBeltOut->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionBeltOut->m_eTaskStatusD)
            ui->lab_BOcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionBeltOut->m_eTaskStatusD)
            ui->lab_BOcolor->setStyleSheet("background-color:green");
        else
            ui->lab_BOcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_BOcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_PoleIN_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emPoleSelute = QMetaEnum::fromType<ActionPoleIn::E_SELUTE_POLEIN>();
    ui->lab_PIcolor->setText(QString(emPoleSelute.valueToKey(m_pActionPoleIn->m_eTaskSeluteD)));
    if((ActionPoleIn::NoErr == m_pActionPoleIn->m_eTaskSeluteD) || (ActionPoleIn::Outtime == m_pActionPoleIn->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionPoleIn->m_eTaskStatusD)
            ui->lab_PIcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionPoleIn->m_eTaskStatusD)
            ui->lab_PIcolor->setStyleSheet("background-color:green");
        else
            ui->lab_PIcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_PIcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_PoleOUT_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emPoleSelute = QMetaEnum::fromType<ActionPoleOut::E_SELUTE_POLEOUT>();
    ui->lab_POcolor->setText(QString(emPoleSelute.valueToKey(m_pActionPoleOut->m_eTaskSeluteD)));
    if((ActionPoleOut::NoErr == m_pActionPoleOut->m_eTaskSeluteD) || (ActionPoleOut::Outtime == m_pActionPoleOut->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionPoleOut->m_eTaskStatusD)
            ui->lab_POcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionPoleOut->m_eTaskStatusD)
            ui->lab_POcolor->setStyleSheet("background-color:green");
        else
            ui->lab_POcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_POcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_Upender_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emUpenderSelute = QMetaEnum::fromType<ActionUpender::E_SELUTE_UPENDER>();
    ui->lab_UPcolor->setText(QString(emUpenderSelute.valueToKey(m_pActionUpender->m_eTaskSeluteD)));
    if((ActionUpender::NoErr == m_pActionUpender->m_eTaskSeluteD) || (ActionUpender::OutTime == m_pActionUpender->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionUpender->m_eTaskStatusD)
            ui->lab_UPcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionUpender->m_eTaskStatusD)
            ui->lab_UPcolor->setStyleSheet("background-color:green");
        else
            ui->lab_UPcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_UPcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_MotorV_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emMotorVSelute = QMetaEnum::fromType<ActionMotorV::E_SELUTE_MOTORV>();
    ui->lab_MVcolor->setText(QString(emMotorVSelute.valueToKey(m_pActionMotorV->m_eTaskSeluteD)));
    if((ActionMotorV::NoErr == m_pActionMotorV->m_eTaskSeluteD) || (ActionMotorV::Outtime == m_pActionMotorV->m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionMotorV->m_eTaskStatusD)
            ui->lab_MVcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionMotorV->m_eTaskStatusD)
            ui->lab_MVcolor->setStyleSheet("background-color:green");
        else
        {
            ui->lab_MVcolor->setText(QString("%1, Cur:%2").
                                     arg(QString(emMotorVSelute.valueToKey(m_pActionMotorV->m_eTaskSeluteD))).arg(m_pActionMotorV->m_i32CurPos));
            ui->lab_MVcolor->setStyleSheet("background-color:transparent");
        }
    }
    else
    {
        ui->lab_MVcolor->setStyleSheet("background-color:red");
    }
//    ui->txtEd_PO->setPlainText(QString(emMotorVSelute.valueToKey(m_pActionMotorV->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionMotorV->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_MotorXYZ_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emMotorXYZTaskType = QMetaEnum::fromType<ActionMotorXYZ::E_TASKTPYE_MOTORXYZ>();
    QMetaEnum emMotorXYZSelute = QMetaEnum::fromType<ActionMotorXYZ::E_SELUTE_MOTORXYZ>();
    ui->lab_taskInfo_type->setText( QString(emMotorXYZTaskType.valueToKey(m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskTypeD)) );
    ui->lab_taskInfo_status->setText( QString(emTaskStatus.valueToKey(m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)) );
//    if(emWorkStatus_ask == m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
//    {
//        ui->lab_taskInfo_status->clear();
//        ui->lab_taskInfo_status->setStyleSheet("background-color:orange");
//    }
//    else if (emWorkStatus_run == m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)
//    {
//        ui->lab_taskInfo_status->clear();
//        ui->lab_taskInfo_status->setStyleSheet("background-color:green");
//    }
//    else
//    {
//        ui->lab_taskInfo_status->setStyleSheet("background-color:transparent");
//        ui->lab_taskInfo_status->setText( QString(emTaskStatus.valueToKey(m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskStatusD)) );
//    }
    ui->lab_taskInfo_selute->setText( QString(emMotorXYZSelute.valueToKey(m_pActionMotorXYZ->m_stDTaskInfo.m_eTaskSeluteD)) );
    //显示边界
    ui->lab_lineXMin->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32XLineLeft);
    ui->lab_lineXMax->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32XLineRight);
    ui->lab_lineYMin->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32YLineOut);
    ui->lab_lineYMax->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32YLineIn);
    ui->lab_lineZMin->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32ZLineUp);
    ui->lab_lineZMax->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stLinePos.m_i32ZLineDown);
    if( m_pActionMotorXYZ->isLineHasLocated(emMorot_X) )
        ui->ledit_moveX->setEnabled(true);
    else
        ui->ledit_moveX->setEnabled(false);

    if( m_pActionMotorXYZ->isLineHasLocated(emMorot_Y) )
        ui->ledit_moveY->setEnabled(true);
    else
        ui->ledit_moveY->setEnabled(false);

    if( m_pActionMotorXYZ->isLineHasLocated(emMorot_Z) )
        ui->ledit_moveZ->setEnabled(true);
    else
        ui->ledit_moveZ->setEnabled(false);

    //显示当前坐标物理值
    ui->lab_CurPhyPosX->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurDPos.m_i32X);
    ui->lab_CurPhyPosY->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurDPos.m_i32Y);
    ui->lab_CurPhyPosZ->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurDPos.m_i32Z);
    //显示当前坐标逻辑值
    ui->lab_CurlogicPosX->setText( QString::number(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurLogicDPos.m_fX, 'f', 4) );
    ui->lab_CurlogicPosY->setText( QString::number(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurLogicDPos.m_fY, 'f', 4) );
    ui->lab_CurlogicPosZ->setText( QString::number(m_pActionMotorXYZ->m_stDTaskInfo.m_stCurLogicDPos.m_fZ, 'f', 4) );
    //显示任务坐标物理值
    ui->lab_AimPhyPosX->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stAimDPos.m_i32X);
    ui->lab_AimPhyPosY->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stAimDPos.m_i32Y);
    ui->lab_AimPhyPosZ->setNum(m_pActionMotorXYZ->m_stDTaskInfo.m_stAimDPos.m_i32Z);
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}
/*
 * ******************************************************入皮带******************************************************
 */
void FormCtrl::on_rbtn_BIin_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltInType_InSideMoving == m_pActionBeltIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltIn->m_stTaskToSend.m_eTaskType = emTskDBeltInType_InSideMoving;
    m_pActionBeltIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionBeltIn->m_stTaskToSend.m_bNeedQR = false;
    _LOG(QString("task is set"));

    m_pActionBeltIn->setTaskSend();
}
void FormCtrl::on_rbtn_BIout_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltInType_OutSideMoving == m_pActionBeltIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltIn->m_stTaskToSend.m_eTaskType = emTskDBeltInType_OutSideMoving;
    m_pActionBeltIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionBeltIn->m_stTaskToSend.m_bNeedQR = false;
    _LOG(QString("task is set"));

    m_pActionBeltIn->setTaskSend();
}
void FormCtrl::on_rbtn_BIoff_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltInType_PowerOff == m_pActionBeltIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltIn->m_stTaskToSend.m_eTaskType = emTskDBeltInType_PowerOff;
    m_pActionBeltIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionBeltIn->m_stTaskToSend.m_bNeedQR = false;
    _LOG(QString("task is set"));

    m_pActionBeltIn->setTaskSend();
}
void FormCtrl::on_rbtn_BIstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltInType_Stop == m_pActionBeltIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltIn->m_stTaskToSend.m_eTaskType = emTskDBeltInType_Stop;
    m_pActionBeltIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionBeltIn->m_stTaskToSend.m_bNeedQR = false;
    _LOG(QString("task is set"));

    m_pActionBeltIn->setTaskSend();
}

/*
 * ******************************************************出皮带******************************************************
 */
void FormCtrl::on_rbtn_BOout_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltOutType_OutSideMoving == m_pActionBeltOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltOut->m_stTaskToSend.m_eTaskType = emTskDBeltOutType_OutSideMoving;
    m_pActionBeltOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);  
    _LOG(QString("task is set"));

    m_pActionBeltOut->setTaskSend();
}

void FormCtrl::on_rbtn_BOin_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltOutType_InSideMoving == m_pActionBeltOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltOut->m_stTaskToSend.m_eTaskType = emTskDBeltOutType_InSideMoving;
    m_pActionBeltOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionBeltOut->setTaskSend();
}

void FormCtrl::on_rbtn_BOoff_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltOutType_PowerOff == m_pActionBeltOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltOut->m_stTaskToSend.m_eTaskType = emTskDBeltOutType_PowerOff;
    m_pActionBeltOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionBeltOut->setTaskSend();
}

void FormCtrl::on_rbtn_BOstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDBeltOutType_Stop == m_pActionBeltOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionBeltOut->m_stTaskToSend.m_eTaskType = emTskDBeltOutType_Stop;
    m_pActionBeltOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionBeltOut->setTaskSend();
}

/*
 * ******************************************************上推杆******************************************************
 */
void FormCtrl::on_rbtn_PIin_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleInType_In == m_pActionPoleIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_In;
    m_pActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleIn->setTaskSend();
}
void FormCtrl::on_rbtn_PIout_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleInType_Out == m_pActionPoleIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_Out;
    m_pActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleIn->setTaskSend();
}
void FormCtrl::on_rbtn_PIoff_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleInType_PowerOff == m_pActionPoleIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_PowerOff;
    m_pActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleIn->setTaskSend();
}
void FormCtrl::on_rbtn_PIstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleInType_Stop == m_pActionPoleIn->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_Stop;
    m_pActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleIn->setTaskSend();
}

/*
 * ******************************************************下推杆******************************************************
 */
void FormCtrl::on_rbtn_POout_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleOutType_Out == m_pActionPoleOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_Out;
    m_pActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleOut->setTaskSend();
}
void FormCtrl::on_rbtn_POin_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleOutType_In == m_pActionPoleOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_In;
    m_pActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleOut->setTaskSend();
}
void FormCtrl::on_rbtn_POoff_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleOutType_PowerOff == m_pActionPoleOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_PowerOff;
    m_pActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleOut->setTaskSend();
}
void FormCtrl::on_rbtn_POstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDPoleOutType_Stop == m_pActionPoleOut->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_Stop;
    m_pActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionPoleOut->setTaskSend();
}

/*
 * ******************************************************翻转装置******************************************************
 */
void FormCtrl::on_rbtn_UPstand_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDUpenderType_StandUp == m_pActionUpender->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_StandUp;
    m_pActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionUpender->setTaskSend();
}
void FormCtrl::on_rbtn_UPlie_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDUpenderType_LieDown == m_pActionUpender->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_LieDown;
    m_pActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionUpender->setTaskSend();
}
void FormCtrl::on_pbtn_UPstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDUpenderType_Stop == m_pActionUpender->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_Stop;
    m_pActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionUpender->setTaskSend();
}
/*
 * ******************************************************垂直扫描轴******************************************************
 */
void FormCtrl::on_rbtn_MVzero_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDMoveBoxVType_zeroSFCheck == m_pActionMotorV->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_zeroSFCheck;
    m_pActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorV->m_stTaskToSend.m_fScanBgn = 0;
    m_pActionMotorV->m_stTaskToSend.m_fScanEnd = 0;
    m_pActionMotorV->m_stTaskToSend.m_fTargetPos = 0;
    _LOG(QString("task is set"));

    m_pActionMotorV->setTaskSend();
}
void FormCtrl::on_rbtn_MVscan_clicked()
{

}
void FormCtrl::on_rbtn_MVmove_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDMoveBoxVType_move == m_pActionMotorV->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    bool ok;
    m_pActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_move;
    m_pActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorV->m_stTaskToSend.m_fScanBgn = 0;
    m_pActionMotorV->m_stTaskToSend.m_fScanEnd = 0;   
    if(ui->ledit_moveV->text().isEmpty())
    {
         if(m_pActionMotorV->getBoxLieDownPos(m_pActionMotorV->m_stTaskToSend.m_fTargetPos))
             ui->rbtn_MVmove->setText("出仓");
         else
             return;
    }
    else
    {
        m_pActionMotorV->m_stTaskToSend.m_fTargetPos = ui->ledit_moveV->text().toInt(&ok);
        ui->rbtn_MVmove->setText("移动");
    }
    _LOG(QString("task is set"));
    m_pActionMotorV->setTaskSend();
}

void FormCtrl::on_pbtn_MVstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDMoveBoxVType_stop == m_pActionMotorV->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_stop;
    m_pActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorV->m_stTaskToSend.m_fScanBgn = 0;
    m_pActionMotorV->m_stTaskToSend.m_fScanEnd = 0;
    m_pActionMotorV->m_stTaskToSend.m_fTargetPos = 0;
    _LOG(QString("task is set"));

    m_pActionMotorV->setTaskSend();
}

/*
 * ******************************************************XYZ模块******************************************************
 */
void FormCtrl::on_rbtn_XLineZero_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_zeroXCheck == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_zeroXCheck;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_rbtn_YLineIn_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_minPosYSet == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_minPosYSet;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_rbtn_YLineOut_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_maxPosYSet == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_maxPosYSet;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_rbtn_ZLineUp_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_maxPosZSet == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_maxPosZSet;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_rbtn_ZLineDown_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_minPosZSet == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_minPosZSet;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_pbtn_xyzStop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTskDXYZType_stop == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_stop;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_pbtn_phyMove_clicked()
{
    //检查输入坐标是否合法
    if( !ui->ledit_moveX->isPhyValue() || !ui->ledit_moveY->isPhyValue() || !ui->ledit_moveZ->isPhyValue() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    bool okx = true;
    bool oky = true;
    bool okz = true;
    ST_XYZ_DPOS m_stAimDPosTemp = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    if(!ui->ledit_moveX->text().isEmpty())
        m_stAimDPosTemp.m_i32X = ui->ledit_moveX->text().toInt(&okx);
    if(!ui->ledit_moveY->text().isEmpty())
        m_stAimDPosTemp.m_i32Y = ui->ledit_moveY->text().toInt(&oky);
    if(!ui->ledit_moveZ->text().isEmpty())
        m_stAimDPosTemp.m_i32Z = ui->ledit_moveZ->text().toInt(&okz);
    if(!okx || !oky || !okz)
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    if( !m_pActionMotorXYZ->isAimPhyPosOverLimit(m_stAimDPosTemp))
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
        return;
    }
    //检查是否重复任务
    if( (emTskDXYZType_move == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X == m_stAimDPosTemp.m_i32X) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y == m_stAimDPosTemp.m_i32Y) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z == m_stAimDPosTemp.m_i32Z) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimDPosTemp.m_i32X;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimDPosTemp.m_i32Y;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimDPosTemp.m_i32Z;
    m_pActionMotorXYZ->convertPhyPosToLogicPos(m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                               m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_pbtn_phyMoveInTime_clicked()
{
    //检查输入坐标是否合法
    if( !ui->ledit_moveX->isPhyValue() || !ui->ledit_moveY->isPhyValue() || !ui->ledit_moveZ->isPhyValue() ||
            !ui->ledit_moveTime->isPositiveInt() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    bool okx = true;
    bool oky = true;
    bool okz = true;
    bool okt = true;
    ST_XYZ_DPOS m_stAimDPosTemp = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
    if(!ui->ledit_moveX->text().isEmpty())
        m_stAimDPosTemp.m_i32X = ui->ledit_moveX->text().toInt(&okx);
    if(!ui->ledit_moveY->text().isEmpty())
        m_stAimDPosTemp.m_i32Y = ui->ledit_moveY->text().toInt(&oky);
    if(!ui->ledit_moveZ->text().isEmpty())
        m_stAimDPosTemp.m_i32Z = ui->ledit_moveZ->text().toInt(&okz);
    uint32_t m_u32MoveTimeTemp = ui->ledit_moveTime->text().toUInt(&okt);
    if(!okx || !oky || !okz || !okt)
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    if( !m_pActionMotorXYZ->isAimPhyPosOverLimit(m_stAimDPosTemp))
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
        return;
    }
    //检查是否重复任务
    if( (emTskDXYZType_moveInTime == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X == m_stAimDPosTemp.m_i32X) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y == m_stAimDPosTemp.m_i32Y) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z == m_stAimDPosTemp.m_i32Z) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_u32MoveTime == m_u32MoveTimeTemp))
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_moveInTime;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32X = m_stAimDPosTemp.m_i32X;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Y = m_stAimDPosTemp.m_i32Y;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos.m_i32Z = m_stAimDPosTemp.m_i32Z;
    m_pActionMotorXYZ->m_stTaskToSend.m_u32MoveTime = m_u32MoveTimeTemp;
    m_pActionMotorXYZ->convertPhyPosToLogicPos(m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos,
                                               m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_pbtn_logicMove_clicked()
{
    if( !ui->ledit_moveX->isLogicValue() || !ui->ledit_moveY->isLogicValue() || !ui->ledit_moveZ->isLogicValue() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }

    bool okx = true;
    bool oky = true;
    bool okz = true;
    ST_XYZ_CPOS m_stAimLogicDPosTemp = {WK_PosNotLimit, WK_PosNotLimit, WK_PosNotLimit};
    if(!ui->ledit_moveX->text().isEmpty())
        m_stAimLogicDPosTemp.m_fX = ui->ledit_moveX->text().toFloat(&okx);
    if(!ui->ledit_moveY->text().isEmpty())
        m_stAimLogicDPosTemp.m_fY = ui->ledit_moveY->text().toFloat(&oky);
    if(!ui->ledit_moveZ->text().isEmpty())
        m_stAimLogicDPosTemp.m_fZ = ui->ledit_moveZ->text().toFloat(&okz);
    if(!okx || !oky || !okz)
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    if( !m_pActionMotorXYZ->isAimLogicPosOverLimit(m_stAimLogicDPosTemp))
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
        return;
    }
    //检查是否重复任务
    if( (emTskDXYZType_move == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX == m_stAimLogicDPosTemp.m_fX) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY == m_stAimLogicDPosTemp.m_fY) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ == m_stAimLogicDPosTemp.m_fZ) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = m_stAimLogicDPosTemp.m_fX;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = m_stAimLogicDPosTemp.m_fY;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = m_stAimLogicDPosTemp.m_fZ;
    m_pActionMotorXYZ->convertLogicPosToPhyPos(m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                               m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}
void FormCtrl::on_pbtn_logicMoveInTime_clicked()
{
    if( !ui->ledit_moveX->isLogicValue() || !ui->ledit_moveY->isLogicValue() || !ui->ledit_moveZ->isLogicValue()
            || !ui->ledit_moveTime->isPositiveInt() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    bool okx = true;
    bool oky = true;
    bool okz = true;
    bool okt = true;
    ST_XYZ_CPOS m_stAimLogicDPosTemp = {WK_PosNotLimit, WK_PosNotLimit, WK_PosNotLimit};
    if(!ui->ledit_moveX->text().isEmpty())
        m_stAimLogicDPosTemp.m_fX = ui->ledit_moveX->text().toFloat(&okx);
    if(!ui->ledit_moveY->text().isEmpty())
        m_stAimLogicDPosTemp.m_fY = ui->ledit_moveY->text().toFloat(&oky);
    if(!ui->ledit_moveZ->text().isEmpty())
        m_stAimLogicDPosTemp.m_fZ = ui->ledit_moveZ->text().toFloat(&okz);
    uint32_t m_u32MoveTimeTemp = ui->ledit_moveTime->text().toUInt(&okt);
    if(!okx || !oky || !okz || !okt)
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
        return;
    }
    if( !m_pActionMotorXYZ->isAimLogicPosOverLimit(m_stAimLogicDPosTemp))
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
        return;
    }
    //检查是否重复任务
    if( (emTskDXYZType_move == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX == m_stAimLogicDPosTemp.m_fX) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY == m_stAimLogicDPosTemp.m_fY) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ == m_stAimLogicDPosTemp.m_fZ) &&
            (m_pActionMotorXYZ->m_stTaskToSend.m_u32MoveTime == m_u32MoveTimeTemp) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTskDXYZType_move;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fX = m_stAimLogicDPosTemp.m_fX;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fY = m_stAimLogicDPosTemp.m_fY;
    m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos.m_fZ = m_stAimLogicDPosTemp.m_fZ;
    m_pActionMotorXYZ->m_stTaskToSend.m_u32MoveTime = m_u32MoveTimeTemp;
    m_pActionMotorXYZ->convertLogicPosToPhyPos(m_pActionMotorXYZ->m_stTaskToSend.m_stAimLogicDPos,
                                               m_pActionMotorXYZ->m_stTaskToSend.m_stAimDPos);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}





