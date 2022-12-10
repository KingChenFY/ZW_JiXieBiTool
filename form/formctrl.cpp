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
 ui->label_4->setText("xbasxjksbjksbcjkbckjcbjdsjkcsdjb");
        ui->label_4->setWordWrap(true);
        ui->label_4->adjustSize();


}

void FormCtrl::initConfig()
{
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


    emTaskStatus = QMetaEnum::fromType<FormCtrl::E_TASK_STATUS>();
}

void FormCtrl::slot_BeltIN_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emBeltSelute = QMetaEnum::fromType<ActionBeltIn::E_SELUTE_BELTIN>();
    ui->txtEd_BI->setPlainText(QString(emBeltSelute.valueToKey(m_pActionBeltIn->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionBeltIn->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_BeltOUT_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emBeltSelute = QMetaEnum::fromType<ActionBeltOut::E_SELUTE_BELTOUT>();
    ui->txtEd_BO->setPlainText(QString(emBeltSelute.valueToKey(m_pActionBeltOut->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionBeltOut->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_PoleIN_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emPoleSelute = QMetaEnum::fromType<ActionPoleIn::E_SELUTE_POLEIN>();
    ui->txtEd_PI->setPlainText(QString(emPoleSelute.valueToKey(m_pActionPoleIn->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionPoleIn->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_PoleOUT_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emPoleSelute = QMetaEnum::fromType<ActionPoleOut::E_SELUTE_POLEOUT>();
    ui->txtEd_PO->setPlainText(QString(emPoleSelute.valueToKey(m_pActionPoleOut->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionPoleOut->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_Upender_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emUpenderSelute = QMetaEnum::fromType<ActionUpender::E_SELUTE_UPENDER>();
    ui->txtEd_PO->setPlainText(QString(emUpenderSelute.valueToKey(m_pActionUpender->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionUpender->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_MotorV_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emMotorVSelute = QMetaEnum::fromType<ActionMotorV::E_SELUTE_MOTORV>();
    ui->txtEd_PO->setPlainText(QString(emMotorVSelute.valueToKey(m_pActionMotorV->m_eTaskSeluteD)) + QString(emTaskStatus.valueToKey(m_pActionMotorV->m_eTaskStatusD)));
//    ui->txtEd_BI->setStyleSheet("QTextEdit{background: rgb(128,0,0)}");
}

void FormCtrl::slot_MotorXYZ_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
//    QMetaEnum emMotorXYZSelute = QMetaEnum::fromType<emSeluteDMoveXYZ>();
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
    m_pActionMotorV->m_stTaskToSend.m_fTargetPos = ui->sbox_MVaimpos->value();
    _LOG(QString("task is set"));

    m_pActionMotorV->setTaskSend();
}
void FormCtrl::on_rbtn_MVstop_clicked()
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

