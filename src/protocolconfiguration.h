#ifndef PROTOCOLCONFIGURATION_H
#define PROTOCOLCONFIGURATION_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class ProtocolConfiguration : public QDialog
{
    Q_OBJECT
public:
    QLineEdit *userEdit;
    QLineEdit *passwordEdit;
    QComboBox *combo;
    explicit ProtocolConfiguration(QWidget *parent = nullptr);
};

#endif // PROTOCOLCONFIGURATION_H
