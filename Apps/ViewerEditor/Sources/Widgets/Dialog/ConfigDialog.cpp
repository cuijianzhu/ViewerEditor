#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QAction>
#include <QSettings>
ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    QSettings settings("Soarscape", "ViewerEditor");
    QString   psPath = settings.value("psPath").toString();
    ui->lineEdit->setText(psPath);

    connect(ui->lineEdit, &QLineEdit::textChanged, [&](const QString&) { 
        QSettings set("Soarscape", "ViewerEditor");
        m_photoshopPath = ui->lineEdit->text();
        set.setValue("psPath", m_photoshopPath);
    });

    connect(ui->toolButton, &QToolButton::clicked, [&]() {
        auto result = QFileDialog::getOpenFileName(nullptr, QString::fromLocal8Bit("PhotoShopÂ·¾¶"), "", "executable file (*.exe);");
        if (!result.isEmpty()) {
            ui->lineEdit->setText(result);
        }
    });
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}
