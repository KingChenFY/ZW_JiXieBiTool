#ifndef FORMCTRL_H
#define FORMCTRL_H

#include <QWidget>
#include <QMetaEnum>

#include "formceju.h"
#include "actionbeltin.h"
#include "actionbeltout.h"
#include "actionpolein.h"
#include "actionpoleout.h"
#include "actionupender.h"
#include "actionmotorv.h"
#include "actionmotorxyz.h"
#include "actionbtransport.h"
#include "actionbfollowmove.h"
#include "actiontriggerset.h"
#include "actionparameterset.h"
#include "actioncleanoil.h"
#include "actiondripoil.h"
#include "actionboiltest.h"
#include "actionclaw.h"
#include "actionfourpostest.h"

#include <QSettings>


namespace Ui {
class FormCtrl;
}

class FormCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit FormCtrl(QWidget *parent = nullptr);
    ~FormCtrl();

    void slot_netConnected();
    void slot_netNoLink();

    // 执行状态
    enum E_TASK_STATUS
    {
        emWorkStatus_start,

        emWorkStatus_ask,//已接受
        emWorkStatus_run,//执行中
        emWorkStatus_finish,//动作已完成

        emWorkStatus_end
    };
    Q_ENUM(E_TASK_STATUS)

    FormCeJu *formCeju;

    ActionBeltIn* m_pActionBeltIn;//入舱皮带控制
    ActionBeltOut* m_pActionBeltOut;//出舱皮带控制
    ActionPoleIn* m_pActionPoleIn;//入舱推杆控制
    ActionPoleOut* m_pActionPoleOut;//出舱推杆控制
    ActionUpender* m_pActionUpender;//翻转装置控制
    ActionMotorV* m_pActionMotorV;//垂直扫描轴控制
    ActionMotorXYZ* m_pActionMotorXYZ;//XYZ电机控制
    ActionTriggerSet *m_pActionTriggerSet;//触发控制
    ActionBTransport *m_pActionBTransport;//运输仓测试流程
    ActionBFollowMove *m_pActionBFollowMove;//跟随测试流程
    ActionParameterSet *m_pActionParameterSet;//参数设置流程
    ActionCleanOil *m_pActionCleanOil;//除油流程
    ActionDripOil *m_pActionDripOil;//滴油流程
    ActionBOilTest *m_pActionBOilTest;//油测试流程
    ActionClaw *m_pActionClaw;//夹爪模块控制
    ActionFourPosTest *m_pActionFourPosTest;//四点稳定性测试

private:
    Ui::FormCtrl *ui;

    QMetaEnum emTaskStatus;
    bool m_bIsOilConfigSet;     //用户是否确认滴油除油位置，确认后可使能开始测试按钮
    bool m_bIsFourPointConfigSet;     //用户是否确认滴油除油位置，确认后可使能开始测试按钮

    //ini写入崩溃测试
    QSettings *iniSet;

private slots:
    void slot_TransportTestThread_Stop();
    void slot_FollowTestThread_Stop();
    void slot_OilTestThread_Stop();
    bool slot_TriggerParameter_Check();
    void slot_FourPosTestThread_Stop();
private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void slot_BeltIN_UiUpdate();
    void slot_BeltOUT_UiUpdate();
    void slot_PoleIN_UiUpdate();
    void slot_PoleOUT_UiUpdate();
    void slot_Upender_UiUpdate();
    void slot_MotorV_UiUpdate();
    void slot_MotorXYZ_UiUpdate();
    void slot_TriggerSet_UiUpdate();
    void slot_CleanOil_UiUpdate();
    void slot_DripOil_UiUpdate();
    void slot_Claw_UiUpdate();
private slots:
    //皮带入
    void on_rbtn_BIin_clicked();
    void on_rbtn_BIout_clicked();
    void on_rbtn_BIoff_clicked();
    void on_rbtn_BIstop_clicked();
    //皮带出
    void on_rbtn_BOout_clicked();
    void on_rbtn_BOin_clicked();
    void on_rbtn_BOoff_clicked();
    void on_rbtn_BOstop_clicked();
    //推杆入
    void on_rbtn_PIin_clicked();
    void on_rbtn_PIout_clicked();
    void on_rbtn_PIoff_clicked();
    void on_rbtn_PIstop_clicked();
    //推杆出
    void on_rbtn_POout_clicked();
    void on_rbtn_POin_clicked();
    void on_rbtn_POoff_clicked();
    void on_rbtn_POstop_clicked();
    //翻转装置
    void on_rbtn_UPstand_clicked();
    void on_rbtn_UPlie_clicked();
    void on_pbtn_UPstop_clicked();
    //垂直扫描电机
    void on_rbtn_MVzero_clicked();
    void on_rbtn_MVscan_clicked();
    void on_rbtn_MVmove_clicked();
    void on_pbtn_MVstop_clicked();
    //XYZ模块
    void on_rbtn_XLineZero_clicked();
    void on_rbtn_YLineIn_clicked();
    void on_rbtn_YLineOut_clicked();
    void on_rbtn_ZLineUp_clicked();
    void on_rbtn_ZLineDown_clicked();
    void on_pbtn_xyzStop_clicked();
    void on_pbtn_phyMove_clicked();
    void on_pbtn_phyMoveInTime_clicked();
    void on_pbtn_logicMove_clicked();
    void on_pbtn_logicMoveInTime_clicked();
    void on_pbtn_transportTestStart_clicked();
    void on_pbtn_TriggerStart_clicked();
    //滴油装置
    void on_rbtn_DOdrip_clicked();
    void on_rbtn_DOdraw_clicked();
    void on_rbtn_DOdrawback_clicked();
    void on_rbtn_DOdripout_clicked();
    void on_pbtn_DOstop_clicked();
    //除油装置
    void on_rb_COputpaper_clicked();
    void on_rb_COfrappaper_clicked();
    void on_rb_COpoweroff_clicked();
    void on_pbtn_COstop_clicked();
    //油测试按钮
    void on_pbtn_cdoTest_clicked();
    void on_pbtn_cdoPSet_clicked();
    void on_cb_skipDripOil_clicked(bool checked);
    //夹爪模块
    void on_pbtn_CWctrl_clicked();
    void on_rbtn_CWslideno_clicked(bool checked);
    void on_rbtn_CWslidein_clicked(bool checked);
    void on_pbtn_fourPConfirm_clicked();
    void on_pbtn_fourPTest_clicked();
};

#endif // FORMCTRL_H
