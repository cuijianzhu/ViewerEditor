#include "FloatTools.h"
#include "ui_FloatTools.h"
#include <QMenu>
#include "GlobalSignal.h"
FloatTools::FloatTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloatTools)
{
    ui->setupUi(this);

    QMenu* menuFile = new QMenu(this);
    menuFile->addAction(ui->actionImport);
    menuFile->addAction(ui->actionExport);
    ui->toolButtonFile->setMenu(menuFile);

    QMenu* menuView = new QMenu(this);
    menuView->addAction(ui->actionHome);
    ui->toolButtonView->setMenu(menuView);
    initConnect();
}

FloatTools::~FloatTools()
{
    delete ui;
}

void FloatTools::initConnect() {
    connect(
        ui->actionHome, &QAction::triggered, [](bool) { emit g_globalSignal.signal_viewHome();
    });
}
