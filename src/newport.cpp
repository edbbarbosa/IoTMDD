#include "newport.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>

NewPort::NewPort(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("New Port");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;
    idEdit = new QLineEdit;

    radioAnalogic = new QRadioButton("Analogic", this);
    radioDigital = new QRadioButton("Digital", this);

    QWidget *container0 = new QWidget;
    QHBoxLayout *hbox0 = new QHBoxLayout(container0);

    hbox0->addWidget(radioAnalogic);
    hbox0->addWidget(radioDigital);
    hbox0->setContentsMargins(0, 0, 0, 0);

    QWidget *container1 = new QWidget;
    QHBoxLayout *hbox1 = new QHBoxLayout(container1);

    radioInput = new QRadioButton("Input", this);
    radioOutput = new QRadioButton("Output", this);

    hbox1->addWidget(radioInput);
    hbox1->addWidget(radioOutput);
    hbox1->setContentsMargins(0, 0, 0, 0);

    form->addRow("Port name:", nameEdit);
    form->addRow("Port type:", container0);
    form->addRow("Pin type", container1);
    form->addRow("Pin ID:", idEdit);

    vbox->addLayout(form);

    QPushButton *okBtn = new QPushButton("OK");

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    vbox->addWidget(okBtn);
}

QString NewPort::getPortType(){
    if(radioAnalogic->isChecked())
        return radioAnalogic->text();
    else if(radioDigital->isChecked())
        return radioDigital->text();
    else
        return "";
}

QString NewPort::getPinType(){
    if(radioInput->isChecked())
        return radioInput->text();
    else if(radioOutput->isChecked())
        return radioOutput->text();
    else
        return "";
}
