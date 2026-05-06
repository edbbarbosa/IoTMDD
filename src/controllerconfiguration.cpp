#include "controllerconfiguration.h"
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QComboBox>

ControllerConfiguration::ControllerConfiguration(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Controller Configuration");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;
    combo = new QComboBox;

    combo->addItem("ESP32");
    combo->addItem("Arduino Uno");

    nameEdit = new QLineEdit;

    form->addRow("Name:", nameEdit);
    form->addRow("Type", combo);

    QPushButton *okBtn = new QPushButton("OK");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    vbox->addLayout(form);
    vbox->addWidget(okBtn);

}
