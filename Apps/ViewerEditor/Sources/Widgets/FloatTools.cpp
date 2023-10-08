#include "FloatTools.h"
#include "ui_FloatTools.h"
#include <QMenu>
FloatTools::FloatTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatTools)
{
    ui->setupUi(this);

    QMenu* menuFile = new QMenu(this);
    menuFile->addAction(ui->actionImport);
    menuFile->addAction(ui->actionExport);
    ui->toolButtonFile->setMenu(menuFile);
}

FloatTools::~FloatTools()
{
    delete ui;
}
