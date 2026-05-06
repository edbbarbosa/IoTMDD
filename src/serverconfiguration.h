#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H

#include <QDialog>
#include <QLineEdit>

class ServerConfiguration : public QDialog
{
    Q_OBJECT
public:
    QLineEdit *nameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *ssidEdit;
    QLineEdit *portEdit;
    explicit ServerConfiguration(QWidget *parent = nullptr);
};

#endif // SERVERCONFIGURATION_H
