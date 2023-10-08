#include "FloatTools.h"
#include "ui_FloatTools.h"

FloatTools::FloatTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatTools)
{
    ui->setupUi(this);
}

FloatTools::~FloatTools()
{
    delete ui;
}
