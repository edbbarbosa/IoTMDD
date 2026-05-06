#include "protocolconfiguration.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QAction>

ProtocolConfiguration::ProtocolConfiguration(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Protocol Configuration");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;
    combo = new QComboBox;

    userEdit = new QLineEdit;
    passwordEdit = new QLineEdit;

    combo->addItem("MQTT");
    combo->addItem("TCP");

    form->addRow("Protocol name:", combo);

    form->addRow("User:", userEdit);
    form->addRow("Password:", passwordEdit);

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
