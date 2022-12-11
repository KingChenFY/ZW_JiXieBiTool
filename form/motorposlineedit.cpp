#include "motorposlineedit.h"

#include <QRegularExpressionMatch>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QDoubleValidator>

QRegularExpression MotorPosLineEdit::rexpPhy("^-?[0-9]{0,10}$");
QRegularExpression MotorPosLineEdit::rexpLogic("^[0-9](\\.[0-9]{1,2}){0,1}$|^[1-9][0-9]{0,2}(\\.[0-9]{1,2}){0,1}$|^1000(\\.0{1,2}){0,1}$");

MotorPosLineEdit::MotorPosLineEdit(QWidget* parent) :
    QLineEdit(parent)
{
    QValidator *Validator = new QRegularExpressionValidator(rexpPhy);
    this->setValidator( Validator );
//    QDoubleValidator *dv = new QDoubleValidator(0,99,2);
//    dv->setNotation(QDoubleValidator::StandardNotation);
//    this->setValidator( dv );
}

void MotorPosLineEdit::setPhyValueRegularExp()
{
    QValidator *Validator = new QRegularExpressionValidator(rexpPhy);
    this->setValidator( Validator );
    this->clear();
}

void MotorPosLineEdit::setLogicValueRegularExp()
{
    QValidator *Validator = new QRegularExpressionValidator(rexpLogic);
    this->setValidator( Validator );
    this->clear();
}

bool MotorPosLineEdit::isPhyValue()
{
    QRegularExpressionMatch match = rexpPhy.match(this->text());
    return match.hasMatch();
}

bool MotorPosLineEdit::isLogicValue()
{
    QRegularExpressionMatch match = rexpLogic.match(this->text());
    return match.hasMatch();
}


