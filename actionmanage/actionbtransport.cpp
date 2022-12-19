#include "actionbtransport.h"
#include "quihelper.h"
#include "global.h"

ActionBTransport::ActionBTransport(ActionBeltIn* &objActionBeltIn, ActionBeltOut* &objActionBeltOut,
                                   ActionPoleIn* &objActionPoleIn, ActionPoleOut* &m_objActionPoleOut,
                                   ActionUpender* &objActionUpender, ActionMotorV* &objActionMotorV, QObject *parent)
    : m_objActionBeltIn(objActionBeltIn),m_objActionBeltOut(objActionBeltOut),m_objActionPoleIn(objActionPoleIn),
      m_objActionPoleOut(m_objActionPoleOut),m_objActionUpender(objActionUpender),m_objActionMotorV(objActionMotorV),
      HardCmdParser(), QThread(parent)
{
    memset(m_eListAllSensorStatus, emSensorStatus_Start, (emSensorId_End+1));
    HardCmdParseAgent::GetInstance().registerParser(EnumBoardId_getAllSensors, this);
}


void ActionBTransport::run()
{
    while(!m_bNeedStop)
    {
        msleep(1000);
        _LOG("{Trans_Auto_Test}: THREAD IN =================================");
        // BeltIn
        if( (emWorkStatus_finish == m_objActionBeltIn->m_eTaskStatusD) &&
                ((emSeluteDBelt_NoErr == m_objActionBeltIn->m_eTaskSeluteD) || (emSeluteDBelt_Outtime == m_objActionBeltIn->m_eTaskSeluteD)) )
        {//入皮带为停止，无任务在进行
            if( (emWorkStatus_finish == m_objActionPoleIn->m_eTaskStatusD) &&
                    ((emSeluteDPole_NoErr == m_objActionPoleIn->m_eTaskSeluteD)||(emSeluteDPole_Outtime == m_objActionPoleIn->m_eTaskSeluteD)) &&
                    (ActionPoleIn::emPosition_One == m_objActionPoleIn->m_eCurPos) )
            {//入推杆在起点，且入推杆无任务
                if( ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_Touch1])||(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_Touch2])) &&
                        ((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos3])||(emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos2])||(emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos1])) )
                {//入皮带终点没有盒子
                    m_objActionBeltIn->m_stTaskToSend.m_eTaskType = emTskDBeltInType_InSideMoving;
                    m_objActionBeltIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                    m_objActionBeltIn->m_stTaskToSend.m_bNeedQR = false;
                    //push到待发送队列
                    m_objActionBeltIn->setTaskSend();
                    _LOG("{Trans_Auto_Test}: BeltIn InSideMoving");
                    continue;
                }
            }
        }

        // PoleIn
        if( (emWorkStatus_finish == m_objActionPoleIn->m_eTaskStatusD) &&
                ((emSeluteDPole_NoErr == m_objActionPoleIn->m_eTaskSeluteD) || (emSeluteDPole_Outtime == m_objActionPoleIn->m_eTaskSeluteD)) )
        {//入推杆停止，且入推杆无任务
            if( (emWorkStatus_finish == m_objActionBeltIn->m_eTaskStatusD) &&
                     ((emSeluteDBelt_NoErr == m_objActionBeltIn->m_eTaskSeluteD) || (emSeluteDBelt_Outtime == m_objActionBeltIn->m_eTaskSeluteD)) )
             {//入皮带为停止，无任务在进行
                if( (ActionPoleIn::emPosition_One != m_objActionPoleIn->m_eCurPos) )
                {//入推杆不在起点
                    m_objActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_Out;
                    m_objActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                    //push到待发送队列
                    m_objActionPoleIn->setTaskSend();
                    _LOG("{Trans_Auto_Test}: PoleIn Out");
                    continue;
                }
                else
                {//入推杆在起点
                    if( (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) )
                    {//垂直盒在上
                        if( (emWorkStatus_finish == m_objActionUpender->m_eTaskStatusD) && (ActionUpender::emPosition_One == m_objActionUpender->m_eCurPos))
                        {//翻转盒停止,且翻转盒平躺
                            if( ((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos3]) || (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos4]) ||
                                 (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos5]) || (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos6]) ||
                                 (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos7]) || (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist]))&&
                                    ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_HatchTouch1])||(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_HatchTouch2])) )
                            {//有盒子，且盒子没到位
                                m_objActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_In;
                                m_objActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                                //push到待发送队列
                                m_objActionPoleIn->setTaskSend();
                                _LOG("{Trans_Auto_Test}: VBox is Up, PoleIn In");
                                continue;
                            }
                            //往下走等盒子平躺
                        }
                    }
                    else if( (((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVDown]) && (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVUp])) ||
                              ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVDown]) && (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVUp]))) &&
                             (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) )
                    {//垂直盒在中间，垂直盒在下
                        if( ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_InPos7])||(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_InPos6])||
                             (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_InPos5])||(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_InPos4])) &&
                                ((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_InPos3]) && (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_Touch1]) &&
                                 (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_Touch2])) )
                        {//入推杆起点有盒，且盒子到位，终点没盒
                            m_objActionPoleIn->m_stTaskToSend.m_eTaskType = emTskDPoleInType_In;
                            m_objActionPoleIn->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionPoleIn->setTaskSend();
                            _LOG("{Trans_Auto_Test}: VBox is Down, PoleIn In");
                            continue;
                        }
                    }
                    //垂直盒不满足推杆移动条件
                }
            }
        }

        // 翻转盒
        if( (emWorkStatus_finish == m_objActionUpender->m_eTaskStatusD) &&
                ((emSeluteDUpender_NoErr == m_objActionUpender->m_eTaskSeluteD) || (emSeluteDUpender_OutTime == m_objActionUpender->m_eTaskSeluteD)) )
        {//翻转盒停止，且翻转盒无任务
            _LOG("{Trans_Auto_Test <Upender>}: Upender finish,no err");
            if( (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) &&
                    ((emSeluteDMoveBoxV_NoErr == m_objActionMotorV->m_eTaskSeluteD) || (emSeluteDMoveBoxV_Outtime == m_objActionMotorV->m_eTaskSeluteD)) )
            {//垂直轴为停止，无任务在进行
                _LOG("{Trans_Auto_Test <Upender>}: MotorV finish,no err");
                if((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVDown]))
                {//垂直轴在上
                    _LOG("{Trans_Auto_Test <Upender>}: MotorV is up");
                    if( (emWorkStatus_finish == m_objActionPoleIn->m_eTaskStatusD) &&
                            ((emSeluteDPole_NoErr == m_objActionPoleIn->m_eTaskSeluteD) || (emSeluteDPole_Outtime == m_objActionPoleIn->m_eTaskSeluteD)) &&
                            (ActionPoleIn::emPosition_One == m_objActionPoleIn->m_eCurPos) )
                    {//入仓推杆在起点，且入仓推杆无任务
                        _LOG("{Trans_Auto_Test <Upender>}: PoleIn is in startpos, finish,no err");
                        if((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_HatchExist])&&(ActionUpender::emPosition_One != m_objActionUpender->m_eCurPos))
                        {//仓内没盒子，且盒子不是平躺
                            m_objActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_LieDown;
                            m_objActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionUpender->setTaskSend();
                            _LOG("{Trans_Auto_Test}: VBox is Up, Upender LieDown start!");
                            continue;
                        }
                        else if((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist])&&(ActionUpender::emPosition_Five != m_objActionUpender->m_eCurPos))
                        {//仓内有盒子，且盒子不是站立
                            m_objActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_StandUp;
                            m_objActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionUpender->setTaskSend();
                            _LOG("{Trans_Auto_Test}: VBox is Up, Upender StandUp start!");
                            continue;
                        }
                    }
                }
                else if((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVDown]))
                {//垂直轴在下
                    _LOG("{Trans_Auto_Test <Upender>}: MotorV is down");
                    if( (emWorkStatus_finish == m_objActionPoleOut->m_eTaskStatusD) &&
                            ((emSeluteDPole_NoErr == m_objActionPoleOut->m_eTaskSeluteD) || (emSeluteDPole_Outtime == m_objActionPoleOut->m_eTaskSeluteD)) &&
                            (ActionPoleOut::emPosition_Two == m_objActionPoleOut->m_eCurPos) )
                    {//出仓推杆在起点，且出仓推杆无任务
                        _LOG("{Trans_Auto_Test <Upender>}: PoleOut is in startpos, finish,no err");
                        if((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist])&&(ActionUpender::emPosition_One != m_objActionUpender->m_eCurPos))
                        {//仓内有盒子，且盒子不是平躺
                            m_objActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_LieDown;
                            m_objActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionUpender->setTaskSend();
                            _LOG("{Trans_Auto_Test <Upender>}: VBox is Down, Upender LieDown start!");
                            continue;
                        }
                        if((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_HatchExist])&&(ActionUpender::emPosition_Five != m_objActionUpender->m_eCurPos))
                        {//仓内没盒子，且盒子不是站立
                            m_objActionUpender->m_stTaskToSend.m_eTaskType = emTskDUpenderType_StandUp;
                            m_objActionUpender->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionUpender->setTaskSend();
                            _LOG("{Trans_Auto_Test <Upender>}: VBox is Down, Upender StandUp start!");
                            continue;
                        }
                    }
                }
                //等待垂直轴修复位置
            }
        }

        //垂直轴
        if( (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) &&
                ((emSeluteDMoveBoxV_NoErr == m_objActionMotorV->m_eTaskSeluteD) || (emSeluteDMoveBoxV_Outtime == m_objActionMotorV->m_eTaskSeluteD)) )
        {//垂直轴为停止，无任务在进行
            _LOG("{Trans_Auto_Test <MotorV>}: MotorV finish,no err");
            if( (emWorkStatus_finish == m_objActionUpender->m_eTaskStatusD) &&
                    ((emSeluteDUpender_NoErr == m_objActionUpender->m_eTaskSeluteD) || (emSeluteDUpender_OutTime == m_objActionUpender->m_eTaskSeluteD)) &&
                    ((ActionUpender::emPosition_Five == m_objActionUpender->m_eCurPos)||(ActionUpender::emPosition_Four == m_objActionUpender->m_eCurPos)) )
            {//翻转盒停止，翻转盒站立，且翻转盒无任务
                _LOG("{Trans_Auto_Test <MotorV>}: Upender is StandUp, finish,no err");
                if((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVDown]))
                {//垂直轴在上
                    _LOG("{Trans_Auto_Test <MotorV>}: MotorV is Up");
                    if(emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist])
                    {
                        m_objActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                        if(m_objActionMotorV->getBoxLieDownPos(m_objActionMotorV->m_stTaskToSend.m_fTargetPos))
                        {//完成较零，移动到出仓位置
                            m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_move;
                            _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Up, VBox has box, MotorV move to down!");
                        }
                        else
                        {//未较零
                            m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_zeroSFCheck;
                            _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Up, VBox has box, zeroSFCheck! ");
                        }

                        m_objActionMotorV->setTaskSend();
                    }
                }
                else if((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVDown]))
                {//垂直轴在下
                    _LOG("{Trans_Auto_Test <MotorV>}: MotorV is Down");
                    if(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_HatchExist])
                    {//仓内没盒子
                        m_objActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                        m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_zeroSFCheck;//移动到上方

                        m_objActionMotorV->setTaskSend();
                        _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Down, VBox no box, MotorV move to up(zeroSFCheck)!");
                    }
                }
                else if((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_BoxVDown]))
                {
                    _LOG("{Trans_Auto_Test <MotorV>}: MotorV is Middle");
                    if(emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist])
                    {
                        m_objActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                        if(m_objActionMotorV->getBoxLieDownPos(m_objActionMotorV->m_stTaskToSend.m_fTargetPos))
                        {//完成较零，移动到出仓位置
                            m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_move;
                            _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Middle, VBox has box, MotorV move to down!");
                        }
                        else
                        {//未较零
                            m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_zeroSFCheck;
                            _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Middle, VBox has box, zeroSFCheck!");
                        }

                        m_objActionMotorV->setTaskSend();
                    }
                    else
                    {//仓内没盒子
                        m_objActionMotorV->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                        m_objActionMotorV->m_stTaskToSend.m_eTaskType = emTaskDMoveBoxVType_zeroSFCheck;//移动到上方

                        m_objActionMotorV->setTaskSend();
                        _LOG("{Trans_Auto_Test <MotorV>}: MotorV at Middle, VBox no box, MotorV move to up(zeroSFCheck)!");
                    }

                }
                //其它情况自修复
            }
        }

        //出皮带
        if( (emWorkStatus_finish == m_objActionBeltOut->m_eTaskStatusD) &&
                ((emSeluteDBelt_NoErr == m_objActionBeltOut->m_eTaskSeluteD) || (emSeluteDBelt_Outtime == m_objActionBeltOut->m_eTaskSeluteD)) )
        {//出皮带为停止，无任务在进行
            _LOG("{Trans_Auto_Test <BeltOut>}: BeltOut finish,no err");
            if( (emWorkStatus_finish == m_objActionPoleOut->m_eTaskStatusD) &&
                    ((emSeluteDPole_NoErr == m_objActionPoleOut->m_eTaskSeluteD) || (emSeluteDPole_Outtime == m_objActionPoleOut->m_eTaskSeluteD)) &&
                    (ActionPoleOut::emPosition_Two == m_objActionPoleOut->m_eCurPos) )
            {//出推杆在起点，且出推杆无任务
                _LOG("{Trans_Auto_Test <BeltOut>}: PoleOut at start, finish,no err");
                if( ((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos5])||(emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos6])) &&
                        ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos6])||(emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos7])) )
                {//出皮带终点没有盒子
                    m_objActionBeltOut->m_stTaskToSend.m_eTaskType = emTskDBeltOutType_OutSideMoving;
                    m_objActionBeltOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                    //push到待发送队列
                    m_objActionBeltOut->setTaskSend();
                    _LOG("{Trans_Auto_Test <BeltOut>}: BeltOut OutSideMoving");
                    continue;
                }
            }
        }

        //出仓推杆
        if( (emWorkStatus_finish == m_objActionPoleOut->m_eTaskStatusD) &&
                ((emSeluteDPole_NoErr == m_objActionPoleOut->m_eTaskSeluteD) || (emSeluteDPole_Outtime == m_objActionPoleOut->m_eTaskSeluteD)) )
        {//出推杆停止，且出推杆无任务
            _LOG("{Trans_Auto_Test <PoleOut>}: PoleOut finish,no err");
            if( (emWorkStatus_finish == m_objActionBeltOut->m_eTaskStatusD) &&
                     ((emSeluteDBelt_NoErr == m_objActionBeltOut->m_eTaskSeluteD) || (emSeluteDBelt_Outtime == m_objActionBeltOut->m_eTaskSeluteD)) )
             {//出皮带为停止，无任务在进行
                _LOG("{Trans_Auto_Test <PoleOut>}: BeltOut finish,no err");
                if( (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVDown]) && (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) )
                {//垂直盒在下
                    _LOG("{Trans_Auto_Test <PoleOut>}: MotorV is down, finish,no err");
                    if( (emWorkStatus_finish == m_objActionUpender->m_eTaskStatusD) &&
                            ((emSeluteDUpender_NoErr == m_objActionUpender->m_eTaskSeluteD) || (emSeluteDUpender_OutTime == m_objActionUpender->m_eTaskSeluteD)) &&
                            ((ActionUpender::emPosition_Five == m_objActionUpender->m_eCurPos) || (ActionUpender::emPosition_One == m_objActionUpender->m_eCurPos)))
                    {//翻转盒停止，且无任务，且翻转盒为平躺或者站立
                        _LOG("{Trans_Auto_Test <PoleOut>}: Upender, posFive_One, finish,no err");
                        if( (ActionPoleOut::emPosition_Two != m_objActionPoleOut->m_eCurPos) )
                        {//出推杆不在起点
                            m_objActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_In;
                            m_objActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionPoleOut->setTaskSend();
                            _LOG("{Trans_Auto_Test <PoleOut>}: PoleOut not at start, PoleOut In start!");
                            continue;
                        }
                        else
                        {//出推杆在起点
                            if( ((emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos5]) || (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos4]) ||
                                 (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos3]) || (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos2]) ||
                                 (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos1])) &&
                                    (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_HatchExist]) )
                            {
                                m_objActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_Out;
                                m_objActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                                //push到待发送队列
                                m_objActionPoleOut->setTaskSend();
                                _LOG("{Trans_Auto_Test <PoleOut>}: PoleOut at start, PoleOut In start!");
                                continue;
                            }
                        }
                    }
                }
                else if( (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_BoxVUp]) && (emWorkStatus_finish == m_objActionMotorV->m_eTaskStatusD) )
                {//垂直盒子在上
                    _LOG("{Trans_Auto_Test <PoleOut>}: MotorV is up, finish,no err");
                    if( (ActionPoleOut::emPosition_Two != m_objActionPoleOut->m_eCurPos) )
                    {//出推杆不在起点
                        m_objActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_In;
                        m_objActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                        //push到待发送队列
                        m_objActionPoleOut->setTaskSend();
                        _LOG("{Trans_Auto_Test <PoleOut>}: PoleOut not at start, PoleOut In start!");
                        continue;
                    }
                    else
                    {//出推杆在起点
                        if( (emSensorStatus_Off == m_eListAllSensorStatus[emSensorId_OutPos5]) &&
                                ((emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos4]) || (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos3]) ||
                                 (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos2]) || (emSensorStatus_On == m_eListAllSensorStatus[emSensorId_OutPos1])) )
                        {//如果有盒子没到出皮带起点，要推
                            m_objActionPoleOut->m_stTaskToSend.m_eTaskType = emTskDPoleOutType_Out;
                            m_objActionPoleOut->m_stTaskToSend.m_uTaskId = QUIHelper::getRandValue(0, 255, true, true);
                            //push到待发送队列
                            m_objActionPoleOut->setTaskSend();
                            _LOG("{Trans_Auto_Test <PoleOut>}: PoleOut at start, PoleOut In start!");
                            continue;
                        }
                    }
                }
            }
        }
    }
    _LOG("{Trans_Auto_Test}: THREAD FINISHED =================================");
    m_bNeedStop = true;
}
emWKCmdType ActionBTransport::parseCmd(uint8_t* puData)
{
    uint8_t uCmdId = puData[CMD_ID_INDEX];
    if (EnumBoardId_getAllSensors != uCmdId)
    {
        _LOG("not match with parse func[EnumBoardId_getAllSensors]");
        return emCMD_GET;
    }
    // 返回内容的第一个字节为 指令FLAG
    uint32_t uReturnCmdFlag = common_read_u32(&puData[CMD_FLAG_INDEX]);
    uint8_t uLen = CMD_CONTENT_INDEX;

    m_bIsBoard2Connect = (1 == puData[uLen++])?true:false;

    for (uint16_t i=emSensorId_DuiShe1; i<emSensorId_End; i++)
    {
        m_eListAllSensorStatus[i] = (emSensorStatus)puData[uLen];
        uLen++;
    }
    return emCMD_GET;
}

bool ActionBTransport::setTaskCmdReSend(uint32_t sdNum)
{
    return false;
}
