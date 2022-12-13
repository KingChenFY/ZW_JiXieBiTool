#include "NumberLineEdit.h"

#include <QRegularExpressionMatch>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QDoubleValidator>

QRegularExpression NumberLineEdit::rexpPhy("^-[0-9]{1,10}$|^[0-9]{0,10}$");
QRegularExpression NumberLineEdit::rexpLogic("^[0-9]{0,}(\\.[0-9]{1,}){0,1}$");
QRegularExpression NumberLineEdit::rexpPositiveInt("^[0-9]{1,10}$");
//QRegularExpression NumberLineEdit::rexpLogic("^[0-9](\\.[0-9]{1,2}){0,1}$|^[1-9][0-9]{0,2}(\\.[0-9]{1,2}){0,1}$|^1000(\\.0{1,2}){0,1}$");

NumberLineEdit::NumberLineEdit(QWidget* parent) :
    QLineEdit(parent)
{
    QRegularExpression rexpMain("^-[0-9]{1,}(\\.[0-9]{1,}){0,1}$|^[0-9]{0,}(\\.[0-9]{1,}){0,1}$");
    QValidator *Validator = new QRegularExpressionValidator(rexpMain);
    this->setValidator( Validator );
//    QDoubleValidator *dv = new QDoubleValidator(0,99,2);
//    dv->setNotation(QDoubleValidator::StandardNotation);
//    this->setValidator( dv );
}

void NumberLineEdit::setPhyValueRegularExp()
{
    QValidator *Validator = new QRegularExpressionValidator(rexpPhy);
    this->setValidator( Validator );
    this->clear();
}

void NumberLineEdit::setLogicValueRegularExp()
{
    QValidator *Validator = new QRegularExpressionValidator(rexpLogic);
    this->setValidator( Validator );
    this->clear();
}

bool NumberLineEdit::isPhyValue()
{
    QRegularExpressionMatch match = rexpPhy.match(this->text());
    return match.hasMatch();
}

bool NumberLineEdit::isLogicValue()
{
    QRegularExpressionMatch match = rexpLogic.match(this->text());
    return match.hasMatch();
}

bool NumberLineEdit::isPositiveInt()
{
    QRegularExpressionMatch match = rexpPositiveInt.match(this->text());
    return match.hasMatch();
}


