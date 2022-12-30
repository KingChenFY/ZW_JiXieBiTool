#include "gettasklinklist.h"

#include <string>
#include "global.h"
#include "hardcmdparseagent.h"

#include <QDebug>
#include <QMutexLocker>

GetTaskLinkList::GetTaskLinkList()
{
    phead = NULL;
    nodeNum = 0;
}

GetTaskLinkList::~GetTaskLinkList()
{
    pCmdLNode p = phead;
    while(p)
    {
        p = p->next;
        delete(phead);
        phead = p;
    }
}

bool GetTaskLinkList::isEmpty()
{
    if(0 == nodeNum)
        phead = NULL;
    return (0 ==  nodeNum) ? true:false;
}

bool GetTaskLinkList::insertTail(pCmdLNode pNode)
{
    QMutexLocker locker(&mtx);
    if( isEmpty() )
    {
        phead = pNode;
    }
    else
    {
        pCmdLNode p = phead;
        while(p->next)
            p = p->next;
        p->next = pNode;
    }
    nodeNum++;
    return true;
}

bool GetTaskLinkList::app_insertTail(uint8_t taskId, emBoardObject boardObj, uint8_t *data, uint16_t len)
{
    pCmdLNode pNode = new CmdLNode;
    pNode->next = NULL;

    pNode->u8TaskId = taskId;
    pNode->eBoardObj = boardObj;
    pNode->stCmd.sdNum = 0;
    pNode->stCmd.eSendState = emSend_NotSend;
    pNode->stCmd.len = len;
    memcpy(pNode->stCmd.data, data, len);

    return insertTail(pNode);
}
/*
 * 删除指定cmdflag的结点
 */
bool GetTaskLinkList::app_deleteNodeWithTaskId(emBoardObject obj, uint8_t taskId)
{
    QMutexLocker locker(&mtx);
    if( isEmpty() )
    {
        return true;
    }
    pCmdLNode p = phead;
    pCmdLNode pprior = NULL;
    //先判断头节点是不是要找的结点
    if( (taskId == phead->u8TaskId) && (obj == phead->eBoardObj) )
    {
        p = phead->next;
        delete(phead);
        phead = p;
        nodeNum--;
    }
    else
    {
        while(p->next)
        {
            pprior = p;
            p = p->next;
            if( (taskId == p->u8TaskId) && (obj == p->eBoardObj) )
            {
                pprior->next = p->next;
                delete(p);
                nodeNum--;
                break;
            }
        }
    }
    return true;
}

/*
 * 删除列表中所有cmdid结点
 */
bool GetTaskLinkList::app_deleteNodeWithCmdId(uint32_t cmdid)
{
    QMutexLocker locker(&mtx);
    if( isEmpty() )
    {        
        return true;
    }

    pCmdLNode pprior = NULL;
    //先判断头节点是不是要找的结点
    while(phead)
    {
        if(cmdid != st_wk_taskcmd_list[phead->eBoardObj].getCmd)
            break;
        pprior = phead;
        phead = phead->next;
        delete(pprior);
        nodeNum--;
    }
    if( (nullptr == phead) || (nullptr == phead->next) )
        return true;
    //头结点不为cmdid的列表删除cmdid的结点
    pCmdLNode p = phead->next;
    pprior = phead;
    while(p)
    {
        if(cmdid != st_wk_taskcmd_list[p->eBoardObj].getCmd)
        {
            pprior = p;
            p = p->next;
        }
        else
        {
            pprior->next = p->next;
            delete(p);
            p = pprior->next;
            nodeNum--;
        }
    }
    return true;
}

/*
 * 依次读取链表，找到第一条未发送的数据，读取该数据到指定地址并返回长度
 * 如果链表为空，或者全部发送过一次（遍历完链表未读到未发送数据）则全部置为未发送，返回false
 */
bool GetTaskLinkList::app_getOneNotSendNodeData(uint8_t* data, uint16_t &len)
{
    QMutexLocker locker(&mtx);
    if( isEmpty() )
    {
        return false;
    }
    pCmdLNode p = phead;
    while(p)
    {
        if(emSend_NotSend == p->stCmd.eSendState)
        {
            bool isReSend = false;
            p->stCmd.eSendState = emSend_OK;
            p->stCmd.sdNum++;
            memcpy(data, p->stCmd.data, p->stCmd.len);
            len = p->stCmd.len;
            //重发机制
            HardCmdParseAgent &cmdParseAgent = HardCmdParseAgent::GetInstance();
            cmdParseAgent.setTaskCmdReSend(st_wk_taskcmd_list[p->eBoardObj].getCmd, p->stCmd.sdNum, isReSend);
            if(isReSend)
                 p->stCmd.sdNum = 0;
            return true;
        }
        else
            p = p->next;
    }
    p = phead;
    while(p)
    {
        p->stCmd.eSendState = emSend_NotSend;
        p = p->next;
    }
    return false;
}


