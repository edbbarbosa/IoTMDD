#include "componentconfiguration.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

ComponentConfiguration::ComponentConfiguration(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Component Configuration");

    QHBoxLayout *hbox = new QHBoxLayout(this);
    QVBoxLayout *vbox0 = new QVBoxLayout;
    QVBoxLayout *vbox1 = new QVBoxLayout;

    radioLed = new QRadioButton("LED", this);
    radioBuzzer = new QRadioButton("Buzzer", this);
    radioSensor = new QRadioButton("Sensor", this);
    radioTimer = new QRadioButton("Timer", this);
    radioButton = new QRadioButton("Button", this);
    radioEngine = new QRadioButton("Engine", this);

    vbox0->addWidget(new QLabel("Components"));
    vbox0->addWidget(radioLed);
    vbox0->addWidget(radioBuzzer);
    vbox0->addWidget(radioSensor);
    vbox0->addWidget(radioTimer);
    vbox0->addWidget(radioButton);
    vbox0->addWidget(radioEngine);

    QFormLayout *form0 = new QFormLayout;
    nameEdit = new QLineEdit;
    form0->addRow("Component name:", nameEdit);

    vbox1->addLayout(form0);
    vbox1->addWidget(new QLabel("Proprieties:"));

    QFormLayout *form1 = new QFormLayout;
    volumeEdit = new QLineEdit;
    toneEdit = new QLineEdit;

    combo = new QComboBox;
    combo->addItem("Temperature");
    combo->addItem("Moisture");
    combo->addItem("Light");
    combo->addItem("Distance");

    //form1->addRow("Volume:", volumeEdit);
    form1->addRow("Tone:", toneEdit);
    form1->addRow("Volume:", combo);

    vbox1->addLayout(form1);

    QPushButton *okBtn = new QPushButton("OK");

    vbox1->addWidget(okBtn);

    hbox->addLayout(vbox0);
    hbox->addLayout(vbox1);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
}

QString ComponentConfiguration::getComponent(){
    if(radioLed->isChecked())
        return radioLed->text();
    else if(radioBuzzer->isChecked())
        return radioBuzzer->text();
    else if(radioSensor->isChecked())
        return radioSensor->text();
    else if(radioTimer->isChecked())
        return radioTimer->text();
    else if(radioButton->isChecked())
        return radioButton->text();
    else if(radioEngine->isChecked())
        return radioEngine->text();
    else
        return "";
}
