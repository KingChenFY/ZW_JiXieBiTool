/* 2022-11-27
 * 该文件为实现单生产者和单消费者场景的线程同步空间而编写，暂不支持其它场景！
 */

#include "settaskringfifo.h"
#include "stddef.h"

SetTaskRingFifo::SetTaskRingFifo()
{
    u8pRead = 0;
    u8pWrite = 0;
    for(uint8_t i=0; i<SETTASK_RINGFIFO_MAX_LIST_NUM; i++)
    {
        st_tasklist[i].bIsValid = false;
    }
}

inline bool SetTaskRingFifo::isEmpty()
{
    return (u8pRead == u8pWrite) ? true:false;
}

inline bool SetTaskRingFifo::isFull()
{
    return(u8pRead == ((u8pWrite + 1) & SETTASK_RINGFIFO_MAX_LIST_NUM_MASk)) ? true:false;
}

/*
 * 从未读取的数据开始遍历，不管数据是否发送把之前指令id相同的数据置为无效
 * 告诉发送线程，如果这条数据还没发就别发了，已经发的就算了
 */
void SetTaskRingFifo::setSameCmdIdMessageInvalid(emBoardCmdId eCmdId)
{
    uint8_t pReadTemp = u8pRead;

    //不为空就读一条数据，判断指令是否重复
    while( u8pWrite != ((pReadTemp++) & SETTASK_RINGFIFO_MAX_LIST_NUM_MASk) )
    {
        if(eCmdId == st_tasklist[pReadTemp].eCmdId)
            st_tasklist[pReadTemp].bIsValid = false;
    }
}

void SetTaskRingFifo::popReadMessageOutFifo(void)
{
    u8pRead = (++u8pRead) & SETTASK_RINGFIFO_MAX_LIST_NUM_MASk;
}

STRUCT_SETTASK_MESSAGE_INFO* SetTaskRingFifo::getReadMessageAddr(void)
{
    //队列不空
    while( false == isEmpty() )
    {
        //先剔除已经无效的数据
        if( false == st_tasklist[u8pRead].bIsValid )
        {
            popReadMessageOutFifo();
            continue;
        }
        return &st_tasklist[u8pRead];
    }
    return NULL;

}

void SetTaskRingFifo::pushWriteMessageToFifo(void)
{
    u8pWrite = (++u8pWrite) & SETTASK_RINGFIFO_MAX_LIST_NUM_MASk;
}

STRUCT_SETTASK_MESSAGE_INFO* SetTaskRingFifo::getWriteMessageAddr(emBoardCmdId eCmdId)
{
    //在塞入数据前先把重复id的指令置为无效
    setSameCmdIdMessageInvalid(eCmdId);
    //队列已满返回空
    if( isFull() )
        return NULL;
    //标记该地址数据有效
    st_tasklist[u8pWrite].eCmdId = eCmdId;
    st_tasklist[u8pWrite].bIsValid = true;
    //返回写空间地址
    return &st_tasklist[u8pWrite];
}
