#ifndef NEWPORT_H
#define NEWPORT_H
#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>

class NewPort : public QDialog
{
    Q_OBJECT
    QRadioButton *radioAnalogic;
    QRadioButton *radioDigital;
    QRadioButton *radioInput;
    QRadioButton *radioOutput;
public:
    QLineEdit *nameEdit;
    QLineEdit *idEdit;
    explicit NewPort(QWidget *parent = nullptr);
    QString getPortType();
    QString getPinType();
};

#endif // NEWPORT_H
