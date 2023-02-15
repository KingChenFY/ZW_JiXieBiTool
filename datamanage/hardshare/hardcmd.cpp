#include "global.h"
uint32_t u32CmdFlagId = 0;

STRUCT_WK_TASKCMD_LIST st_wk_taskcmd_list[EnumBoardObject_End] =
{
    {EnumBoardObject_BeltIn, EnumBoardId_setBeltInTask, EnumBoardId_getBeltInTaskInfo},
    {EnumBoardObject_BeltOut, EnumBoardId_setBeltOutTask, EnumBoardId_getBeltOutTaskInfo},
    {EnumBoardObject_PoleIn, EnumBoardId_setPoleInTask, EnumBoardId_getPoleInTaskInfo},
    {EnumBoardObject_PoleOut, EnumBoardId_setPoleOutTask, EnumBoardId_getPoleOutTaskInfo},
    {EnumBoardObject_Upender, EnumBoardId_setUpenderTask, EnumBoardId_getUpenderTaskInfo},    
    {EnumBoardObject_MotorV, EnumBoardId_setBoxVTask, EnumBoardId_getBoxVTaskInfo},
    {EnumBoardObject_MotorXYZ, EnumBoardId_setXYZTask, EnumBoardId_getXYZTaskInfo},
    {EnumBoardObject_CleanOil, EnumBoardId_setCleanOilTask, EnumBoardId_getCleanOilTaskInfo},
    {EnumBoardObject_DripOil, EnumBoardId_setOilTask, EnumBoardId_getOilTaskInfo}
};

uint16_t formatBoardCmd(uint8_t cmdid, uint8_t* cmdcontent, uint16_t cmdcontentLen)
{
    uint16_t crcValue = 0xFFFF;
    uint16_t i = 0;

    common_write_u16(&cmdcontent[i], WK_BOARD_COMMAND_HEAD); i+=2;// 指令头
    common_write_u16(&cmdcontent[i], 10 + cmdcontentLen); i+=2;// 指令长度
    cmdcontent[i++] = ENUM_BOARD_NO_ENCRYPT_VER_SERIAL_PORT;// 类型标识
    cmdcontent[i++] = ENUM_BOARD_ASK_TABLE;// 请求标识
    cmdcontent[i++] = 1;// 预留，1表示指令版本为QT版本上位机，下层解析时根据该标志解析指令
    cmdcontent[i++] = cmdid;// 指令ID
    common_write_u32(&cmdcontent[i], u32CmdFlagId++); i+=4;// 指令FLAG
    i += cmdcontentLen;//指令内容
    crcValue = Mb_CRC16_Generate(&cmdcontent[2], 10 + cmdcontentLen);
    common_write_u16(&cmdcontent[i], crcValue); i+=2;// CRC校验
    common_write_u16(&cmdcontent[i], WK_BOARD_COMMAND_END); i+=2;// 指令尾

    return i;
}

//bool HardCmd::parseBoardResponse(QByteArray &boardcmd, quint8 &cmdid, QByteArray &cmdconetent)
//{
//    ushort crcValue, crcCnt;
//    ushort head = QUIHelperData::byteToUShort(boardcmd.mid(0,2));
//    ushort tail;
//    ushort cmdlen =boardcmd.size();
//    ushort datalen;

//    if(WK_BOARD_COMMAND_HEAD == head)
//    {
//        datalen = QUIHelperData::byteToUShort(boardcmd.mid(2,2));
//        if((datalen + 6) <= cmdlen)
//        {
//            tail = QUIHelperData::byteToUShort(boardcmd.mid(datalen + 4,2));
//            if(WK_BOARD_COMMAND_END == tail)
//            {
//                crcCnt = QUIHelperData::getModbus16((quint8*)&boardcmd.data()[2], datalen);
//                crcValue = QUIHelperData::byteToUShortRec(boardcmd.mid(datalen + 2, 2));
//                if(crcValue == crcCnt)
//                {
//                    cmdid = boardcmd[7];
//                    cmdconetent = boardcmd.mid(12, datalen - 10);
//                    return true;
//                }
//            }
//        }
//    }
//    return false;
//}
