#include "FloatTools.h"
#include "ui_FloatTools.h"
#include <QMenu>
#include <QFileDialog>
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

    QMenu* menuEdit = new QMenu(this);
    ui->actionSelect->setCheckable(true);
    menuEdit->addAction(ui->actionSelect);
    ui->toolButtonEdit->setMenu(menuEdit);
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

    connect(ui->actionSelect, &QAction::triggered, [](bool checked) {
        emit g_globalSignal.signal_select(checked);
    });

    connect(
        ui->actionImport, &QAction::triggered, [](bool) { 
            auto result = QFileDialog::getOpenFileName(nullptr,
                                                       QString::fromLocal8Bit("打开模型文件"),
                                                       "",
                                                       tr("obj file (*.obj)"));
            if (!result.isEmpty()) {
                emit g_globalSignal.signal_importMesh(result); 
            }
        });

    connect(ui->actionExport, &QAction::triggered, [](bool) {
        auto result = QFileDialog::getSaveFileName(
            nullptr, QString::fromLocal8Bit("导出模型文件"), "", tr("obj file (*.obj)"));
        if (!result.isEmpty()) {
            emit g_globalSignal.signal_exportMesh(result);
        }
    });
}
