#ifndef GETTASKLINKLIST_H
#define GETTASKLINKLIST_H

#include <stdint.h>
#include "hardcmd.h"
#include <QMutex>

typedef enum
{
    emSend_NotSend,
    emSend_OK,
    emSend_ERR,
}emSendState;

#pragma pack(1)
typedef struct
{
    uint32_t sdNum;                          //发送次数
    emSendState eSendState;                 //当前指令状态
    uint16_t len;
    uint8_t data[WK_NET_ONE_MESSAGE_MAX_LEN];   //链表结点中存储的是整包查询指令
}ST_ZW_NET_MESSAGE_INFO;

typedef struct cmdlnode
{
    uint8_t u8TaskId;                       //指令防重复ID
    emBoardObject eBoardObj;                 //该结点查询的机械臂模块
    ST_ZW_NET_MESSAGE_INFO stCmd;
    struct cmdlnode *next;
}CmdLNode, *pCmdLNode;
#pragma pack()

class GetTaskLinkList
{
public:
//    GetTaskLinkList();
    static GetTaskLinkList &GetInstance()
    {
        static GetTaskLinkList m_instance;
        return m_instance;
    }
    ~GetTaskLinkList();

    bool isEmpty();
    bool app_insertTail(uint8_t taskId, emBoardObject boardObj, uint8_t *data, uint16_t len);
    bool app_deleteNodeWithTaskId(emBoardObject obj, uint8_t taskId);
    bool app_deleteNodeWithCmdId(uint32_t cmdid);
    bool app_getOneNotSendNodeData(uint8_t* data, uint16_t &len);

private:
    GetTaskLinkList();

    bool insertTail(pCmdLNode pNode);    
    uint32_t nodeNum;
    pCmdLNode phead;
    QMutex mtx;

};

#endif // GETTASKLINKLIST_H
