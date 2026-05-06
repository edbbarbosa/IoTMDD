#include "topicconfiguration.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>

TopicConfiguration::TopicConfiguration(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Topic Configuration");

    QVBoxLayout *vbox = new QVBoxLayout(this);
    QFormLayout *form = new QFormLayout;

    nameEdit = new QLineEdit;

    checkPublish = new QCheckBox("Publish", this);
    checkSubscribe = new QCheckBox("Subscribe", this);

    QWidget *container0 = new QWidget;
    QVBoxLayout *vbox1 = new QVBoxLayout(container0);

    vbox1->addWidget(checkPublish);
    vbox1->addWidget(checkSubscribe);
    vbox1->setContentsMargins(0, 0, 0, 0);

    form->addRow("Topic name:", nameEdit);
    form->addRow("Topic", container0);

    QPushButton *okBtn = new QPushButton("OK");
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    vbox->addLayout(form);
    vbox->addWidget(okBtn);
}

QString TopicConfiguration::getTopic(){
    if(checkPublish->isChecked() && checkSubscribe->isChecked())
        return "Publish/Subscribe";
    else if(checkPublish->isChecked())
        return "Publish";
    else if(checkSubscribe->isChecked())
        return "Subscribe";
    else
        return "";
}
