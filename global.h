#ifndef GLOBAL_H
#define GLOBAL_H

#include "savelog.h"
#include <QDebug>

#include "head.h"
#include "hardcmd.h"
#include "harddef.h"
#include "hardselutedef.h"

#include "settaskringfifo.h"
#include "netrecvringfifo.h"
#include "gettasklinklist.h"

void common_write_u64 (uint8_t* puData, uint64_t uNum);
uint64_t common_read_u64 (uint8_t *puData);
void common_write_u32 (uint8_t* puData, uint32_t uNum);
uint32_t common_read_u32 (uint8_t *puData);
void common_write_u16 (uint8_t *puData, uint16_t uNum );
uint16_t common_read_u16 (uint8_t *puData);

uint16_t Mb_CRC16_Generate(uint8_t *puckMsg, uint16_t usDataLen);

/************** 打印 **************/
//_LOG(QString("parse setTask cmd return"));
#define _LOG(format)    qDebug() << qPrintable(QString("[%1]%2[%3][%4]>> %5").arg(TIMEMS).arg(__FILE__).\
                            arg(__LINE__).arg(__FUNCTION__).arg(format))
//#define _LOG(format)

#endif // GLOBAL_H
