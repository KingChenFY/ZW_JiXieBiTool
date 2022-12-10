/*
 * 判空 read == write
 * 判满 write + 1 == read
 * 实际可用长度 = 缓存总长度 -1
 */

#include "netrecvringfifo.h"
#include <string>

uint32_t NetRecvRingFifo::u32NetRecvBufferLenMask = NET_RECV_BUFFER_LEN - 1;

NetRecvRingFifo::NetRecvRingFifo()
{
    u32pRead = 0;
    u32pWrite = 0;
}

uint32_t NetRecvRingFifo::getFifoDataLength()
{
    int32_t tmp = u32pWrite - u32pRead;
    return (tmp >= 0) ? tmp : (u32NetRecvBufferLenMask + tmp + 1);
}

uint32_t NetRecvRingFifo::getFifoRemainCount()
{
    return ( u32NetRecvBufferLenMask - (getFifoDataLength()) );
}

uint32_t NetRecvRingFifo::peekFifoDataLength()
{
    int32_t tmp = u32pWrite - u32pPeekPos;
    return (tmp >= 0) ? tmp : (u32NetRecvBufferLenMask + tmp + 1);
}

uint32_t NetRecvRingFifo::peekFifoRemainCount()
{
    return ( u32NetRecvBufferLenMask - (peekFifoDataLength()) );
}

/*
 * 用于往后偏移fifo的指针num个字节
 * point为 操作的指针 对象
 * 注意：这个函数要在检查 缓存剩余可用数量后调用，未做边界安全检查
 */
void NetRecvRingFifo::movePointNBytes(uint32_t &point, uint32_t num)
{
    point += num;
    point = (point > u32NetRecvBufferLenMask) ? (point - u32NetRecvBufferLenMask - 1) : point;
}

bool NetRecvRingFifo::pushOnePackToFifo(uint8_t const * p_byte_array, uint32_t size)
{
    if( getFifoRemainCount() < size )
        return false;

    memcpy(&u8NetRecvBuffer[u32pWrite], p_byte_array, size);
    movePointNBytes(u32pWrite, size);
    return true;
}

/*
 * 从fifo中取出size个bytes数据到p_byte_array
 */
bool NetRecvRingFifo::popOnePackFromFifo(uint8_t* p_byte_array, uint32_t size)
{
    if( getFifoDataLength() < size )
        return false;
    memcpy(p_byte_array, &u8NetRecvBuffer[u32pRead], size);
    movePointNBytes(u32pRead, size);
    return true;
}

/*
 * 从缓存中查找一包可用数据
 * 找到-把指针偏移到目标位置，并返回数据的长度
 * 未找到-返回0
 */
uint32_t NetRecvRingFifo::findOnePackFromFifo()
{
    uint32_t packLength;

    u32pPeekPos = u32pRead;
    while(peekFifoDataLength() >= WK_NET_ONE_MESSAGE_MIN_LEN)    //可读取数据大于最小一包数据的长度
    {
        if(WK_BOARD_COMMAND_HEAD == common_read_u16(&u8NetRecvBuffer[u32pPeekPos]))             //校验包头
        {
            u32pRead = u32pPeekPos;             //找到包头后直接把读指针偏移到该位置
            uint16_t u16CmdLen, u16CrcRead, u16CrcGet;
            u16CmdLen = common_read_u16(&u8NetRecvBuffer[u32pRead + WK_BOARD_COMMAND_HEAD_LENGTH]);

            packLength = u16CmdLen + WK_BOARD_COMMAND_HEAD_LENGTH + WK_BOARD_CMD_END_LENGTH;
            if(getFifoDataLength() < packLength)   //校验包长。如果可读的数据长度小于一包数据的长度，查找失败，等待接受完整包数据
                break;

            if(WK_BOARD_COMMAND_END == common_read_u16(&u8NetRecvBuffer[u32pRead + packLength - 2]))  //校验包尾
            {
                u16CrcRead = common_read_u16(&u8NetRecvBuffer[u32pRead + packLength - WK_BOARD_CMD_END_LENGTH]);
                u16CrcGet = Mb_CRC16_Generate(&u8NetRecvBuffer[u32pRead+WK_BOARD_COMMAND_HEAD_LENGTH], u16CmdLen);
                if(u16CrcRead == u16CrcGet)
                {
                    return packLength;
                }
            }
        }
        movePointNBytes(u32pPeekPos, 1);    //u32pPeekPos指针后移一位
    }
    return 0;
}
