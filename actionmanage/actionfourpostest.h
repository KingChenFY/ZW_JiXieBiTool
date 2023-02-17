#ifndef ACTIONFOURPOSTEST_H
#define ACTIONFOURPOSTEST_H

#include <QObject>
#include <QThread>
#include "hardcmdparseagent.h"
#include "harddef.h"

#include "cejutcpclient.h"
#include "actionmotorxyz.h"

typedef enum
{
    emPointPos_One = 0,
    emPointPos_Two,
    emPointPos_Three,
    emPointPos_Four,
    emPointPos_End
}emPointPos;

class ActionFourPosTest : public QThread
{
    Q_OBJECT
public:
    explicit ActionFourPosTest(ActionMotorXYZ* &objActionMotorXYZ,
                               CeJuTcpClient* &objCeJuTcpClient, QObject *parent = nullptr);
    bool m_bNeedStop;
protected:
    void run();
private:
    emPointPos m_ePointPos;
    ST_XYZ_DPOS m_stAimPoint[emPointPos_End];

    ActionMotorXYZ* &m_objActionMotorXYZ;
    CeJuTcpClient* &m_objCeJuTcpClient;


signals:

};

#endif // ACTIONFOURPOSTEST_H
