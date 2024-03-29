#include "formctrl.h"
#include "ui_formctrl.h"
#include <QMetaEnum>
#include "quihelper.h"
#include "global.h"
#include "appconfig.h"

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
    formCeju = new FormCeJu;
    ui->tabW_apps->addTab(formCeju, "ZW-5030");

    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();
    iniSet = new QSettings(AppConfig::ConfigFile, QSettings::IniFormat);

    m_bIsOilConfigSet = false;
    m_bIsFourPointConfigSet = false;
    m_bIsFourPointSavePathSet = false;

    ui->tabW_main->setDisabled(true);
    ui->gb_xyzMotor->setDisabled(true);
    ui->ledit_moveX->setEnabled(false);
    ui->ledit_moveY->setEnabled(false);
    ui->ledit_moveZ->setEnabled(false);
    ui->cb_LT1->setCheckState(Qt::Unchecked);
    ui->hsd_LT1->setDisabled(true);
    ui->cb_LT2->setCheckState(Qt::Unchecked);
    ui->hsd_LT2->setDisabled(true);
    ui->spb_LT1->setDisabled(true);
    ui->spb_LT2->setDisabled(true);

    ui->ledit_coXPos->setText(AppConfig::coXPos);
    connect(ui->ledit_coXPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_coYPos->setText(AppConfig::coYPos);
    connect(ui->ledit_coYPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_coZPos->setText(AppConfig::coZPos);
    connect(ui->ledit_coZPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ledit_doXPos->setText(AppConfig::doXPos);
    connect(ui->ledit_doXPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_doYPos->setText(AppConfig::doYPos);
    connect(ui->ledit_doYPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_doZPos->setText(AppConfig::doZPos);
    connect(ui->ledit_doZPos, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ledit_stposX->setText(AppConfig::fourStPosX);
    connect(ui->ledit_stposX, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_stposY->setText(AppConfig::fourStPosY);
    connect(ui->ledit_stposY, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_towardsposX->setText(AppConfig::fourToPosX);
    connect(ui->ledit_towardsposX, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_towardsposY->setText(AppConfig::fourToPosY);
    connect(ui->ledit_towardsposY, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_posHeight->setText(AppConfig::fourPointH);
    connect(ui->ledit_posHeight, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_onePosTime->setText(AppConfig::fourPointT);
    connect(ui->ledit_onePosTime, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_changePosTime->setText(AppConfig::fourPointTurnT);
    connect(ui->ledit_changePosTime, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->ledit_MVout->setText(AppConfig::MVOutPos);
    connect(ui->ledit_MVout, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_MVstart->setText(AppConfig::MVScanStartPos);
    connect(ui->ledit_MVstart, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
    ui->ledit_MVend->setText(AppConfig::MVScanEndPos);
    connect(ui->ledit_MVend, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
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
    m_pActionTriggerSet = new ActionTriggerSet;
    m_pActionParameterSet = new ActionParameterSet;
    m_pActionCleanOil = new ActionCleanOil;
    m_pActionDripOil = new ActionDripOil;
    m_pActionClaw = new ActionClaw;
    m_pActionQRScan = new ActionQRScan;
    m_pActionLight = new ActionLight;
    connect(m_pActionBeltIn, &ActionBeltIn::signal_UiUpdate, this, &FormCtrl::slot_BeltIN_UiUpdate);
    connect(m_pActionBeltOut, &ActionBeltOut::signal_UiUpdate, this, &FormCtrl::slot_BeltOUT_UiUpdate);
    connect(m_pActionPoleIn, &ActionPoleIn::signal_UiUpdate, this, &FormCtrl::slot_PoleIN_UiUpdate);
    connect(m_pActionPoleOut, &ActionPoleOut::signal_UiUpdate, this, &FormCtrl::slot_PoleOUT_UiUpdate);
    connect(m_pActionUpender, &ActionUpender::signal_UiUpdate, this, &FormCtrl::slot_Upender_UiUpdate);
    connect(m_pActionMotorV, &ActionMotorV::signal_UiUpdate, this, &FormCtrl::slot_MotorV_UiUpdate);
    connect(m_pActionMotorXYZ, &ActionMotorXYZ::signal_UiUpdate, this, &FormCtrl::slot_MotorXYZ_UiUpdate);
    connect(m_pActionTriggerSet, &ActionTriggerSet::signal_UiUpdate, this, &FormCtrl::slot_TriggerSet_UiUpdate);
    connect(m_pActionCleanOil, &ActionCleanOil::signal_UiUpdate, this, &FormCtrl::slot_CleanOil_UiUpdate);
    connect(m_pActionDripOil, &ActionDripOil::signal_UiUpdate, this, &FormCtrl::slot_DripOil_UiUpdate);
    connect(m_pActionClaw, &ActionClaw::signal_UiUpdate, this, &FormCtrl::slot_Claw_UiUpdate);
    connect(m_pActionParameterSet, &ActionParameterSet::signal_SlideInfoUiUpdate, this, &FormCtrl::slot_SlideInfo_UiUpdate);
    connect(m_pActionParameterSet, &ActionParameterSet::signal_MVLaserUiUpdate, this, &FormCtrl::slot_MVLaser_UiUpdate);
    connect(m_pActionQRScan, &ActionQRScan::signal_UiUpdate, this, &FormCtrl::slot_QRScan_UiUpdate);
    connect(m_pActionLight, &ActionLight::signal_UiUpdate, this, &FormCtrl::slot_Light_UiUpdate);

    connect(ui->hsd_LT1, SIGNAL(sliderReleased()), this, SLOT(on_pbtn_LTOpen_clicked()));
    connect(ui->hsd_LT2, SIGNAL(sliderReleased()), this, SLOT(on_pbtn_LTOpen_clicked()));
//    connect(ui->hsd_LT2, SIGNAL(valueChanged(int)), this, SLOT(on_pbtn_LTOpen_clicked()));//拖动不准
    connect(ui->cb_LT1, SIGNAL(stateChanged(int)), this, SLOT(slot_cbLT1_StateChanged(int)));
    connect(ui->cb_LT2, SIGNAL(stateChanged(int)), this, SLOT(slot_cbLT2_StateChanged(int)));

    connect(ui->spb_LT1, SIGNAL(editingFinished()), this, SLOT(slot_spbLT1_editFinished()));
    connect(ui->spb_LT2, SIGNAL(editingFinished()), this, SLOT(slot_spbLT2_editFinished()));

    //运输仓测试
    m_pActionBTransport = new ActionBTransport(m_pActionBeltIn, m_pActionBeltOut, m_pActionPoleIn, m_pActionPoleOut, m_pActionUpender, m_pActionMotorV);
    connect(m_pActionBTransport, &QThread::finished, this, &FormCtrl::slot_TransportTestThread_Stop);

    //跟随测试
    m_pActionBFollowMove = new ActionBFollowMove(m_pActionTriggerSet, m_pActionMotorXYZ, formCeju->cejuClient, m_pActionParameterSet);
    connect(m_pActionBFollowMove, &QThread::finished, this, &FormCtrl::slot_FollowTestThread_Stop);
    //测试跟随测试线程中的 测距结束 的处理放到 测距线程中跑， 看看能不能解决测距网络线程数据处理经常异常
    connect(m_pActionBFollowMove, &ActionBFollowMove::signal_Ceju_RecordStart, formCeju->cejuClient, &CeJuTcpClient::Ceju_RecordStart);
    connect(m_pActionBFollowMove, &ActionBFollowMove::signal_Ceju_RecordEnd, formCeju->cejuClient, &CeJuTcpClient::Ceju_RecordEnd);

    //跟随测试
    m_pActionBOilTest = new ActionBOilTest(m_pActionMotorXYZ, m_pActionDripOil, m_pActionCleanOil);
    connect(m_pActionBOilTest, &QThread::finished, this, &FormCtrl::slot_OilTestThread_Stop);

    //四点稳定性测试
    m_pActionFourPosTest = new ActionFourPosTest(m_pActionMotorXYZ, formCeju->cejuClient);
    connect(m_pActionFourPosTest, &QThread::finished, this, &FormCtrl::slot_FourPosTestThread_Stop);
    connect(m_pActionFourPosTest, &ActionFourPosTest::signal_FourPoint_CejuInit, formCeju->cejuClient, &CeJuTcpClient::Ceju_FourPoint_Init);
    connect(m_pActionFourPosTest, &ActionFourPosTest::signal_Ceju_RecordStart, formCeju->cejuClient, &CeJuTcpClient::slot_Ceju_FourPoint_RecordStart);
}

void FormCtrl::saveConfig()
{
//    AppConfig::coXPos = ui->ledit_coXPos->text();
//    AppConfig::coYPos = ui->ledit_coYPos->text();
//    AppConfig::coZPos = ui->ledit_coZPos->text();

//    AppConfig::doXPos = ui->ledit_doXPos->text();
//    AppConfig::doYPos = ui->ledit_doYPos->text();
//    AppConfig::doZPos = ui->ledit_doZPos->text();

    iniSet->beginGroup("OilConfig");
    iniSet->setValue("coXPos", ui->ledit_coXPos->text());
    iniSet->setValue("coYPos", ui->ledit_coYPos->text());
    iniSet->setValue("coZPos", ui->ledit_coZPos->text());

    iniSet->setValue("doXPos", ui->ledit_doXPos->text());
    iniSet->setValue("doYPos", ui->ledit_doYPos->text());
    iniSet->setValue("doZPos", ui->ledit_doZPos->text());
    iniSet->endGroup();
    m_bIsOilConfigSet = false;

    iniSet->beginGroup("FourPointConfig");
    iniSet->setValue("fourStPosX", ui->ledit_stposX->text());
    iniSet->setValue("fourStPosY", ui->ledit_stposY->text());
    iniSet->setValue("fourToPosX", ui->ledit_towardsposX->text());
    iniSet->setValue("fourToPosY", ui->ledit_towardsposY->text());
    iniSet->setValue("fourPointH", ui->ledit_posHeight->text());
    iniSet->setValue("fourPointT", ui->ledit_onePosTime->text());
    iniSet->setValue("fourPointTurnT", ui->ledit_changePosTime->text());
    iniSet->endGroup();
    m_bIsFourPointConfigSet = false;

    iniSet->beginGroup("MotorVConfig");
    iniSet->setValue("MVOutPos", ui->ledit_MVout->text());
    iniSet->setValue("MVScanStartPos", ui->ledit_MVstart->text());
    iniSet->setValue("MVScanEndPos", ui->ledit_MVend->text());
    iniSet->endGroup();
//    AppConfig::writeConfig();
}

void FormCtrl::slot_netConnected()
{
    ui->tabW_main->setEnabled(true);
    ui->gb_xyzMotor->setEnabled(true);
}

void FormCtrl::slot_netNoLink()
{
    ui->tabW_main->setDisabled(true);
    ui->gb_xyzMotor->setDisabled(true);
}

void FormCtrl::slot_cbLT1_StateChanged(int state)
{
    if(Qt::Unchecked == state)
    {
        ui->hsd_LT1->setDisabled(true);
        ui->spb_LT1->setDisabled(true);
    }
    else
    {
        ui->hsd_LT1->setEnabled(true);
        ui->spb_LT1->setEnabled(true);
    }
}

void FormCtrl::slot_cbLT2_StateChanged(int state)
{
    if(Qt::Unchecked == state)
    {
        ui->hsd_LT2->setDisabled(true);
        ui->spb_LT2->setDisabled(true);
    }
    else
    {
        ui->hsd_LT2->setEnabled(true);
        ui->spb_LT2->setEnabled(true);
    }
}

void FormCtrl::slot_spbLT1_editFinished()
{
    ui->hsd_LT1->setValue(ui->spb_LT1->value());
}
void FormCtrl::slot_spbLT2_editFinished()
{
    ui->hsd_LT2->setValue(ui->spb_LT2->value());
}
/*
 * ******************************************************四点测试模块******************************************************
 */
void FormCtrl::on_cb_FpSave_clicked(bool checked)
{
    if(checked)
    {
        fp_savePath = QFileDialog::getExistingDirectory(this, "选择四点测试保存路径");
        if (!fp_savePath.isEmpty())
        {
            m_bIsFourPointSavePathSet = true;
            m_pActionFourPosTest->FourPointSavePathSet(fp_savePath);
        }
        else
        {
            m_bIsFourPointSavePathSet = false;
            ui->cb_FpSave->setCheckState(Qt::Unchecked);
        }
    }
    else
        m_bIsFourPointSavePathSet = false;
}


void FormCtrl::on_pbtn_fourPConfirm_clicked()
{
    if("确认测试参数" == ui->pbtn_fourPConfirm->text())
    {
        m_bIsFourPointConfigSet = true;
    }
}


void FormCtrl::on_pbtn_fourPTest_clicked()
{
    if("开始测试" == ui->pbtn_fourPTest->text())
    {
        //初始化环境检查
        if(!m_bIsFourPointConfigSet)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请在确认四点测试参数后，点击确认测试参数"));
            return;
        }
        if(!formCeju->cejuClient->m_bIsCejuInitSucceed)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请等待测距初始化完成"));
            return;
        }
        if(!m_pActionMotorXYZ->isAllLineHasLocated())
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请完成机械臂定位"));
            return;
        }
        if(!m_bIsFourPointSavePathSet)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请选择保存路径"));
            return;
        }

        bool okx = true;
        bool oky = true;
        bool okz = true;
        ST_FOURPOINT_TEST_INFO m_stFpInfoTemp;
        ST_XYZ_DPOS m_stPosTemp = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
        if(!ui->ledit_stposX->text().isEmpty())
            m_stPosTemp.m_i32X = ui->ledit_stposX->text().toInt(&okx);
        if(!ui->ledit_stposY->text().isEmpty())
            m_stPosTemp.m_i32Y = ui->ledit_stposY->text().toInt(&oky);
        if(!ui->ledit_posHeight->text().isEmpty())
            m_stPosTemp.m_i32Z = ui->ledit_posHeight->text().toInt(&oky);
        if(!okx || !oky || !okz)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        if( !m_pActionMotorXYZ->isAimPhyPosOverLimit(m_stPosTemp))
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
            return;
        }
        m_stFpInfoTemp.m_i32StPosX = m_stPosTemp.m_i32X;
        m_stFpInfoTemp.m_i32StPosY = m_stPosTemp.m_i32Y;
        m_stFpInfoTemp.m_i32Height = m_stPosTemp.m_i32Z;

        m_stPosTemp = {WK_PhyPosNotLimit, WK_PhyPosNotLimit, WK_PhyPosNotLimit};
        if(!ui->ledit_towardsposX->text().isEmpty())
            m_stPosTemp.m_i32X = ui->ledit_towardsposX->text().toInt(&okx);
        if(!ui->ledit_towardsposY->text().isEmpty())
            m_stPosTemp.m_i32Y = ui->ledit_towardsposY->text().toInt(&oky);
        if(!okx || !oky)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        if( !m_pActionMotorXYZ->isAimPhyPosOverLimit(m_stPosTemp))
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
            return;
        }
        m_stFpInfoTemp.m_i32ToPosX = m_stPosTemp.m_i32X;
        m_stFpInfoTemp.m_i32ToPosY = m_stPosTemp.m_i32Y;

        uint16_t m_u16TimeTemp;
        if(!ui->ledit_onePosTime->text().isEmpty())
            m_u16TimeTemp = ui->ledit_onePosTime->text().toUInt(&okx);
        else
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入单点时长为空"));
            return;
        }
        if(!okx)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        m_stFpInfoTemp.m_u16PointT = m_u16TimeTemp;

        if(!ui->ledit_changePosTime->text().isEmpty())
            m_u16TimeTemp = ui->ledit_changePosTime->text().toUInt(&oky);
        else
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入采集频率为空"));
            return;
        }
        if(!oky)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        m_stFpInfoTemp.m_u16ReadFreq = m_u16TimeTemp;

        m_pActionFourPosTest->getPointSetInfo(m_stFpInfoTemp);
        //启动四点测试线程
        m_pActionFourPosTest->m_bNeedStop = false;
        m_pActionFourPosTest->InitFourPointTest();
        _LOG("{Four_Point_Test}: THREAD START =================================");
        m_pActionFourPosTest->start();
        ui->pbtn_fourPTest->setText("停止测试");
    }
    else if("停止测试" == ui->pbtn_fourPTest->text())
    {
        ui->pbtn_fourPTest->setDisabled(true);
        ui->pbtn_fourPTest->setText("正在停止测试");
        m_pActionFourPosTest->m_bNeedStop = true;
        _LOG("{Four_Point_Test}: STOP THREAD =================================");
    }
    else
    {
        return;
    }
}
void FormCtrl::slot_FourPosTestThread_Stop()
{
    on_pbtn_xyzStop_clicked();
    ui->pbtn_fourPTest->setText("开始测试");
    ui->pbtn_fourPTest->setEnabled(true);
    _LOG("{Four_Point_Test}: THREAD FINISH END=================================");
}
/*
 * ******************************************************油测试模块******************************************************
 */
void FormCtrl::on_cb_skipDripOil_clicked(bool checked)
{
    m_pActionBOilTest->m_bIsSkipDripOil = checked;
}

void FormCtrl::on_pbtn_cdoPSet_clicked()
{
    if("确认位置值" == ui->pbtn_cdoPSet->text())
    {
        m_bIsOilConfigSet = true;
    }
}

void FormCtrl::on_pbtn_cdoTest_clicked()
{
    if("开始测试" == ui->pbtn_cdoTest->text())
    {
        //初始化环境检查
        if(!m_bIsOilConfigSet)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请先确认滴油位置和除油位置后，点击确认位置值"));
            return;
        }
        if(!m_pActionMotorXYZ->isAllLineHasLocated())
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请完成机械臂定位"));
            return;
        }

        bool okx = true;
        bool oky = true;
        bool okz = true;
        ST_OILTEST_TAGETPOS m_stOilPosTemp;
        m_stOilPosTemp.m_stCleanOilPos = {WK_PosNotLimit, WK_PosNotLimit, WK_PosNotLimit};
        if(!ui->ledit_coXPos->text().isEmpty())
            m_stOilPosTemp.m_stCleanOilPos.m_fX = ui->ledit_coXPos->text().toFloat(&okx);
        if(!ui->ledit_coYPos->text().isEmpty())
            m_stOilPosTemp.m_stCleanOilPos.m_fY = ui->ledit_coYPos->text().toFloat(&oky);
        if(!ui->ledit_coZPos->text().isEmpty())
            m_stOilPosTemp.m_stCleanOilPos.m_fZ = ui->ledit_coZPos->text().toFloat(&okz);
        if(!okx || !oky || !okz)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        if( !m_pActionMotorXYZ->isAimLogicPosOverLimit(m_stOilPosTemp.m_stCleanOilPos))
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
            return;
        }

        m_stOilPosTemp.m_stDripOilPos = {WK_PosNotLimit, WK_PosNotLimit, WK_PosNotLimit};
        if(!ui->ledit_doXPos->text().isEmpty())
            m_stOilPosTemp.m_stDripOilPos.m_fX = ui->ledit_doXPos->text().toFloat(&okx);
        if(!ui->ledit_doYPos->text().isEmpty())
            m_stOilPosTemp.m_stDripOilPos.m_fY = ui->ledit_doYPos->text().toFloat(&oky);
        if(!ui->ledit_doZPos->text().isEmpty())
            m_stOilPosTemp.m_stDripOilPos.m_fZ = ui->ledit_doZPos->text().toFloat(&okz);
        if(!okx || !oky || !okz)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入不合法"));
            return;
        }
        if( !m_pActionMotorXYZ->isAimLogicPosOverLimit(m_stOilPosTemp.m_stDripOilPos))
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("输入超边界"));
            return;
        }

        ui->ledit_coXPos->setReadOnly(true);
        ui->ledit_coYPos->setReadOnly(true);
        ui->ledit_coZPos->setReadOnly(true);
        ui->ledit_doXPos->setReadOnly(true);
        ui->ledit_doYPos->setReadOnly(true);
        ui->ledit_doZPos->setReadOnly(true);
        m_pActionBOilTest->GetOilConifgPos(m_stOilPosTemp);
        //启动油测试线程
        m_pActionBOilTest->m_bNeedStop = false;
        m_pActionBOilTest->InitOilTest();
        _LOG("{Oil_Auto_Test}: THREAD START =================================");
        m_pActionBOilTest->start();
        ui->pbtn_cdoTest->setText("停止测试");
    }
    else if("停止测试" == ui->pbtn_cdoTest->text())
    {
        ui->pbtn_cdoTest->setDisabled(true);
        ui->pbtn_cdoTest->setText("正在停止测试");
        m_pActionBOilTest->m_bNeedStop = true;
        _LOG("{Oil_Auto_Test}: STOP THREAD =================================");
    }
    else
    {
        return;
    }
}

void FormCtrl::slot_OilTestThread_Stop()
{
    on_pbtn_xyzStop_clicked();
    ui->pbtn_cdoTest->setText("开始测试");
    ui->pbtn_cdoTest->setEnabled(true);
    _LOG("{Oil_Auto_Test}: THREAD FINISH END=================================");
}
/*
 * ******************************************************跟随测试模块******************************************************
 */
bool FormCtrl::slot_TriggerParameter_Check()
{
    ST_TRIG_SETTASK_INFO trigUiInfo;
    bool ok;
    trigUiInfo.m_u8TrigObj = 0;//触发测距
    if(ui->rbtn_TriggerX->isChecked())
        trigUiInfo.m_eAxis = emAxis_X;
    else if(ui->rbtn_TriggerY->isChecked())
        trigUiInfo.m_eAxis = emAxis_Y;
    else
        trigUiInfo.m_eAxis = emAxis_Z;

    if(!ui->ledit_TriggerDistance->text().isEmpty())
    {
        trigUiInfo.m_u16Interval = ui->ledit_TriggerDistance->text().toInt(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("触发间距输入不合法"));
            return false;
        }
    }
    else
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("请输入触发间距"));
        return false;
    }

//    if( (trigUiInfo.m_u8TrigObj == m_pActionTriggerSet->m_stTaskToSend.m_u8TrigObj) &&
//            (trigUiInfo.m_eAxis == m_pActionTriggerSet->m_stTaskToSend.m_eAxis) &&
//            (trigUiInfo.m_u16Interval == m_pActionTriggerSet->m_stTaskToSend.m_u16Interval) )
//    {
//        _LOG(QString("same task"));
//        return false;
//    }

//    ui->pbtn_TriggerStart->setDisabled(true);
    //非重复任务
    m_pActionTriggerSet->m_stTaskToSend.m_u8TrigObj = trigUiInfo.m_u8TrigObj;
    m_pActionTriggerSet->m_stTaskToSend.m_eAxis = trigUiInfo.m_eAxis;
    m_pActionTriggerSet->m_stTaskToSend.m_u16Interval = trigUiInfo.m_u16Interval;

    return true;
//    _LOG(QString("task is set"));

//    m_pActionTriggerSet->setTaskSend();
}
void FormCtrl::on_pbtn_TriggerStart_clicked()
{
    if("开始触发" == ui->pbtn_TriggerStart->text())
    {
        //初始化环境检查
        if(!m_pActionMotorXYZ->isAllLineHasLocated())
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请完成机械臂定位"));
            return;
        }
        if(!formCeju->cejuClient->m_bIsCejuInitSucceed)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("请等待测距初始化完成"));
            return;
        }
        if(!slot_TriggerParameter_Check())
        {
            return;
        }
        bool okySt = true;
        bool okyEd = true;
        m_pActionBFollowMove->m_stAimStPhyPos.m_i32X = WK_PhyPosNotLimit;
        m_pActionBFollowMove->m_stAimStPhyPos.m_i32Y = WK_PhyPosNotLimit;
        m_pActionBFollowMove->m_stAimStPhyPos.m_i32Z = WK_PhyPosNotLimit;

        m_pActionBFollowMove->m_stAimEdPhyPos.m_i32X = WK_PhyPosNotLimit;
        m_pActionBFollowMove->m_stAimEdPhyPos.m_i32Y = WK_PhyPosNotLimit;
        m_pActionBFollowMove->m_stAimEdPhyPos.m_i32Z = WK_PhyPosNotLimit;
        if(!ui->ledit_TriggerStPos->text().isEmpty())
            m_pActionBFollowMove->m_stAimStPhyPos.m_i32Y = ui->ledit_TriggerStPos->text().toInt(&okySt);
        if(!ui->ledit_TriggerEdPos->text().isEmpty())
            m_pActionBFollowMove->m_stAimEdPhyPos.m_i32Y = ui->ledit_TriggerEdPos->text().toInt(&okyEd);
        if(!okySt || !okyEd)
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("触发起始或结束地址输入不合法"));
            return;
        }
        if(!m_pActionMotorXYZ->isAimPhyPosOverLimit(m_pActionBFollowMove->m_stAimStPhyPos) ||
                !m_pActionMotorXYZ->isAimPhyPosOverLimit(m_pActionBFollowMove->m_stAimEdPhyPos))
        {
            QMessageBox::warning(this, tr("警告对话框"), tr("触发起始或结束地址输入超边界"));
            return;
        }

        //启动测距触发线程
        m_pActionBFollowMove->m_bNeedStop = false;
        _LOG("{Follow_Auto_Test}: THREAD START =================================");
        m_pActionBFollowMove->start();
        ui->pbtn_TriggerStart->setText("停止触发");
    }
    else if("停止触发" == ui->pbtn_TriggerStart->text())
    {
        ui->pbtn_TriggerStart->setDisabled(true);
        m_pActionBFollowMove->m_bNeedStop = true;
        _LOG("{Follow_Auto_Test}: STOP THREAD =================================");
        ui->pbtn_TriggerStart->setText("正在停止触发");
    }
    else
    {
        return;
    }
}

void FormCtrl::slot_FollowTestThread_Stop()
{
    on_pbtn_xyzStop_clicked();
    ui->pbtn_TriggerStart->setEnabled(true);
    _LOG("{Follow_Auto_Test}: THREAD FINISH END=================================");
    ui->pbtn_TriggerStart->setText("开始触发");
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
//    ui->lab_ErrCodeX->setText(QString("%1").arg(m_pActionMotorXYZ->m_stDTaskInfo.m_u32XErrCode));
//    ui->lab_ErrCodeY->setText(QString("%1").arg(m_pActionMotorXYZ->m_stDTaskInfo.m_u32YErrCode));
    ui->lab_ErrCodeX->setNum((int)m_pActionMotorXYZ->m_stDTaskInfo.m_u32YErrCode);
    ui->lab_ErrCodeY->setNum((int)m_pActionMotorXYZ->m_stDTaskInfo.m_u32YErrCode);
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

void FormCtrl::slot_TriggerSet_UiUpdate()
{
//    ui->pbtn_TriggerStart->setEnabled(true);
}
void FormCtrl::slot_CleanOil_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emCleanOilSelute = QMetaEnum::fromType<ActionCleanOil::E_SELUTE_CLEANOIL>();
    ui->lab_COcolor->setText(QString(emCleanOilSelute.valueToKey(m_pActionCleanOil->m_stTaskD.m_eTaskSeluteD)));
    if((ActionCleanOil::NoErr == m_pActionCleanOil->m_stTaskD.m_eTaskSeluteD) || (ActionCleanOil::Outtime == m_pActionCleanOil->m_stTaskD.m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionCleanOil->m_stTaskD.m_eTaskStatusD)
            ui->lab_COcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionCleanOil->m_stTaskD.m_eTaskStatusD)
            ui->lab_COcolor->setStyleSheet("background-color:green");
        else
            ui->lab_COcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_COcolor->setStyleSheet("background-color:red");
    }
}
void FormCtrl::slot_DripOil_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emDripOilSelute = QMetaEnum::fromType<ActionDripOil::E_SELUTE_DRIPOIL>();
    ui->lab_DOcolor->setText(QString(emDripOilSelute.valueToKey(m_pActionDripOil->m_stDTaskInfo.m_eTaskSeluteD)));
    if((ActionDripOil::NoErr == m_pActionDripOil->m_stDTaskInfo.m_eTaskSeluteD) || (ActionDripOil::Outtime == m_pActionDripOil->m_stDTaskInfo.m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionDripOil->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_DOcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionDripOil->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_DOcolor->setStyleSheet("background-color:green");
        else
            ui->lab_DOcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_DOcolor->setStyleSheet("background-color:red");
    }
}
void FormCtrl::slot_Claw_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emClawSelute = QMetaEnum::fromType<ActionClaw::E_SELUTE_CLAW>();
    ui->lab_CWcolor->setText(QString(emClawSelute.valueToKey(m_pActionClaw->m_stDTaskInfo.m_eTaskSeluteD)));
    if(emSlide_In == m_pActionClaw->m_stDTaskInfo.m_u8IsSlideIn)
        ui->lab_CWslide->setText("有");
    else
        ui->lab_CWslide->setText("无");
    ui->lab_CWlen->setText(QString("0-%1").arg(m_pActionClaw->m_stDTaskInfo.m_u16MaxOpenPos));
    ui->lab_CWpos->setText(QString("%1").arg(m_pActionClaw->m_stDTaskInfo.m_i32CurPosD));
    if((ActionClaw::NoErr == m_pActionClaw->m_stDTaskInfo.m_eTaskSeluteD) || (ActionClaw::Outtime == m_pActionClaw->m_stDTaskInfo.m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionClaw->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_CWcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionClaw->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_CWcolor->setStyleSheet("background-color:green");
        else
            ui->lab_CWcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_CWcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_QRScan_UiUpdate()
{
    uint8_t u8ScanDataTemp[COM_FIFO_DATA_LENGHT_QRSCAN];
    static emWorkStatus lastStatus = m_pActionQRScan->m_stDTaskInfo.m_eTaskStatusD;
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emQRScanSelute = QMetaEnum::fromType<ActionQRScan::E_SELUTE_QRSCAN>();
    ui->lab_QRcolor->setText(QString(emQRScanSelute.valueToKey(m_pActionQRScan->m_stDTaskInfo.m_eTaskSeluteD)));
    if((ActionQRScan::NoErr == m_pActionQRScan->m_stDTaskInfo.m_eTaskSeluteD) || (ActionQRScan::Outtime == m_pActionQRScan->m_stDTaskInfo.m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionQRScan->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_QRcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionQRScan->m_stDTaskInfo.m_eTaskStatusD)
            ui->lab_QRcolor->setStyleSheet("background-color:green");
        else
        {
            if(emWorkStatus_run == lastStatus)
            {
                m_pActionQRScan->getQRScanContent(u8ScanDataTemp);
                ui->txt_QRdata->setText(QString::fromLocal8Bit((const char *)u8ScanDataTemp, COM_FIFO_DATA_LENGHT_QRSCAN));
            }
            ui->lab_QRcolor->setStyleSheet("background-color:transparent");
        }
    }
    else
    {
        ui->lab_QRcolor->setStyleSheet("background-color:red");
    }
    lastStatus = m_pActionQRScan->m_stDTaskInfo.m_eTaskStatusD;
}

void FormCtrl::slot_Light_UiUpdate()
{
    //刷新页面数据 m_eTaskStatusD 和 m_eTaskSeluteD
    QMetaEnum emLightSelute = QMetaEnum::fromType<ActionLight::E_SELUTE_LIGHT>();
    ui->lab_LTcolor->setText(QString(emLightSelute.valueToKey(m_pActionLight->m_stTaskD.m_eTaskSeluteD)));
    ui->lab_LTlight->setText(QString("%1,%2").arg(m_pActionLight->m_stTaskD.m_u16Level1).arg(m_pActionLight->m_stTaskD.m_u16Level2));
    if((ActionLight::NoErr == m_pActionLight->m_stTaskD.m_eTaskSeluteD) || (ActionLight::Outtime == m_pActionLight->m_stTaskD.m_eTaskSeluteD))
    {
        if(emWorkStatus_ask == m_pActionLight->m_stTaskD.m_eTaskStatusD)
            ui->lab_LTcolor->setStyleSheet("background-color:orange");
        else if (emWorkStatus_run == m_pActionLight->m_stTaskD.m_eTaskStatusD)
            ui->lab_LTcolor->setStyleSheet("background-color:green");
        else
            ui->lab_LTcolor->setStyleSheet("background-color:transparent");
    }
    else
    {
        ui->lab_LTcolor->setStyleSheet("background-color:red");
    }
}

void FormCtrl::slot_SlideInfo_UiUpdate()
{
    ST_MOTORV_SLIDEINFO_D m_stSlideInfoTemp;
    m_pActionParameterSet->getMotorVSlideData(m_stSlideInfoTemp);
    SfScan_SlideInfo_Show(m_stSlideInfoTemp);
}

void FormCtrl::SfScan_SlideInfo_Show(ST_MOTORV_SLIDEINFO_D &stData)
{
    uint8_t m_u8Edge = SmearCoverFlagTurnOff;
    int32_t m_i32Pos = 48987;

    QStringList headText;
    headText << "边沿" << "等待列" << "边沿" << "完成列";
    QList<int> columnWidth;
    columnWidth << 50 << 80 << 50 << 80;

    int columnCount = headText.count();
    ui->tbw_SlideInfo->setColumnCount(columnCount);
    ui->tbw_SlideInfo->setHorizontalHeaderLabels(headText);
    ui->tbw_SlideInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_SlideInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbw_SlideInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_SlideInfo->verticalHeader()->setVisible(true);
    ui->tbw_SlideInfo->horizontalHeader()->setStretchLastSection(false);
    ui->tbw_SlideInfo->horizontalHeader()->setHighlightSections(false);
    ui->tbw_SlideInfo->verticalHeader()->setDefaultSectionSize(20);
    ui->tbw_SlideInfo->verticalHeader()->setHighlightSections(false);
    for (int i = 0; i < columnCount; i++) {
        ui->tbw_SlideInfo->setColumnWidth(i, columnWidth.at(i));
    }

    uint8_t waitNum = 0;//等待列沿的数量
    uint8_t FinishNum = 0;//完成列沿的数量
    uint8_t RowCount = 0;//取完成和等待中大的那个作为表格总行数
    uint8_t SlideNum = 0;
    for (int i=stData.u8BgnIdx; i<stData.u8GetNum; i++)
    {
        if(SmearBoxColIdWait == stData.stSlidePos[i].m_u8insex)
            waitNum++;
        else if(SmearBoxColIdFinish == stData.stSlidePos[i].m_u8insex)
            FinishNum++;
        else
            return;
    }
    RowCount = (waitNum >= FinishNum)?waitNum:FinishNum;
    ui->tbw_SlideInfo->setRowCount(RowCount);

    uint8_t row_finish = 0;
    uint8_t row_wait = 0;
    ST_MOTORV_SLIDE_CAC stSlideCac[2][100];
    int32_t i32thick[2][50];

    for (int i=stData.u8BgnIdx; i<stData.u8GetNum; i++)
    {
        QTableWidgetItem *edgeWait = new QTableWidgetItem;
        if(SmearCoverFlagTurnOff == stData.stSlidePos[i].m_u8ztoo)
            edgeWait->setText("下沿");
        else if(SmearCoverFlagTurnOn == stData.stSlidePos[i].m_u8ztoo)
            edgeWait->setText("上沿");
        else
            edgeWait->setText("异常");

        QTableWidgetItem *posWait = new QTableWidgetItem;
        posWait->setText(QString::number(stData.stSlidePos[i].m_i32pos));

        edgeWait->setTextAlignment(Qt::AlignCenter);
        posWait->setTextAlignment(Qt::AlignCenter);

        if(SmearBoxColIdWait == stData.stSlidePos[i].m_u8insex)
        {
            stSlideCac[0][row_wait].m_i32pos = stData.stSlidePos[i].m_i32pos;
            stSlideCac[0][row_wait].m_u8ztoo = stData.stSlidePos[i].m_u8ztoo;
            ui->tbw_SlideInfo->setItem(row_wait, 0, edgeWait);
            ui->tbw_SlideInfo->setItem(row_wait, 1, posWait);
            row_wait++;
        }
        else
        {
            stSlideCac[1][row_finish].m_i32pos = stData.stSlidePos[i].m_i32pos;
            stSlideCac[1][row_finish].m_u8ztoo = stData.stSlidePos[i].m_u8ztoo;
            ui->tbw_SlideInfo->setItem(row_finish, 2, edgeWait);
            ui->tbw_SlideInfo->setItem(row_finish, 3, posWait);
            row_finish++;
        }


        if (i % 100 == 0) {
            qApp->processEvents();
        }
    }


    //算厚度
    uint8_t waitThick_num =0;
    uint8_t FinishThick_num =0;
    for(uint8_t j=0; j<2; j++)
    {//j表示当前操作列是等待列还是完成列， 0是wait，1是finish
        uint8_t maxNum = (0==j)?waitNum:FinishNum;
        uint8_t isGetUp = 0;//是否得到过上沿
        uint8_t thick_up = 0;//上一个上沿在数组中的下标
        uint8_t thick_j = 0;//厚度数组下标
        for(uint8_t i=0; i<maxNum; i++)
        {
            if(SmearCoverFlagTurnOn == stSlideCac[j][i].m_u8ztoo)
            {
                isGetUp = 1;
                thick_up = i;
            }
            else
            {
                if(1  == isGetUp)
                {
                    //坐标值从上到下逐渐变大，所以电机从下往上扫描过程中，对应玻片盒子内的玻片边沿从上往下坐标值依次减小，即下沿<上沿
                    i32thick[j][thick_j] = abs(stSlideCac[j][i].m_i32pos - stSlideCac[j][thick_up].m_i32pos);//下沿减去上沿
                    isGetUp = 0;
                    thick_j++;
                }
            }
        }
        if(0 == j)
            waitThick_num = thick_j;
        else
            FinishThick_num = thick_j;
    }

    QStringList headCnt;
    headCnt << "等待厚度" << "完成厚度";
    QList<int> columnWidthCnt;
    columnWidthCnt << 80 << 80;
    columnCount = headCnt.count();

    ui->tbw_SlideCnt->setColumnCount(columnCount);
    ui->tbw_SlideCnt->setHorizontalHeaderLabels(headCnt);
    ui->tbw_SlideCnt->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbw_SlideCnt->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbw_SlideCnt->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbw_SlideCnt->verticalHeader()->setVisible(true);
    ui->tbw_SlideCnt->horizontalHeader()->setStretchLastSection(false);
    ui->tbw_SlideCnt->horizontalHeader()->setHighlightSections(false);
    ui->tbw_SlideCnt->verticalHeader()->setDefaultSectionSize(20);
    ui->tbw_SlideCnt->verticalHeader()->setHighlightSections(false);
    for (int i = 0; i < columnCount; i++) {
        ui->tbw_SlideCnt->setColumnWidth(i, columnWidthCnt.at(i));
    }

    SlideNum = RowCount>>1;
    ui->tbw_SlideCnt->setRowCount(SlideNum);
    for(uint8_t j=0; j<2; j++)
    {
        uint8_t maxNum = (0==j)?waitThick_num:FinishThick_num;
        for (int i = 0; i < maxNum; i++)
        {
            QTableWidgetItem *thick = new QTableWidgetItem;
            thick->setText(QString::number(i32thick[j][i]));
            thick->setTextAlignment(Qt::AlignCenter);
            ui->tbw_SlideCnt->setItem(i, j, thick);
            if (i % 100 == 0) {
                qApp->processEvents();
            }
        }
    }
}

void FormCtrl::slot_MVLaser_UiUpdate(uint8_t isSucceed)
{
    if(isSucceed)
    {
        if("打开激光" == ui->pbtn_MVLaser->text())
            ui->pbtn_MVLaser->setText("关闭激光");
        else
            ui->pbtn_MVLaser->setText("打开激光");
    }
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
void FormCtrl::on_rbtn_MVout_clicked()
{
    //检查是否获得坐标
    if( !m_pActionMotorV->isBoxVLineGet() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("请先完成垂直轴定位"));
        return;
    }
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
    if(ui->ledit_MVout->text().isEmpty())
    {
         if(!m_pActionMotorV->getBoxLieDownPos(m_pActionMotorV->m_stTaskToSend.m_fTargetPos))
             return;
    }
    else
    {
        m_pActionMotorV->m_stTaskToSend.m_fTargetPos = ui->ledit_MVout->text().toInt(&ok);
    }
    _LOG(QString("task is set"));
    m_pActionMotorV->setTaskSend();
}

void FormCtrl::on_rbtn_MVmove_clicked()
{
    //检查是否获得坐标
    if( !m_pActionMotorV->isBoxVLineGet() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("请先完成垂直轴定位"));
        return;
    }
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

void FormCtrl::on_pbtn_MVscan_clicked()
{
    //检查是否获得坐标
    if( !m_pActionMotorV->isBoxVLineGet() )
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("请先完成垂直轴定位"));
        return;
    }
    //上一次任务与这次相同，重复任务
    if( (emTaskDMoveBoxVType_slideScan == m_pActionMotorV->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_slideScan;
    m_pActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionMotorV->m_stTaskToSend.m_fScanBgn = ui->ledit_MVstart->text().toInt();
    m_pActionMotorV->m_stTaskToSend.m_fScanEnd = ui->ledit_MVend->text().toInt();
    m_pActionMotorV->m_stTaskToSend.m_fTargetPos = 0;
    _LOG(QString("task is set"));

    m_pActionMotorV->setTaskSend();
}

void FormCtrl::on_pbtn_MVrefresh_clicked()
{
    //非重复任务
    m_pActionParameterSet->m_stMotorVGetSlideInfoSet.u8AskNum = DEFAULT_SLIDE_MAX_SUM;
    m_pActionParameterSet->m_stMotorVGetSlideInfoSet.u8BgnIdx = 0;
//    m_pActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionParameterSet->getMotorVScanInfo();

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


void FormCtrl::on_rbtn_DOdrip_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDDripOil_Drop == m_pActionDripOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_Drop;
    m_pActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionDripOil->setTaskSend();
}


void FormCtrl::on_rbtn_DOdraw_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDDripOil_Draw == m_pActionDripOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_Draw;
    m_pActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionDripOil->setTaskSend();
}


void FormCtrl::on_rbtn_DOdrawback_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDDripOil_DrawBack == m_pActionDripOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_DrawBack;
    m_pActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionDripOil->setTaskSend();
}


void FormCtrl::on_rbtn_DOdripout_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDDripOil_DripOut == m_pActionDripOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_DripOut;
    m_pActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionDripOil->setTaskSend();
}


void FormCtrl::on_pbtn_DOstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDDripOil_Stop == m_pActionDripOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionDripOil->m_stTaskToSend.m_eTaskType = emTaskDDripOil_Stop;
    m_pActionDripOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionDripOil->setTaskSend();
}


void FormCtrl::on_rb_COputpaper_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDCleanOilType_Clean == m_pActionCleanOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Clean;
    m_pActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionCleanOil->setTaskSend();
}


void FormCtrl::on_rb_COfrappaper_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDCleanOilType_Frap == m_pActionCleanOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Frap;
    m_pActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionCleanOil->setTaskSend();
}


void FormCtrl::on_rb_COpoweroff_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDCleanOilType_PowerOff == m_pActionCleanOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_PowerOff;
    m_pActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionCleanOil->setTaskSend();
}


void FormCtrl::on_pbtn_COstop_clicked()
{
    //上一次任务与这次相同，重复任务
    if( (emTaskDCleanOilType_Stop == m_pActionCleanOil->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionCleanOil->m_stTaskToSend.m_eTaskType = emTaskDCleanOilType_Stop;
    m_pActionCleanOil->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionCleanOil->setTaskSend();
}


void FormCtrl::on_pbtn_CWctrl_clicked()
{
    bool okx = true;
    uint16_t m_u16ClawPosTemp = 0;
    uint8_t m_u8NeedBrake = 0;
    if(!m_pActionClaw->moveEnvironmentCheck())
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("夹爪玻片上电信息未同步"));
        return;
    }
    if(ui->ledit_CWpos->text().isEmpty())
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("夹爪目标位置不能为空"));
        return;
    }
    m_u16ClawPosTemp = ui->ledit_CWpos->text().toUInt(&okx);
    if(!okx)
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("坐标输入有误"));
        return;
    }
    if(!m_pActionClaw->movePosIsValid(m_u16ClawPosTemp))
    {
        QMessageBox::warning(this, tr("警告对话框"), tr("坐标输入超边界"));
        return;
    }

    m_u8NeedBrake = ui->cb_CWbrake->isChecked();
    //检查是否重复任务
    if( (emTaskDGripperType_Control == m_pActionClaw->m_stTaskToSend.m_eTaskType) &&
            (m_u16ClawPosTemp == m_pActionClaw->m_stTaskToSend.m_AimPos) &&
            (m_u8NeedBrake == m_pActionClaw->m_stTaskToSend.m_u8IsBrake) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionClaw->m_stTaskToSend.m_eTaskType = emTaskDGripperType_Control;
    m_pActionClaw->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionClaw->m_stTaskToSend.m_AimPos = m_u16ClawPosTemp;
    m_pActionClaw->m_stTaskToSend.m_u8IsBrake = m_u8NeedBrake;
    _LOG(QString("task is set"));

    m_pActionClaw->setTaskSend();
}

void FormCtrl::on_rbtn_CWslideno_clicked(bool checked)
{
    //检查是否重复任务
    if( (emTaskDGripperType_SynSlide == m_pActionClaw->m_stTaskToSend.m_eTaskType) &&
            (emSlide_No == m_pActionClaw->m_stTaskToSend.m_u8IsSlideIn))
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionClaw->m_stTaskToSend.m_eTaskType = emTaskDGripperType_SynSlide;
    m_pActionClaw->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionClaw->m_stTaskToSend.m_u8IsSlideIn = emSlide_No;
    _LOG(QString("task is set"));

    m_pActionClaw->setTaskSend();
}

void FormCtrl::on_rbtn_CWslidein_clicked(bool checked)
{
    //检查是否重复任务
    if( (emTaskDGripperType_SynSlide == m_pActionClaw->m_stTaskToSend.m_eTaskType) &&
            (emSlide_In == m_pActionClaw->m_stTaskToSend.m_u8IsSlideIn))
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionClaw->m_stTaskToSend.m_eTaskType = emTaskDGripperType_SynSlide;
    m_pActionClaw->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    m_pActionClaw->m_stTaskToSend.m_u8IsSlideIn = emSlide_In;
    _LOG(QString("task is set"));

    m_pActionClaw->setTaskSend();
}

void FormCtrl::on_pbtn_motorOn_clicked()
{
    emTaskDXYZType emTaskTemp;

    if(ui->rbtn_selX->isChecked())
        emTaskTemp = emTskDXYZType_enableAxisX;
    else if(ui->rbtn_selY->isChecked())
        emTaskTemp = emTskDXYZType_enableAxisY;
    else
        return;

    //上一次任务与这次相同，重复任务
    if( (emTaskTemp == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTaskTemp;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}


void FormCtrl::on_pbtn_motorOff_clicked()
{
    emTaskDXYZType emTaskTemp;

    if(ui->rbtn_selX->isChecked())
        emTaskTemp = emTskDXYZType_disableAxisX;
    else if(ui->rbtn_selY->isChecked())
        emTaskTemp = emTskDXYZType_disableAxisY;
    else
        return;

    //上一次任务与这次相同，重复任务
    if( (emTaskTemp == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTaskTemp;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}

void FormCtrl::on_pbtn_motorClear_clicked()
{
    emTaskDXYZType emTaskTemp;

    if(ui->rbtn_selX->isChecked())
        emTaskTemp = emTskDXYZType_clearXAlarm;
    else if(ui->rbtn_selY->isChecked())
        emTaskTemp = emTskDXYZType_clearYAlarm;
    else
        return;

    //上一次任务与这次相同，重复任务
    if( (emTaskTemp == m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionMotorXYZ->m_stTaskToSend.m_eTaskType = emTaskTemp;
    m_pActionMotorXYZ->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionMotorXYZ->setTaskSend();
}

void FormCtrl::on_pbtn_QRscan_clicked()
{
    //上一次任务与这次相同，重复任务
//    if( (emTaskDQRScanner_Scan == m_pActionQRScan->m_stTaskToSend.m_eTaskType) )
//    {
//        _LOG(QString("same task"));
//        return;
//    }
    //非重复任务
    m_pActionQRScan->m_stTaskToSend.m_eTaskType = emTaskDQRScanner_Scan;
    m_pActionQRScan->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
    _LOG(QString("task is set"));

    m_pActionQRScan->setTaskSend();
}


void FormCtrl::on_pbtn_LTOpen_clicked()
{
    ST_LIGHT_SETTASK_INFO stTemp;
    stTemp.m_eTaskType = emTaskDSetLED_Control;
    stTemp.m_uIsTri1 = DISABLE_COOL_LED_TRI;
    stTemp.m_uIsTri2 = DISABLE_COOL_LED_TRI;
    stTemp.m_u16Tri1Time = LIGHT_TRI_TIME_DEFAULT;
    stTemp.m_u16Tri2Time = LIGHT_TRI_TIME_DEFAULT;

    stTemp.m_u16Level1 = (ui->cb_LT1->isChecked())?ui->hsd_LT1->value():0;
    stTemp.m_u16Level2 = (ui->cb_LT2->isChecked())?ui->hsd_LT2->value():0;
    ui->spb_LT1->setValue(stTemp.m_u16Level1);
    ui->spb_LT2->setValue(stTemp.m_u16Level2);//相当于hsd值改变的槽

    //检查是否重复任务
    if( (stTemp.m_eTaskType == m_pActionLight->m_stTaskToSend.m_eTaskType)
            && (stTemp.m_u16Level1 == m_pActionLight->m_stTaskToSend.m_u16Level1)
            && (stTemp.m_u16Level2 == m_pActionLight->m_stTaskToSend.m_u16Level2) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionLight->m_stTaskToSend = stTemp;
    _LOG(QString("task is set"));

    m_pActionLight->setTaskSend();
}


void FormCtrl::on_pbtn_LTClose_clicked()
{
    ST_LIGHT_SETTASK_INFO stTemp;
    stTemp.m_eTaskType = emTaskDSetLED_Control;
    stTemp.m_uIsTri1 = DISABLE_COOL_LED_TRI;
    stTemp.m_uIsTri2 = DISABLE_COOL_LED_TRI;
    stTemp.m_u16Tri1Time = LIGHT_TRI_TIME_DEFAULT;
    stTemp.m_u16Tri2Time = LIGHT_TRI_TIME_DEFAULT;
    if(ui->cb_LT1->isChecked())
        stTemp.m_u16Level1 = 0;
    else
        stTemp.m_u16Level1 = m_pActionLight->m_stTaskToSend.m_u16Level1;
    if(ui->cb_LT2->isChecked())
        stTemp.m_u16Level2 = 0;
    else
        stTemp.m_u16Level2 = m_pActionLight->m_stTaskToSend.m_u16Level2;

    //检查是否重复任务
    if( (stTemp.m_eTaskType == m_pActionLight->m_stTaskToSend.m_eTaskType)
            && (stTemp.m_u16Level1 == m_pActionLight->m_stTaskToSend.m_u16Level1)
            && (stTemp.m_u16Level2 == m_pActionLight->m_stTaskToSend.m_u16Level2) )
    {
        _LOG(QString("same task"));
        return;
    }
    //非重复任务
    m_pActionLight->m_stTaskToSend = stTemp;
    _LOG(QString("task is set"));

    m_pActionLight->setTaskSend();
}


void FormCtrl::on_pbtn_MVLaser_clicked()
{
    if("打开激光" == ui->pbtn_MVLaser->text())
        m_pActionParameterSet->m_u8IsOpenLaser = 1;
    else
        m_pActionParameterSet->m_u8IsOpenLaser = 0;

    m_pActionParameterSet->setMotorVLaser();
}

