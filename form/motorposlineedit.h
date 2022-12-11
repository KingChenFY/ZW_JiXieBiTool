#ifndef MOTORPOSLINEEDIT_H
#define MOTORPOSLINEEDIT_H

#include <QLineEdit>
#include <QRegularExpression>

class MotorPosLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    MotorPosLineEdit(QWidget* parent = 0);

//private:
    static QRegularExpression rexpPhy;
    static QRegularExpression rexpLogic;
    void setPhyValueRegularExp();
    void setLogicValueRegularExp();
    bool isPhyValue();
    bool isLogicValue();

};

#endif // MOTORPOSLINEEDIT_H
