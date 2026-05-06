#ifndef COMPONENTCONFIGURATION_H
#define COMPONENTCONFIGURATION_H
#include <QLineEdit>
#include <QDialog>
#include <QRadioButton>
#include <QComboBox>

class ComponentConfiguration : public QDialog
{
    Q_OBJECT
public:
    QRadioButton *radioLed;
    QRadioButton *radioBuzzer;
    QRadioButton *radioSensor;
    QRadioButton *radioTimer;
    QRadioButton *radioButton;
    QRadioButton *radioEngine;
    QComboBox *combo;
    QLineEdit *nameEdit;
    QLineEdit *volumeEdit;
    QLineEdit *toneEdit;
    explicit ComponentConfiguration(QWidget *parent = nullptr);
    QString getComponent();
};

#endif // COMPONENTCONFIGURATION_H
