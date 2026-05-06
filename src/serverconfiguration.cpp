#include "serverconfiguration.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QAction>
#include <QStyle>

ServerConfiguration::ServerConfiguration(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Server Configuration");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;
    passwordEdit = new QLineEdit;
    ssidEdit = new QLineEdit;
    portEdit = new QLineEdit;

    form->addRow("Server name:", nameEdit);
    form->addRow("Password:", passwordEdit);
    form->addRow("SSID:", ssidEdit);
    form->addRow("Port:", portEdit);

    passwordEdit->setEchoMode(QLineEdit::Password);
    QAction *toggleAction = new QAction(passwordEdit);
    toggleAction->setIcon(passwordEdit->style()->standardIcon(QStyle::SP_FileDialogContentsView));
    passwordEdit->addAction(toggleAction, QLineEdit::TrailingPosition);
    QObject::connect(toggleAction, &QAction::triggered, [=]() {
        if (passwordEdit->echoMode() == QLineEdit::Password) {
            passwordEdit->setEchoMode(QLineEdit::Normal);
            // Opcional: Trocar para um ícone de "esconder"
            //toggleAction->setIcon(QIcon(":/icons/eye-off.png"));
        } else {
            passwordEdit->setEchoMode(QLineEdit::Password);
            // Opcional: Trocar para um ícone de "mostrar"
            //toggleAction->setIcon(QIcon(":/icons/eye-on.png"));
        }
    });

    QPushButton *okBtn = new QPushButton("OK");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    vbox->addLayout(form);
    vbox->addWidget(okBtn);
}
