#ifndef SETTASKRINGFIFO_H
#define SETTASKRINGFIFO_H

#include "harddef.h""
#include "hardcmd.h"
#include "stdbool.h"

#define SETTASK_RINGFIFO_MAX_LIST_NUM           8
#define SETTASK_RINGFIFO_MAX_LIST_NUM_MASk      (SETTASK_RINGFIFO_MAX_LIST_NUM -1)

typedef struct
{
    bool bIsValid;                                              //是否有效
    emBoardCmdId eCmdId;
    uint8_t u8CmdContent[WK_BOARD_CMD_CONTENT_MAX_LENGTH];
    uint16_t u16CmdContentLen;
}STRUCT_SETTASK_MESSAGE_INFO;


class SetTaskRingFifo
{
public:
    SetTaskRingFifo();

    STRUCT_SETTASK_MESSAGE_INFO* getWriteMessageAddr(emBoardCmdId eCmdId);
    void pushWriteMessageToFifo(void);
    STRUCT_SETTASK_MESSAGE_INFO* getReadMessageAddr(void);
    void popReadMessageOutFifo(void);


private:
    bool isEmpty();
    bool isFull();
    void setSameCmdIdMessageInvalid(emBoardCmdId eCmdId);

    uint8_t u8pRead;
    uint8_t u8pWrite;
    STRUCT_SETTASK_MESSAGE_INFO st_tasklist[SETTASK_RINGFIFO_MAX_LIST_NUM];
};

#endif // SETTASKRINGFIFO_H
