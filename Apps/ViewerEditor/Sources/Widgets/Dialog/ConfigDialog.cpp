#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QAction>
ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    connect(ui->lineEdit, &QLineEdit::textChanged, [&](const QString&) { 
            m_photoshopPath = ui->lineEdit->text();
    });

    connect(ui->toolButton, &QToolButton::clicked, [&]() {
        auto result = QFileDialog::getOpenFileName();
        if (!result.isEmpty()) {
            ui->lineEdit->setText(result);
        }
    });
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}
