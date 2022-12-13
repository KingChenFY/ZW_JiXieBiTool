#ifndef FORMCTRL_H
#define FORMCTRL_H

#include <QWidget>
#include <QMetaEnum>

#include "actionbeltin.h"
#include "actionbeltout.h"
#include "actionpolein.h"
#include "actionpoleout.h"
#include "actionupender.h"
#include "actionmotorv.h"
#include "actionmotorxyz.h"

namespace Ui {
class FormCtrl;
}

class FormCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit FormCtrl(QWidget *parent = nullptr);
    ~FormCtrl();

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

    ActionBeltIn* m_pActionBeltIn;//入舱皮带控制
    ActionBeltOut* m_pActionBeltOut;//出舱皮带控制
    ActionPoleIn* m_pActionPoleIn;//入舱推杆控制
    ActionPoleOut* m_pActionPoleOut;//出舱推杆控制
    ActionUpender* m_pActionUpender;//翻转装置控制
    ActionMotorV* m_pActionMotorV;//垂直扫描轴控制
    ActionMotorXYZ* m_pActionMotorXYZ;//XYZ电机控制

private:
    Ui::FormCtrl *ui;

    QMetaEnum emTaskStatus;

private slots:
    void initForm();
    void initConfig();
    void slot_BeltIN_UiUpdate();
    void slot_BeltOUT_UiUpdate();
    void slot_PoleIN_UiUpdate();
    void slot_PoleOUT_UiUpdate();
    void slot_Upender_UiUpdate();
    void slot_MotorV_UiUpdate();
    void slot_MotorXYZ_UiUpdate();

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
};

#endif // FORMCTRL_H
