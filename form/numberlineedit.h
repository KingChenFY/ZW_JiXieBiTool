#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>
#include <QRegularExpression>

class NumberLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    NumberLineEdit(QWidget* parent = 0);

//private:
    static QRegularExpression rexpPhy;
    static QRegularExpression rexpLogic;
    static QRegularExpression rexpPositiveInt;
    void setPhyValueRegularExp();
    void setLogicValueRegularExp();
    bool isPhyValue();
    bool isLogicValue();
    bool isPositiveInt();

};

#endif // NUMBERLINEEDIT_H
