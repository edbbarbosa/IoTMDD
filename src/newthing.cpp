#include "newthing.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>

NewThing::NewThing(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("New Thing");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;
    descriptionEdit = new QLineEdit;

    form->addRow("Name:", nameEdit);
    form->addRow("Description:", descriptionEdit);

    QPushButton *okBtn = new QPushButton("OK");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    vbox->addLayout(form);
    vbox->addWidget(okBtn);
}
