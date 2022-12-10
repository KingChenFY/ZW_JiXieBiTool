#ifndef NETRECVRINGFIFO_H
#define NETRECVRINGFIFO_H

#include "global.h"

#define NET_RECV_BUFFER_LEN                     4096

class NetRecvRingFifo
{
public:
    NetRecvRingFifo();
    ~NetRecvRingFifo(){};
    uint32_t findOnePackFromFifo();
    bool pushOnePackToFifo(uint8_t const * p_byte_array, uint32_t size);
    bool popOnePackFromFifo(uint8_t* p_byte_array, uint32_t size);

private:
    uint32_t getFifoDataLength();
    uint32_t getFifoRemainCount();
    uint32_t peekFifoDataLength();
    uint32_t peekFifoRemainCount();
    void movePointNBytes(uint32_t &point, uint32_t num);

    static uint32_t u32NetRecvBufferLenMask;
    uint32_t u32pRead;
    uint32_t u32pWrite;
    uint32_t u32pPeekPos;
    uint8_t u8NetRecvBuffer[NET_RECV_BUFFER_LEN];
};

#endif // NETRECVRINGFIFO_H
