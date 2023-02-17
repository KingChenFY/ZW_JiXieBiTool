#include "actionfourpostest.h"

ActionFourPosTest::ActionFourPosTest(ActionMotorXYZ* &objActionMotorXYZ,
                                     CeJuTcpClient* &objCeJuTcpClient, QObject *parent)
    : m_objActionMotorXYZ(objActionMotorXYZ),m_objCeJuTcpClient(objCeJuTcpClient),QThread(parent)
{

}

void ActionFourPosTest::run()
{}
