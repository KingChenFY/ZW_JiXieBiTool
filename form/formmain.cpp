#include "formmain.h"
#include "ui_formmain.h"

FormMain::FormMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMain)
{
    ui->setupUi(this);
}

FormMain::~FormMain()
{
    delete ui;
}
